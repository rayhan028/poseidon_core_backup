#ifndef relationships_hpp_
#define relationships_hpp_

#include <vector>
#include <limits>
#include <memory>
#include <string>
#include <future>
#include <cassert>

#include "vec.hpp"
#include "defs.hpp"
#include "exceptions.hpp"
#include "nodes.hpp"
#include "transaction.hpp"
#include "txn_data.hpp"

#ifndef POSEIDON_MAX_TIME
#define POSEIDON_MAX_TIME
constexpr uint64_t MAX_TIME = std::numeric_limits<uint64_t>::max();
#endif

struct relationship;
using dirty_rship = dirty_object<relationship>;
using dirty_rship_ptr = std::unique_ptr<dirty_rship>;

/**
 * relationship represents a versioned edge in a temporal graph database.
 */
struct relationship : public txn<dirty_rship_ptr> {
  template <template <typename I> typename V> friend class relationship_list;

  using id_t = offset_t;         // physical storage ID
  using logical_id_t = uint64_t; // stable logical ID across versions

private:
  id_t id_;
  logical_id_t logical_id_;

public:
  offset_t src_node;        // source node ID
  offset_t dest_node;       // destination node ID
  offset_t next_src_rship;  // next relationship in source adjacency list
  offset_t next_dest_rship; // next relationship in destination adjacency list
  offset_t property_list;   // index in property list
  dcode_t rship_label;      // dictionary code for relationship type
  // VALID-TIME METADATA
  uint64_t vt_start;
  uint64_t vt_end;
  // VERSION CHAIN
  offset_t prev_version;
  offset_t next_version;
   // NEW FIELD
  uint16_t update_count;

  relationship()
      : txn(),
        id_(UNKNOWN),
        logical_id_(UNKNOWN),
        src_node(UNKNOWN),
        dest_node(UNKNOWN),
        next_src_rship(UNKNOWN),
        next_dest_rship(UNKNOWN),
        property_list(UNKNOWN),
        rship_label(0),
        vt_start(UNKNOWN),
        vt_end(UNKNOWN),
        prev_version(UNKNOWN),
        next_version(UNKNOWN),
		update_count(0) {} //<--- Initialize

  relationship(dcode_t label, offset_t src, offset_t dest,
               logical_id_t log_id = UNKNOWN)
      : txn(),
        id_(UNKNOWN),
        logical_id_(log_id),
        src_node(src),
        dest_node(dest),
        next_src_rship(UNKNOWN),
        next_dest_rship(UNKNOWN),
        property_list(UNKNOWN),
        rship_label(label),
        vt_start(UNKNOWN),
        vt_end(UNKNOWN),
        prev_version(UNKNOWN),
        next_version(UNKNOWN),
		update_count(0) {} // <--- Initialize

  relationship(const relationship&) = delete;

  relationship(relationship&& r)
      : txn(std::move(r)),
        id_(r.id_),
        logical_id_(r.logical_id_),
        src_node(r.src_node),
        dest_node(r.dest_node),
        next_src_rship(r.next_src_rship),
        next_dest_rship(r.next_dest_rship),
        property_list(r.property_list),
        rship_label(r.rship_label),
        vt_start(r.vt_start),
        vt_end(r.vt_end),
        prev_version(r.prev_version),
        next_version(r.next_version),
		update_count(r.update_count) {} // <--- copy

  relationship& operator=(const relationship& r) {
    txn::operator=(r);
    id_ = r.id_;
    logical_id_ = r.logical_id_;
    src_node = r.src_node;
    dest_node = r.dest_node;
    next_src_rship = r.next_src_rship;
    next_dest_rship = r.next_dest_rship;
    property_list = r.property_list;
    rship_label = r.rship_label;
    vt_start = r.vt_start;
    vt_end = r.vt_end;
    prev_version = r.prev_version;
    next_version = r.next_version;
	update_count = r.update_count; // <--- Copy
    return *this;
  }

  relationship& operator=(relationship&& r) {
    txn::operator=(std::move(r));
    id_ = r.id_;
    logical_id_ = r.logical_id_;
    src_node = r.src_node;
    dest_node = r.dest_node;
    next_src_rship = r.next_src_rship;
    next_dest_rship = r.next_dest_rship;
    property_list = r.property_list;
    rship_label = r.rship_label;
    vt_start = r.vt_start;
    vt_end = r.vt_end;
    prev_version = r.prev_version;
    next_version = r.next_version;
	update_count = r.update_count; // <--- Copy
    return *this;
  }

  id_t id() const { return id_; }
  logical_id_t logical_id() const { return logical_id_; }

  node::id_t to_node_id() const { return dest_node; }
  node::id_t from_node_id() const { return src_node; }

  bool is_visible_at_tt(uint64_t query_time) const {
    return bts() <= query_time && query_time < cts();
  }

  bool is_visible_at_vt(uint64_t query_time) const {
    return vt_start <= query_time && query_time < vt_end;
  }

  bool is_current_version() const {
    return next_version == UNKNOWN && vt_end == MAX_TIME;
  }
};

/**
 * A class providing complete information for a relationship.
 */
struct rship_description {
  relationship::id_t id;                  // physical ID
  relationship::logical_id_t logical_id;  // stable logical ID
  node::id_t from_id, to_id;              // source and destination node IDs
  std::string label;                      // label (type)
  properties_t properties;                // properties

  // Temporal metadata
  uint64_t vt_start;
  uint64_t vt_end;
  uint64_t tt_start;
  uint64_t tt_end;

  std::string to_string() const;
  bool has_property(const std::string& pname) const;

  bool operator==(const rship_description& other) const;

  bool operator<(const rship_description& other) const { return logical_id < other.logical_id; }
};

std::ostream &operator<<(std::ostream &os, const rship_description &rdescr);

/**
 * A class for storing all relationships of a graph.
 */
template <template <typename I> typename T>
class relationship_list {

  struct init_rship_task {
    using range = std::pair<std::size_t, std::size_t>;
    init_rship_task(T<relationship> &r, std::size_t first, std::size_t last)
        : rships_(r), range_(first, last) {}

    void operator()() {
      auto iter = rships_.range(range_.first, range_.second);
      while (iter) {
        auto &r = *iter;
        r.runtime_initialize();
        ++iter;
      }
    }

    T<relationship> &rships_;
    range range_;
  };

public:
  using vec = T<relationship>;
  using range_iterator = typename T<relationship>::range_iter;
  using iterator = typename T<relationship>::iter;

  template <typename ... Args>
  relationship_list(Args&& ... args) : rships_(std::forward<Args>(args)...) {}
  relationship_list(const relationship_list &) = delete;
  ~relationship_list() = default;

  void runtime_initialize() {
#ifdef PARALLEL_INIT
    const int nchunks = 100;
    std::vector<std::future<void>> res;
    res.reserve(num_chunks() / nchunks + 1);
    thread_pool pool;
    std::size_t start = 0, end = nchunks - 1;
    while (start < num_chunks()) {
      res.push_back(pool.submit(init_rship_task(*this, start, end)));
      start = end + 1;
      end += nchunks;
    }
    for (auto &f : res)
      f.get();
#else
    for (auto &r : rships_) {
      r.runtime_initialize();
    }
#endif
  }

  relationship::id_t add(relationship &&r, xid_t owner = 0) {
    if (rships_.is_full())
      rships_.resize(1);

    auto id = rships_.first_available();
    assert(id != UNKNOWN);
    r.id_ = id;
    if (owner != 0) {
      r.lock(owner);
    }
    rships_.store_at(id, std::move(r));
    return id;
  }

  relationship::id_t insert(relationship &&r, xid_t owner = 0,
                            std::function<void(offset_t)> callback = nullptr) {
    auto p = rships_.store(std::move(r), callback);
    p.second->id_ = p.first;
    if (owner != 0) {
      p.second->lock(owner);
    }
    return p.first;
  }

  relationship::id_t append(relationship &&r, xid_t owner = 0,
                            std::function<void(offset_t)> callback = nullptr) {
    auto p = rships_.append(std::move(r), callback);
    p.second->id_ = p.first;
    if (owner != 0) {
      p.second->lock(owner);
    }
    return p.first;
  }

  relationship &get(relationship::id_t id) {
    if (rships_.capacity() <= id) {
      spdlog::warn("unknown relationship_id {}", id);
      throw unknown_id();
    }
    auto &r = rships_.at(id);
    return r;
  }

  void remove(relationship::id_t id) {
    if (rships_.capacity() <= id)
      throw unknown_id();
    rships_.erase(id);
  }

  relationship &last_in_from_list(relationship::id_t id) {
    relationship *rship = &get(id);
    while (rship->next_src_rship != UNKNOWN) {
      rship = &get(rship->next_src_rship);
    }
    return *rship;
  }

  relationship &last_in_to_list(relationship::id_t id) {
    relationship *rship = &get(id);
    while (rship->next_dest_rship != UNKNOWN) {
      rship = &get(rship->next_dest_rship);
    }
    return *rship;
  }

  auto &as_vec() { return rships_; }

  range_iterator range(std::size_t first_chunk, std::size_t last_chunk) {
    return rships_.range(first_chunk, last_chunk);
  }

  void dump() {
    std::cout << "------- RELATIONSHIPS -------\n";
    for (auto& r : rships_) {
      std::cout << std::dec
                << "#" << r.id()
                << " (logical=" << r.logical_id() << ")"
                << ", @" << (unsigned long)&r
                << " [ txn-id=" << short_ts(r.txn_id())
                << ", bts=" << short_ts(r.bts())
                << ", cts=" << short_ts(r.cts())
                << ", dirty=" << (r.d_ != nullptr ? r.d_->is_dirty_ : false)
                << " ], "
                << "VT=[" << short_ts(r.vt_start) << "," << short_ts(r.vt_end) << "], "
                << "TT=[" << short_ts(r.bts()) << "," << short_ts(r.cts()) << "], "
                << "ver(prev=" << uint64_to_string(r.prev_version)
                << ", next=" << uint64_to_string(r.next_version) << "), "
                << "label=" << r.rship_label
                << ", from=" << uint64_to_string(r.src_node)
                << " -> to=" << uint64_to_string(r.dest_node)
                << ", next_src=" << uint64_to_string(r.next_src_rship)
                << ", next_dest=" << uint64_to_string(r.next_dest_rship)
                << ", props=" << uint64_to_string(r.property_list);

      if (r.has_dirty_versions()) {
        std::cout << " {\n";
        for (const auto& dr : *(r.dirty_list())) {
          std::cout << "\t( @" << (unsigned long)&(dr->elem_)
                    << ", logical=" << dr->elem_.logical_id()
                    << ", txn-id=" << short_ts(dr->elem_.txn_id())
                    << ", bts=" << short_ts(dr->elem_.bts())
                    << ", cts=" << short_ts(dr->elem_.cts())
                    << ", label=" << dr->elem_.rship_label
                    << ", dirty=" << dr->elem_.is_dirty()
                    << ", VT=[" << short_ts(dr->elem_.vt_start) << "," << short_ts(dr->elem_.vt_end) << "]"
                    << ", TT=[" << short_ts(dr->elem_.bts()) << "," << short_ts(dr->elem_.cts()) << "]"
                    << ", ver(prev=" << uint64_to_string(dr->elem_.prev_version)
                    << ", next=" << uint64_to_string(dr->elem_.next_version) << ")"
                    << ", from=" << uint64_to_string(dr->elem_.src_node)
                    << " -> to=" << uint64_to_string(dr->elem_.dest_node)
                    << " )\n";
        }
        std::cout << "}";
      }
      std::cout << "\n";
    }
    std::cout << "-----------------------------\n";
  }

  std::size_t num_chunks() const { return rships_.num_chunks(); }

private:
  T<relationship> rships_; // the actual list of relationships
};

#endif