#ifndef recovery_hpp_
#define recovery_hpp_

#include <atomic>
#include <map>
#include <vector>

#include <boost/any.hpp>
#include "spdlog/spdlog.h"

#include "vec.hpp"
#include "defs.hpp"
#include "exceptions.hpp"
#include "properties.hpp"
#include "transaction.hpp"
#include "txn_data.hpp"

 /*
  Wrapper structure for the stored intermediate query result.
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

/*
 Wrapper structure for storing a single argument of a query.
 operator_id_ the position of the operator in the pipeline to which the argument belongs
 argument_ contains the actual raw value (int, uint, double) or label_code in case of a string argument
 type_ represents the actual type of the argument.
*/
struct query_argument {
  offset_t operator_id_;
  offset_t argument_;
  offset_t type_;

  query_argument() = default;

  query_argument(const query_argument &) = delete;
};

class query_argument_list {
public:
  using range_iterator = chunked_vec<query_argument>::range_iter;

  query_argument_list() = default;
  query_argument_list(const query_argument_list &) = delete;

  ~query_argument_list();

  void add(offset_t opid, offset_t value, offset_t type);

private:
chunked_vec<query_argument> args_;
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

  /**
   * Method for adding the elements of a results tuple to the intermediate result storage 
   */
  std::vector<std::size_t> add(qr_tuple &&qr, dict &d, std::size_t chunk);

  void update(std::size_t chunked_id, qr_tuple &qr, dict &d);

  /**
   * Get a tuple element via its identifier.
   */
  intermediate_result &get(offset_t id);

  /**
   * Method for clearing the storage 
   */
  void clear();

  /*
   * Returns the chunk vector of intermediate results
  */
  chunked_vec<intermediate_result> &as_vec() { return results_; }

  const int size() { return tuple_cnt_.load(); }

  /**
   * Return a range iterator to traverse the node_list from first_chunk to
   * last_chunk.
   */
  range_iterator range(std::size_t first_chunk, std::size_t last_chunk, std::size_t start_pos = 0) {
    return results_.range(first_chunk, last_chunk, start_pos);
  }
  
  /**
   * Returns the number of occupied chunks of the underlying chunked_vec.
   */
  std::size_t num_chunks() const { return results_.num_chunks(); }

  int get_stored_tuples();

private:
  chunked_vec<intermediate_result> results_; // the actual list of stored intermediate_results
  std::atomic<int> tuple_cnt_;
  
};

#endif