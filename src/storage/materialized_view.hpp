#ifndef mat_view_hpp_
#define mat_view_hpp_

#include <atomic>
#include <map>
#include <vector>
#include <functional>
#include <boost/any.hpp>
#if USE_PMDK
#include <libpmemobj++/experimental/radix_tree.hpp>
#endif
#include "spdlog/spdlog.h"
#include "chunked_vec.hpp"
#include "defs.hpp"
#include "exceptions.hpp"
#include "properties.hpp"
#include "transaction.hpp"
#include "txn_data.hpp"
#include "recovery.hpp"
#include "relationships.hpp"

#include "btree.hpp"

#if USE_PMDK
namespace nvm = pmem::obj;
#endif

struct view_primitive {
    offset_t primitive_id_;
    offset_t prev_;
    offset_t next_;
    uint32_t type_;
    uint32_t tuple_id;
    
  view_primitive() = default;

  view_primitive(const view_primitive &) = delete;
};

class materialized_view {
public:
    using range_iterator = chunked_vec<view_primitive>::range_iter;

  /**
   * Constructor
   */
  materialized_view();

  materialized_view(const materialized_view &) = delete;

  /**
   * Destructor
   */
  ~materialized_view();

  void add_path(const qr_tuple &qr);

  bool get_path(offset_t offset, view_primitive *path);
  
  chunked_vec<view_primitive> & as_vec() { return results_; }
  std::size_t size() { return count_.load();  }
  void scan_view(node_list &nl, relationship_list &rl, std::function<void(qr_tuple&)> consumer);
private:
  
  chunked_vec<view_primitive> results_; 
  btree_ptr path_index_;
  std::atomic_int count_;
};



#endif