/*
 * Copyright (C) 2019-2024 DBIS Group - TU Ilmenau, All Rights Reserved.
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

#ifndef qresult_hpp_
#define qresult_hpp_

#include <memory>
#include <vector>
#include "defs.hpp"
#include "query_ctx.hpp"
#include <boost/config.hpp>               // Fixes the BOOST_NOEXCEPT errors
#include <boost/container_hash/hash.hpp> // Fixes the hash_combine / hash errors

struct node;
struct relationship;

struct null_t {
  explicit constexpr null_t(int) {}
  inline bool operator()(const null_t& one, const null_t& two) { return true; }
  inline bool operator==(const null_t& other) const { return true; }
  inline bool operator<(const null_t& other) const { return false; }
};

inline constexpr null_t null_val(-1);

struct array_t {
  array_t(std::vector<uint64_t> v) : elems(v) {}
  inline bool operator==(const array_t& other) const { return elems == other.elems; }
  inline bool operator<(const array_t& other) const { return elems < other.elems; }
  
  std::vector<uint64_t> elems;
};

/**
 * Typedef for an element (node, relationship, value) that might be part of a
 * query result. null_t is used to represent NULL values.
 */
using query_result =
    boost::variant<node *, relationship *, int, double, std::string, 
                    uint64_t, boost::posix_time::ptime, array_t, null_t, node_description, rship_description>;

#define qv_ query_result

inline node* qv_get_node(const query_result& v) { return boost::get<node*>(v); }
inline relationship* qv_get_relationship(const query_result& v) { return boost::get<relationship*>(v); }
inline int qv_get_int(const query_result& v) { return boost::get<int>(v); }
inline uint64_t qv_get_uint64(const query_result& v) { return boost::get<uint64_t>(v); }
inline double qv_get_double(const query_result& v) { return boost::get<double>(v); }
inline boost::posix_time::ptime qv_get_ptime(const query_result& v) { return boost::get<boost::posix_time::ptime>(v); }
inline std::string qv_get_string(const query_result& v) { return boost::get<std::string>(v); }
inline const node_description& qv_get_node_descr(const query_result& v) { return boost::get<const node_description&>(v); }
inline const rship_description& qv_get_rship_descr(const query_result& v) { return boost::get<const rship_description&>(v); }

enum qr_type {
  node_ptr_type = 0,
  rship_ptr_type = 1,
  int_type = 2,
  double_type = 3,
  string_type = 4,
  uint64_type = 5,
  ptime_type = 6,
  array_type = 7,
  null_type = 8,
  node_descr_type = 9,
  rship_descr_type = 10
};

namespace boost {
    /**
     * Specialize std::hash for ptime
     */
    template<>
    class hash<boost::posix_time::ptime> {
    public:
        size_t operator()(const boost::posix_time::ptime& t) const {
          std::size_t seed = 0;
          auto d = t.date();
          auto td = t.time_of_day();
          boost::hash_combine(seed, d.day_count().as_number());
          boost::hash_combine(seed, td.ticks());
          return seed;
        }
    };

    template<>
    class hash<query_result> {
    public:
        size_t operator()(const query_result& qr) const {
          switch(qr.which()) {
          case int_type: return boost::hash<int>()(qv_get_int(qr));
          case double_type: return boost::hash<double>()(qv_get_double(qr));
          case string_type: return boost::hash<std::string>()(qv_get_string(qr));
          case uint64_type: return boost::hash<uint64_t>()(qv_get_uint64(qr));
          case ptime_type: return boost::hash<boost::posix_time::ptime>()(qv_get_ptime(qr));
          default: return 0;
          }
        }
    };
}

/**
 * Typedef for a list of result elements which are passed to the next query
 * operator in an execution plan.
 */
using qr_tuple = std::vector<query_result>;

/**
 * Create a query_result object from a property value.
 */
query_result qv_from_pitem(const p_item& p);

/**
 * Create an any value from query_result.
 */
std::any qv_to_any(const query_result& qr);

/**
 * Output a query_result.
 */
std::ostream &operator<<(std::ostream &os, const query_result &qr);

/**
 * Comparison operators.
 */
bool equal(const qr_tuple& qr1, const qr_tuple& qr2);

bool qv_equal(const query_result& qr1, const query_result& qr2);

bool less_than(const query_result& qr1, const query_result& qr2);

inline bool less_or_equal(const query_result& qr1, const query_result& qr2) { 
  return less_than(qr1, qr2) || qv_equal(qr1, qr2);
}

bool greater_than(const query_result& qr1, const query_result& qr2);

inline bool greater_or_equal(const query_result& qr1, const query_result& qr2) {
    return greater_than(qr1, qr2) || qv_equal(qr1, qr2);
}

#endif