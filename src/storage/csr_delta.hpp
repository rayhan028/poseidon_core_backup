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
  void store_delta(uint64_t nid, const std::list<uint64_t> &ids,
                   const std::list<double> &weights, uint64_t txid);

  /**
   * Returns a reference to the underlying vector of delta elements.
   */
  chunked_vec<delta_element>& get_delta_elements() { return delta_elements_; }

  /**
   * Deletes all chunks of the vector of delta elements and sets the vector as empty.
   */
  void clear_delta_elements();

  /**
   * Restores deltas from their corresponding delta elements into a delta map.
   * An item in a delta map is of the form: 
   * {node id, <[ids of neighbours], [edge weights]>}
   */
  void restore_deltas(delta_map_t &deltas, uint64_t txid);

  /**
   * Returns the weight function.
   */
  const rship_weight& get_weight_func() { return weight_func_; }

  /**
   * Sets the weight function.
   */
  void set_weight_func(const rship_weight &func) { weight_func_ = func; }

  /**
   * Returns whether only outgoing relationships are considered (false) 
   * or both outgoing and incoming relationships are considered (true).
   */
  bool get_bidirectional() { return bidirectional_; }

  /**
   * Sets whether only outgoing relationships are considered (false) 
   * or both outgoing and incoming relationships are considered (true).
   */
  void set_bidirectional(bool b) { bidirectional_ = b; }

  /**
   * Returns the last node id in the current CSR
   */
  offset_t get_last_node_id() { return last_node_id_; }

  /**
   * Sets the last node id in the current CSR
   */
  void set_last_node_id(offset_t id) { last_node_id_ = id; }

  /**
   * Returns the id of the last transaction that made a CSR update.
   */
  uint64_t get_last_txn_id() { return last_txn_id_; }

  /**
   * Sets the id of the last transaction that made a CSR update.
   */
  void set_last_txn_id(uint64_t txid) { last_txn_id_ = txid; }

  /**
   * Returns the row offsets array of the current CSR.
   */
  const std::vector<offset_t>& get_row_offs() { return row_offsets_; }

  /**
   * Sets the row offsets array of the current CSR.
   */
  void set_row_offs(std::vector<offset_t> &vec) { row_offsets_ = vec; }

  /**
   * Returns the column indices array of the current CSR.
   */
  const std::vector<offset_t>& get_col_inds() { return col_indices_; }

  /**
   * Sets the column indices array of the current CSR.
   */
  void set_col_inds(std::vector<offset_t> &vec) { col_indices_ = vec; }

  /**
   * Returns the edge values array of the current CSR.
   */
  const std::vector<float>& get_edge_vals() { return edge_values_; }

  /**
   * Sets the edge values array of the current CSR.
   */
  void set_edge_vals(std::vector<float> &vec) { edge_values_ = vec; }

private:
  bool bidirectional_ = false;  // bi/uni-directional traversal of relationships
  rship_weight weight_func_ =
    [](relationship &r) { return 1.3; };  // function to compute weights of relationships

  offset_t last_node_id_ = UNKNOWN; // id of the last node in the current CSR
  uint64_t last_txn_id_ = UNKNOWN;  // id of the last transaction that made a CSR update

  chunked_vec<delta_element> delta_elements_; // the underlying chunked vector of delta elements

  // TODO these arrays are not needed here when CSR update is done directly on GPU
  std::vector<offset_t> row_offsets_ = {};  // row offsets array of the current CSR
  std::vector<offset_t> col_indices_ = {};  // column indices array of the current CSR
  std::vector<float> edge_values_ = {};     // edge values array of the current CSR
};


#endif