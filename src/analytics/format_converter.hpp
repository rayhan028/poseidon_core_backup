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
 * Typedef for a predicate to check that a relationship is followed via the search.
 */
using rship_predicate = std::function<bool(relationship&)>;

/**
 * Typedef for a function that computes the weight of a relationship.
 */
using rship_weight = std::function<double(relationship&)>;

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
inline void poseidon_to_csr(graph_db_ptr gdb,
    std::vector<uint64_t> &row_offsets, std::vector<uint64_t> &col_indices,
    std::vector<float> &edge_values, rship_weight weight_func, bool bidirectional = false) {

  offset_t edges = 0;
  offset_t max_num_edges = gdb->get_relationships()->as_vec().last_used() + 1;
  auto &cv_nodes = gdb->get_nodes()->as_vec();
  offset_t max_num_nodes = cv_nodes.last_used() + 1;

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
}

inline void parallel_poseidon_to_csr(graph_db_ptr gdb,
    std::vector<uint64_t> &row_offsets, std::vector<uint64_t> &col_indices,
    std::vector<float> &edge_values, rship_weight weight_func, bool bidirectional = false) {


  offset_t max_num_nodes = gdb->get_nodes()->as_vec().last_used() + 1;
  offset_t max_num_edges = gdb->get_relationships()->as_vec().last_used() + 1;
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
}

inline void update_csr_with_delta(graph_db_ptr graph,
    std::vector<uint64_t> &old_row_offs, std::vector<uint64_t> &old_col_inds,
    std::vector<float> &old_edge_vals, std::vector<uint64_t> &new_row_offs,
    std::vector<uint64_t> &new_col_inds, std::vector<float> &new_edge_vals) {

  csr_delta::delta_map_t update_deltas;
  csr_delta::delta_map_t append_deltas;

  graph->get_csr_delta()->restore_deltas(update_deltas, append_deltas);

  auto update_edge_delta = 0; /// edge delta of existing nodes
  for (auto &d : update_deltas) {
    int old_edges = old_row_offs[d.first + 1] - old_row_offs[d.first];
    int new_edges = d.second.first.size();
    auto diff = new_edges - old_edges;
    update_edge_delta += diff;
  }
  auto append_edge_delta = 0; /// edge delta of newly appended nodes (no record slot reuse)
  for (auto &d : append_deltas)
    append_edge_delta += d.second.first.size();

  uint64_t new_row_offs_size = old_row_offs.size() + append_deltas.size();
  uint64_t new_col_inds_size = old_col_inds.size() + update_edge_delta + append_edge_delta;
  new_row_offs.reserve(new_row_offs_size);
  new_col_inds.reserve(new_col_inds_size);
  new_edge_vals.reserve(new_col_inds_size);

  /// entries for the first node until the last updated
  uint64_t next_id = 0;
  new_row_offs.push_back(0);
  for (auto &d : update_deltas) { // TODO potential optimization using next_id and prev_id
    while (next_id < d.first) { /// unchanged nodes
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
    new_row_offs.push_back(new_row_offs.back() + d.second.first.size());
    new_col_inds.insert(new_col_inds.end(), d.second.first.begin(), d.second.first.end());
    new_edge_vals.insert(new_edge_vals.end(), d.second.second.begin(), d.second.second.end());
    next_id++;
  }

  /// entries for remaining unchanged nodes
  new_col_inds.insert(new_col_inds.end(), old_col_inds.begin() + old_row_offs[next_id], old_col_inds.end());
  new_edge_vals.insert(new_edge_vals.end(), old_edge_vals.begin() + old_row_offs[next_id], old_edge_vals.end());
  while (next_id < (old_row_offs.size() - 1)) {
    auto edges = old_row_offs[next_id + 1] - old_row_offs[next_id];
    new_row_offs.push_back(new_row_offs.back() + edges);
    next_id++;
  }

  /// entries for newly appended nodes (no record reuse)
  for (auto &d : append_deltas) {
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
