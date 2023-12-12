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

#ifndef defs_hpp_
#define defs_hpp_

#include <cstdint>
#include <limits>
#include <memory>
#include <string>
#include <vector>
#include <optional>
#include <mutex>

#include <boost/variant.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#ifdef USE_GUNROCK
#include <thrust/host_vector.h>
#include <thrust/device_vector.h>
#include <thrust/device_ptr.h>
#endif

#define POSEIDON_VERSION "0.0.8"

#define DICT_FILE_ID   0
#define NODE_FILE_ID   1
#define RSHIP_FILE_ID  2
#define NPROPS_FILE_ID 3
#define RPROPS_FILE_ID 4
#define INDEX_FILE_ID  5

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

constexpr uint32_t UNKNOWN_CODE = std::numeric_limits<uint32_t>::max();

inline std::string uint64_to_string(uint64_t v) {
  return v == UNKNOWN ? std::string("<null>") : std::to_string(v);
}

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

#define PMDK_PATH(p) poseidon::gPmemPath + p

#else

template <typename T> using p_ptr = std::shared_ptr<T>;

template <typename T> using p = T;

template <typename T, typename... Args>
inline p_ptr<T> p_make_ptr(Args &&... args) {
  return std::make_shared<T>(std::forward<Args>(args)...);
}

#define PMDK_PATH(p) p

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
