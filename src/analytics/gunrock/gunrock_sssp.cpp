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

#include "gunrock_sssp.hpp"

int64_t gunrock_weighted_sssp_csr(graph_db_ptr gdb, node::id_t start, bool bidirectional,
                                  rship_weight weight_func, sssp_result &result, bool quiet) {

  csr_arrays csr;
  auto start_conversion = std::chrono::steady_clock::now();
  gdb->run_transaction([&]() {
    gdb->poseidon_to_csr(csr, weight_func, bidirectional);
    return true;
  });
  auto end_conversion = std::chrono::steady_clock::now();

  offset_t num_nodes = gdb->get_nodes()->as_vec().last_used() + 1;
  offset_t num_edges = gdb->get_relationships()->as_vec().last_used() + 1;

  // Allocate memory for gunrock output
  float *dist = (float *)malloc(sizeof(float) * num_nodes);
  uint64_t *preds = (uint64_t *)malloc(sizeof(uint64_t) * num_nodes);

  // Using a custom written function within Gunrock. You need to 
  // add this to gunrock before compiling Gunrock in order to use it!
  poseidon_gunrock_sssp_csr(num_nodes, num_edges, (unsigned long long*) csr.row_offsets.data(),
    (unsigned long long*) csr.col_indices.data(), (float *)csr.edge_values.data(), start, true, dist, (unsigned long long*) preds);

  // Conversion to our output format
  result.setResult(dist, preds, num_nodes);

  free(dist);
  free(preds);

  std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

  if(!quiet){ // For performance analysis
    std::cout << "Executed SSSP using Gunrock with CSR graph representation (SSSP_gunrock_CSR). \n";
#if defined CSR_DELTA && defined USE_TX
    std::cout << "CSR update using Delta:         " << std::chrono::duration_cast<std::chrono::milliseconds>(end_conversion - start_conversion).count() << "[ms]" << "\n";
#else
    std::cout << "poseidonToCSR:                  " << std::chrono::duration_cast<std::chrono::milliseconds>(end_conversion - start_conversion).count() << "[ms]" << "\n";
#endif
    std::cout << "Gunrock (SSSP-solver):          " << std::chrono::duration_cast<std::chrono::milliseconds>(end - end_conversion).count() << "[ms]" << "\n";
    std::cout << "Total Elapsed time:             " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start_conversion).count() << "[ms]" << "\n";
  }
  return std::chrono::duration_cast<std::chrono::milliseconds>(end - start_conversion).count();
}

int64_t gunrock_weighted_sssp_coo(graph_db_ptr gdb, node::id_t start, bool bidirectional,
                                  rship_weight weight_func, sssp_result &result, bool quiet) {

  std::chrono::steady_clock::time_point start_conversion = std::chrono::steady_clock::now();

  offset_t max_index_edges = gdb->get_relationships()->as_vec().capacity();
  int multi = 1;
  if(bidirectional){multi = 2;} 

  // Allocate memory for COO graph representation
  edge_coords* edge_coordinates = (edge_coords*) aligned_alloc(16, multi * max_index_edges * sizeof(edge_coords));
  float* edge_weights = (float *) malloc(sizeof(float) * multi * max_index_edges);

  offset_t num_nodes = gdb->get_nodes()->as_vec().last_used() + 1; // n
  offset_t num_edges = gdb->get_relationships()->as_vec().last_used() + 1; // m

  gdb->poseidon_to_coo(edge_coordinates, edge_weights, weight_func, bidirectional);

  std::chrono::steady_clock::time_point end_conversion = std::chrono::steady_clock::now();

  // Allocate memory for gunrock output
  float *dist = (float *)malloc(sizeof(float) * num_nodes);
  unsigned long long *preds = (unsigned long long *)malloc(sizeof(unsigned long long) * num_nodes);

  // Using a custom written function within Gunrock. You need to 
  // add this to gunrock before compiling Gunrock in order to use it!
  poseidon_gunrock_sssp_coo(num_nodes, num_edges, edge_coordinates, (float *)edge_weights,
                            start, true, dist, preds);
  
  // Conversion to our output format
  result.setResult(dist, (uint64_t*) preds, num_nodes);

  free(edge_coordinates);
  free(edge_weights);
  free(dist);
  free(preds);

  std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

  if(!quiet){ // For performance analysis
    std::cout << "Executed SSSP using Gunrock with COO graph representation (SSSP_gunrock_COO). \n";
    std::cout << "poseidonToCOO:                           " << std::chrono::duration_cast<std::chrono::milliseconds>(end_conversion - start_conversion).count() << "[ms]" << "\n";
    std::cout << "Gunrock (GunrockCOOtoCSR + SSSP-solver): " << std::chrono::duration_cast<std::chrono::milliseconds>(end - end_conversion).count() << "[ms]" << "\n";
    std::cout << "Total Elapsed time:                      " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start_conversion).count() << "[ms]" << "\n";
  }
  return std::chrono::duration_cast<std::chrono::milliseconds>(end - start_conversion).count();
}

typedef typename boost::heap::fibonacci_heap<nodeFibHeap, boost::heap::compare<compare_nodes>>::handle_type handle_t;
int64_t weighted_SSSP_sequential(graph_db_ptr gdb, node::id_t start, bool bidirectional,
                                 rship_weight weight_func, sssp_result &result, bool quiet) {

  std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

  uint64_t max_num_nodes = gdb->get_nodes()->as_vec().capacity();
  float undef = std::numeric_limits<float>::max();
  boost::heap::fibonacci_heap<nodeFibHeap, boost::heap::compare<compare_nodes>> fibHeap;

  // Allocate memory for intermediate results and fib-heap-handle lookup table
  handle_t* nodeID_to_handle = (handle_t*)malloc(sizeof(handle_t) * max_num_nodes);
  float *distances = (float *)malloc(sizeof(float) * max_num_nodes);
  uint64_t *preds = (uint64_t *)malloc(sizeof(uint64_t) * max_num_nodes);

  // Init fibonacci-heap and lookup table
  for(uint64_t i = 0; i < max_num_nodes; i++){
      nodeID_to_handle[i] = fibHeap.push(nodeFibHeap(i,undef));
      distances[i] = undef;
      preds[i] = i;
  }

  fibHeap.increase(nodeID_to_handle[start], nodeFibHeap(start,0));
  preds[start] =UNKNOWN;
  distances[start] = 0;

  // Standard sequential Dijkstra-Algorithm using fib-heap
  while(fibHeap.top().distance != undef){
    nodeFibHeap u = fibHeap.top();
    fibHeap.pop();
    auto uid = u.nodeID;
    distances[uid] = u.distance;

    auto& n = gdb->node_by_id(uid);
    gdb->foreach_from_relationship_of_node(n, [&](auto &r) {
      auto vid = r.to_node_id();
      auto uv_weight = weight_func(r);
      auto alternativ = distances[uid] + uv_weight;
      if(alternativ < distances[vid]){
        distances[vid] = alternativ;
        preds[vid] = uid;
        fibHeap.increase(nodeID_to_handle[vid], nodeFibHeap(vid,alternativ));
      }
    });

    if (bidirectional) {
      gdb->foreach_to_relationship_of_node(n, [&](auto &r) {
        auto vid = r.from_node_id();
        auto uv_weight = weight_func(r);
        auto alternativ = distances[uid] + uv_weight;
        if(alternativ < distances[vid]){
          distances[vid] = alternativ;
          preds[vid] = uid;
          fibHeap.increase(nodeID_to_handle[vid], nodeFibHeap(vid,alternativ));
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
    std::cout << "Executed SSSP using sequential Dijkstra with fibonacci-heap (SSSP_sequential_Dijkstra). \n";
    std::cout << "Total Elapsed time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << "\n";
  }
  return std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
}
