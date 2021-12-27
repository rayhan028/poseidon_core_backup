/*
 * Copyright (C) 2019-2021 DBIS Group - TU Ilmenau, All Rights Reserved.
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

#ifndef format_converter_hpp_
#define format_converter_hpp_

#include "query.hpp"
#include <vector>
#include <chrono> // for elapsed time measurement

/**
 * A wrapper struct for CSR arrays 
 */
struct csr_arrays {
  csr_arrays() = default;
  csr_arrays(const csr_arrays &) = delete;

  std::vector<offset_t> row_offsets = {};
  std::vector<offset_t> col_indices = {};
  std::vector<float> edge_values = {};
};

/**
 * Converts a given Poseidon Graph to a CSR representation, including all relationships satisfying the
 * predicate rpred. The weight of a traversed relationship is calculated from the weight function. The 
 * bidirectional flag determines whether only outgoing relationships are considered (bidirectional = false) 
 * or both outgoing and incoming relationships (bidirectional = true).
 *
 * Input:
 *   gdb           -> Pointer to Poseidon Graph Database
 *   bidirectional -> Set true to treat relationships bidirectionally
 *   rpred         -> Function returning a bool for each relationship
 *   weight_func   -> Function returning a weight for each relationship
 * Output: 
 *   row_offsets   -> CSR-formatted graph row offsets, to be allocated in advance
 *   col_indices   -> CSR-formatted graph column indices, to be allocated in advance
 *   edge_values   -> CSR-formatted graph edge weights, to be allocated in advance
 *   num_nodes     -> Number of veritces in the input graph. Note that unused slots in
 *                    chunked_vectors are considered here as well! Sparsely populated
 *                    chunked_vectors will lead to unneccesary runtime growth. 
 *   num_edges     -> Number of edges in the input graph
 */
inline void csr_build(graph_db_ptr gdb, csr_arrays &csr,
              rship_weight weight_func, bool bidirectional = false) {

  offset_t edges = 0;
  offset_t max_num_edges = gdb->get_relationships()->as_vec().last_used() + 1;
  auto &cv_nodes = gdb->get_nodes()->as_vec();
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
      auto& n = gdb->node_by_id(nid);

      gdb->foreach_from_relationship_of_node(n, [&](auto &r) {
        col_indices.push_back(r.to_node_id());
        edge_values.push_back(weight_func(r));
        edges++;
      });

      if (bidirectional) {
        gdb->foreach_to_relationship_of_node(n, [&](auto &r) {
          col_indices.push_back(r.from_node_id());
          edge_values.push_back(weight_func(r));
          edges++;
        });
      }
    }
    row_offsets.push_back(edges);
  }
#if defined CSR_DELTA_STORE && defined USE_TX
  auto txid = current_transaction()->xid();
  auto &delta = gdb->get_csr_delta();
  delta->set_last_txn_id(txid); // update id of the last transaction that made a CSR update
  delta->set_last_node_id(row_offsets.size() - 2); // update last node id in the CSR

  bool clear = true;
  for (auto &elem : delta->get_delta_elements()) {
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
    delta->get_delta_elements().clear();
  }

  // TODO this is not needed when CSR update is done directly on GPU
  delta->set_row_offs(row_offsets);
  delta->set_col_inds(col_indices);
  delta->set_edge_vals(edge_values);
#endif
}

inline void parallel_csr_build(graph_db_ptr gdb, csr_arrays &csr,
                      rship_weight weight_func, bool bidirectional = false) {

  offset_t max_num_nodes = gdb->get_nodes()->as_vec().last_used() + 1;
  offset_t max_num_edges = gdb->get_relationships()->as_vec().last_used() + 1;

  auto &row_offsets = csr.row_offsets;
  auto &col_indices = csr.col_indices;
  auto &edge_values = csr.edge_values;

  int multi = bidirectional ? 1 : 2;

  row_offsets.reserve(max_num_nodes + 1);
  col_indices.reserve(multi * max_num_edges);
  edge_values.reserve(multi * max_num_edges);

  row_offsets.push_back(0); // First value is always 0

  result_set rs;
  auto q = query(gdb)
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
  auto &delta = gdb->get_csr_delta();
  delta->set_last_txn_id(txid); // update id of the last transaction that made a CSR update
  delta->set_last_node_id(row_offsets.size() - 2); // update last node id in the CSR

  bool clear = true;
  for (auto &elem : delta->get_delta_elements()) {
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
    delta->get_delta_elements().clear();
  }

  // TODO this is not needed when CSR update is done directly on GPU
  delta->set_row_offs(row_offsets);
  delta->set_col_inds(col_indices);
  delta->set_edge_vals(edge_values);
#endif
}

#if defined CSR_DELTA_STORE && defined USE_TX
inline void csr_update_with_delta(graph_db_ptr graph, csr_arrays &csr) {
  auto tx = current_transaction();
  if (!tx)
    throw out_of_transaction_scope();

  auto &delta = graph->get_csr_delta();
  auto last_txid = delta->get_last_txn_id();
  auto txid = tx->xid();

  if (last_txid == UNKNOWN) {
    // no CSR exists yet for the delta, so we make the initial CSR build
#ifdef PARALLEL_CSR_BUILD
    parallel_csr_build(gdb, csr, delta->get_weight_func(), delta->get_bidirectional());
#else
    csr_build(graph, csr, delta->get_weight_func(), delta->get_bidirectional());
#endif
    return;
  }
  else if (last_txid > txid) {
    // a more recent transaction updated the CSR to a newer snapshot. We abort, since
    // 1) transaction with id txid may see updates it shouldn't, and
    // 2) we cannot restore the CSR to an older snapshot 
    throw invalid_csr_update();
  }

  auto &old_row_offs = delta->get_row_offs();
  auto &old_col_inds = delta->get_col_inds();
  auto &old_edge_vals = delta->get_edge_vals();

  auto &new_row_offs = csr.row_offsets;
  auto &new_col_inds = csr.col_indices;
  auto &new_edge_vals = csr.edge_values;

  // restore deltas into a delta map
  // an entry in a delta map is of the form: {node id, <[ids of neighbours], [edge weights]>}
  csr_delta::delta_map_t deltas;
  delta->restore_deltas(deltas, txid);

  auto update_edge_delta = 0; // edge delta of existing nodes i.e. from 0 to last node id in the current (i.e. old) CSR
  auto last_node_id = delta->get_last_node_id();
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

  delta->set_last_txn_id(txid); // update id of the last transaction that made a CSR update
  delta->set_last_node_id(new_row_offs.size() - 2); // update last node id in the CSR

  // TODO this is not needed when CSR update is done directly on GPU
  delta->set_row_offs(new_row_offs);
  delta->set_col_inds(new_col_inds);
  delta->set_edge_vals(new_edge_vals);
}
#endif

inline void poseidon_to_csr(graph_db_ptr gdb, csr_arrays &csr,
              rship_weight weight_func, bool bidirectional = false) {
#if defined CSR_DELTA_STORE && defined USE_TX
  auto &delta = gdb->get_csr_delta();
  delta->set_weight_func(weight_func);
  delta->set_bidirectional(bidirectional);
  csr_update_with_delta(gdb, csr);
#elif defined PARALLEL_CSR_BUILD
  parallel_csr_build(gdb, csr, weight_func, bidirectional);
#else
  csr_build(gdb, csr, weight_func, bidirectional);
#endif
}

/*
 * Struct used to store edge-coordinates in COO format
 * Needs to be allocated 16-bit alligned!
 */
struct edge_coords {
  node::id_t x, y;
};

/**
 * Converts a given Poseidon Graph to a COO representation, including all relationships satisfying the
 * predicate rpred. The weight of a traversed relationship is calculated from the weight function. The 
 * bidirectional flag determines whether only outgoing relationships are considered (bidirectional = false) 
 * or both outgoing and incoming relationships (bidirectional = true).
 *
 * Input:
 *   gdb              -> Pointer to Poseidon Graph Database
 *   bidirectional    -> Set true to treat relationships bidirectionally
 *   rpred            -> Function returning a bool for each relationship
 *   weight_func      -> Function returning a weight for each relationship
 * Output: 
 *   edge_coordinates -> COO-formatted graph edge coordinates, to be allocated in advance
 *   edge_values      -> COO-formatted graph edge weights, to be allocated in advance
 *   num_nodes        -> Number of veritces in the input graph. Note that unused slots in
 *                       chunked_vectors are considered here as well! 
 *   num_edges        -> Number of edges in the input graph
 */
inline void poseidon_to_coo(graph_db_ptr gdb, bool bidirectional, rship_predicate rpred, rship_weight weight_func, 
                edge_coords* edge_coordinates, float* edge_values, uint64_t* num_nodes, uint64_t* num_edges){
  chunked_vec<relationship, RSHIP_CHUNK_SIZE> &cv_rsips = gdb->get_relationships()->as_vec();
  auto iter = cv_rsips.begin();
  auto last = cv_rsips.end();
  unsigned long long edges = 0;

  // Iterates over relationship list
  while (iter != last) {
    relationship r = (*iter);
    if (rpred(r)) {
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
    } // if rpred
    ++iter;
  } // Outer while loop

  *num_nodes = (uint64_t) gdb->get_nodes()->as_vec().capacity();
  *num_edges = (uint64_t) edges;
}

#endif
