#ifndef csr_delta_hpp_
#define csr_delta_hpp_

#define VOLATILE_DELTA

#ifdef VOLATILE_DELTA
#include <mutex>  // For std::unique_lock
#include <shared_mutex>
#endif

#include "defs.hpp"
#include "chunked_vec.hpp"

#ifdef VOLATILE_DELTA
/**
 * A struct for a DRAM-based CSR delta element. A delta element is associated with a node 
 * updated by a transaction. It contains the id of the updated node, the ids of the nodes 
 * connected to it (neighbours) and the corresponding relationship weights. 
 */
struct delta_element {
  delta_element() = default;
  delta_element(const delta_element &) = delete;

  uint64_t txid_; // id of the transaction that stored the delta element for its modification
  uint64_t node_id_; // id of the node to which the delta element is associated
  std::vector<uint64_t> ids_; // list of the neigbour node ids connected to the node with id "node_id_"
  std::vector<double> weights_; // list of corresponding weights of relationships of the node with id "node_id_"
  bool restored_; // a flag indicating whether the delta element has been used in a CSR restore
};

/**
 * vchunk is a strictly volatile version of chunk in chunked_vec
 */
template <typename T, int num_records>
struct alignas(64) vchunk {
  std::array<T, num_records> data_;
  vchunk<T, num_records> *next_;
  std::bitset<num_records> slots_;
  uint32_t first_;

  vchunk() : next_(nullptr), first_(0) {}

  ~vchunk() { next_ = nullptr; }

  inline bool is_used(std::size_t i) const {
    return slots_.test(i);
  }

  inline void set(std::size_t i, bool b) {
    slots_.set(i, b);
    if (!b && i < first_) {
      // the record was deleted - update first_
      first_ = i;   
      return;
    }
    if (b && i == first_) {
      // we have to find the next available slot starting from first_
      for (auto j = first_; j < num_records; j++) {
        if (!slots_.test(j)) {
          first_ = j;
          return;
        }
      }
      if (first_ == i) first_ = num_records;
    }
  }

  std::size_t first_available() const {
    if (slots_.all())
      return SIZE_MAX;

    for (auto i = first_; i < num_records; i++)
      if (!slots_.test(i))
        return i;
    return SIZE_MAX;
  }

  inline bool is_full() const {
    return slots_.all();
  }
};

/**
 * vchunked_vec is a strictly volatile version of chunked_vec.
 */
template <typename T, int chunk_size = DEFAULT_CHUNK_SIZE>
class vchunked_vec {
  static constexpr auto num_entries = chunk_size / sizeof(T);

  using vchunk_ptr = vchunk<T, num_entries> *;

 public:
  /**
   * An implementation of an iterator for chunked_vec.
   */
  class iter {
   public:
    iter(vchunk_ptr ptr, offset_t p = 0) : cptr_(ptr), pos_(p) {
      // make sure the element at pos_ isn't deleted
      if (cptr_ != nullptr) {
        while (pos_ < num_entries) {
          if (cptr_->is_used(pos_))
            break;
          pos_++;
        }
        if (pos_ == num_entries) {
          cptr_ = nullptr;
          pos_ = 0;
          // TODO: we assume that we don't have empty chunks
        }
      }
    }

    bool operator!=(const iter &other) const {
      return cptr_ != other.cptr_ || pos_ != other.pos_;
    }

    T &operator*() const {
      assert(cptr_ != nullptr && cptr_->is_used(pos_));
      return cptr_->data_[pos_];
    }

    iter &operator++() {
      do {
        if (++pos_ == num_entries) {
          cptr_ = cptr_->next_;
          pos_ = 0;
        }
        // make sure, cptr_[pos_] is valid
      } while (cptr_ != nullptr && !cptr_->is_used(pos_));
      return *this;
    }

  private:
    vchunk_ptr cptr_; // pointer to the current chunk
    offset_t pos_;   // position within the current chunk
  };

  vchunked_vec(const std::string& unused = "")
      : capacity_(0), available_slots_(0),
        elems_per_chunk_(num_entries) {}

  ~vchunked_vec() {
    for (auto p : vchunk_list_)
      delete p;
  }

  void clear() {
    for (auto p : vchunk_list_)
      delete p;
    vchunk_list_.clear();
    free_list_.clear();
    capacity_ = 0;
    available_slots_ = 0;
  }

  iter begin() {
    return iter(vchunk_list_.empty() ? nullptr : vchunk_list_.front());
  }

  iter end() { return iter(nullptr); }

  std::pair<offset_t, T *> store(T &&o, std::function<void(offset_t)> callback = nullptr) {
    vchunk_ptr ch;
    offset_t idx = 0;

    std::unique_lock lock(fl_mtx_);
    if (free_list_.empty()) {
      // if we don't have anything in the freelist, we have to resize
      resize(1);
      // the new vchunk is at the end of the vchunklist
      ch = vchunk_list_.back();
      // and we find its idx in the freelist
      idx = find_in_free_list();
    }
    else {
      // otherwise we find the next available vchunk in the freelist
      idx = find_in_free_list();
      ch = find_chunk(idx);
    }
    offset_t pos = ch->first_available();
    assert(pos != SIZE_MAX);
    if (callback != nullptr) callback(idx + pos);
    available_slots_--;
    ch->set(pos, true);
    ch->data_[pos] = o;
    if (ch->is_full()) {
      remove_from_free_list(idx);
    }
    return std::make_pair(idx + pos, &ch->data_[pos]);
  }

  offset_t first_available() const {
    if (available_slots_ == 0)
      return UNKNOWN;

    vchunk_ptr ptr = vchunk_list_.front();
    offset_t offs = 0;
    while (ptr != nullptr) {
      auto first = ptr->first_available();
      if (first != SIZE_MAX) {
        return offs + first;
      }
      ptr = ptr->next_;
      offs += elems_per_chunk_;
    }
    return UNKNOWN;
  }

  void resize(int nchunks) {
    int num = nchunks;
    vchunk_ptr ptr = nullptr;

    if (vchunk_list_.empty()) {
      ptr = new vchunk<T, num_entries>();
      vchunk_list_.push_back(ptr);
      available_slots_ = capacity_ = elems_per_chunk_;
      num--;
      add_to_free_list(0);
    } else {
      // initialize ptr
      ptr = vchunk_list_.back();
    }
    for (auto i = 0; i < num; i++) {
      auto c = new vchunk<T, num_entries>();
      ptr->next_ = c;
      ptr = c;
      vchunk_list_.push_back(ptr);
      capacity_ += elems_per_chunk_;
      available_slots_ += elems_per_chunk_;
      add_to_free_list((vchunk_list_.size() - 1) * elems_per_chunk_);
    }
  }

private:
  void add_to_free_list(offset_t idx) {
    // spdlog::info("add_to_free_list: {}", idx);
    free_list_.push_back(idx);
  }

  void remove_from_free_list(offset_t idx) {
    // spdlog::info("remove_from_free_list: {}", idx);
    free_list_.erase(std::remove_if(std::begin(free_list_), std::end(free_list_), 
      [idx](auto i) { return i == idx; }), 
      std::end(free_list_));

  }

  offset_t find_in_free_list() {
    // spdlog::info("find_in_free_list: {} ({})", free_list_.front(), free_list_.size());
    // assert(!free_list_.empty());
    return free_list_.front();
  }

  vchunk_ptr find_chunk(offset_t idx) const {
    auto n = idx / elems_per_chunk_;
    if (n >= vchunk_list_.size())
      throw index_out_of_range();
    return vchunk_list_[n];
  }

  offset_t capacity_;
  offset_t available_slots_;
  uint32_t elems_per_chunk_;
  std::vector<vchunk_ptr> vchunk_list_;
  std::vector<offset_t> free_list_;
  mutable std::shared_mutex fl_mtx_;
};
#else
/**
 * A struct for a PMem-based CSR delta element. A delta element is associated with a node.
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
#endif

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
#ifdef VOLATILE_DELTA
  const vchunked_vec<delta_element>& get_delta_elements() { return delta_elements_; }
#else
  const chunked_vec<delta_element>& get_delta_elements() { return delta_elements_; }
#endif

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

#ifdef VOLATILE_DELTA
  vchunked_vec<delta_element> delta_elements_; // the underlying chunked vector of delta elements
  
  // TODO these arrays are not needed here when CSR update is done directly on GPU
  std::vector<offset_t> row_offsets_ = {};  // row offsets array of the current CSR
  std::vector<offset_t> col_indices_ = {};  // column indices array of the current CSR
  std::vector<float> edge_values_ = {};     // edge values array of the current CSR
#else
  chunked_vec<delta_element> delta_elements_; // the underlying chunked vector of delta elements

  // TODO these arrays are not needed here when CSR update is done directly on GPU
  pmem::obj::vector<offset_t> row_offsets_; // row offsets array of the current CSR
  pmem::obj::vector<offset_t> col_indices_; // column indices array of the current CSR
  pmem::obj::vector<float> edge_values_; // edge values array of the current CSR
#endif
};


#endif