#ifndef csr_delta_hpp_
#define csr_delta_hpp_

#include "defs.hpp"
#include "chunked_vec.hpp"

#ifdef VOLATILE_DELTA
#include <mutex>  // For std::unique_lock
#include <shared_mutex>
#endif

/**
 * A struct for a CSR delta record. A delta record is associated with a node 
 * updated by a transaction. It contains the id of the updated node, the ids of the nodes 
 * connected to it (column indices) and the corresponding relationship weights (edge values). 
 */
#ifdef VOLATILE_DELTA
struct delta_rec {
  delta_rec() = default;
  delta_rec(const delta_rec &) = delete;
  delta_rec(uint64_t txid, uint64_t nid,
    const std::vector<uint64_t> &ids, const std::vector<double> &weights)
    : txid_(txid), node_id_(nid), ids_(std::move(ids)), weights_(std::move(weights)) {}

  uint64_t txid_;               // id of the transaction that stored the delta record
  uint64_t node_id_;            // id of the node associated with the delta record
  std::vector<uint64_t> ids_;   // list of the neigbour node ids (column indices)
  std::vector<double> weights_; // list of corresponding relationship weights (edge values)
  bool merged_ = false;         // a flag indicating whether the delta record has been merged in a CSR update
};
#elif defined PERSISTENT_DELTA
struct delta_rec {
  delta_rec() = default;
  delta_rec(const delta_rec &) = delete;
  delta_rec(uint64_t txid, uint64_t nid, offset_t pos, int count)
    : txid_(txid), node_id_(nid), pos_(pos), count_(count) {}

  uint64_t txid_;       // id of the transaction that stored the delta record
  uint64_t node_id_;    // id of the node associated with the delta record
  offset_t pos_;    // offset of the neigbour node ids in the persistent vector "ids_"
                        // and the corresponding relationship weights in the persistent vector "weights_"
  int count_;           // number of the neighbour nodes (with corresponding relationship weights)
  bool merged_ = false; // a flag indicating whether the delta record has been merged in a CSR update
};
#endif

#ifdef VOLATILE_DELTA
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

  std::size_t last_used() const {
    if (slots_.none())
      return SIZE_MAX;

    for (auto i = (num_records - 1); i >= 0; i--)
      if (slots_.test(i))
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
template <typename T, int vchunk_size = DEFAULT_CHUNK_SIZE>
class vchunked_vec {
  static constexpr auto num_entries = vchunk_size / sizeof(T);

  using vchunk_ptr = vchunk<T, num_entries> *;

 public:
  /**
   * An implementation of an iterator for vchunked_vec.
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
          // TODO: we assume that we don't have empty vchunks
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
    vchunk_ptr cptr_; // pointer to the current vchunk
    offset_t pos_;   // position within the current vchunk
  };

  vchunked_vec(const std::string& unused = "")
      : capacity_(0), available_slots_(0),
        elems_per_vchunk_(num_entries) {}

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
      ch = find_vchunk(idx);
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
      offs += elems_per_vchunk_;
    }
    return UNKNOWN;
  }

  offset_t last_used() const {
    if (vchunk_list_.empty())
      return UNKNOWN;
    vchunk_ptr ch = vchunk_list_.back();
    std::size_t idx = (vchunk_list_.size() - 1) * elems_per_vchunk_ + ch->last_used();
    return idx;
  }

  void resize(int nvchunks) {
    int num = nvchunks;
    vchunk_ptr ptr = nullptr;

    if (vchunk_list_.empty()) {
      ptr = new vchunk<T, num_entries>();
      vchunk_list_.push_back(ptr);
      available_slots_ = capacity_ = elems_per_vchunk_;
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
      capacity_ += elems_per_vchunk_;
      available_slots_ += elems_per_vchunk_;
      add_to_free_list((vchunk_list_.size() - 1) * elems_per_vchunk_);
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

  vchunk_ptr find_vchunk(offset_t idx) const {
    auto n = idx / elems_per_vchunk_;
    if (n >= vchunk_list_.size())
      throw index_out_of_range();
    return vchunk_list_[n];
  }

  offset_t capacity_;
  offset_t available_slots_;
  uint32_t elems_per_vchunk_;
  std::vector<vchunk_ptr> vchunk_list_;
  std::vector<offset_t> free_list_;
  mutable std::shared_mutex fl_mtx_;
};
#endif

/**
 * The CSR delta store manages delta records. Delta records are merged to 
 * update the current CSR representation of the main PMem Graph so that 
 * analytics are executed on the latest snapshot of the graph. 
 */
class delta_store {
public:
  using delta_map_t =
    std::map<uint64_t, std::pair<std::vector<uint64_t>, std::vector<double>>>;

  /**
   * Constructor
   */
  delta_store() = default;
  delta_store(const delta_store &) = delete;

  /**
   * Destructor
   */
  ~delta_store() {}

  void initialize();

  /**
   * Stores updates of a transaction as a delta record in the vector of delta records.
   */
  void store_delta(uint64_t txid, uint64_t nid,
    const std::vector<uint64_t> &ids, const std::vector<double> &weights);

  /**
   * Returns a reference to the underlying vector of delta records.
   */
#ifdef VOLATILE_DELTA
  const vchunked_vec<delta_rec>& csr_delta_recs() { return delta_recs_; }
#elif defined PERSISTENT_DELTA
  const chunked_vec<delta_rec>& csr_delta_recs() { return delta_recs_; }
#endif

  /**
   * Merge valid delta records from different transactions into a delta map.
   * The deltas in the delta map are used to update the current CSR. 
   * Each delta in the delta map is of the form: 
   * {node id, <[ids of neighbours], [relationship weights]>}
   */
  void merge_deltas(delta_map_t &deltas, uint64_t txid);

  /**
   * Delete all chunks of the vector of delta records and reset it to an empty vector.
   */
  void clear_deltas();

  /**
   * Returns the weight function.
   */
  const rship_weight& weight_func() { return weight_func_; }

  /**
   * Returns whether only outgoing relationships are considered (false) 
   * or both outgoing and incoming relationships are considered (true).
   */
  bool bidirectional() { return bidirectional_; }

  /**
   * Returns the last node id in the current CSR
   */
  offset_t last_node_id() { return last_node_id_; }

  /**
   * Returns the id of the last transaction that made a CSR update.
   */
  uint64_t last_txn_id() { return last_txn_id_; }

private:
  friend class graph_db;

  bool bidirectional_ = false;            // bi/uni-directional traversal of relationships
  rship_weight weight_func_ =
    [](relationship &r) { return 1.3; };  // function to compute weights of relationships

  offset_t last_node_id_ = UNKNOWN;       // id of the last node in the current CSR
  uint64_t last_txn_id_ = UNKNOWN;        // id of the last transaction that made a CSR update

  bool delta_mode_ = true;                // delta mode for adaptive CSR update
  uint64_t num_delta_recs_ = 0;           // number of stored delta records
  uint64_t max_delta_recs_ = 18174889;    // maximum number of delta records for adaptive CSR update

#ifdef VOLATILE_DELTA
  vchunked_vec<delta_rec> delta_recs_;    // the underlying vchunked vector of volatile delta records
  
  // TODO these arrays are not needed here when CSR update is done directly on GPU
  std::vector<offset_t> row_offsets_ = {};  // row offsets array of the current CSR
  std::vector<offset_t> col_indices_ = {};  // column indices array of the current CSR
  std::vector<float> edge_values_ = {};     // edge values array of the current CSR
#elif defined PERSISTENT_DELTA
  pmem::obj::vector<uint64_t> ids_;         // ids of deleted neighbours for all deltas records
  pmem::obj::vector<double> weights_;       // relationship weights for all deltas delta records
  offset_t pos_ = 0;                        // start of empty slots in the "ids_" and "weights_" vectors

  chunked_vec<delta_rec> delta_recs_;       // the underlying chunked vector of persistent delta records

  // TODO these arrays are not needed here when CSR update is done directly on GPU
  pmem::obj::vector<offset_t> row_offsets_; // row offsets array of the current CSR
  pmem::obj::vector<offset_t> col_indices_; // column indices array of the current CSR
  pmem::obj::vector<float> edge_values_;    // edge values array of the current CSR
#endif
};


#endif