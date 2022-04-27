/*
TODO:
- range_iter
- clear
- store
- last_used
*/

/*
 * Copyright (C) 2019-2022 DBIS Group - TU Ilmenau, All Rights Reserved.
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

#ifndef buffered_vec_hpp_
#define buffered_vec_hpp_

#include <array>
#include <bitset>
#include <cassert>
#include <list>

#include <iostream>

#include <mutex>  // For std::unique_lock
#include <shared_mutex>

#include "defs.hpp"
#include "exceptions.hpp"
#include "bufferpool.hpp"
#include "paged_file.hpp"
#include "spdlog/spdlog.h"

#define DEFAULT_CHUNK_SIZE PAGE_SIZE

/**
 * chunk is a contiguous buffer of a fixed size which stores records (byte
 * sequences) of the type given as template parameter. chunks can form a linked
 * list.
 * @tparam T type to records
 * @tparam num_records number of records to store per chunk
 */
template <typename T, int num_records>
struct chunk {
  T data_[num_records];             // the array of data
  std::bitset<num_records> slots_;  // bitstring representing empty slots (0), used slots (1)
  uint32_t first_;                  // the index of the first available slot
  /**
   * Create a new chunk, allocate and initialize the memory.
   */
  chunk() : first_(0) {}

  /**
   * Deallocate the memory.
   */
  ~chunk() = default;

  /**
   * Returns true if the slot at position i is used by a record.
   */
  inline bool is_used(std::size_t i) const {
    return slots_.test(i);
  }

  /**
   * Sets the slot at position i to used (b = true) or available (b = false);
   */
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

  /**
   * Returns the first available slot or SIZE_MAX if no slot is available
   * anymore.
   */
  std::size_t first_available() const {
    if (slots_.all())
      return SIZE_MAX;

    for (auto i = first_; i < num_records; i++)
      if (!slots_.test(i))
        return i;
    return SIZE_MAX;
  }

  /**
   * Returns the last used slot or SIZE_MAX if no slot is
   * used.
   */
  std::size_t last_used() const {
    if (slots_.none())
      return SIZE_MAX;

    for (auto i = (num_records - 1); i >= 0; i--)
      if (slots_.test(i))
        return i;
    return SIZE_MAX;
  }

  /**
   * Returns true if all slots of the chunk are used, i.e. no slots are
   * available.
   */
  inline bool is_full() const {
    return slots_.all();
  }

  /**
   * Returns true if none of the slots of the chunk are used, i.e. the chunk is
   * empty.
   */
  inline bool is_empty() const {
    return slots_.none();
  }
};

/**
 * chunked_vec is a class implementing a persistent vector as a sequence of
 * fixed-size elements. The vector consists of a linked list of chunks where
 * each chunk represents a contiguous memory region. In this way, the entire
 * sequence does not require to be physically contiguous stored in memory and
 * allows to extend the vector more easily without copying the entire data.
 * Elements of the vector are of type T which can be addressed via their index
 * (starting at 0). There are two ways to add element to chunked_vec: either via
 * store_at() to store an element at a given position or via append() where the
 * element is added to the end of the vector. For iterating over chunked_vec
 * also two methods are available: begin()/end() are used for an element-wise
 * iteration while range allows to specifiy a start and end chunk for the
 * iteration.
 */
template <typename T, int chunk_size = DEFAULT_CHUNK_SIZE>
class buffered_vec {
  static constexpr auto num_entries = static_cast<int>((chunk_size - 5) / (sizeof(T) + 1/8.0));

/// The type for pointers to single chunks.
  using chunk_ptr = chunk<T, num_entries> *;

 public:
  /**
   * An implementation of an iterator for buffered_vec.
   */
  class iter {
   public:
    iter(buffered_vec& bv, paged_file::page_id pid, paged_file::page_id num, offset_t p = 0) : 
      bvec_(bv), curr_pid_(pid), npages_(num), pos_(p) {
        cptr_ = bvec_.load_chunk(pid); 
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
      return cptr_ != other.cptr_|| pos_ != other.pos_;
    }

    T &operator*() const {
      assert(cptr_ != nullptr && cptr_->is_used(pos_));
      return cptr_->data_[pos_];
    }

    iter &operator++() {
      do {
        if (++pos_ == num_entries) {
          cptr_ = bvec_.load_chunk(++curr_pid_); 
          pos_ = 0;
        }
        // make sure, cptr_[pos_] is valid
      } while (cptr_ != nullptr && !cptr_->is_used(pos_));
      return *this;
    }

  private:
    buffered_vec& bvec_;
    paged_file::page_id curr_pid_;  // page_id of the current chunk
    paged_file::page_id npages_;    // number of chunks
    chunk_ptr cptr_;                // pointer to the current chunk
    offset_t pos_;                  // position within the current chunk
  };

#if 0
  struct range_iter {
    range_iter(chunked_vec &v, std::size_t first, std::size_t last, std::size_t pos = 0)
        : vec_(v), range_(first, last), current_chunk_(first),
          cptr_(nullptr), pos_(pos) {
            if (vec_.chunk_list_.size() > 0)
              cptr_ = vec_.chunk_list_[first];
        }

    operator bool() const { return current_chunk_ <= range_.second && cptr_; }

    T &operator*() const { return cptr_->data_[pos_]; }

    range_iter &operator++() {
      do {
        if (++pos_ == num_entries) {
          cptr_ = cptr_->next_;
          current_chunk_++;
          pos_ = 0;
        }
        // make sure, cptr_[pos_] is valid
      } while (current_chunk_ <= range_.second && cptr_ &&
               !cptr_->is_used(pos_));
      return *this;
    }

    std::size_t get_cur_chunk() { return current_chunk_; }
    std::size_t get_cur_pos() { return pos_; }

  private:
    chunked_vec &vec_; // reference to the actual chunked_vec
    std::pair<std::size_t, std::size_t> range_; // range of chunks to visit
    std::size_t current_chunk_; // position of the current chunk (initially
                                // range_.first)
    chunk_ptr cptr_;            // pointer to the current chunk
    offset_t pos_;              // position within the current chunk
  };
#endif

  /**
   * Create a new vector associated with a file in the bufferpool.
   */
  buffered_vec(bufferpool& pool, uint64_t file_id)
      : bpool_(pool), file_id_(file_id), file_mask_(file_id << 60),
        available_slots_(0), elems_per_chunk_(num_entries) {
          // TODO: initialize available_slots_ for an existing file
  }

  /**
   * Destructor.
   */
  ~buffered_vec() = default;

  /**
   * Delete all chunks of the vector and reset it to an empty vector.
   */
  void clear() {
    // TODO
  }

  /**
   * Return an iterator pointing to the begin of the chunked_vec.
   */
  iter begin() {
    return iter(*this, 1, num_chunks());
  }

  /**
   * Return an iterator pointing to the end of the chunked_vec.
   */
  iter end() { return iter(*this, num_chunks(), num_chunks()); }

#if 0
  range_iter range(std::size_t first_chunk, std::size_t last_chunk, std::size_t start_pos = 0) {
    return range_iter(*this, first_chunk, last_chunk, start_pos);
  }

  range_iter* range_ptr(std::size_t first_chunk, std::size_t last_chunk, std::size_t start_pos = 0) {
    return new range_iter(*this, first_chunk, last_chunk, start_pos);
  }
#endif

  /**
   * Store the given record at position idx (note: move semantics) and mark this
   * slot as used.
   */
  void store_at(offset_t idx, T &&o) {
    auto ch = find_chunk(idx, true);
    offset_t pos = idx % elems_per_chunk_;
    ch->data_[pos] = std::move(o); // Move the temporary object.
    ch->set(pos, true);
    bool is_used = ch->is_used(pos);

    if (!is_used)
      available_slots_--;
    /*  
    if (ch->is_full()) {
      std::unique_lock lock(fl_mtx_);
      remove_from_free_list(idx);
    }
    */
  }

  /**
   * Store the record at the end of the vector and return its position and a
   * pointer(!) to the record as a pair.
   */
  std::pair<offset_t, T *> append(T &&o, std::function<void(offset_t)> callback = nullptr) {
    std::unique_lock lock(fl_mtx_);

    if (is_full())
      resize(1);
    auto tail = get_last_chunk();
    assert(tail != nullptr);

    if (tail->is_full()) {
      resize(1);
      tail = get_last_chunk();
    }
    auto pos = tail->first_available();
    assert(pos != SIZE_MAX);
    auto offs = (bpool_.get_file(file_id_)->num_pages() - 1) * elems_per_chunk_;
    if (callback != nullptr) callback(offs + pos);
    available_slots_--;
    tail->set(pos, true);
    tail->data_[pos] = o;
    /*
    if (tail->is_full()) {
      remove_from_free_list(offs);
    }
    */
    return std::make_pair(offs + pos, &tail->data_[pos]);
  }

  /**
   * Store the record at the first available slot and return its position and a
   * pointer(!) to the record as a pair.
   */
  std::pair<offset_t, T *> store(T &&o, std::function<void(offset_t)> callback = nullptr) {
    chunk_ptr ch;
    offset_t idx = 0;

    std::unique_lock lock(fl_mtx_);
    if (free_list_.empty()) {
      // if we don't have anything in the freelist, we have to resize
      resize(1);
      // the new chunk is at the end of the chunklist
      ch = chunk_list_.back();
      // and we find its idx in the freelist
      //idx = find_in_free_list();
    }
    else {
      // otherwise we find the next available chunk in the freelist
      //idx = find_in_free_list();
      ch = find_chunk(idx);
    }
    offset_t pos = ch->first_available();
    assert(pos != SIZE_MAX);
    if (callback != nullptr) callback(idx + pos);
    available_slots_--;
    ch->set(pos, true);
    ch->data_[pos] = o;
    /*
    if (ch->is_full()) {
      remove_from_free_list(idx);
    }
    */
    return std::make_pair(idx + pos, &ch->data_[pos]);
  }

  /**
   * Erase the record at the given position, i.e. mark the slot as available.
   */
  void erase(offset_t idx) {
    auto ch = find_chunk(idx, true);
    offset_t pos = idx % elems_per_chunk_;
    ch->set(pos, false);
    if (ch->is_used(pos))
      available_slots_++;
    // TODO: if (ch->empty()) delete ch;
    // if this was the first slot on this chunk which is now empty, 
    // add this chunk to the free list
    /*
    bool was_full = ch->is_full();
    if (was_full) {
      std::unique_lock lock(fl_mtx_);
      add_to_free_list(idx);
    }
    */
  }

  /**
   * Return the index of the first available slot in any of the
   * chunks. The index is a global offset in the chunked_vec.
   */
  offset_t first_available() const {
    if (available_slots_ == 0)
      return UNKNOWN;

    auto npages = bpool_.get_file(file_id_)->num_pages();
    offset_t offs = 0;
    for (auto pid = 1u; pid <= npages; pid++) {
      auto ch = load_chunk(pid);
      if (ch != nullptr) {
        auto first = ch->first_available();
        if (first != SIZE_MAX) {
          return offs + first;
        }
      }
      offs += elems_per_chunk_;
    }
    return UNKNOWN;
  }

  /**
   * Return the index of the last used slot in the last
   * chunk. The index is a global offset in the chunked_vec.
   */
  offset_t last_used() const {
    chunk_ptr ch = chunk_list_.back();
    std::size_t idx = (chunk_list_.size() - 1) * elems_per_chunk_ + ch->last_used();
    return idx;
  }

  /**
   * Returns true if the slot at position i is used by a record.
   * The position is a global offset in the chunked_vec.
   */
  inline bool is_used(std::size_t i) const {
    auto ch = find_chunk(i);
    offset_t pos = i % elems_per_chunk_;
    return ch->slots_.test(pos);
  }

  /**
   * Return a const ref to the element stored at the given position (index) or
   * raises an exception if the index is invalid (or the slot is not used). The
   * offset is relative to the begining of the chunked_vec. The first element of
   * the chunked_vec has always an offset=0.
   */
  const T &const_at(offset_t idx) const {
    auto ch = find_chunk(idx);
    offset_t pos = idx % elems_per_chunk_;
    if (!ch->is_used(pos))
        throw unknown_id();
    return ch->data_[pos];
  }

  /**
   * Return a reference to the element stored at the given position (index) or
   * raises an exception if the index is invalid. The offset is relative to the
   * begining of the chunked_vec. The first element of the chunked_vec has
   * always an offset=0.
   * Note, that the corresponding slot is not marked as used.
   */
  T &at(offset_t idx) {
    auto ch = find_chunk(idx);
    offset_t pos = idx % elems_per_chunk_;
    if (!ch->is_used(pos))
      throw unknown_id();
    return ch->data_[pos];
  }

  /**
   * Resize the chunked_vec by the given number of additional chunks.
   */
  void resize(int nchunks) {
    std::cout << "resize" << std::endl;
    for (auto i = 0; i < nchunks; i++) {
      auto pg = bpool_.allocate_page(file_id_);
      // initialize pg with chunk
      auto chk = new(pg.first->payload) chunk<T, num_entries>();
      chk->slots_.reset();
      chk->first_ = 0;
      capacity_ += elems_per_chunk_;
      available_slots_ += elems_per_chunk_;
    }
  }

  /**
   * Return the capacity of the chunked_vec, which is the total number of
   * elements to be stored. Note, that this includes the number of already
   * existing elements.
   */
  offset_t capacity() const { return capacity_; }

  /**
   * Return true if the chunked_vec does not contain any empty slot anymore.
   */
  bool is_full() const { return available_slots_ == 0; }

  /**
   * Return the number of occupied chunks.
   */
  std::size_t num_chunks() const { return bpool_.get_file(file_id_)->num_pages(); }

  /**
   * Return the number of records stored per chunk.
   */
  uint32_t elements_per_chunk() const { return elems_per_chunk_; }

  uint32_t real_chunk_size() const { return sizeof(chunk<T, num_entries>); }

private:
#if 0
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
#endif

  /**
   * Finds the chunk that stores the record at the given index or raises
   * an exception if the index is out of range.
   */
  chunk_ptr find_chunk(offset_t idx, bool modify = false) const {
    auto page_id = idx / elems_per_chunk_ + 1;
    auto pg = bpool_.fetch_page(page_id | file_mask_);
    
    // std::cout << "find_chunk: " << page_id << ", addr=" << (uint64_t)pg->payload << std::endl;

    if (modify)
      bpool_.mark_dirty(page_id | file_mask_);
    return reinterpret_cast<chunk_ptr>(pg->payload);
  }

  chunk_ptr load_chunk(paged_file::page_id pid) const {
    if (!bpool_.get_file(file_id_)->is_valid(pid))
      return nullptr;

    auto pg = bpool_.fetch_page(pid | file_mask_);
    return reinterpret_cast<chunk_ptr>(pg->payload);
  }

  chunk_ptr get_last_chunk() const {
    auto pg = bpool_.last_valid_page(file_id_);
    return pg.first != nullptr ? reinterpret_cast<chunk_ptr>(pg.first->payload) : nullptr;
  }

  //--
  bufferpool& bpool_;
  uint64_t file_id_, file_mask_;
  offset_t available_slots_; // total number of available slots for records
  uint32_t elems_per_chunk_; // number of elements per chunk
  offset_t capacity_; // total capacity of the chunked_vec in number of records

  //--
  std::vector<chunk_ptr> chunk_list_; // the list of pointers to all chunks
  std::vector<offset_t> free_list_;   // the list of chunks with empty slots (described by their indexes)
  mutable std::shared_mutex fl_mtx_;  // mutex for accessing the free list
};

#endif
