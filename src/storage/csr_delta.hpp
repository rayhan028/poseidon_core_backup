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
  using range_iter = chunked_vec<delta_element>::range_iter;
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
   * Returns the underlying vector of the update delta elements.
   */
  chunked_vec<delta_element>& update_deltas_as_vec() { return update_deltas_; }

  /**
   * Returns the underlying vector of the append delta elements.
   */
  chunked_vec<delta_element>& append_deltas_as_vec() { return append_deltas_; }

  /**
   * Adds the elements of an update delta to the list of update delta elements.
   */
  void add_update_delta(uint64_t nid, std::vector<uint64_t> &&ids,
                        std::vector<double> &&weights);

  /**
   * Adds the elements of an append delta to the list of append delta elements.
   */
  void add_append_delta(uint64_t nid, std::vector<uint64_t> &&ids,
                        std::vector<double> &&weights);

  /**
   * TODO.
   */
  void restore_deltas(std::map<uint64_t, std::pair<std::vector<uint64_t>, std::vector<double>>> &update_deltas,
                               std::map<uint64_t, std::pair<std::vector<uint64_t>, std::vector<double>>> &append_deltas);

private:
  chunked_vec<delta_element> update_deltas_; // the actual list of update delta elements
  chunked_vec<delta_element> append_deltas_; // the actual list of append delta elements
};


#endif