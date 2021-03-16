#ifndef recovery_hpp_
#define recovery_hpp_

#include <atomic>
#include <map>
#include <vector>

#include <boost/any.hpp>
#include "spdlog/spdlog.h"

#include "chunked_vec.hpp"
#include "defs.hpp"
#include "exceptions.hpp"
#include "properties.hpp"
#include "transaction.hpp"
#include "txn_data.hpp"

 /*
  Wrapper type for the stored intermediate query result.
  Tuple_id_ is the identifier of the tuple to which the result belongs.
  In case of node/rship the res_ is the identifier, otherwise, the raw value.
  The type indicates the actual type of the stored result,
 */
struct intermediate_result {
    int tuple_id_;
    offset_t res_;
    offset_t type_;
    offset_t chunk_;
    
    intermediate_result() = default;

    intermediate_result(const intermediate_result &) = delete;
};

class recovery_list {
public:
    using range_iterator = chunked_vec<intermediate_result>::range_iter;

  /**
   * Constructor
   */
  recovery_list() = default;
  recovery_list(const recovery_list &) = delete;

  /**
   * Destructor
   */
  ~recovery_list();

  void runtime_initialize();

  void add(qr_tuple &&qr, std::size_t chunk);

  chunked_vec<intermediate_result> &as_vec() { return results_; }
  

private:
  chunked_vec<intermediate_result> results_; // the actual list of stored intermediate_results
  std::atomic<int> tuple_cnt_;
};

#endif