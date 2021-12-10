#ifndef csr_delta_hpp_
#define csr_delta_hpp_

#include "defs.hpp"
#include "chunked_vec.hpp"

/**
 * A struct for a CSR delta element. An element can be the id of a 
 * node to which updates are associated, or the ids of it neigbouring 
 * nodes or the weight of its relationships. 
 */
struct delta_element {
  delta_element() = default;
  delta_element(const delta_element &) = delete;

  enum element_type { node_id, neighbour_id, rship_weight };

  uint64_t txid_;
  uint64_t node_id_;
  element_type type_;
  uint64_t val_;
};

/**
 * CSR delta store for updating existing CSR representations to 
 * reflect the latest snapshot of the graph. It stores delta entries, 
 * each corresponding to an update in the graph made by a transaction. 
 */
class csr_delta {
public:
  using rship_weight = std::function<double(relationship&)>;
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
   * Adds the elements of an update delta to the list of update delta elements.
   */
  void add_update_delta(uint64_t txid, uint64_t nid, const std::list<uint64_t> &ids,
                        const std::list<double> &weights);

  /**
   * Adds the elements of an append delta to the list of append delta elements.
   */
  void add_append_delta(uint64_t txid, uint64_t nid, const std::list<uint64_t> &ids,
                        const std::list<double> &weights);

  /**
   * Restores update and append deltas from the list of update and append delta elements 
   * into update and append delta maps respectively.
   */
  void restore_deltas(delta_map_t &&update_deltas, delta_map_t &&append_deltas, offset_t last_id);

  /**
   * Deletes all chunks of the update and append delta lists and sets the lists as empty.
   * And sets the bidirectional flag, relationship weight function and last node is 
   * to the given corresponding parameters. 
   */
  void reset_csr_delta(bool bidir = false, rship_weight func = [](relationship &r) { return 1.3; },
                       offset_t node_id = UNKNOWN);

  /**
   * Returns the weight of a given relationship using the weight function.
   */
  double get_rship_weight(relationship &r) { return weight_func_(r); }

  /**
   * Returns whether only outgoing relationships are considered (false) 
   * or both outgoing and incoming relationships are considered (true).
   */
  bool get_bidirectional() { return bidirectional_; }

  /**
   * Returns the last node id in current CSR
   */
  offset_t get_last_node_id() { return last_node_id_; }

private:
  bool bidirectional_ = false;
  rship_weight weight_func_ = [](relationship &r) { return 1.3; };
  offset_t last_node_id_ = UNKNOWN;

  chunked_vec<delta_element> update_deltas_; // the actual list of update delta elements
  chunked_vec<delta_element> append_deltas_; // the actual list of append delta elements
};


#endif