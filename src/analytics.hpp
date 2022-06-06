#ifndef analytics_hpp_
#define analytics_hpp_

#include "defs.hpp"

/* ---------------- Definitions for Analytics ---------------- */

/**
 * Typedef for a function that computes the weight of a relationship.
 */
using rship_weight = std::function<double(relationship&)>;

/**
 * Typedef for a predicate to check that a relationship is followed via the search.
 */
using rship_predicate = std::function<bool(relationship&)>;

/**
 * Typedef for a node visitor callback.
 */
using node_visitor = std::function<void(node&)>;

/**
 * Typedef for a node visitor callback which receives the full path.
 */
using path = std::vector<offset_t>;

using path_visitor = std::function<void(node&, const path&)>;

/**
 * A wrapper struct for CSR arrays 
 */
struct csr_arrays {
  csr_arrays() = default;
  csr_arrays(const csr_arrays &) = delete;

#ifdef USE_GUNROCK
  // TODO
  // thrust::host_vector<offset_t> row_offsets = {};
  // thrust::host_vector<offset_t> col_indices = {};
  // thrust::host_vector<float> edge_values = {};
  std::vector<offset_t> row_offsets = {};
  std::vector<offset_t> col_indices = {};
  std::vector<float> edge_values = {};
#else
  std::vector<offset_t> row_offsets = {};
  std::vector<offset_t> col_indices = {};
  std::vector<float> edge_values = {};
#endif
};

#define VOLATILE_DELTA
// #define PERSISTENT_DELTA
// #define DIFF_DELTA
#define ADJ_DELTA

/*
 * Struct used to store edge-coordinates in COO format
 * Needs to be allocated 16-bit alligned!
 */
struct edge_coords {
  offset_t x, y;
};

#endif