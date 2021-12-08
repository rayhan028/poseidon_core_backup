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

#include "gunrock_bfs.hpp"

uint64_t gunrock_bfs_csr(graph_db_ptr gdb, node::id_t start, bool bidirectional,
                     rship_predicate rpred, bfs_result &result, bool quiet) {

    auto t1 = std::chrono::steady_clock::now();

    offset_t num_nodes = gdb->get_nodes()->as_vec().last_used() + 1;
    offset_t num_edges = gdb->get_relationships()->as_vec().last_used() + 1;
    std::vector<offset_t> row_offsets = {};
    std::vector<offset_t> col_indices = {};
    std::vector<float> edge_dists = {};

    poseidon_to_csr(gdb, row_offsets, col_indices, edge_dists, [](auto& r) { return 1; }, bidirectional);

    auto t2 = std::chrono::steady_clock::now();

    // Allocate memory for Gunrock output
    int *dists = (int *)malloc(sizeof(int) * num_nodes);
    offset_t *preds = (offset_t *)malloc(sizeof(off64_t) * num_nodes);

    // Custom-written Poseidon function for Gunrock
    // add this to Gunrock before compiling Gunrock in order to use it!
    // double exec_time = poseidon_gunrock_bfs(num_nodes, num_edges, (unsigned long long*)row_offsets.data(),
    //                      (unsigned long long*)col_indices.data(), start, true, true, true,
    //                      dists, (unsigned long long*)preds);

    result.set_result(dists, preds, num_nodes);
    free(dists);
    free(preds);

    auto t3 = std::chrono::steady_clock::now();

    if (!quiet) { // For performance analysis
        std::cout << "Executed SSSP using Gunrock with CSR graph representation. \n";
        std::cout << "Format Conversion to CSR:         " << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << "[ms]" << "\n";
        std::cout << "Execution: " << std::chrono::duration_cast<std::chrono::milliseconds>(t3 - t2).count() << "[ms]" << "\n";
        // std::cout << "Execution (measurement in Gunrock): " << exec_time << "[ms]" << "\n";
        std::cout << "Total Elapsed time:    " << std::chrono::duration_cast<std::chrono::milliseconds>(t3 - t1).count() << "[ms]" << "\n";
    }

    return std::chrono::duration_cast<std::chrono::milliseconds>(t3 - t1).count();
}