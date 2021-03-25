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

#include "SSSP_gunrock.hpp"

void graph_PoseidonToCSR(graph_db_ptr gdb, bool bidirectional, rship_predicate rpred, rship_weight weight_func, 
                std::vector<uint64_t>* row_offsets, std::vector<uint64_t>* col_indices, std::vector<float>* edge_values, 
                uint64_t* num_nodes, uint64_t* num_edges){
    int edgeID = 0;
    chunked_vec<node, NODE_CHUNK_SIZE> &cv_nodes = gdb->get_nodes()->as_vec();
    offset_t max_num_nodes = cv_nodes.capacity();
    offset_t max_num_edges = gdb->get_relationships()->as_vec().capacity();
    
    // Reserve enough memory. Saves runtime
    int multi = 1;
    if(bidirectional){multi = 2;} 
    row_offsets->reserve(max_num_nodes+1);
    col_indices->reserve(multi*max_num_edges);
    edge_values->reserve(multi*max_num_edges);
    
    row_offsets->push_back(0); // First value is always 0

    // Loop over the complete nodes chunked vector.
    // There might exist a better solution for very sparse 
    // chunked vectors (a lot of deleted entries). 
    for(offset_t nodeID = 0; nodeID < max_num_nodes; nodeID++){
        if(cv_nodes.is_used(nodeID)){
            auto& n = gdb->node_by_id(nodeID);

            gdb->foreach_from_relationship_of_node(n, [&](auto &r) {
                if (rpred(r)) {
                    col_indices->push_back(int(r.to_node_id()));
                    edge_values->push_back(weight_func(r));
                    edgeID++;
                } // IF rpred
            }); // foreach

            if(bidirectional){
                gdb->foreach_to_relationship_of_node(n, [&](auto &r) {
                    if (rpred(r)) {
                        col_indices->push_back(r.from_node_id());
                        edge_values->push_back(weight_func(r));
                        edgeID++;
                    } // if rpred
                }); // foreach
            } // if bidirectional
        } // if is_used
        row_offsets->push_back(edgeID);


    } // Outer for loop

    *num_nodes = max_num_nodes; 
    *num_edges = edgeID;
}

int64_t weighted_SSSP_gunrock_CSR(graph_db_ptr gdb, node::id_t start, bool bidirectional,
                rship_predicate rpred, rship_weight weight_func, SSSP_result &result, bool quiet) {

    std::chrono::steady_clock::time_point start_conversion = std::chrono::steady_clock::now();

    uint64_t max_index_nodes = 0;
    uint64_t num_edges = 0;
    std::vector<uint64_t> row_offsets = {};
    std::vector<uint64_t> col_indices = {};
    std::vector<float> edge_values = {};
    
    graph_PoseidonToCSR(gdb, bidirectional, rpred, weight_func, &row_offsets, &col_indices, &edge_values, &max_index_nodes, &num_edges);

    std::chrono::steady_clock::time_point end_conversion = std::chrono::steady_clock::now();

    // Allocate memory for gunrock output
    float *dist = (float *)malloc(sizeof(float) * max_index_nodes);
    uint64_t *preds = (uint64_t *)malloc(sizeof(uint64_t) * max_index_nodes);

    // Using a custom written function within Gunrock. You need to 
    // add this to gunrock before compiling Gunrock in order to use it!
    gunrock_sssp_CSR(max_index_nodes, num_edges, (unsigned long long*) row_offsets.data(), (unsigned long long*) col_indices.data(), 
                    edge_values.data(), start, true, dist, (unsigned long long*) preds);

    // Conversion to our output format
    result.setResult(dist, preds, max_index_nodes);

    free(dist);
    free(preds);

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    if(!quiet){ // For performance analysis
        std::cout << "Executed SSSP using Gunrock with CSR graph representation.\n";
        std::cout << "Elapsed time conversion to CSR: " << std::chrono::duration_cast<std::chrono::milliseconds>(end_conversion - start_conversion).count() << "[ms]" << "\n";
        std::cout << "Elapsed time gunrock:           " << std::chrono::duration_cast<std::chrono::milliseconds>(end - end_conversion).count() << "[ms]" << "\n";
        std::cout << "Total Elapsed time:             " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start_conversion).count() << "[ms]" << "\n";
    }
    return std::chrono::duration_cast<std::chrono::milliseconds>(end - start_conversion).count();
}

typedef chunked_vec<relationship, RSHIP_CHUNK_SIZE> chunked_vec_relationships_t;
void graph_PoseidonToCOO(graph_db_ptr gdb, bool bidirectional, rship_predicate rpred, rship_weight weight_func, 
                edge_coords* edge_coordinates, float* edge_values, uint64_t* num_nodes, uint64_t* num_edges){
    chunked_vec_relationships_t &cV_rels = gdb->get_relationships()->as_vec();
    chunked_vec_relationships_t::iter cV_rels_iter = cV_rels.begin();
    chunked_vec_relationships_t::iter cV_rels_iter_last = cV_rels.end();
    unsigned long long edgeIndex = 0;

    // Iterates over relationship list
    while(cV_rels_iter != cV_rels_iter_last){
        relationship r = (*cV_rels_iter);
        if (rpred(r)){
            edge_coordinates[edgeIndex].x = r.from_node_id();
            edge_coordinates[edgeIndex].y = r.to_node_id();
            edge_values[edgeIndex] = weight_func(r);
            edgeIndex++;
            if(bidirectional){
                edge_coordinates[edgeIndex].x = r.to_node_id();
                edge_coordinates[edgeIndex].y = r.from_node_id();
                edge_values[edgeIndex] = weight_func(r);
                edgeIndex++;
            } // if bidirectional
        } // if rpred
        ++cV_rels_iter;
    } // Outer while loop

    *num_nodes = (uint64_t) gdb->get_nodes()->as_vec().capacity();
    *num_edges = (uint64_t) edgeIndex;
}

int64_t weighted_SSSP_gunrock_COO(graph_db_ptr gdb, node::id_t start, bool bidirectional,
                rship_predicate rpred, rship_weight weight_func, SSSP_result &result, bool quiet) {

    std::chrono::steady_clock::time_point start_conversion = std::chrono::steady_clock::now();

    offset_t max_index_edges = gdb->get_relationships()->as_vec().capacity();
    int multi = 1;
    if(bidirectional){multi = 2;} 

    // Allocate memory for COO graph representation
    edge_coords* edge_coordinates = (edge_coords*) aligned_alloc(16, multi * max_index_edges * sizeof(edge_coords));
    float* edge_weights = (float *) malloc(sizeof(float) * multi * max_index_edges);

    uint64_t max_index_nodes = 0; // n
    uint64_t num_edges = 0;       // m

    graph_PoseidonToCOO(gdb, bidirectional, rpred, weight_func, edge_coordinates, edge_weights, &max_index_nodes, &num_edges);

    std::chrono::steady_clock::time_point end_conversion = std::chrono::steady_clock::now();

    // Allocate memory for gunrock output
    float *dist = (float *)malloc(sizeof(float) * max_index_nodes);
    unsigned long long *preds = (unsigned long long *)malloc(sizeof(unsigned long long) * max_index_nodes);

    // Using a custom written function within Gunrock. You need to 
    // add this to gunrock before compiling Gunrock in order to use it!
    gunrock_sssp_COO(max_index_nodes, num_edges, edge_coordinates, edge_weights, start, true, dist, preds);
    
    // Conversion to our output format
    result.setResult(dist, (uint64_t*) preds, max_index_nodes);

    free(edge_coordinates);
    free(edge_weights);
    free(dist);
    free(preds);

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    if(!quiet){ // For performance analysis
        std::cout << "Executed SSSP using Gunrock with COO graph representation.\n";
        std::cout << "Elapsed time conversion to COO: " << std::chrono::duration_cast<std::chrono::milliseconds>(end_conversion - start_conversion).count() << "[ms]" << "\n";
        std::cout << "Elapsed time gunrock:           " << std::chrono::duration_cast<std::chrono::milliseconds>(end - end_conversion).count() << "[ms]" << "\n";
        std::cout << "Total Elapsed time:             " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start_conversion).count() << "[ms]" << "\n";
    }
    return std::chrono::duration_cast<std::chrono::milliseconds>(end - start_conversion).count();
}

typedef typename boost::heap::fibonacci_heap<nodeFibHeap, boost::heap::compare<compare_nodes>>::handle_type handle_t;
int64_t weighted_SSSP_sequential(graph_db_ptr gdb, node::id_t start, bool bidirectional,
                rship_predicate rpred, rship_weight weight_func, SSSP_result &result, bool quiet) {

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    uint64_t max_num_nodes = gdb->get_nodes()->as_vec().capacity();
    float indef = std::numeric_limits<float>::max();
    boost::heap::fibonacci_heap<nodeFibHeap, boost::heap::compare<compare_nodes>> fibHeap;

    // Allocate memory for intermediate results and fib-heap-handle lookup table
    handle_t* nodeID_to_handle = (handle_t*)malloc(sizeof(handle_t) * max_num_nodes);
    float *distances = (float *)malloc(sizeof(float) * max_num_nodes);
    uint64_t *preds = (uint64_t *)malloc(sizeof(uint64_t) * max_num_nodes);

    // Init fibonacci-heap and lookup table
    for(uint64_t i = 0; i < max_num_nodes; i++){
        nodeID_to_handle[i] = fibHeap.push(nodeFibHeap(i,indef));
        distances[i] = indef;
        preds[i] = i;
    }

    fibHeap.increase(nodeID_to_handle[start], nodeFibHeap(start,0));
    preds[start] =UNKNOWN;
    distances[start] = 0;

    // Standard sequential Dijkstra-Algorithm using fib-heap
    while(fibHeap.top().distance != indef){
        nodeFibHeap u = fibHeap.top();
        fibHeap.pop();
        auto uid = u.nodeID;
        distances[uid] = u.distance;

        auto& n = gdb->node_by_id(uid);
        gdb->foreach_from_relationship_of_node(n, [&](auto &r) {
            if(rpred(r)){
                auto vid = r.to_node_id();
                auto uv_weight = weight_func(r);
                auto alternativ = distances[uid] + uv_weight;
                if(alternativ < distances[vid]){
                    distances[vid] = alternativ;
                    preds[vid] = uid;
                    fibHeap.increase(nodeID_to_handle[vid], nodeFibHeap(vid,alternativ));
                }
            }
        });

        if (bidirectional) {
            gdb->foreach_to_relationship_of_node(n, [&](auto &r) {
                if(rpred(r)){
                    auto vid = r.from_node_id();
                    auto uv_weight = weight_func(r);
                    auto alternativ = distances[uid] + uv_weight;
                    if(alternativ < distances[vid]){
                        distances[vid] = alternativ;
                        preds[vid] = uid;
                        fibHeap.increase(nodeID_to_handle[vid], nodeFibHeap(vid,alternativ));
                    }
                }
            });
        }
    }

    // Conversion to our output format
    result.setResult(distances, preds, max_num_nodes);

    free(nodeID_to_handle);
    free(distances);
    free(preds);

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    
    if(!quiet){ // For performance analysis
        std::cout << "Executed SSSP using sequential Dijkstra with fibonacci-heap.\n";
        std::cout << "Total Elapsed time:             " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << "\n";
    }
    return std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
}
