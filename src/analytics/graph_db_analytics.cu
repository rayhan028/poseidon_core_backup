/*
 * Copyright (C) 2019-2022 DBIS Group - TU Ilmenau, All Rights Reserved.
 *
 * This file is part of the Poseidon package.
 *
 * Poseidon is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Poseidon is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Poseidon. If not, see <http://www.gnu.org/licenses/>.
 */

#include "graph_db.hpp"
#include <iostream>

#include <cuda_runtime.h>
#include <helper_cuda.h>

#include <thrust/host_vector.h>
#include <thrust/device_vector.h>
#include <thrust/device_ptr.h>

/**
 * A struct on GPU for merged CSR delta record(s) associated with the same node.
 */
struct d_delta {
  d_delta() = default;
  d_delta(const d_delta &) = delete;

  uint64_t node_id_;  // id of the node associated with the delta

  uint64_t *ids_;   // ids of the neighbour nodes
  double *weights_; // corresponding relationship weights
  int size_;        // number of neighbours
};

template<typename T>
__global__ void memcpy_kernel(T *dest, const T *src, uint64_t start, int size) {
    int tid = blockDim.x * blockIdx.x + threadIdx.x;
    int blk_offs = gridDim.x * blockDim.x;
    for (int i = tid; i < size; i += blk_offs) {
        dest[i] = src[i + start];
    }
}

extern "C" void device_csr_build(graph_db_ptr graph, csr_arrays &csr, rship_weight weight_func, bool bidirectional) {
  auto txid = current_transaction()->xid();

  offset_t edges = 0;
  // offset_t max_num_edges = rships_->as_vec().last_used() + 1;
  offset_t max_num_edges = graph->get_relationships()->as_vec().last_used() + 1;
  // auto &cv_nodes = nodes_->as_vec();
  auto &cv_nodes = graph->get_nodes()->as_vec();
  offset_t max_num_nodes = cv_nodes.last_used() + 1;

  auto &row_offsets = csr.row_offsets;
  auto &col_indices = csr.col_indices;
  auto &edge_values = csr.edge_values;

  int multi = bidirectional ? 1 : 2;
  row_offsets.reserve(max_num_nodes + 1);
  col_indices.reserve(multi * max_num_edges);
  edge_values.reserve(multi * max_num_edges);

  row_offsets.push_back(0); // First value is always 0

  for (offset_t nid = 0; nid < max_num_nodes; nid++) {
    if (cv_nodes.is_used(nid)) {
      // auto& n = node_by_id(nid);
      auto& n = graph->node_by_id(nid);

      auto rid = n.from_rship_list;
      while (rid != UNKNOWN) {
        // auto &r = rship_by_id(rid);
        auto &r = graph->rship_by_id(rid);
        if (r.is_valid_for(txid)) {
          col_indices.push_back(r.to_node_id());
          edge_values.push_back(weight_func(r));
          edges++;
        }
        rid = r.next_src_rship;
      }

      if (bidirectional) {
        rid = n.to_rship_list;
        while (rid != UNKNOWN) {
          // auto &r = rship_by_id(rid);
          auto &r = graph->rship_by_id(rid);
          if (r.is_valid_for(txid)) {
            col_indices.push_back(r.from_node_id());
            edge_values.push_back(weight_func(r));
            edges++;
          }
          rid = r.next_dest_rship;
        }
      }
    }
    row_offsets.push_back(edges);
  }

  auto &delta_store = graph->csr_delta_store();
  delta_store->last_txn_id_ = txid; // update id of the last transaction that made a CSR update
  delta_store->last_node_id_ = row_offsets.size() - 2; // update last node id in the CSR
  // delta_store_->last_txn_id_ = txid; // update id of the last transaction that made a CSR update
  // delta_store_->last_node_id_ = row_offsets.size() - 2; // update last node id in the CSR

  bool clear = true;
  for (auto &rec : delta_store->delta_recs_) {
    if (rec.txid_ < txid) {
      // the modifications of transaction with id "txid_" has been included in the CSR build
      // therefore, the delta record is not needed later for CSR update
      rec.merged_ = true;
    }
    else if (rec.txid_ > txid) {
      // txid_ started after txid but committed before it
      // updates by txid_ were not include in the CSR build, since they are not visible to txid

      // however, this delta record is needed later for CSR update
      // therefore, we do not clear the vector of delta records after the CSR build
      clear = false;
    }
  }
  if (clear) {
    // no delta record is needed later for updating CSR
    delta_store->clear_deltas();
  }

  // TODO
  // delta_store->row_offsets_ = row_offsets;
  // delta_store->col_indices_ = col_indices;
  // delta_store->edge_values_ = edge_values;

  delta_store->row_offs_size_ = row_offsets.size();
  delta_store->col_inds_size_ = col_indices.size();

  offset_t ro_size = delta_store->row_offs_size_ * sizeof(offset_t);
  offset_t ci_size = delta_store->col_inds_size_ * sizeof(offset_t);
  offset_t ev_size = delta_store->col_inds_size_ * sizeof(float);

  cudaMalloc((void **)&delta_store->row_offsets_, ro_size);
  cudaMalloc((void **)&delta_store->col_indices_, ci_size);
  cudaMalloc((void **)&delta_store->edge_values_, ev_size);
  cudaMemcpy(delta_store->row_offsets_, row_offsets.data(), ro_size, cudaMemcpyHostToDevice);
  cudaMemcpy(delta_store->row_offsets_, col_indices.data(), ci_size, cudaMemcpyHostToDevice);
  cudaMemcpy(delta_store->row_offsets_, edge_values.data(), ev_size, cudaMemcpyHostToDevice);
}

extern "C" void device_csr_update_with_delta(graph_db_ptr graph, csr_arrays &csr) {
  auto tx = current_transaction();
  if (!tx)
    throw out_of_transaction_scope();

  auto &delta_store = graph->csr_delta_store();
  auto last_txid = delta_store->last_txn_id();
  auto txid = tx->xid();

  if (last_txid == UNKNOWN) {
    // no CSR exists yet, so we make the initial CSR build
    device_csr_build(graph, csr, delta_store->weight_func_, delta_store->bidirectional_);
    return;
  }
  else if (last_txid > txid) {
    // a more recent transaction updated the CSR to a newer snapshot. We abort, since
    // 1) transaction with id "txid" might see updates that should not be visible to it, and
    // 2) we cannot update the CSR to an older snapshot
    throw invalid_csr_update();
  }

  // TODO
  // thrust::device_vector<offset_t> old_row_offs = delta_store->row_offsets_;
  // thrust::device_vector<offset_t> old_col_inds = delta_store->col_indices_;
  // thrust::device_vector<float> old_edge_vals = delta_store->edge_values_;

  offset_t *old_row_offs;
  offset_t *old_col_inds;
  float *old_edge_vals;

  std::size_t ro_size = delta_store->row_offs_size_ * sizeof(offset_t);
  std::size_t ci_size = delta_store->col_inds_size_ * sizeof(offset_t);
  std::size_t ev_size = delta_store->col_inds_size_ * sizeof(float);

  cudaMalloc((void **)&old_row_offs, ro_size);
  cudaMalloc((void **)&old_col_inds, ci_size);
  cudaMalloc((void **)&old_edge_vals, ev_size);
  cudaMemcpy(old_row_offs, delta_store->row_offsets_, ro_size, cudaMemcpyDeviceToDevice);
  cudaMemcpy(old_col_inds, delta_store->col_indices_, ci_size, cudaMemcpyDeviceToDevice);
  cudaMemcpy(old_edge_vals, delta_store->edge_values_, ev_size, cudaMemcpyDeviceToDevice);

  // TODO
  // delta_store->row_offsets_.clear();
  // delta_store->col_indices_.clear();
  // delta_store->edge_values_.clear();

  cudaFree(delta_store->row_offsets_);
  cudaFree(delta_store->col_indices_);
  cudaFree(delta_store->edge_values_);

  auto &new_row_offs = delta_store->row_offsets_;
  auto &new_col_inds = delta_store->col_indices_;
  auto &new_edge_vals = delta_store->edge_values_;

  // merge delta records into a delta map and transfer to GPU
  delta_store::delta_map_t h_deltas;
  delta_store->merge_deltas(h_deltas, txid);

  if (h_deltas.empty()) {
    // nothing to update CSR
    return;
  }

  auto edge_diff = 0; // difference between the sum of edges of all existing nodes
                      // (i.e. from 0 to last node id) in the new and current (i.e. old) CSR
  auto num_new_nodes = 0; // number of newly appended nodes after the last node id
                          // in the current (i.e. old) CSR
  auto num_new_edges = 0; // sum of edges of newly appended nodes

  std::size_t num_deltas = h_deltas.size();
  auto lnid = delta_store->last_node_id();
  uint64_t lnid_pos = std::distance(h_deltas.begin(), h_deltas.upper_bound(lnid));

  // transfer deltas to GPU
  d_delta *d_deltas;
  auto size_deltas = num_deltas * sizeof(d_delta);
  cudaMalloc((void **)&d_deltas, size_deltas);

  auto size_id_ptrs = num_deltas * sizeof(uint64_t *);
  auto size_weight_ptrs = num_deltas * sizeof(double *);
  uint64_t **d_ids = (uint64_t **)malloc(size_id_ptrs);
  uint64_t **d_weights = (uint64_t **)malloc(size_weight_ptrs);

#ifdef DIFF_DELTA
#elif defined ADJ_DELTA
  std::size_t i = 0;
  for (auto &[nid, h_delta] : h_deltas) {
    cudaMemcpy(&(d_deltas[i].node_id_), &h_delta.node_id_, sizeof(uint64_t), cudaMemcpyHostToDevice);

    auto size_col_inds = h_delta.ids_.size() * sizeof(uint64_t);
    cudaMalloc((void **)&(d_ids[i]), size_col_inds);
    cudaMemcpy(&(d_deltas[i].ids_), &(d_ids[i]), sizeof(uint64_t *), cudaMemcpyHostToDevice);
    cudaMemcpy(&(d_deltas[i].ids_), h_delta.ids_.data(), size_col_inds, cudaMemcpyHostToDevice);

    auto size_edge_vals = h_delta.weights_.size() * sizeof(double);
    cudaMalloc((void **)&(d_weights[i]), size_edge_vals);
    cudaMemcpy(&(d_deltas[i].weights_), &(d_weights[i]), sizeof(double *), cudaMemcpyHostToDevice);
    cudaMemcpy(&(d_deltas[i].weights_), h_delta.weights_.data(), size_edge_vals, cudaMemcpyHostToDevice);

    auto neighbours = h_delta.ids_.size();
    cudaMemcpy(&(d_deltas[i].size_), &neighbours, sizeof(int), cudaMemcpyHostToDevice);

    if (i < lnid_pos) { // TODO: too expensive
      offset_t offs_a = 0, offs_b = 0;
      cudaMemcpy(&offs_a, &(old_row_offs[nid + 1]), sizeof(uint64_t), cudaMemcpyDeviceToHost);
      cudaMemcpy(&offs_b, &(old_row_offs[nid]), sizeof(uint64_t), cudaMemcpyDeviceToHost);
      int old_edges = offs_a - offs_b;
      int new_edges = h_delta.ids_.size();
      auto diff = new_edges - old_edges;
      edge_diff += diff;
    }
    else {
      num_new_nodes++;
      num_new_edges += h_delta.ids_.size();
    }

    i++;
  }

  // TODO
  // uint64_t new_row_offs_size = old_row_offs.size() + num_new_nodes;
  // uint64_t new_col_inds_size = old_col_inds.size() + edge_diff + num_new_edges;

  // new_row_offs.reserve(new_row_offs_size);
  // new_col_inds.reserve(new_col_inds_size);
  // new_edge_vals.reserve(new_col_inds_size);

  uint64_t new_row_offs_size = delta_store->row_offs_size_ + num_new_nodes;
  uint64_t new_col_inds_size = delta_store->col_inds_size_ + edge_diff + num_new_edges;

  std::size_t new_ro_size = new_row_offs_size * sizeof(offset_t);
  std::size_t new_ci_size = new_col_inds_size * sizeof(offset_t);
  std::size_t new_ev_size = new_col_inds_size * sizeof(float);

  cudaMalloc((void **)&new_row_offs, new_ro_size);
  cudaMalloc((void **)&new_col_inds, new_ci_size);
  cudaMalloc((void **)&new_edge_vals, new_ev_size);

  // process entries for the first node until the last updated node
  uint64_t next_nid = 0;
  offset_t val = 0;
  cudaMemcpy(&(new_row_offs[0]), &val, sizeof(offset_t), cudaMemcpyHostToDevice);
  int threads_per_block = 256;
  for (uint64_t i = 0; i < lnid_pos; i++) { // TODO
    uint64_t nid;
    cudaMemcpy(&nid, &(d_deltas[i].node_id_), sizeof(uint64_t), cudaMemcpyDeviceToHost);
    while (next_nid < nid) {
      // possibly, some nodes between the first node and the last updated node are unchanged
      // we just copy the entries for such nodes from the current (i.e. old) CSR to the new CSR
      offset_t offs_a = 0, offs_b = 0;
      cudaMemcpy(&offs_a, &(old_row_offs[next_nid + 1]), sizeof(uint64_t), cudaMemcpyDeviceToHost);
      cudaMemcpy(&offs_b, &(old_row_offs[next_nid]), sizeof(uint64_t), cudaMemcpyDeviceToHost);
      auto edges = offs_a - offs_b;
      offset_t offs_c = 0;
      cudaMemcpy(&offs_c, &(new_row_offs[next_nid]), sizeof(uint64_t), cudaMemcpyDeviceToHost);
      offset_t offs_d = offs_c + edges;
      cudaMemcpy(&(new_row_offs[next_nid + 1]), &offs_d, sizeof(offset_t), cudaMemcpyHostToDevice);

      int blocks_per_grid = (edges + threads_per_block - 1) / threads_per_block;
      memcpy_kernel<offset_t> <<<blocks_per_grid, threads_per_block>>>(new_col_inds, old_col_inds, offs_b, edges);
      memcpy_kernel<float> <<<blocks_per_grid, threads_per_block>>>(new_edge_vals, old_edge_vals, offs_b, edges);

      next_nid++;
    }
    // assert(next_nid == nid);
    // using the delta map item, insert entries for the updated node into the new CSR
    offset_t offs_a = 0;
    cudaMemcpy(&offs_a, &(new_row_offs[next_nid]), sizeof(uint64_t), cudaMemcpyDeviceToHost);
    int neighbours = 0;
    cudaMemcpy(&neighbours, &(d_deltas[i].size_), sizeof(int), cudaMemcpyDeviceToHost);
    offset_t offs_b = offs_a + neighbours;
    cudaMemcpy(&(new_row_offs[next_nid + 1]), &offs_b, sizeof(offset_t), cudaMemcpyHostToDevice);

    int blocks_per_grid = (neighbours + threads_per_block - 1) / threads_per_block;
    memcpy_kernel<offset_t> <<<blocks_per_grid, threads_per_block>>>(new_col_inds, d_deltas[i].ids_, 0, neighbours);
    memcpy_kernel<float> <<<blocks_per_grid, threads_per_block>>>(new_edge_vals, d_deltas[i].weights_, 0, neighbours);

    next_nid++;
  }

  // possibly, some nodes between the last updated node and the last node id in the current (i.e. old) CSR are unchanged
  // similarly, we just copy the entries for such nodes from the current (i.e. old) CSR to the new CSR

  while (next_nid < (delta_store->row_offs_size_ - 1)) {
    offset_t offs_a = 0, offs_b = 0;
    cudaMemcpy(&offs_a, &(old_row_offs[next_nid + 1]), sizeof(uint64_t), cudaMemcpyDeviceToHost);
    cudaMemcpy(&offs_b, &(old_row_offs[next_nid]), sizeof(uint64_t), cudaMemcpyDeviceToHost);
    auto edges = offs_a - offs_b;
    offset_t offs_c = 0;
    cudaMemcpy(&offs_c, &(new_row_offs[next_nid]), sizeof(uint64_t), cudaMemcpyDeviceToHost);
    offset_t offs_d = offs_c + edges;
    cudaMemcpy(&(new_row_offs[next_nid + 1]), &offs_d, sizeof(offset_t), cudaMemcpyHostToDevice);

    int blocks_per_grid = (edges + threads_per_block - 1) / threads_per_block;
    memcpy_kernel<offset_t> <<<blocks_per_grid, threads_per_block>>>(new_col_inds, old_col_inds, offs_b, edges);
    memcpy_kernel<float> <<<blocks_per_grid, threads_per_block>>>(new_edge_vals, old_edge_vals, offs_b, edges);

    next_nid++;
  }

  // finally, using the remaining delta map items, insert entries for the newly appended nodes into the new CSR
  for (uint64_t i = lnid_pos; i < num_deltas; i++) {
    uint64_t nid;
    cudaMemcpy(&nid, &(d_deltas[i].node_id_), sizeof(uint64_t), cudaMemcpyDeviceToHost);
    while (next_nid < nid) { // unused node record slots
      offset_t offs_a = 0;
      cudaMemcpy(&offs_a, &(new_row_offs[next_nid]), sizeof(uint64_t), cudaMemcpyDeviceToHost);
      cudaMemcpy(&(new_row_offs[next_nid + 1]), &offs_a, sizeof(offset_t), cudaMemcpyHostToDevice);

      next_nid++;
    }
    // assert(next_nid == nid);
    offset_t offs_a = 0;
    cudaMemcpy(&offs_a, &(new_row_offs[next_nid]), sizeof(uint64_t), cudaMemcpyDeviceToHost);
    int neighbours = 0;
    cudaMemcpy(&neighbours, &(d_deltas[i].size_), sizeof(int), cudaMemcpyDeviceToHost);
    offset_t offs_b = offs_a + neighbours;
    cudaMemcpy(&(new_row_offs[next_nid + 1]), &offs_b, sizeof(offset_t), cudaMemcpyHostToDevice);

    int blocks_per_grid = (neighbours + threads_per_block - 1) / threads_per_block;
    memcpy_kernel<offset_t> <<<blocks_per_grid, threads_per_block>>>(new_col_inds, d_deltas[i].ids_, 0, neighbours);
    memcpy_kernel<float> <<<blocks_per_grid, threads_per_block>>>(new_edge_vals, d_deltas[i].weights_, 0, neighbours);

    next_nid++;
  }
#endif

  delta_store_->last_txn_id_ = txid; // update id of the last transaction that made a CSR update
  delta_store_->last_node_id_ = next_nid - 1; // update last node id in the CSR
}