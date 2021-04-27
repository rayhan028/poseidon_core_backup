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

#ifndef SSSP_gunrock_hpp_
#define SSSP_gunrock_hpp_

#include "graph_db.hpp"
#include "gunrock.h"
#include <vector>
#include <boost/heap/fibonacci_heap.hpp> // used by dijkstra (sequential SSSP)
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
 * Struct used for efficient sequential weighted SSSP
 */
struct nodeFibHeap{
    node::id_t nodeID;
    float distance;

    nodeFibHeap(node::id_t ID, float dist){
        nodeID = ID;
        distance = dist;
    }
};

/*
 * Struct used for efficient sequential weighted SSSP
 * https://stackoverflow.com/questions/12698431/boost-fibonacci-heap-decrease-operation
 * https://stackoverflow.com/questions/16705894/defining-compare-function-for-fibonacci-heap-in-boost
 */
struct compare_nodes{
    bool operator()(const nodeFibHeap& n1, const nodeFibHeap& n2) const
    {
        return n1.distance > n2.distance;
    }
};

/**
 * Struct containing the results of SSSP
 */
struct SSSP_result {
    SSSP_result(){};

    /**
     * Returns the predecessor of a given node. Returns 
     * UNKNOWN for invalid or non-reachable nodes.
    **/
    offset_t getPredecessor(offset_t nodeID){
        if(nodeID <= max_index_nodes){
            return offset_t(predecessors[nodeID]);
        } else {
            return UNKNOWN;
        }
    }

    /**
     * Returns the predecessor of a given node. Returns 
     * UNKNOWN for invalid or non-reachable nodes.
    **/
    float getDistance(offset_t nodeID){
        if(nodeID <= max_index_nodes){
            return distances[nodeID];
        } else {
            return UNKNOWN;
        }
    }

    /**
     * Setter function for the private variables.
     * Arrays dist and preds must be of length max_index_nodes!
    **/
    void setResult(float* dist, uint64_t* preds, uint64_t max_index_nodes){
        distances = std::vector<float> (dist, dist+max_index_nodes);
        predecessors = std::vector<uint64_t> (preds, preds+max_index_nodes);
        this->max_index_nodes = max_index_nodes;
    }

private:
    std::vector<float> distances;
    std::vector<uint64_t> predecessors;
    uint64_t max_index_nodes;
};

/*
 * Struct used to store edge-coordinates in COO format
 * Needs to be allocated 16-bit alligned!
 */
struct edge_coords{
    unsigned long long x,y;
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
 *   weight_func   -> Function returning a float weight for each relationship
 * Output: 
 *   row_offsets   -> CSR-formatted graph row offsets, to be allocated in advance
 *   col_indices   -> CSR-formatted graph column indices, to be allocated in advance
 *   edge_values   -> CSR-formatted graph edge weights, to be allocated in advance
 *   num_nodes     -> Number of veritces in the input graph. Note that unused slots in
 *                    chunked_vectors are considered here as well! Sparsely populated
 *                    chunked_vectors will lead to unneccesary runtime growth. 
 *   num_edges     -> Number of edges in the input graph
 */
void poseidonToCSR(graph_db_ptr gdb, bool bidirectional, rship_predicate rpred, rship_weight weight_func, 
                std::vector<uint64_t>* row_offsets, std::vector<uint64_t>* col_indices, std::vector<float>* edge_values, 
                uint64_t* num_nodes, uint64_t* num_edges);

/**
 * An implementation of weighted SSSP leveraging the GPU-Library Gunrock, using CSR graph representation. The search 
 * starts at the given start node and follows all relationships satisfying the predicate rpred. The weight of a traversed 
 * relationship is calculated from the weight function. The bidirectional flag determines whether only outgoing relationships 
 * are considered (bidirectional = false) or both outgoing and incoming relationships (bidirectional = true).
 *
 * Input:
 *   gdb           -> Pointer to Poseidon Graph Database
 *   start         -> Source node to begin traverse
 *   bidirectional -> Set true to treat relationships bidirectionally
 *   rpred         -> Function returning a bool for each relationship
 *   weight_func   -> Function returning a float weight for each relationship
 *   quiet         -> Set true to mute std::cout outputs during execution
 * Output: 
 *   spath         -> A struct containing shortest path information
 * Return value:
 *   Total elapsed time in ms, measured with std::chrono
 */
int64_t weighted_SSSP_gunrock_CSR(graph_db_ptr gdb, node::id_t start, bool bidirectional,
                rship_predicate rpred, rship_weight weight_func, SSSP_result &result, bool quiet);

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
 *   weight_func      -> Function returning a float weight for each relationship
 * Output: 
 *   edge_coordinates -> COO-formatted graph edge coordinates, to be allocated in advance
 *   edge_values      -> COO-formatted graph edge weights, to be allocated in advance
 *   num_nodes        -> Number of veritces in the input graph. Note that unused slots in
 *                       chunked_vectors are considered here as well! 
 *   num_edges        -> Number of edges in the input graph
 */
void poseidonToCOO(graph_db_ptr gdb, bool bidirectional, rship_predicate rpred, rship_weight weight_func, 
                edge_coords* edge_coordinates, float* edge_values, uint64_t* num_nodes, uint64_t* num_edges);

/**
 * An implementation of weighted SSSP leveraging the GPU-Library Gunrock, using COO graph representation. The search 
 * starts at the given start node and follows all relationships satisfying the predicate rpred. The weight of a traversed 
 * relationship is calculated from the weight function. The bidirectional flag determines whether only outgoing relationships 
 * are considered (bidirectional = false) or both outgoing and incoming relationships (bidirectional = true).
 *
 * Input:
 *   gdb           -> Pointer to Poseidon Graph Database
 *   start         -> Source node to begin traverse
 *   bidirectional -> Set true to treat relationships bidirectionally
 *   rpred         -> Function returning a bool for each relationship
 *   weight_func   -> Function returning a float weight for each relationship
 *   quiet         -> Set true to mute std::cout outputs during execution
 * Output: 
 *   spath         -> A struct containing shortest path information
 * Return value:
 *   Total elapsed time in ms, measured with std::chrono
 */
int64_t weighted_SSSP_gunrock_COO(graph_db_ptr gdb, node::id_t start, bool bidirectional,
                rship_predicate rpred, rship_weight weight_func, SSSP_result &result, bool quiet);

/**
 * A sequential implementation of weighted SSSP on the given graph, implementing Dijkstra with Fibonacci-Heap. 
 * The search starts at the given start node and follows all relationships satisfying the predicate rpred. The 
 * weight of a traversed relationship is calculated from the weight function. The bidirectional flag determines 
 * whether only outgoing relationships are considered (bidirectional = false) or both outgoing and incoming 
 * relationships (bidirectional = true).
 *
 * Input:
 *   gdb           -> Pointer to Poseidon Graph Database
 *   start         -> Source node to begin traverse
 *   bidirectional -> Set true to treat relationships bidirectionally
 *   rpred         -> Function returning a bool for each relationship
 *   weight_func   -> Function returning a float weight for each relationship
 *   quiet         -> Set true to mute std::cout outputs during execution
 * Output: 
 *   spath         -> A struct containing shortest path information
 * Return value:
 *   Total elapsed time in ms, measured with std::chrono
 */
int64_t weighted_SSSP_sequential(graph_db_ptr gdb, node::id_t start, bool bidirectional,
                rship_predicate rpred, rship_weight weight_func, SSSP_result &result, bool quiet);

#endif
