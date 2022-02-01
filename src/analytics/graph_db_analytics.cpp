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
#include "parser.hpp"
#include "spdlog/spdlog.h"
#include "query.hpp"
#include <iostream>


void graph_db::poseidon_to_csr(csr_arrays &csr, rship_weight weight_func, bool bidirectional) {
#if defined CSR_DELTA_STORE && defined USE_TX
  if (csr_delta_->delta_mode_ ) {
    // we use weight_func and bidirectional as per the delta store
    csr_update_with_delta(csr);
  }
  else {
#ifdef PARALLEL_CSR_BUILD
    parallel_csr_build(csr, weight_func, bidirectional);
#else
    csr_build(csr, weight_func, bidirectional);
#endif
  }
#elif defined PARALLEL_CSR_BUILD
  parallel_csr_build(csr, weight_func, bidirectional);
#else
  csr_build(csr, weight_func, bidirectional);
#endif
}

void graph_db::csr_build(csr_arrays &csr, rship_weight weight_func, bool bidirectional) {
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
#if defined CSR_DELTA_STORE && defined USE_TX
  csr_delta_->last_txn_id_ = txid; // update id of the last transaction that made a CSR update
  csr_delta_->last_node_id_ = row_offsets.size() - 2; // update last node id in the CSR

  bool clear = true;
  for (auto &elem : csr_delta_->delta_elements_) {
    if (elem.txid_ < txid) {
      // the modifications of txid_ has been included in the CSR build 
      // therefore, the delta element is not needed for later restores
      elem.restored_ = true;
    }  
    else if (elem.txid_ > txid) {
      // txid_ started after txid but committed before it
      // updates by txid_ were not include in the CSR build, since they are not visible to txid
      
      // however, this delta element is needed later for updating the CSR 
      // therefore, we do not clear the vector of delta elements after the CSR build 
      clear = false;
    }
  }
  if (clear) {
    // no delta element is needed later for updating CSR
    csr_delta_->delta_elements_.clear();
    csr_delta_->num_delta_elements_ = 0;
    csr_delta_->delta_mode_ = true;
  }

  // TODO this is not needed when CSR update is done directly on GPU
  csr_delta_->row_offsets_ = row_offsets;
  csr_delta_->col_indices_ = col_indices;
  csr_delta_->edge_values_ = edge_values;
#endif
}

void graph_db::parallel_csr_build(csr_arrays &csr, rship_weight weight_func, bool bidirectional) {
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
  auto q = query(this)
                .all_nodes()
                .csr(weight_func, bidirectional)
                .orderby([&](const qr_tuple q1, const qr_tuple q2) {
                  return boost::get<uint64_t>(q1[1]) < boost::get<uint64_t>(q2[1]); })
                .collect(rs);
  q.start();

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
#if defined CSR_DELTA_STORE && defined USE_TX
  auto txid = current_transaction()->xid();
  csr_delta_->last_txn_id_ = txid; // update id of the last transaction that made a CSR update
  csr_delta_->last_node_id_ = row_offsets.size() - 2; // update last node id in the CSR

  bool clear = true;
  for (auto &elem : csr_delta_->delta_elements_) {
    if (elem.txid_ < txid) {
      // the modifications of txid_ has been included in the CSR build 
      // therefore, the delta element is not needed for later restores
      elem.restored_ = true;
    }  
    else if (elem.txid_ > txid) {
      // txid_ started after txid but committed before it
      // updates by txid_ were not include in the CSR build, since they are not visible to txid
      
      // however, this delta element is needed later for updating the CSR 
      // therefore, we do not clear the vector of delta elements after the CSR build 
      clear = false;
    }
  }
  if (clear) {
    // no delta element is needed later for updating CSR
    csr_delta_->delta_elements_.clear();
    csr_delta_->num_delta_elements_ = 0;
    csr_delta_->delta_mode_ = true;
  }

  // TODO this is not needed when CSR update is done directly on GPU
  csr_delta_->row_offsets_ = row_offsets;
  csr_delta_->col_indices_ = col_indices;
  csr_delta_->edge_values_ = edge_values;
#endif
}

#if defined CSR_DELTA_STORE && defined USE_TX
void graph_db::csr_update_with_delta(csr_arrays &csr) {
  auto tx = current_transaction();
  if (!tx)
    throw out_of_transaction_scope();

  auto last_txid = csr_delta_->get_last_txn_id();
  auto txid = tx->xid();

  if (last_txid == UNKNOWN) {
    // no CSR exists yet for the delta, so we make the initial CSR build
#ifdef PARALLEL_CSR_BUILD
    parallel_csr_build(csr, csr_delta_->weight_func_, csr_delta_->bidirectional_);
#else
    csr_build(csr, csr_delta_->weight_func_, csr_delta_->bidirectional_);
#endif
    return;
  }
  else if (last_txid > txid) {
    // a more recent transaction updated the CSR to a newer snapshot. We abort, since
    // 1) transaction with id txid may see updates it shouldn't, and
    // 2) we cannot restore the CSR to an older snapshot 
    throw invalid_csr_update();
  }

  auto &old_row_offs = csr_delta_->row_offsets_;
  auto &old_col_inds = csr_delta_->col_indices_;
  auto &old_edge_vals = csr_delta_->edge_values_;

  auto &new_row_offs = csr.row_offsets;
  auto &new_col_inds = csr.col_indices;
  auto &new_edge_vals = csr.edge_values;

  // restore deltas into a delta map
  // an entry in a delta map is of the form: {node id, <[ids of neighbours], [edge weights]>}
  csr_delta::delta_map_t deltas;
  csr_delta_->restore_deltas(deltas, txid);

  if (deltas.empty()) {
    // nothing to update CSR
    return;
  }

  auto update_edge_delta = 0; // edge delta of existing nodes i.e. from 0 to last node id in the current (i.e. old) CSR
  auto last_node_id = csr_delta_->get_last_node_id();
  const auto it1 = deltas.begin();
  const auto it2 = deltas.upper_bound(last_node_id);
  const auto it3 = deltas.end();

  for (auto iter = it1; iter != it2; ++iter) {
    auto &d = *iter;
    int old_edges = old_row_offs[d.first + 1] - old_row_offs[d.first];
    int new_edges = d.second.first.size();
    auto diff = new_edges - old_edges;
    update_edge_delta += diff;
  }

  auto append_node_delta = 0; // number of newly appended nodes after the last node id in the current (i.e. old) CSR
  auto append_edge_delta = 0; // edge delta of newly appended nodes
  for (auto iter = it2; iter != it3; ++iter) {
    auto &d = *iter;
    append_node_delta++;
    append_edge_delta += d.second.first.size();
  }

  uint64_t new_row_offs_size = old_row_offs.size() + append_node_delta;
  uint64_t new_col_inds_size = old_col_inds.size() + update_edge_delta + append_edge_delta;

  new_row_offs.reserve(new_row_offs_size);
  new_col_inds.reserve(new_col_inds_size);
  new_edge_vals.reserve(new_col_inds_size);

  // process entries for the first node until the last updated node
  uint64_t next_id = 0;
  new_row_offs.push_back(0);
  for (auto iter = it1; iter != it2; ++iter) { // TODO potential optimization using next_id and prev_id
    auto &d = *iter;
    while (next_id < d.first) {
      // possibly, some nodes between the first node and the last updated node are unchanged
      // we just copy the entries for such nodes from the current (i.e. old) CSR to the new CSR 
      auto edges = old_row_offs[next_id + 1] - old_row_offs[next_id];
      new_row_offs.push_back(new_row_offs.back() + edges);

      auto beg_col_inds_iter = old_col_inds.begin() + old_row_offs[next_id];
      auto end_col_inds_iter = beg_col_inds_iter + edges;
      new_col_inds.insert(new_col_inds.end(), beg_col_inds_iter, end_col_inds_iter);

      auto beg_edge_vals_iter = old_edge_vals.begin() + old_row_offs[next_id];
      auto end_edge_vals_iter = beg_edge_vals_iter + edges;
      new_edge_vals.insert(new_edge_vals.end(), beg_edge_vals_iter, end_edge_vals_iter);

      next_id++;
    }
    // assert(next_id == d.first);
    // using the delta map item, insert entries for the updated node into the new CSR
    new_row_offs.push_back(new_row_offs.back() + d.second.first.size());
    new_col_inds.insert(new_col_inds.end(), d.second.first.begin(), d.second.first.end());
    new_edge_vals.insert(new_edge_vals.end(), d.second.second.begin(), d.second.second.end());
    next_id++;
  }

  // possibly, some nodes between the last updated node and the last node id in the current (i.e. old) CSR are unchanged
  // similarly, we just copy the entries for such nodes from the current (i.e. old) CSR to the new CSR 
  new_col_inds.insert(new_col_inds.end(), old_col_inds.begin() + old_row_offs[next_id], old_col_inds.end());
  new_edge_vals.insert(new_edge_vals.end(), old_edge_vals.begin() + old_row_offs[next_id], old_edge_vals.end());
  while (next_id < (old_row_offs.size() - 1)) {
    auto edges = old_row_offs[next_id + 1] - old_row_offs[next_id];
    new_row_offs.push_back(new_row_offs.back() + edges);
    next_id++;
  }

  // finally, using the remaining delta map items, insert entries for the newly appended nodes into the new CSR
  for (auto iter = it2; iter != it3; ++iter) {
    auto &d = *iter;
    while (next_id < d.first) { // unused node record slots
      new_row_offs.push_back(new_row_offs.back());
      next_id++;
    }
    // assert(next_id == d.first);
    new_row_offs.push_back(new_row_offs.back() + d.second.first.size());
    new_col_inds.insert(new_col_inds.end(), d.second.first.begin(), d.second.first.end());
    new_edge_vals.insert(new_edge_vals.end(), d.second.second.begin(), d.second.second.end());
    next_id++;
  }

  csr_delta_->last_txn_id_ = txid; // update id of the last transaction that made a CSR update
  csr_delta_->last_node_id_ = new_row_offs.size() - 2; // update last node id in the CSR

  // TODO this is not needed when CSR update is done directly on GPU
  csr_delta_->row_offsets_ = new_row_offs;
  csr_delta_->col_indices_ = new_col_inds;
  csr_delta_->edge_values_ = new_edge_vals;
}
#endif

void graph_db::poseidon_to_coo(edge_coords* edge_coordinates, float* edge_values, rship_weight weight_func, bool bidirectional) {
  chunked_vec<relationship, RSHIP_CHUNK_SIZE> &cv_rsips = rships_->as_vec();
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