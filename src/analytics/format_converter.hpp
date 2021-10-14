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

#include "graph_db.hpp"
#include <vector>
#include <chrono> // for elapsed time measurement

/**
 * Typedef for a predicate to check that a relationship is followed via the search.
 */
using rship_predicate = std::function<bool(relationship&)>;

/**
 * Typedef for a function that computes the weight of a relationship.
 */
using rship_weight = std::function<float(relationship&)>;

/*
 * Struct used to store edge-coordinates in COO format
 * Needs to be allocated 16-bit alligned!
 */
struct edge_coords {
    node::id_t x, y;
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
inline void poseidon_to_csr(graph_db_ptr gdb, bool bidirectional, rship_predicate rpred, rship_weight weight_func, 
                std::vector<uint64_t>* row_offsets, std::vector<uint64_t>* col_indices, std::vector<float>* edge_values, 
                uint64_t* num_nodes, uint64_t* num_edges) {
    offset_t edges = 0;
    chunked_vec<node, NODE_CHUNK_SIZE> &cv_nodes = gdb->get_nodes()->as_vec();
    offset_t max_num_nodes = cv_nodes.capacity();
    offset_t max_num_edges = gdb->get_relationships()->as_vec().capacity();
    
    // Reserve enough memory. Saves runtime
    int multi = bidirectional ? 1 : 2;
    row_offsets->reserve(max_num_nodes + 1);
    col_indices->reserve(multi * max_num_edges);
    edge_values->reserve(multi * max_num_edges);
    
    row_offsets->push_back(0); // First value is always 0

    // Loop over the complete nodes chunked vector.
    // There might exist a better solution for very sparse 
    // chunked vectors (a lot of deleted entries). 
    for (offset_t nid = 0; nid < max_num_nodes; nid++) {
        if (cv_nodes.is_used(nid)) {
            auto& n = gdb->node_by_id(nid);

            gdb->foreach_from_relationship_of_node(n, [&](auto &r) {
                if (rpred(r)) {
                    col_indices->push_back(r.to_node_id());
                    edge_values->push_back(weight_func(r));
                    edges++;
                } // IF rpred
            }); // foreach

            if (bidirectional) {
                gdb->foreach_to_relationship_of_node(n, [&](auto &r) {
                    if (rpred(r)) {
                        col_indices->push_back(r.from_node_id());
                        edge_values->push_back(weight_func(r));
                        edges++;
                    } // if rpred
                }); // foreach
            } // if bidirectional
        } // if is_used
        row_offsets->push_back(edges);


    } // Outer for loop

    *num_nodes = max_num_nodes; 
    *num_edges = edges;
}

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
