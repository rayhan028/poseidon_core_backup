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

#ifndef gunrock_bfs_hpp_
#define gunrock_bfs_hpp_

#include "graph_db.hpp"
#include "gunrock.h"
#include "format_converter.hpp"
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

/**
 * Struct containing the results of SSSP
 */
struct bfs_result {

    bfs_result() {};

    /**
     * Setter function for the private variables.
     * Arrays dist and preds must be of length max_index_nodes!
    **/
    void set_result(int* dists, offset_t* preds, uint64_t max_nid) {
        distances = std::vector<int>(dists, dists + max_nid);
        predecessors = std::vector<offset_t>(preds, preds + max_nid);
        max_node_idx = max_nid;
    }

    /**
     * Returns the predecessor of a given node. Returns 
     * UNKNOWN for invalid or non-reachable nodes.
    **/
    offset_t get_predecessor(offset_t nid) {
        return nid < max_node_idx ? predecessors[nid] : UNKNOWN;
    }

    /**
     * Returns the predecessor of a given node. Returns 
     * UNKNOWN for invalid or non-reachable nodes.
    **/
    int get_distance(offset_t nid) {
        return nid < max_node_idx ? distances[nid] : UNKNOWN;
    }

private:
    uint64_t max_node_idx;
    std::vector<int> distances;
    std::vector<offset_t> predecessors;
};

/**
 * An implementation of BFS leveraging the GPU-Library Gunrock, using CSR graph representation. 
 * The search starts from the given start node and follows all edges (relationships) satisfying 
 * the predicate rpred. For each visited node, the node_visitor callback is invoked. The 
 * unidirectional flag determines whether only outgoing relationships are considered 
 * (unidirectional = false) or both outgoing and incoming relationships (unidirectional = true).
 * Setting quiet to true mutes std::cout outputs during execution. The traversal info is stored
 * in the result struct.
 */
uint64_t gunrock_bfs_csr(graph_db_ptr gdb, node::id_t start, bool bidirectional,
                rship_predicate rpred, bfs_result &result, bool quiet);

#endif