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

#ifndef defs_hpp_
#define defs_hpp_

#include <cstdint>
#include <limits>
#include <memory>
#include <string>
#include <vector>
#include <optional>

#include <boost/variant.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#define POSEIDON_VERSION "0.0.4"

/**
 * Typedef used for codes in string dictionaries and type tables.
 */
using dcode_t = uint32_t;

/**
 * Typedef used for offset in tables used as index. In a table the offset
 * starts always with 0.
 */
using offset_t = uint64_t;

/**
 * Typedef used for memory pointers.
 */
using ptr_t = uint8_t *;

/**
 *
 */
constexpr uint64_t UNKNOWN = std::numeric_limits<uint64_t>::max();

inline std::string uint64_to_string(uint64_t v) {
  return v == UNKNOWN ? std::string("<null>") : std::to_string(v);
}

struct node;
struct relationship;

struct null_t {
    explicit constexpr null_t(int) {}
  inline bool operator()(const null_t& one, const null_t& two) { return true; }
 inline bool operator==(const null_t& other) const { return true; }
};

inline constexpr null_t null_val(-1);

struct array_t {
  array_t(std::vector<uint64_t> v) : elems(v) {}
  inline bool operator()(const array_t& one, const array_t& two) { return true; }
  inline bool operator==(const array_t& other) const { return true; }
  std::vector<uint64_t> elems;
};

/**
 * Typedef for an element (node, relationship, value) that might be part of a
 * query result. null_t is used to represent NULL values.
 */
using query_result =
    boost::variant<node *, relationship *, int, double, std::string, 
                    uint64_t, boost::posix_time::ptime, array_t, null_t>;

/**
 * Typedef for a list of result elements which are passed to the next query
 * operator in an execution plan.
 */
using qr_tuple = std::vector<query_result>;

#ifdef USE_PMDK

/**
 * Includes for PMDK.
 */
#include <libpmemobj++/make_persistent.hpp>
#include <libpmemobj++/p.hpp>
#include <libpmemobj++/persistent_ptr.hpp>
#include "polymorphic_string.hpp"

template <typename T> using p_ptr = pmem::obj::persistent_ptr<T>;

template <typename T> using p = pmem::obj::p<T>;

template <typename T, typename... Args>
inline p_ptr<T> p_make_ptr(Args &&... args) {
  return pmem::obj::make_persistent<T>(std::forward<Args>(args)...);
}

using string_t = polymorphic_string;

/**
 * Helper class for persistent concurrent hashmap.
 */
class string_hasher {
  /* hash multiplier used by fibonacci hashing */
  static const size_t hash_multiplier = 11400714819323198485ULL;

public:
  size_t operator()(const string_t &str) const {
    return hash(str.c_str(), str.size());
  }

private:
  size_t hash(const char *str, size_t size) const {
    size_t h = 0;
    for (size_t i = 0; i < size; ++i) {
      h = static_cast<size_t>(str[i]) ^ (h * hash_multiplier);
    }
    return h;
  }
};

#else

template <typename T> using p_ptr = std::shared_ptr<T>;

template <typename T> using p = T;

template <typename T, typename... Args>
inline p_ptr<T> p_make_ptr(Args &&... args) {
  return std::make_shared<T>(std::forward<Args>(args)...);
}

#endif

// #define SMALL_CHUNKS

#ifdef SMALL_CHUNKS

#define PROP_CHUNK_SIZE  4040 // ensures chunk_size of 4096 Bytes
#define NODE_CHUNK_SIZE  4040 // ensures chunk_size of 4096 Bytes
#define RSHIP_CHUNK_SIZE 4096 // ensures chunk_size of 4096 Bytes

#else

#define PROP_CHUNK_SIZE  1048576 // ensures chunk_size of 64 MB
#define NODE_CHUNK_SIZE  1048576 // ensures chunk_size of 64 MB
#define RSHIP_CHUNK_SIZE 1048576 // ensures chunk_size of 64 MB

#endif

#endif
