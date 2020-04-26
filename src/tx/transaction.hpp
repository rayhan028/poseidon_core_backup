/*
 * Copyright (C) 2019-2020 DBIS Group - TU Ilmenau, All Rights Reserved.
 *
 * This file is part of the Poseidon package.
 *
 * Poseidon is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Poseidon is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Poseidon. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef transaction_hpp_
#define transaction_hpp_

#include "defs.hpp"
#include "exceptions.hpp"
#include "properties.hpp"
#include <atomic>
#include <list>

#ifdef USE_PMDK
#include <libpmemobj++/experimental/v.hpp>
#endif

/**
 * Typedef for transaction ids.
 */
using xid_t = unsigned long;

/**
 * Typedef for timestamps.
 */
using timestamp_t = unsigned long;

/**
 * Value for infinity timestamp.
 */
constexpr unsigned long INF = std::numeric_limits<unsigned long>::max();

/**
 * A helper function for debugging to return a shortened version of the timestamps.
 */
inline int short_ts(timestamp_t ts) {
  return (ts == INF) ? -1 : static_cast<int>(ts & 0xffff);
}

struct node;
struct relationship;

/**
 * A transaction represents the atomic unit of execution graph operations.
 */
class transaction {
public:
/**
   * Default constructor. Shouldn't be used directly, because transactions are
   * created only via the begin_transaction() method of the class graph_db.
   */
  transaction();

  /**
   * Default destructor.
   */
  ~transaction() = default;

  /**
   * Returns the transaction id (=timestamp).
   */
  xid_t xid() const { return xid_; }

  /**
   * Add the given node to the vector of dirty node objects.
   */
  void add_dirty_node(offset_t id);

  /**
   * Add the given relationship to the vector of dirty relationships objects.
   */
  void add_dirty_relationship(offset_t id);

  /**
   * Return the list of dirty nodes modified in this transaction.
   */
   std::vector<offset_t>& dirty_nodes() { return dirty_nodes_; }

  /**
   * Return the list of dirty relationships modified in this transaction.
   */
   std::vector<offset_t>& dirty_relationships() { return dirty_rships_; }

  /**
   * Store the id of the log associated with this transaction.
   */
  void set_logid(std::size_t lid) { logid_ = lid; }

  /**
   * Return the id of the log associated with this transaction.
   */
  std::size_t logid() const { return logid_; }

private:
  xid_t xid_; // transaction identifier
  std::size_t logid_; // log identifier
  std::vector<offset_t>
      dirty_nodes_; // the vector of ids of nodes which were modified by this transaction
  std::vector<offset_t> dirty_rships_; // the vector of ids of relationships which
                                       // were modified by this transaction
};

using transaction_ptr = std::shared_ptr<transaction>;

/**
 * The currently active transaction is stored as thread local attribute.
 */
extern thread_local transaction_ptr current_transaction_;

/**
 * Check whether we are inside an active transaction that is associated
 * with the current thread. If not then an exception is raised.
 */
void check_tx_context();

/**
 * Return a pointer to the active transaction.
 */
transaction_ptr current_transaction();

/**
 * This template encapsulates all member variables and methods
 * used for multiversion transaction processing;
 */
template <typename T> struct txn_data {
  using dirty_list_ptr = std::list<T>*; // typedef for the list of dirty objects from
                                        // currently active transactions
  timestamp_t bts_, cts_, rts_;         // begin timestamp, commit timestamp, read timestamp
  std::atomic<xid_t> txn_id_;           // transaction id if locked, 0 otherwise
  dirty_list_ptr dirty_list_;           // the list of dirty objects
  bool is_dirty_;                       // true if the object represents a dirty object

  txn_data() : bts_(0), cts_(INF), rts_(0), txn_id_(0), dirty_list_(nullptr), is_dirty_(false) {}

  txn_data(const txn_data &n)
      : bts_(n.bts_), cts_(n.cts_), rts_(n.rts_), txn_id_(n.txn_id_.load()), 
        dirty_list_(n.dirty_list_), is_dirty_(n.is_dirty_) {}

  ~txn_data() { /* don't delete dirty_list_ here - it will be deleted at other places! */ }

 /**
   * Copy assignment operator.
   */
  txn_data &operator=(const txn_data &t) {
    bts_ = t.bts_;
    cts_ = t.cts_;
    rts_ = t.rts_;
    txn_id_ = t.txn_id_.load();
    is_dirty_ = t.is_dirty_;
    dirty_list_ = t.dirty_list_;
    return *this;
  }
    
  /**
   * Move assignment operator to move resources.
   */
  txn_data &operator=(txn_data &&t) {
    bts_ = t.bts_;
    cts_ = t.cts_;
    rts_ = t.rts_;
    txn_id_ = t.txn_id_.load();
    is_dirty_ = t.is_dirty_;
    dirty_list_ = t.dirty_list_;
    t.dirty_list_ = nullptr; //After moving resource from source, reset its pointer.
    return *this;
  }

  void runtime_initialize() { 
    txn_id_ = 0;
    dirty_list_ = nullptr;
  }
};

template <typename T> struct txn {
#ifdef USE_PMDK
  pmem::obj::experimental::v<txn_data<T>> d_;
#else
  txn_data<T> d_;
#endif
#if 0
  timestamp_t bts_, cts_, rts_; // begin timestamp, commit timestamp, read timestamp
  std::atomic<xid_t> txn_id; // transaction id if locked, 0 otherwise
  using dirty_list_ptr =
      std::list<T> *;        // typedef for the list of dirty objects from
                             // currently active transactions
  dirty_list_ptr dirty_list; // the list of dirty objects
  bool is_dirty_;            // true if the object represents a dirty object
#endif

  const txn_data<T>& d() const { 
#ifdef USE_PMDK
    return d_.get(); 
#else
    return d_;
#endif
  }

  txn_data<T>& d() { 
#ifdef USE_PMDK
    return d_.get(); 
#else
    return d_;
#endif
  }
  /**
   * Default constructor.
   */
  txn() {}

  /**
   * Copy constructor.
   */
  txn(const txn &n) : d_(n.d()) {}

  /**
   * Copy assignment operator.
   */
  txn &operator=(const txn &t) {
    d_ = t.d();
    return *this;
  }

  /**
   * Move assignment operator to move resources.
   */
  txn &operator=(txn &&t) {
    d_ = t.d();
    return *this;
  }

  /* ---------------- concurrency control ---------------- */

  inline xid_t txn_id() const { return d().txn_id_.load(); }
  /**
   * Return the value of the begin timestamp.
   */
  inline timestamp_t bts() const { return d().bts_; }

  /**
   * Return the value of the commit timestamp.
   */
  inline timestamp_t cts() const { return d().cts_; }

  /**
   * Return the value of the read timestamp.
   */
  inline timestamp_t rts() const { return d().rts_; }

  /**
   * Set the begin and commit timestamps.
   */
  void set_timestamps(xid_t beg, xid_t end) {
    d().bts_ = beg;
    d().cts_ = end;
  }

  /**
   * Set the commit timestamp.
   */
  void set_cts(xid_t end) { d().cts_ = end; }

  /**
   * Set the read timestamp.
   */
  void set_rts(xid_t end) { 
    // update only if rts < end
    if (d().rts_ < end) 
      d().rts_ = end; 
  }

  /**
   * Return true if the node is locked by a transaction.
   */
  bool is_locked() const { return d().txn_id_ != 0; }

  /**
   * Checks if the node is already locked by a transaction with the given xid.
   */
  bool is_locked_by(xid_t xid) const { return d().txn_id_ == xid; }

  /**
   * Locks the object. xid is the id of the owner transaction.
   */
  void lock(xid_t xid) {
    xid_t expected = 0;
    while (!d().txn_id_.compare_exchange_weak(expected, xid) && expected != xid)
      ;
  }

  /**
   * Release the lock.
   */
  void unlock() { d().txn_id_ = 0; }

  /**
   * Try to lock the object and return true if successful. Otherwise,
   * false is returned.
   */
  bool try_lock(xid_t xid) {
    xid_t expected = 0;
    return d().txn_id_.compare_exchange_strong(expected, xid);
  }

  /* ---------------- dirty object handling ---------------- */

  /**
   * Set the dirty flag to true, i.e. indicating that the object
   * (node/relationship) is a dirty object where properties are kept separately
   * from the properties_ table.
   */
  void set_dirty() { d().is_dirty_ = true; }

  /**
   * Check of the dirty flag is set. In this case the object is stored in volatile memory.
   */
  bool is_dirty() const { return d().is_dirty_; }

  /**
   * Check if the node is valid for the transaction with the give xid.
   */
  bool is_valid(xid_t xid) const { return d().bts_ <= xid && xid < d().cts_; }

  decltype(auto) dirty_list() { return d().dirty_list_; }

  /**
   * Return true if dirty copies of this node exist, i.e. if
   * other active transactions working on it.
   */
  bool has_dirty_versions() const { return d().dirty_list_ != nullptr && !d().dirty_list_->empty(); }

  /**
   * Find a valid version from the list of objects (stored in the dirty list)
   * that is valid for the transaction with the given xid by checking bts and
   * cts timestamps.
   */
  const T& find_valid_version(xid_t xid) const {
    if (has_dirty_versions()) {
      bool abort = false;
      for (const auto& dn : *(d().dirty_list_)) {
       if (!dn->elem_.is_locked() || dn->elem_.is_locked_by(xid)) {
        if (dn->elem_.is_valid(xid))
          return dn;
        else {
          // if the object is locked by us but not valid, then we have it 
          // already deleted!
          if (dn->elem_.is_locked_by(xid) && (dn->elem_.bts() == dn->elem_.cts()))
            throw unknown_id();

          // if the object is not locked but we cannot find a valid version
          // then we probably should abort the transaction instead of
          // throw unknown_id: but let's first check all versions
          abort = true;
        }
       } 
      }
      if (abort)
        // no valid version found -> abort the transaction
        throw transaction_abort();
    }
    throw unknown_id();
  }

  /**
   * Retrieve the dirty object version belonging to the transaction with the
   * given xid.
   */
  const T& get_dirty_version(xid_t xid) {
    if (has_dirty_versions()) {
      for (const auto& dn : *(d().dirty_list_)) {
        if (dn->elem_.txn_id() == xid) // TODO: !!!!
          return dn;
      }
    }
    throw unknown_id();
  }

  /**
   * Return a pointer to the dirty list if it exists and has dirty versions.
   * Currently used in Garbage collection Test case.
   */
  decltype(auto) get_dirty_objects() const {
    using dirty_list_ptr = const std::list<T>*;
    return has_dirty_versions() 
      ? std::optional<dirty_list_ptr>(d().dirty_list_) 
      : std::optional<dirty_list_ptr>{};
  }

  /**
   * Remove the dirty object versions belonging to the transaction with the
   * given xid.
   */
  void remove_dirty_version(xid_t xid) {
    if (has_dirty_versions()) {
      auto iter =
          std::remove_if(d().dirty_list_->begin(), d().dirty_list_->end(), [&](const T& dn) {
            return dn->elem_.d().txn_id_ == xid && dn->elem_.cts() == INF;
          });
      // assert(iter != dirty_list->end());
      d_.dirty_list_->erase(iter, d().dirty_list_->end());
    }
  }

  /**
   * Add a new dirty object to the list of dirty objects and return a reference
   * of the newly inserted object.
   */
  T& add_dirty_version(T&& tptr) {
    if (!d().dirty_list_) {//Cannot use  if(!has_dirty_versions()) as it will leak memory on heap
      d().dirty_list_ = new std::list<T>;
      spdlog::info("create dirty_list");
    }
    tptr->elem_.d_.dirty_list_ = d().dirty_list_;
    d().dirty_list_->push_front(std::move(tptr));

    return d().dirty_list_->front();
  }

  /**
   * Check if the version belonging to transaction given by xid was updated. In this case
   * return true. Otherwise, the object was added and return false.
   */
  bool updated_in_version(xid_t xid) {
    if (!d().dirty_list_) return false;
    for (const auto& dn : *(d().dirty_list_)) {
        if (dn->elem_.d().txn_id_ == xid) 
          return dn->updated();
    }
    return false;
  }

  /**
   * Perform garbage collection by deleting all dirty nodes which are
   * not used anymore.
   */
  void gc(xid_t oldest) {
    // we can safely delete all elements from the dirty list where cts <= txn
    // of the oldest active transaction
    if (has_dirty_versions()) {
      // spdlog::info("GC: remove everything smaller than {}: #{} elements",
      //             oldest, dirty_list->size());
      d().dirty_list_->remove_if([oldest](const T& dn) { return dn->elem_.cts() <= oldest; });
      // spdlog::info("GC done: #{} elements", dirty_list->size());
    }
    //Optional: After garbage collection, if there are no more versions, then we can delete the list.
    if (!has_dirty_versions()) {
      spdlog::info("delete dirty_list");
     delete d().dirty_list_;
     d().dirty_list_ = nullptr;
    }
  }
};

/**
 * dirty_object represents a version of a node or relationship kept in
 * volatile memory to implement MVTO. All these versions are managed in a
 * dirty object list associated with the persistent node.
 */
template <typename T> struct dirty_object {
  /**
   * Create a new dirty version from the object and the list of property
   * items.
   */
  dirty_object(const T &n, const std::list<p_item> &p, bool updated = true)
      : update_mode_(updated), properties_(p) {
    elem_ = n;
  }
  /**
   * Default destructor.
   */
  ~dirty_object() = default;

  /**
   * Returns true of the object was updated, false if the object was newly
   * inserted.
   */
  bool updated() const { return update_mode_; }

  bool update_mode_; // true if the object was updated, false if it was
                     // inserted
  T elem_;           // the actual object
  std::list<p_item> properties_; // the list of property items
};

#endif
