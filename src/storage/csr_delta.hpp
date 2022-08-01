#ifndef csr_delta_hpp_
#define csr_delta_hpp_

#include "transaction.hpp"
#include "vec.hpp"
#include "relationships.hpp"
#include "nodes.hpp"

#ifdef VOLATILE_DELTA
#include <mutex>  // For std::unique_lock
#include <shared_mutex>
#endif

#ifdef DIFF_DELTA
/**
 * A struct for a CSR delta record. A delta record is associated with 
 * a node updated by a transaction. It contains the id of the updated node. 
 * If the updated node is not deleted, the delta record contains either 
 * the ids of the inserted nodes connected to it and the corresponding 
 * relationship weights, or the ids of the deleted nodes, or both.
 */
struct delta_rec {
  delta_rec() = default;
  delta_rec(const delta_rec &) = delete;
  delta_rec(uint64_t txid, offset_t nid, bool d)
    : txid_(txid), node_id_(nid), deleted_(d) {}

  uint64_t txid_;         // id of the transaction that stored the delta record
  offset_t node_id_;      // id of the node associated with the delta record

  bool deleted_;          // whether the node with id "node_id_" is deleted 

  offset_t deletes_pos_;  // offset of the deleted neigbour node ids in the vector "deletes_"
  int deletes_count_ = 0; // number of the deleted neighbour nodes
  offset_t inserts_pos_;  // offset of the inserted neigbour node ids in the vector "inserts_"
                          // and the corresponding relationship weights in the vector "weights_"
  int inserts_count_ = 0; // number of the inserted neighbour nodes (with corresponding relationship weights)

  bool merged_ = false;   // a flag indicating whether the delta record has been merged in a CSR update
};

/**
 * A struct for merged CSR delta record(s) associated with the same node.
 */
struct delta {
  delta() = default;
  delta(const delta &) = delete;
  delta(offset_t nid, bool d) : node_id_(nid), deleted_(d) {}

  offset_t node_id_;              // id of the node associated with the delta
  bool deleted_;                  // whether the node with id "node_id_" is deleted  

  std::vector<uint64_t> deletes_; // ids of the deleted neighbours
  std::vector<uint64_t> inserts_; // ids of the inserted neighbours
  std::vector<double> weights_;   // relationship weights (for the inserted neighbours)
};

#elif defined ADJ_DELTA
/**
 * A struct for a CSR delta record. A delta record is associated with a node
 * updated by a transaction. It contains the id of the updated node, the ids of the nodes
 * connected to it (column indices) and the corresponding relationship weights (edge values).
 */
struct delta_rec {
  delta_rec() = default;
  delta_rec(const delta_rec &) = delete;
  delta_rec(uint64_t txid, uint64_t nid, offset_t pos, int count)
    : txid_(txid), node_id_(nid), pos_(pos), count_(count) {}

  uint64_t txid_;       // id of the transaction that stored the delta record
  uint64_t node_id_;    // id of the node associated with the delta record
  offset_t pos_;        // offset of the neigbour node ids in the vector "ids_"
                        // and the corresponding relationship weights in the vector "weights_"
  int count_;           // number of the neighbour nodes (with corresponding relationship weights)
  bool merged_ = false; // a flag indicating whether the delta record has been merged in a CSR update
};

/**
 * A struct for merged CSR delta record(s) associated with the same node.
 */
struct delta {
  delta() = default;
  delta(const delta &) = delete;

  offset_t node_id_;            // id of the node associated with the delta

  std::vector<uint64_t> ids_;   // ids of the neighbour nodes
  std::vector<double> weights_; // corresponding relationship weights
};
#endif

/**
 * The CSR delta store manages delta records. Delta records are merged to
 * update the current CSR representation of the main PMem Graph so that
 * analytics are executed on the latest snapshot of the graph.
 */
class delta_store {
public:
  using delta_map_t = std::map<uint64_t, delta>;

  /**
   * Constructor
   */
  delta_store() = default;
  delta_store(const delta_store &) = delete;

  /**
   * Destructor
   */
  ~delta_store() {}

  void initialize();

#ifdef DIFF_DELTA
  /**
   * Stores all updates of a transaction as delta records in the vector of delta records.
   */
  void store_deltas(uint64_t txid, const transaction::delta_ids &txn_delta_ids);
#elif defined ADJ_DELTA
  /**
   * Stores updates of a transaction on a node as a delta record in the vector of delta records.
   */
  void store_delta(uint64_t txid, uint64_t nid,
    const std::vector<uint64_t> &ids, const std::vector<double> &weights);
#endif

  /**
   * Returns a reference to the underlying vector of delta records.
   */
#ifdef VOLATILE_DELTA
  const mem_chunked_vec<delta_rec>& csr_delta_recs() { return delta_recs_; }
#elif defined PERSISTENT_DELTA
  const nvm_chunked_vec<delta_rec>& csr_delta_recs() { return delta_recs_; }
#endif

  /**
   * Merge valid delta records from different transactions into a delta map.
   * The deltas in the delta map are used to update the current CSR.
   */
  void merge_deltas(delta_map_t &deltas, uint64_t txid);

  /**
   * Delete all chunks of the vector of delta records and reset it to an empty vector.
   */
  void clear_deltas();

  /**
   * Returns the weight function.
   */
  const rship_weight& weight_func() { return weight_func_; }

  /**
   * Returns whether only outgoing relationships are considered (false)
   * or both outgoing and incoming relationships are considered (true).
   */
  bool bidirectional() { return bidirectional_; }

  /**
   * Returns the last node id in the current CSR
   */
  offset_t last_node_id() { return last_node_id_; }

  /**
   * Returns the id of the last transaction that made a CSR update.
   */
  uint64_t last_txn_id() { return last_txn_id_; }

// private: // TODO
  friend class graph_db;

  bool bidirectional_ = false;              // bi/uni-directional traversal of relationships
  rship_weight weight_func_ =
    [](relationship &r) { return 1.3; };    // function to compute weights of relationships

  offset_t last_node_id_ = UNKNOWN;         // id of the last node in the current CSR
  uint64_t last_txn_id_ = UNKNOWN;          // id of the last transaction that made a CSR update

  bool delta_mode_ = true;                  // delta mode for adaptive CSR update
  uint64_t num_delta_recs_ = 0;             // number of stored delta records
  uint64_t max_delta_recs_ = 18174889;      // maximum number of delta records for adaptive CSR update

#ifdef VOLATILE_DELTA

  #ifdef DIFF_DELTA
  std::vector<uint64_t> deletes_;           // ids of the deleted neighbours for all deltas records
  std::vector<uint64_t> inserts_;           // ids of the inserted neighbours for all deltas records
  std::vector<double> weights_;             // relationship weights (for inserted neighbours) for all deltas delta records
  offset_t next_deletes_pos_ = 0;           // start of empty slots in the "deletes_" vector
  offset_t next_inserts_pos_ = 0;           // start of empty slots in the "inserts_" and "weights_" vectors
  #elif defined ADJ_DELTA
  std::vector<uint64_t> ids_;               // ids of neighbours (column indices) for all deltas records
  std::vector<double> weights_;             // relationship weights (edge values) for all deltas delta records
  offset_t next_pos_ = 0;                   // start of empty slots in the "ids_" and "weights_" vectors
  #endif

  mem_chunked_vec<delta_rec> delta_recs_;      // the underlying vchunked vector of volatile delta records

#ifdef USE_GUNROCK
  // TODO
  // thrust::device_vector<offset_t> row_offsets_ = {};  // row offsets array of the current CSR on GPU
  // thrust::device_vector<offset_t> col_indices_ = {};  // column indices array of the current CSR on GPU
  // thrust::device_vector<float> edge_values_ = {};     // edge values array of the current CSR on GPU
  offset_t *row_offsets_;     // row offsets array of the current CSR on GPU
  offset_t *col_indices_;     // column indices array of the current CSR on GPU
  float *edge_values_;        // edge values array of the current CSR on GPU
  std::size_t row_offs_size_; // size of the row_offsets_ array
  std::size_t col_inds_size_; // size of the col_indices_ (and edge_values_) array
#else
  std::vector<offset_t> row_offsets_ = {};  // row offsets array of the current CSR
  std::vector<offset_t> col_indices_ = {};  // column indices array of the current CSR
  std::vector<float> edge_values_ = {};     // edge values array of the current CSR
#endif

#elif defined PERSISTENT_DELTA

  #ifdef DIFF_DELTA
  pmem::obj::vector<uint64_t> deletes_;     // ids of the deleted neighbours for all deltas records
  pmem::obj::vector<uint64_t> inserts_;     // ids of the inserted neighbours for all deltas records
  pmem::obj::vector<double> weights_;       // relationship weights (for inserted neighbours) for all deltas delta records
  offset_t next_deletes_pos_ = 0;           // start of empty slots in the "deletes_" vector
  offset_t next_inserts_pos_ = 0;           // start of empty slots in the "inserts_" and "weights_" vectors
  #elif defined ADJ_DELTA
  pmem::obj::vector<uint64_t> ids_;         // ids of neighbours (column indices) for all deltas records
  pmem::obj::vector<double> weights_;       // relationship weights (edge values) for all deltas delta records
  offset_t next_pos_ = 0;                   // start of empty slots in the "ids_" and "weights_" vectors
  #endif

  nvm_chunked_vec<delta_rec> delta_recs_;       // the underlying chunked vector of persistent delta records

#ifdef USE_GUNROCK
  // TODO
  // thrust::device_vector<offset_t> row_offsets_ = {};  // row offsets array of the current CSR on GPU
  // thrust::device_vector<offset_t> col_indices_ = {};  // column indices array of the current CSR on GPU
  // thrust::device_vector<float> edge_values_ = {};     // edge values array of the current CSR on GPU
  offset_t *row_offsets_;     // row offsets array of the current CSR on GPU
  offset_t *col_indices_;     // column indices array of the current CSR on GPU
  float *edge_values_;        // edge values array of the current CSR on GPU
  std::size_t row_offs_size_; // size of the row_offsets_ array
  std::size_t col_inds_size_; // size of the col_indices_ (and edge_values_) array
#else
  pmem::obj::vector<offset_t> row_offsets_; // row offsets array of the current CSR
  pmem::obj::vector<offset_t> col_indices_; // column indices array of the current CSR
  pmem::obj::vector<float> edge_values_;    // edge values array of the current CSR
#endif

#endif
};


#endif