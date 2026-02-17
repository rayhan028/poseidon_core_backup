#ifndef nodes_hpp_
#define nodes_hpp_
#include <atomic>
#include <map>
#include <vector>
#include <utility>
#include <any>
#include <limits>
#include "vec.hpp"
#include "defs.hpp"
#include "exceptions.hpp"
#include "properties.hpp"
#include "transaction.hpp"
#include "txn_data.hpp"

#ifndef POSEIDON_MAX_TIME
#define POSEIDON_MAX_TIME
constexpr uint64_t MAX_TIME = std::numeric_limits<uint64_t>::max();
#endif

struct node;
using dirty_node = dirty_object<node>;
using dirty_node_ptr = std::unique_ptr<dirty_node>;

/**
 * node represents a versioned vertex in a temporal graph database.
 */
struct node : public txn<dirty_node_ptr> {
  friend class graph_db;
  template <template <typename I> typename V> friend class node_list;

  using id_t = offset_t;         // physical storage ID
  using logical_id_t = uint64_t; // logical ID (stable across versions)

  private:
    id_t id_;
    logical_id_t logical_id_;

  public:
    offset_t from_rship_list;
    offset_t to_rship_list;
    offset_t property_list;
    dcode_t node_label;
    // VALID-TIME
    uint64_t vt_start;
    uint64_t vt_end;
    // VERSION CHAIN
    offset_t prev_version;
    offset_t next_version;
	// ANCHORING TRACKER -> Without update_count in the struct,
 	//every time we update a node, we wont know how long the history chain is without traversing it
    uint16_t update_count;

  /**
   * Default constructor.
   */

  node() = default;

    /**
     * Copy constructor (deleted).
     */
    node(const node &) = delete;

    /**
     * Move constructor - handle txn base properly.
     */
    node(node &&n)
        : txn(n), //ensures transaction ownership is preserved correctly when nodes are archived or relocated
          id_(n.id_),
          logical_id_(n.logical_id_), //pass logical id
          from_rship_list(n.from_rship_list),
          to_rship_list(n.to_rship_list),
          property_list(n.property_list),
          node_label(n.node_label),
          vt_start(n.vt_start), //pass valid times
          vt_end(n.vt_end),
          prev_version(n.prev_version), //pass version chain pointers
          next_version(n.next_version),
		  update_count(n.update_count){} // <--- Move update_count

    /**
     * Constructor for creating a node with label and logical ID.
     */
    node(dcode_t label, logical_id_t log_id = UNKNOWN)
        : id_(UNKNOWN),
          logical_id_(log_id),
          from_rship_list(UNKNOWN),
          to_rship_list(UNKNOWN),
          property_list(UNKNOWN),
          node_label(label),
          vt_start(UNKNOWN),
          vt_end(UNKNOWN),
          prev_version(UNKNOWN),
          next_version(UNKNOWN),
		  update_count(0){} // <--- Start new chain at 0

    /**
     * Copy assignment operator.
     */
    node &operator=(const node &n) {
      txn::operator=(n);
      node_label = n.node_label;
      from_rship_list = n.from_rship_list;
      to_rship_list = n.to_rship_list;
      property_list = n.property_list;
      id_ = n.id_;
      logical_id_ = n.logical_id_;
      vt_start = n.vt_start;
      vt_end = n.vt_end;
      prev_version = n.prev_version;
      next_version = n.next_version;
	  update_count = n.update_count; // <--- Copy count
      return *this;
    }

    /**
     * Move assignment operator - handle txn base properly.
     */
    node &operator=(node &&n) {
      txn::operator=(std::move(n));
      node_label = n.node_label;
      from_rship_list = n.from_rship_list;
      to_rship_list = n.to_rship_list;
      property_list = n.property_list;
      id_ = n.id_;
      logical_id_ = n.logical_id_;
      vt_start = n.vt_start;
      vt_end = n.vt_end;
      prev_version = n.prev_version;
      next_version = n.next_version;
	  update_count = n.update_count; // <--- Move count
      return *this;
    }

  /**
   * Returns the physical node identifier.
   */
  id_t id() const { return id_; }

  std::size_t _offset() const {
    return (uint64_t)((uint8_t *)&id_) - (uint64_t)((uint8_t *)this);
  }

  /**
   * Returns the logical node identifier.
   */
  logical_id_t logical_id() const { return logical_id_; }

  /**
   * Transaction-time visibility: [bts, cts). Checks if a node version is visible at a given system transaction time.
   */
  bool is_visible_at_tt(uint64_t query_time) const {
    return bts() <= query_time && query_time < cts();
  }

  /**
   * Valid-time visibility: [vt_start, vt_end). Checks if a node version is valid at a given real‑world time.
	Together with is_visible_at_tt, nodes are now bi‑temporal.
   */
  bool is_visible_at_vt(uint64_t query_time) const {
    return vt_start <= query_time && query_time < vt_end;
  }

  /**
   * Current version: latest in chain and VT still open. Determines if this node is the latest version in its chain.
   */
  bool is_current_version() const {
    return next_version == UNKNOWN && vt_end == MAX_TIME;
  }

};

/**
 * A class providing complete information for a node.
 */
struct node_description {
    node::id_t id;
    node::logical_id_t logical_id;
    std::string label;
    properties_t properties;

    // explicit VT
    uint64_t vt_start;
    uint64_t vt_end;
    // TT for description (populate from txn bts()/cts() when building)
    uint64_t tt_start;
    uint64_t tt_end;

    std::string to_string() const;
    bool has_property(const std::string& pname) const;
    bool operator==(const node_description& other) const;
    bool operator<(const node_description& other) const {
      return logical_id < other.logical_id;
    }
};

std::ostream &operator<<(std::ostream &os, const node_description &ndescr);
std::ostream &operator<<(std::ostream &os, const std::any &any_value);

/**
 * A class for storing all nodes of a graph.
 */
template <template <typename I> typename T>
class node_list {

  struct init_node_task {
    using range = std::pair<std::size_t, std::size_t>;

    init_node_task(T<node> &n, std::size_t first, std::size_t last)
        : nodes_(n), range_(first, last) {}

    void operator()() {
      auto iter = nodes_.range(range_.first, range_.second);
      while (iter) {
        auto &n = *iter;
        n.runtime_initialize();
        ++iter;
      }
    }

    T<node> &nodes_;
    range range_;
  };

public:
  using vec = T<node>;
  using range_iterator = typename T<node>::range_iter;

  template <typename ... Args>
  node_list(Args&& ... args) : nodes_(std::forward<Args>(args)...) {}

  node_list(const node_list &) = delete;
  ~node_list() = default;

  void runtime_initialize() {
#ifdef PARALLEL_INIT
    const int nchunks = 100;
    std::vector<std::future<void>> res;
    res.reserve(num_chunks() / nchunks + 1);
    thread_pool pool;
    std::size_t start = 0, end = nchunks - 1;
    while (start < num_chunks()) {
      res.push_back(pool.submit(init_node_task(*this, start, end)));
      start = end + 1;
      end += nchunks;
    }
    for (auto &f : res)
      f.get();
#else
    for (auto &n : nodes_) {
      n.runtime_initialize();
    }
#endif
  }

  node::id_t add(node &&n, xid_t owner = 0) {
    if (nodes_.is_full())
      nodes_.resize(1);

    auto id = nodes_.first_available();
    assert(id != UNKNOWN);
    n.id_ = id;
    if (owner != 0) {
      n.lock(owner);
    }
    nodes_.store_at(id, std::move(n));
    return id;
  }

  node::id_t insert(node &&n, xid_t owner = 0, std::function<void(offset_t)> callback = nullptr) {
    auto p = nodes_.store(std::move(n), callback);
    p.second->id_ = p.first;
    if (owner != 0) {
      p.second->lock(owner);
    }
    return p.first;
  }

  node::id_t append(node &&n, xid_t owner = 0, std::function<void(offset_t)> callback = nullptr) {
    auto p = nodes_.append(std::move(n), callback);
    p.second->id_ = p.first;
    if (owner != 0) {
      p.second->lock(owner);
    }
    return p.first;
  }

  node &get(node::id_t id) {
    if (nodes_.capacity() <= id) {
      spdlog::warn("unknown node_id {}", id);
      throw unknown_id();
    }
    auto &n = nodes_.at(id);
    return n;
  }

  void remove(node::id_t id) {
    if (nodes_.capacity() <= id)
      throw unknown_id();
    nodes_.erase(id);
  }

  auto &as_vec() { return nodes_; }

  range_iterator range(std::size_t first_chunk, std::size_t last_chunk, std::size_t start_pos = 0) {
    return nodes_.range(first_chunk, last_chunk, start_pos);
  }

  range_iterator* range_ptr(std::size_t first_chunk, std::size_t last_chunk, std::size_t start_pos = 0) {
    return nodes_.range_ptr(first_chunk, last_chunk, start_pos);
  }

  void dump() {
    std::cout << "----------- NODES -----------\n";
    for (auto& n : nodes_) {
      std::cout << std::dec << "#" << n.id()
                << " (logical=" << n.logical_id() << ")"
                << ", @" << (unsigned long)&n
                << " [ txn-id=" << short_ts(n.txn_id())
                << ", bts=" << short_ts(n.bts())
                << ", cts=" << short_ts(n.cts())
                << ", dirty=" << (n.d_ != nullptr ? n.d_->is_dirty_ : false)
                << " ], "
                << "VT=[" << short_ts(n.vt_start) << "," << short_ts(n.vt_end) << "], "
                << "TT=[" << short_ts(n.bts()) << "," << short_ts(n.cts()) << "], "
                << "ver(prev=" << uint64_to_string(n.prev_version)
                << ", next=" << uint64_to_string(n.next_version) << "), "
                << "label=" << n.node_label
                << ", from=" << uint64_to_string(n.from_rship_list)
                << ", to=" << uint64_to_string(n.to_rship_list)
                << ", props=" << uint64_to_string(n.property_list);

      if (n.has_dirty_versions()) {
        std::cout << " {\n";
        for (const auto& dn : *(n.dirty_list())) {
          std::cout << "\t( @" << (unsigned long)&(dn->elem_)
                    << ", logical=" << dn->elem_.logical_id()
                    << ", txn-id=" << short_ts(dn->elem_.txn_id())
                    << ", bts=" << short_ts(dn->elem_.bts())
                    << ", cts=" << short_ts(dn->elem_.cts())
                    << ", label=" << dn->elem_.node_label
                    << ", dirty=" << dn->elem_.is_dirty()
                    << ", VT=[" << short_ts(dn->elem_.vt_start) << "," << short_ts(dn->elem_.vt_end) << "]"
                    << ", TT=[" << short_ts(dn->elem_.bts()) << "," << short_ts(dn->elem_.cts()) << "]"
                    << ", ver(prev=" << uint64_to_string(dn->elem_.prev_version)
                    << ", next=" << uint64_to_string(dn->elem_.next_version) << ")"
                    << ", from=" << uint64_to_string(dn->elem_.from_rship_list)
                    << ", to=" << uint64_to_string(dn->elem_.to_rship_list)
                    << ", [";
          for (const auto& pi : dn->properties_) {
            std::cout << " " << pi;
          }
          std::cout << " ])\n";
        }
        std::cout << "}";
      }
      std::cout << "\n";
    }
    std::cout << "-----------\n";
  }

  std::size_t num_chunks() const { return nodes_.num_chunks(); }

private:
  T<node> nodes_;
};

#endif