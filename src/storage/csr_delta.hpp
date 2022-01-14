#ifndef csr_delta_hpp_
#define csr_delta_hpp_

#include "defs.hpp"
#include "chunked_vec.hpp"

/**
 * A struct for a CSR delta element. A delta element is associated with a node.
 * The element can either be the id of a node to which updates of a transaction 
 * are associated, or the id of a neigbour to that node, or the weight of 
 * a relationship attached to that node. 
 */
struct delta_element {
  delta_element() = default;
  delta_element(const delta_element &) = delete;

  enum element_type { node_id, neighbour_id, rship_weight };

  uint64_t txid_; // id of the transaction that stored the delta element for its modification
  uint64_t node_id_; // id of the node to which the delta element is associated
  element_type type_; // type of the delta element
  uint64_t val_; // actual value of the delta element
  bool restored_; // a flag indicating whether the delta element has been used in a CSR restore
};

/**
 * CSR delta store for updating existing CSR representations to 
 * reflect the latest snapshot of the graph. It stores delta entries, 
 * each corresponding to an update in the graph made by a transaction. 
 */
class csr_delta {
public:
  using delta_map_t =
    std::map<uint64_t, std::pair<std::vector<uint64_t>, std::vector<double>>>;

  /**
   * Constructor
   */
  csr_delta() = default;
  csr_delta(const csr_delta &) = delete;

  /**
   * Destructor
   */
  ~csr_delta() {}

  void initialize();

  /**
   * Stores the elements of a delta to the vector of delta elements.
   */
  void store_delta(uint64_t nid, const std::vector<uint64_t> &ids,
                   const std::vector<double> &weights, uint64_t txid);

  /**
   * Returns a reference to the underlying vector of delta elements.
   */
  const chunked_vec<delta_element>& get_delta_elements() { return delta_elements_; }

  /**
   * Restores deltas from their corresponding delta elements into a delta map.
   * The deltas are used to update the existing CSR representations to reflect 
   * the latest snapshot of the graph. A delta in the delta map is of the form: 
   * {node id, <[ids of neighbours], [edge weights]>}
   */
  void restore_deltas(delta_map_t &deltas, uint64_t txid);

  /**
   * Returns the weight function.
   */
  const rship_weight& get_weight_func() { return weight_func_; }

  /**
   * Returns whether only outgoing relationships are considered (false) 
   * or both outgoing and incoming relationships are considered (true).
   */
  bool get_bidirectional() { return bidirectional_; }

  /**
   * Returns the last node id in the current CSR
   */
  offset_t get_last_node_id() { return last_node_id_; }

  /**
   * Returns the id of the last transaction that made a CSR update.
   */
  uint64_t get_last_txn_id() { return last_txn_id_; }

private:
  friend class graph_db;

  bool bidirectional_ = false;  // bi/uni-directional traversal of relationships
  rship_weight weight_func_ =
    [](relationship &r) { return 1.3; };  // function to compute weights of relationships

  offset_t last_node_id_ = UNKNOWN; // id of the last node in the current CSR
  uint64_t last_txn_id_ = UNKNOWN;  // id of the last transaction that made a CSR update

  chunked_vec<delta_element> delta_elements_; // the underlying chunked vector of delta elements

  // TODO these arrays are not needed here when CSR update is done directly on GPU
  pmem::obj::vector<offset_t> row_offsets_; // row offsets array of the current CSR
  pmem::obj::vector<offset_t> col_indices_; // column indices array of the current CSR
  pmem::obj::vector<float> edge_values_; // edge values array of the current CSR
};


#endif