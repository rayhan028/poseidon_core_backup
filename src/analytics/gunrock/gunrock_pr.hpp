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

#ifndef gunrock_pr_hpp_
#define gunrock_pr_hpp_

#include "graph_db.hpp"
#include "gunrock.h"
#include <vector>
#include <chrono> // for elapsed time measurement

/**
 * Struct containing the results of SSSP
 */
struct pr_result {

  pr_result() {};

  /**
   * Setter function for the private variables.
   * Arrays dist and preds must be of length max_index_nodes!
  **/
  void set_result(float* ranks, offset_t* ids, uint64_t max_nid) {
    ranks_ = std::vector<float>(ranks, ranks + max_nid);
    nids_ = std::vector<offset_t>(ids, ids + max_nid);
    max_node_idx_ = max_nid;
  }

  /**
   * Returns the predecessor of a given node. Returns 
   * UNKNOWN for invalid or non-reachable nodes.
  **/
  offset_t get_nid(uint64_t idx) {
    return idx < max_node_idx_ ? nids_[idx] : UNKNOWN;
  }

  /**
   * Returns the predecessor of a given node. Returns 
   * UNKNOWN for invalid or non-reachable nodes.
  **/
  float get_rank(offset_t nid) {
    return nid < max_node_idx_ ? ranks_[nid]
                                : std::numeric_limits<float>::max();;
  }

private:
  uint64_t max_node_idx_;
  std::vector<float> ranks_;
  std::vector<offset_t> nids_;
};

/**
 * An implementation of BFS leveraging the GPU-Library Gunrock, using CSR graph representation. 
 * The bidirectional flag determines whether both outgoing and incoming relationships are 
 * considered (bidirectional = true) or only outgoing relationships are considered 
 * (bidirectional = false). Setting quiet to true mutes std::cout outputs during execution. 
 * The traversal info is stored in the result struct.
 */
uint64_t gunrock_pr_csr(graph_db_ptr gdb, bool bidirectional, pr_result &result, bool quiet);

#endif