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

#include "gunrock_pr.hpp"

uint64_t gunrock_pr_csr(graph_db_ptr gdb, bool bidirectional, pr_result &result, bool quiet) {

  csr_arrays csr;
  auto t1 = std::chrono::steady_clock::now();
  gdb->run_transaction([&]() {
    gdb->poseidon_to_csr(csr, gdb->get_csr_delta()->get_weight_func(), bidirectional);
    return true;
  });
  auto t2 = std::chrono::steady_clock::now();

  offset_t num_nodes = gdb->get_nodes()->as_vec().last_used() + 1;
  offset_t num_edges = gdb->get_relationships()->as_vec().last_used() + 1;

  // Allocate memory for Gunrock output
  offset_t *nids = (offset_t *)malloc(sizeof(off64_t) * num_nodes);
  float *ranks = (float *)malloc(sizeof(float) * num_nodes);

  // Custom-written Poseidon function for Gunrock
  // add this to Gunrock before compiling Gunrock in order to use it!
  double exec_time = poseidon_gunrock_pr(num_nodes, num_edges, (unsigned long long*)csr.row_offsets.data(),
                        (unsigned long long*)csr.col_indices.data(), true, (unsigned long long*)nids, (float *)ranks);

  result.set_result(ranks, nids, num_nodes);
  free(nids);
  free(ranks);

  auto t3 = std::chrono::steady_clock::now();

  if (!quiet) { // For performance analysis
    std::cout << "Executed PR using Gunrock with CSR graph representation. \n";
#if defined CSR_DELTA_STORE && defined USE_TX
    std::cout << "Full CSR build:         " << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << "[ms]" << "\n";
#else
    std::cout << "CSR update with delta:         " << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << "[ms]" << "\n";
#endif
    std::cout << "Execution: " << std::chrono::duration_cast<std::chrono::milliseconds>(t3 - t2).count() << "[ms]" << "\n";
    std::cout << "Execution (measurement in Gunrock): " << exec_time << "[ms]" << "\n";
    std::cout << "Total Elapsed time:    " << std::chrono::duration_cast<std::chrono::milliseconds>(t3 - t1).count() << "[ms]" << "\n";
  }

  return std::chrono::duration_cast<std::chrono::milliseconds>(t3 - t1).count();
}