/*
 * Copyright (C) 2019-2021 DBIS Group - TU Ilmenau, All Rights Reserved.
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
#ifndef file_vec_hpp_
#define file_vec_hpp_

#include "defs.hpp"
#include "exceptions.hpp"
#include "mm_file.hpp"
#include <string>
#include <cstdio>

#define DEFAULT_VEC_SIZE 10000

template <typename T>
class file_vec {
public:
  static void remove_file_vec(const std::string& fname) {
    ::remove(fname.c_str());
    auto s = "slots_" + fname; 
    ::remove(s.c_str());
  } 

    /**
     * Create a new empty vector.
     */
    file_vec(const std::string& fname) : data_file_(fname, DEFAULT_VEC_SIZE * sizeof(T) + 2 * sizeof(offset_t)), slot_file_("slots_" + fname, DEFAULT_VEC_SIZE / sizeof(uint64_t)) {
      uint8_t *base_addr = static_cast<uint8_t *>(data_file_.base_address());
      std::size_t region_size = data_file_.size();
      capacity_     = region_size / sizeof(T);
      data_         = new (base_addr + 2 * sizeof(offset_t)) T[capacity_];
      slots_        = new (slot_file_.base_address()) uint64_t[capacity_];
      // first_        = find_first_available(0);
      memcpy(&first_, base_addr, sizeof(offset_t));
      memcpy(&last_, base_addr + sizeof(offset_t), sizeof(offset_t));
      // std::cout << "first_ = " << first_ << ", last_ = " << last_ << std::endl;
      is_open_      = true;
    }

  /**
   * Destructor.
   */
  ~file_vec() { close(); }

  void close() { 
    if (is_open_) {
      // TODO: save first_, last_
      uint8_t *base_addr = static_cast<uint8_t *>(data_file_.base_address());
      memcpy(base_addr, &first_, sizeof(offset_t));
      memcpy(base_addr + sizeof(offset_t), &last_, sizeof(offset_t));
      data_file_.close(); 
      slot_file_.close();
      is_open_ = false;
    }
  }
  /**
   * Delete all chunks of the vector and reset it to an empty vector.
   */
  void clear() {}

 /**
   * An implementation of an iterator for chunked_vec.
   */
  class iter {
   public:
    iter(T *ptr, uint64_t *slots, offset_t last = 0) : ptr_(ptr), slots_(slots), pos_(0), last_(last) {
      if (ptr_ != nullptr) {
        // skip empty slots
        while (!is_used(pos_)) pos_++;
      }
    }

    bool operator!=(const iter &other) const {
      return ptr_ != other.ptr_;
    }

    T &operator*() const {
      assert(ptr_ != nullptr && is_used(pos_));
      return ptr_[pos_];
    }

    iter &operator++() {
      do {
        if (++pos_ == last_)
          ptr_ = nullptr;
      } while (ptr_ != nullptr && !is_used(pos_));
      return *this;
    }

  private:
    bool is_used(offset_t i) const { return slots_[i / 64] & (0x8000000000000000 >> (i % 64)); }

    uint64_t* slots_;
    T* ptr_; // pointer to the current chunk
    offset_t pos_;
    offset_t last_;   // position within the current chunk
  };

  /**
   * Return an iterator pointing to the begin of the chunked_vec.
   */
  iter begin() {
    return iter(data_, slots_, last_);
  }

  /**
   * Return an iterator pointing to the end of the chunked_vec.
   */
  iter end() { return iter(nullptr, nullptr); }

  /**
   * Store the given record at position idx (note: move semantics) and mark this
   * slot as used.
   */
  void store_at(offset_t idx, T &&o) {
    if (idx >= capacity_)
      throw index_out_of_range();
    data_[idx] = std::move(o);
    slots_[idx / 64] |= 0x8000000000000000 >> (idx % 64);
    // std::cout << "store at #" << idx << " --> " << std::hex << slots_[idx / 64] << std::endl; 
    // std::cout << "store_at: first = " << first_ << ", idx = " << idx << std::endl;
    if (first_ == idx)
      first_ = find_first_available(first_);
    last_ = std::max(last_, idx);
    }

   /**
   * Store the record at the end of the vector and return its position and a
   * pointer(!) to the record as a pair.
   */
  std::pair<offset_t, T *> append(T &&o, std::function<void(offset_t)> callback = nullptr) {
    if (last_ >= capacity_)
      throw index_out_of_range();
    data_[last_] = std::move(o);
    slots_[last_ / 64] |= 0x8000000000000000 >> (last_ % 64);
    if (callback != nullptr) callback(last_);
    auto pos = last_;
    last_++;
    return std::make_pair(pos, &data_[pos]);
  }

  /**
   * Store the record at the first available slot and return its position and a
   * pointer(!) to the record as a pair.
   */
  // std::pair<offset_t, T *> store(T &&o, std::function<void(offset_t)> callback = nullptr);

  /**
   * Erase the record at the given position, i.e. mark the slot as available.
   */
  void erase(offset_t idx) {
    memset(&data_[idx], 0, sizeof(T));
    offset_t pos = idx / 64;
    auto v = slots_[pos];
    slots_[pos] = v & ~(0x8000000000000000 >> (idx % 64));
    if (idx < first_)
      first_ = idx;
  }

  /**
   * Return the index of the first available slot in any of the
   * chunks. The index is a global offset in the chunked_vec.
   */
  offset_t first_available() const { return first_; }

  /**
   * Returns true if the slot at position i is used by a record.
   * The position is a global offset in the chunked_vec.
   */
  inline bool is_used(std::size_t i) const { 
    return slots_[i / 64] & (0x8000000000000000 >> (i % 64)); 
  }

  /**
   * Return a const ref to the element stored at the given position (index) or
   * raises an exception if the index is invalid (or the slot is not used). The
   * offset is relative to the begining of the chunked_vec. The first element of
   * the chunked_vec has always an offset=0.
   */
   const T &const_at(offset_t idx) const {
      if (idx >= capacity_)
        throw index_out_of_range();
      if (!is_used(idx))
        throw unknown_id();
      return data_[idx];  
   }

  /**
   * Return a reference to the element stored at the given position (index) or
   * raises an exception if the index is invalid. The offset is relative to the
   * begining of the chunked_vec. The first element of the chunked_vec has
   * always an offset=0.
   * Note, that the corresponding slot is not marked as used.
   */
  T &at(offset_t idx) {
    if (idx >= capacity_)
      throw index_out_of_range();
    if (!is_used(idx))
      throw unknown_id();
    return data_[idx];      
  }

  /**
   * Resize the chunked_vec by the given number of additional chunks.
   */
  // void resize(int nchunks) {}

 /**
   * Return the capacity of the chunked_vec, which is the total number of
   * elements to be stored. Note, that this includes the number of already
   * existing elements.
   */
  offset_t capacity() const { return capacity_; }

  /**
   * Return true if the chunked_vec does not contain any empty slot anymore.
   */
  // bool is_full() const { return available_slots_ == 0; }

  /**
   * Return the number of occupied chunks.
   */
  std::size_t num_chunks() const { return 1; }

  /**
   * Return the number of records stored per chunk.
   */
  // uint32_t elements_per_chunk() const { return elems_per_chunk_; }

  // uint32_t real_chunk_size() const { return sizeof(chunk<T, num_entries>); }

  void print_slots() {
    std::cout << std::hex;
    for (auto i = 0ul; i < capacity_ / 64; i++) {
      std::cout << slots_[i] << " ";
    }
    std::cout << std::endl;
  }

private:
  offset_t find_first_available(offset_t start) const {
    auto nslots = capacity_ / 64;
    for (auto i = start / 64; i < nslots; i++) {
      if (slots_[i] < 0xffffffffffffffff) {
        auto v = slots_[i];
        for (auto j = 0ul; j < 64; j++) {
          if (!(v & 0x8000000000000000)) {
            return j + i * 64;
          }
          v <<= 1;
        }
      }
    }
    return SIZE_MAX;
  }

    bool is_open_;
    mm_file data_file_;
    mm_file slot_file_;
    T* data_;
    uint64_t* slots_;
    offset_t capacity_;
    offset_t first_, last_; // TODO
};

#endif