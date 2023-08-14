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

#include <boost/algorithm/string.hpp>

#include "graph_db.hpp"
#include "spdlog/spdlog.h"
#include "query_builder.hpp"
#ifdef USE_GUNROCK
#include "graph_db_analytics.cu"
#endif
#include <iostream>


void graph_db::poseidon_to_csr(csr_arrays &csr, rship_weight weight_func, bool bidirectional) {
#if defined CSR_DELTA
  if (delta_store_->delta_mode_) {
    // we use weight_func and bidirectional as per the delta store
    #ifdef USE_GUNROCK
    device_csr_update_with_delta(std::make_shared<graph_db>(*this), csr);
    #else
    host_csr_update_with_delta(csr);
    #endif
  }
  else {
    #ifdef PARALLEL_CSR_BUILD
    parallel_host_csr_build(csr, weight_func, bidirectional);
    #elif defined USE_GUNROCK
    device_csr_build(std::make_shared<graph_db>(*this), csr, weight_func, bidirectional);
    #else
    host_csr_build(csr, weight_func, bidirectional);
    #endif
  }
#elif defined PARALLEL_CSR_BUILD
  parallel_host_csr_build(csr, weight_func, bidirectional);
#else
  host_csr_build(csr, weight_func, bidirectional);
#endif
}

#ifndef USE_GUNROCK

void graph_db::host_csr_build(csr_arrays &csr, rship_weight weight_func, bool bidirectional) {
  auto txid = current_transaction()->xid();

  offset_t edges = 0;
  offset_t max_num_edges = rships_->as_vec().last_used() + 1;
  auto &cv_nodes = nodes_->as_vec();
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
      auto& n = node_by_id(nid);

      auto rid = n.from_rship_list;
      while (rid != UNKNOWN) {
        auto &r = rship_by_id(rid);
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
          auto &r = rship_by_id(rid);
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
#if defined CSR_DELTA
  delta_store_->last_txn_id_ = txid; // update id of the last transaction that made a CSR update
  delta_store_->last_node_id_ = row_offsets.size() - 2; // update last node id in the CSR

  bool clear = true;
  for (auto &rec : delta_store_->delta_recs_) {
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
    delta_store_->clear_deltas();
  }

  delta_store_->row_offsets_ = row_offsets;
  delta_store_->col_indices_ = col_indices;
  delta_store_->edge_values_ = edge_values;
#endif
}

void graph_db::parallel_host_csr_build(csr_arrays &csr, rship_weight weight_func, bool bidirectional) {
  offset_t max_num_nodes = nodes_->as_vec().last_used() + 1;
  offset_t max_num_edges = rships_->as_vec().last_used() + 1;

  auto &row_offsets = csr.row_offsets;
  auto &col_indices = csr.col_indices;
  auto &edge_values = csr.edge_values;

  int multi = bidirectional ? 1 : 2;

  row_offsets.reserve(max_num_nodes + 1);
  col_indices.reserve(multi * max_num_edges);
  edge_values.reserve(multi * max_num_edges);

  row_offsets.push_back(0); // First value is always 0

  result_set rs;
  graph_db_ptr gptr(this);
  query_ctx ctx(gptr);
  auto q = query_builder(ctx)
                .all_nodes()
                .csr(weight_func, bidirectional)
                .orderby([&](const qr_tuple q1, const qr_tuple q2) {
                  return boost::get<uint64_t>(q1[1]) < boost::get<uint64_t>(q2[1]); })
                .collect(rs);
  q.get_pipeline().start(ctx);

  // std::cout << rs << "\n";

  offset_t nid = 0;
  offset_t total_edges = 0;
  for (auto &tuple : rs.data) {
    while (nid < std::stoull(boost::get<std::string>(tuple[1]))) {
      row_offsets.push_back(total_edges);
      nid++;
    }
    assert(std::stoull(boost::get<std::string>(tuple[1])) == nid);
    int edges = std::stoi(boost::get<std::string>(tuple[2]));
    for (auto i = 0; i < (2 * edges - 1); i += 2) {
      col_indices.push_back(std::stoull(boost::get<std::string>(tuple[3 + i])));
      edge_values.push_back(std::stod(boost::get<std::string>(tuple[3 + i + 1])));
    }
    total_edges += edges;
    row_offsets.push_back(total_edges);
    nid++;
  }
#if defined CSR_DELTA
  auto txid = current_transaction()->xid();
  delta_store_->last_txn_id_ = txid; // update id of the last transaction that made a CSR update
  delta_store_->last_node_id_ = row_offsets.size() - 2; // update last node id in the CSR

  bool clear = true;
  for (auto &rec : delta_store_->delta_recs_) {
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
    delta_store_->clear_deltas();
  }

  delta_store_->row_offsets_ = row_offsets;
  delta_store_->col_indices_ = col_indices;
  delta_store_->edge_values_ = edge_values;
#endif
}

#if defined CSR_DELTA
void graph_db::host_csr_update_with_delta(csr_arrays &csr) {
  auto tx = current_transaction();
  if (!tx)
    throw out_of_transaction_scope();

  auto last_txid = delta_store_->last_txn_id();
  auto txid = tx->xid();

  if (last_txid == UNKNOWN) {
    // no CSR exists yet, so we make the initial CSR build
#ifdef PARALLEL_CSR_BUILD
    parallel_host_csr_build(csr, delta_store_->weight_func_, delta_store_->bidirectional_);
#else
    host_csr_build(csr, delta_store_->weight_func_, delta_store_->bidirectional_);
#endif
    return;
  }
  else if (last_txid > txid) {
    // a more recent transaction updated the CSR to a newer snapshot. We abort, since
    // 1) transaction with id "txid" might see updates that should not be visible to it, and
    // 2) we cannot update the CSR to an older snapshot
    throw invalid_csr_update();
  }

  auto &old_row_offs = delta_store_->row_offsets_;
  auto &old_col_inds = delta_store_->col_indices_;
  auto &old_edge_vals = delta_store_->edge_values_;

  auto &new_row_offs = csr.row_offsets;
  auto &new_col_inds = csr.col_indices;
  auto &new_edge_vals = csr.edge_values;

  // merge delta records into a delta map
  delta_store::delta_map_t deltas;
  delta_store_->merge_deltas(deltas, txid);

  if (deltas.empty()) {
    // nothing to update CSR
    return;
  }

  auto edge_diff = 0; // difference between the sum of edges of all existing nodes
                      // (i.e. from 0 to last node id) in the new and current (i.e. old) CSR
  auto lnid = delta_store_->last_node_id();
  const auto beg_iter = deltas.begin();
  const auto lnid_iter = deltas.upper_bound(lnid);
  const auto end_iter = deltas.end();

#ifdef DIFF_DELTA
  for (auto iter = beg_iter; iter != lnid_iter; ++iter) {
    auto nid = iter->first;
    auto &delta = iter->second;
    int ins_edges = delta.inserts_.size();
    int del_edges = delta.deletes_.size();
    auto diff = ins_edges - del_edges;
    edge_diff += diff;
  }

  auto num_new_nodes = 0; // number of newly appended nodes after the last node id
                          // in the current (i.e. old) CSR
  auto num_new_edges = 0; // sum of edges of newly appended nodes
  for (auto iter = lnid_iter; iter != end_iter; ++iter) {
    auto nid = iter->first;
    auto &delta = iter->second;
    num_new_nodes++;
    num_new_edges += delta.inserts_.size();
  }

  uint64_t new_row_offs_size = old_row_offs.size() + num_new_nodes;
  uint64_t new_col_inds_size = old_col_inds.size() + edge_diff + num_new_edges;

  new_row_offs.reserve(new_row_offs_size);
  new_col_inds.reserve(new_col_inds_size);
  new_edge_vals.reserve(new_col_inds_size);

  // process entries for the first node until the last updated node
  uint64_t next_nid = 0; // TODO potential optimization using next_nid and prev_nid
  new_row_offs.push_back(0);
  for (auto iter = beg_iter; iter != lnid_iter; ++iter) {
    auto nid = iter->first;
    auto &delta = iter->second;
    while (next_nid < nid) {
      // possibly, some nodes between the first node and the last updated node are unchanged
      // we just copy the entries for such nodes from the current (i.e. old) CSR to the new CSR
      auto edges = old_row_offs[next_nid + 1] - old_row_offs[next_nid];
      new_row_offs.push_back(new_row_offs.back() + edges);

      auto beg_col_inds_iter = old_col_inds.begin() + old_row_offs[next_nid];
      auto end_col_inds_iter = beg_col_inds_iter + edges;
      new_col_inds.insert(new_col_inds.end(),
        std::make_move_iterator(beg_col_inds_iter),
          std::make_move_iterator(end_col_inds_iter));

      auto beg_edge_vals_iter = old_edge_vals.begin() + old_row_offs[next_nid];
      auto end_edge_vals_iter = beg_edge_vals_iter + edges;
      new_edge_vals.insert(new_edge_vals.end(),
        std::make_move_iterator(beg_edge_vals_iter),
          std::make_move_iterator(end_edge_vals_iter));

      next_nid++;
    }
    // assert(next_nid == d.first);

    // get a temporary copy of old neighbours
    std::vector<offset_t> tmp_col_inds;
    auto edges = old_row_offs[next_nid + 1] - old_row_offs[next_nid];
    tmp_col_inds.reserve(edges + delta.inserts_.size());
    {
      auto beg_iter = old_col_inds.begin() + old_row_offs[next_nid];
      auto end_iter = beg_iter + edges;
      tmp_col_inds.insert(tmp_col_inds.end(),
        std::make_move_iterator(beg_iter),
          std::make_move_iterator(end_iter));
    }

    // add inserted neighbours
    {
      auto beg_iter = delta.inserts_.begin();
      auto end_iter = delta.inserts_.end();
      tmp_col_inds.insert(tmp_col_inds.end(),
        std::make_move_iterator(beg_iter),
          std::make_move_iterator(end_iter));
    }

    // get a temporary copy of old weights
    std::vector<double> tmp_edge_vals;
    tmp_edge_vals.reserve(edges + delta.inserts_.size());
    {
      auto beg_iter = old_edge_vals.begin() + old_row_offs[next_nid];
      auto end_iter = beg_iter + edges;
      tmp_edge_vals.insert(tmp_edge_vals.end(),
        std::make_move_iterator(beg_iter),
          std::make_move_iterator(end_iter));
    }

    // add inserted neighbours
    {
      auto beg_iter = delta.weights_.begin();
      auto end_iter = delta.weights_.end();
      tmp_edge_vals.insert(tmp_edge_vals.end(),
        std::make_move_iterator(beg_iter),
          std::make_move_iterator(end_iter));
    }

    auto tmp_col_inds_iter = tmp_col_inds.begin();
    auto tmp_edge_vals_iter = tmp_edge_vals.begin();
    while (tmp_col_inds_iter != tmp_col_inds.end()) {
      bool deleted = false;
      for (auto dels_iter = delta.deletes_.begin();
              dels_iter != delta.deletes_.end(); ++dels_iter) {
        if (*tmp_col_inds_iter == *dels_iter) {
          // delta.deletes_.erase(k);
          deleted = true;
          break;
        }
      }
      if (deleted) {
        tmp_col_inds.erase(tmp_col_inds_iter);
        tmp_edge_vals.erase(tmp_edge_vals_iter);
      }
      else {
        ++tmp_col_inds_iter;
        ++tmp_edge_vals_iter;
      }
    }
    // using the delta map item, insert entries for the updated node into the new CSR
    new_row_offs.push_back(new_row_offs.back() + tmp_col_inds.size());

    auto beg_col_inds_iter = tmp_col_inds.begin();
    auto end_col_inds_iter = tmp_col_inds.end();
    new_col_inds.insert(new_col_inds.end(),
      std::make_move_iterator(beg_col_inds_iter),
        std::make_move_iterator(end_col_inds_iter));

    auto beg_edge_vals_iter = tmp_edge_vals.begin();
    auto end_edge_vals_iter = tmp_edge_vals.end();
    new_edge_vals.insert(new_edge_vals.end(),
      std::make_move_iterator(beg_edge_vals_iter),
        std::make_move_iterator(end_edge_vals_iter));

    next_nid++;
  }

  // possibly, some nodes between the last updated node and the last node id in the current (i.e. old) CSR are unchanged
  // similarly, we just copy the entries for such nodes from the current (i.e. old) CSR to the new CSR
  auto beg_col_inds_iter = old_col_inds.begin() + old_row_offs[next_nid];
  auto end_col_inds_iter = old_col_inds.end();
  new_col_inds.insert(new_col_inds.end(),
    std::make_move_iterator(beg_col_inds_iter),
      std::make_move_iterator(end_col_inds_iter));

  auto beg_edge_vals_iter = old_edge_vals.begin() + old_row_offs[next_nid];
  auto end_edge_vals_iter = old_edge_vals.end();
  new_edge_vals.insert(new_edge_vals.end(),
    std::make_move_iterator(beg_edge_vals_iter),
      std::make_move_iterator(end_edge_vals_iter));

  while (next_nid < (old_row_offs.size() - 1)) {
    auto edges = old_row_offs[next_nid + 1] - old_row_offs[next_nid];
    new_row_offs.push_back(new_row_offs.back() + edges);
    next_nid++;
  }

  // finally, using the remaining delta map items, insert entries for the newly appended nodes into the new CSR
  for (auto iter = lnid_iter; iter != end_iter; ++iter) {
    auto nid = iter->first;
    auto &delta = iter->second;
    while (next_nid < nid) { // unused node record slots
      new_row_offs.push_back(new_row_offs.back());
      next_nid++;
    }
    // assert(next_nid == d.first);
    new_row_offs.push_back(new_row_offs.back() + delta.inserts_.size());

    auto beg_col_inds_iter = delta.inserts_.begin();
    auto end_col_inds_iter = delta.inserts_.end();
    new_col_inds.insert(new_col_inds.end(),
      std::make_move_iterator(beg_col_inds_iter),
        std::make_move_iterator(end_col_inds_iter));

    auto beg_edge_vals_iter = delta.weights_.begin();
    auto end_edge_vals_iter = delta.weights_.end();
    new_edge_vals.insert(new_edge_vals.end(),
      std::make_move_iterator(beg_edge_vals_iter),
        std::make_move_iterator(end_edge_vals_iter));

    next_nid++;
  }

#elif defined ADJ_DELTA
  for (auto iter = beg_iter; iter != lnid_iter; ++iter) {
    auto nid = iter->first;
    auto &delta = iter->second;
    int old_edges = old_row_offs[nid + 1] - old_row_offs[nid];
    int new_edges = delta.ids_.size();
    auto diff = new_edges - old_edges;
    edge_diff += diff;
  }

  auto num_new_nodes = 0; // number of newly appended nodes after the last node id
                          // in the current (i.e. old) CSR
  auto num_new_edges = 0; // sum of edges of newly appended nodes
  for (auto iter = lnid_iter; iter != end_iter; ++iter) {
    auto &delta = iter->second;
    num_new_nodes++;
    num_new_edges += delta.ids_.size();
  }

  uint64_t new_row_offs_size = old_row_offs.size() + num_new_nodes;
  uint64_t new_col_inds_size = old_col_inds.size() + edge_diff + num_new_edges;

  new_row_offs.reserve(new_row_offs_size);
  new_col_inds.reserve(new_col_inds_size);
  new_edge_vals.reserve(new_col_inds_size);

  // process entries for the first node until the last updated node
  uint64_t next_nid = 0; // TODO potential optimization using next_nid and prev_nid
  new_row_offs.push_back(0);
  for (auto iter = beg_iter; iter != lnid_iter; ++iter) {
    auto nid = iter->first;
    auto &delta = iter->second;
    auto &ids = delta.ids_;
    auto &weights = delta.weights_;
    while (next_nid < nid) {
      // possibly, some nodes between the first node and the last updated node are unchanged
      // we just copy the entries for such nodes from the current (i.e. old) CSR to the new CSR
      auto edges = old_row_offs[next_nid + 1] - old_row_offs[next_nid];
      new_row_offs.push_back(new_row_offs.back() + edges);

      auto beg_col_inds_iter = old_col_inds.begin() + old_row_offs[next_nid];
      auto end_col_inds_iter = beg_col_inds_iter + edges;
      new_col_inds.insert(new_col_inds.end(),
        std::make_move_iterator(beg_col_inds_iter),
          std::make_move_iterator(end_col_inds_iter));

      auto beg_edge_vals_iter = old_edge_vals.begin() + old_row_offs[next_nid];
      auto end_edge_vals_iter = beg_edge_vals_iter + edges;
      new_edge_vals.insert(new_edge_vals.end(),
        std::make_move_iterator(beg_edge_vals_iter),
          std::make_move_iterator(end_edge_vals_iter));

      next_nid++;
    }
    // assert(next_nid == nid);
    // using the delta map item, insert entries for the updated node into the new CSR
    new_row_offs.push_back(new_row_offs.back() + ids.size());

    auto beg_col_inds_iter = ids.begin();
    auto end_col_inds_iter = ids.end();
    new_col_inds.insert(new_col_inds.end(),
      std::make_move_iterator(beg_col_inds_iter),
        std::make_move_iterator(end_col_inds_iter));

    auto beg_edge_vals_iter = weights.begin();
    auto end_edge_vals_iter = weights.end();
    new_edge_vals.insert(new_edge_vals.end(),
      std::make_move_iterator(beg_edge_vals_iter),
        std::make_move_iterator(end_edge_vals_iter));

    next_nid++;
  }

  // possibly, some nodes between the last updated node and the last node id in the current (i.e. old) CSR are unchanged
  // similarly, we just copy the entries for such nodes from the current (i.e. old) CSR to the new CSR
  auto beg_col_inds_iter = old_col_inds.begin() + old_row_offs[next_nid];
  auto end_col_inds_iter = old_col_inds.end();
  new_col_inds.insert(new_col_inds.end(),
    std::make_move_iterator(beg_col_inds_iter),
      std::make_move_iterator(end_col_inds_iter));

  auto beg_edge_vals_iter = old_edge_vals.begin() + old_row_offs[next_nid];
  auto end_edge_vals_iter = old_edge_vals.end();
  new_edge_vals.insert(new_edge_vals.end(),
    std::make_move_iterator(beg_edge_vals_iter),
      std::make_move_iterator(end_edge_vals_iter));

  while (next_nid < (old_row_offs.size() - 1)) {
    auto edges = old_row_offs[next_nid + 1] - old_row_offs[next_nid];
    new_row_offs.push_back(new_row_offs.back() + edges);
    next_nid++;
  }

  // finally, using the remaining delta map items, insert entries for the newly appended nodes into the new CSR
  for (auto iter = lnid_iter; iter != end_iter; ++iter) {
    auto nid = iter->first;
    auto &delta = iter->second;
    auto &ids = delta.ids_;
    auto &weights = delta.weights_;
    while (next_nid < nid) { // unused node record slots
      new_row_offs.push_back(new_row_offs.back());
      next_nid++;
    }
    // assert(next_nid == nid);
    new_row_offs.push_back(new_row_offs.back() + ids.size());

    auto beg_col_inds_iter = ids.begin();
    auto end_col_inds_iter = ids.end();
    new_col_inds.insert(new_col_inds.end(),
      std::make_move_iterator(beg_col_inds_iter),
        std::make_move_iterator(end_col_inds_iter));

    auto beg_edge_vals_iter = weights.begin();
    auto end_edge_vals_iter = weights.end();
    new_edge_vals.insert(new_edge_vals.end(),
      std::make_move_iterator(beg_edge_vals_iter),
        std::make_move_iterator(end_edge_vals_iter));

    next_nid++;
  }
#endif

  delta_store_->last_txn_id_ = txid; // update id of the last transaction that made a CSR update
  delta_store_->last_node_id_ = new_row_offs.size() - 2; // update last node id in the CSR

  // TODO this is not needed when CSR update is done directly on GPU
  delta_store_->row_offsets_ = new_row_offs;
  delta_store_->col_indices_ = new_col_inds;
  delta_store_->edge_values_ = new_edge_vals;
}
#endif

#endif

void graph_db::poseidon_to_coo(edge_coords* edge_coordinates, float* edge_values, rship_weight weight_func, bool bidirectional) {
  auto &cv_rsips = rships_->as_vec();
  auto iter = cv_rsips.begin();
  auto last = cv_rsips.end();
  unsigned long long edges = 0;

  // Iterates over relationship list
  while (iter != last) {
    relationship r = (*iter);
    edge_coordinates[edges].x = r.from_node_id();
    edge_coordinates[edges].y = r.to_node_id();
    edge_values[edges] = weight_func(r);
    edges++;
    if(bidirectional){
      edge_coordinates[edges].x = r.to_node_id();
      edge_coordinates[edges].y = r.from_node_id();
      edge_values[edges] = weight_func(r);
      edges++;
    } // if bidirectional
    ++iter;
  } // Outer while loop
}