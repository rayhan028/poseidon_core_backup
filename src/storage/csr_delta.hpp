#ifndef csr_delta_hpp_
#define csr_delta_hpp_

#include "defs.hpp"
#include "chunked_vec.hpp"

#ifdef USE_PMDK
#include <libpmemobj++/experimental/concurrent_map.hpp>
// #include <libpmemobj++/container/concurrent_hash_map.hpp>
#endif

/**
 * CSR delta store for updating existing CSR representations to 
 * reflect the latest snapshot of the graph. It stores delta entries, 
 * each corresponding to an update in the graph made by a transaction. 
 */
class csr_delta {
public:
#ifdef USE_PMDK
  using id_vector_t = pmem::obj::vector<offset_t>;
  using weight_vector_t = pmem::obj::vector<double>;
  using vector_pair_t =
      pmem::detail::pair<p_ptr<id_vector_t>, p_ptr<weight_vector_t>>;
  using delta_map_t =
      pmem::obj::experimental::concurrent_map<p<uint64_t>, p_ptr<vector_pair_t>>;
  // using delta_map_t =
  //     pmem::obj::concurrent_hash_map<p<uint64_t>, p_ptr<vector_pair_t>>;
  using delta_map_val_t = delta_map_t::value_type;
#else
  using delta_map_t =
      std::map<uint64_t, std::pair<std::vector<uint64_t>, std::vector<double>>>;
#endif

  /**
   * Constructor.
   */
  csr_delta();

  /**
   * Destructor.
   */
  ~csr_delta();

  /**
   * The underlying persistent maps need a runtime initialization if
   * stored in persistent memory.
   */
  void initialize();

  /**
   * Returns a reference to the map of update deltas.
   */
  p_ptr<delta_map_t>& get_update_deltas() { return update_deltas_; }

  /**
   * Returns a reference to the map of append deltas.
   */
  p_ptr<delta_map_t>& get_append_deltas() { return append_deltas_; }

  /**
   * Inserts an entry into the map of update deltas.
   */
  void add_update_delta(uint64_t nid, const std::vector<uint64_t> &ids,
                        const std::vector<double> &weights);

  /**
   * Inserts an entry into the map of append deltas.
   */
  void add_append_delta(uint64_t nid, const std::vector<uint64_t> &ids,
                        const std::vector<double> &weights);

private:
  p_ptr<delta_map_t> update_deltas_;
  p_ptr<delta_map_t> append_deltas_;
};


#endif