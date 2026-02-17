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

#include <boost/hana.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "defs.hpp"
#include "nodes.hpp"
#include "relationships.hpp"
#include "qresult.hpp"

using namespace boost::posix_time;

query_result qv_from_pitem(const p_item& p) {
  query_result qr;
  switch(p.typecode()) {
    case p_item::p_int: 
      qr = p.get<int>();
      break;
    case p_item::p_double:
      qr = p.get<double>();
      break;
    case p_item::p_dcode:
      qr = (int)p.get<dcode_t>();
      break;
    case p_item::p_uint64:
      qr = p.get<uint64_t>();
      break;
    case p_item::p_ptime:
      qr = p.get<boost::posix_time::ptime>();
      break;
    default:
      break;
  }
  return qr;
}

std::any qv_to_any(const query_result& qr) {
  std::any res;
  switch (qr.which()) {
  case int_type: res = qv_get_int(qr); break;
  case double_type: res = qv_get_double(qr); break;
  case string_type: res = qv_get_string(qr); break;
  case uint64_type: res = qv_get_uint64(qr); break;
  case ptime_type: res = qv_get_ptime(qr); break;
  default:
  break;
 }
  return res;
}

/*
bool equal(const query_result& v1, const query_result& v2) {
  if (v1.which() != v2.which()) return false;

  switch (v1.which()) {
  case node_ptr_type: return qv_get_node(v1)->id() == qv_get_node(v2)->id();
  case rship_ptr_type: return qv_get_relationship(v1)->id() == qv_get_relationship(v2)->id();
  case int_type: return qv_get_int(v1) == qv_get_int(v2); 
  case double_type: return qv_get_double(v1) == qv_get_double(v2);
  case string_type: return qv_get_string(v1) == qv_get_string(v2); 
  case uint64_type: return qv_get_uint64(v1) == qv_get_uint64(v2); 
  case ptime_type: return qv_get_ptime(v1) == qv_get_ptime(v2);
  case array_type: return false;  // TODO
  case null_type: return true; 
  case node_descr_type: { 
    auto& n1 = qv_get_node_descr(v1);
    auto& n2 = qv_get_node_descr(v2); 
    return n1 == n2;
  }
  case rship_descr_type: { 
    auto& r1 = qv_get_rship_descr(v1);
    auto& r2 = qv_get_rship_descr(v2); 
    return r1 == r2;
  }
  }

  return false; 
}
*/

std::ostream &operator<<(std::ostream &os, const query_result &qr) {
  switch (qr.which()) {
  case node_ptr_type: { auto n = qv_get_node(qr); os << n->id(); break; }
  case rship_ptr_type: { auto r = qv_get_relationship(qr); os << r->id(); break; }
  case int_type: os << qv_get_int(qr); break;
  case double_type: os << qv_get_double(qr); break;
  case string_type: os << qv_get_string(qr); break;
  case uint64_type: os << qv_get_uint64(qr); break;
  case ptime_type: os << qv_get_ptime(qr); break;
  case array_type: os << "array"; break;
  case null_type: os << "NULL"; break;
  case node_descr_type: { auto& n = qv_get_node_descr(qr); os << n; break; }
  case rship_descr_type: { auto& r = qv_get_rship_descr(qr); os << r; break; }
  }
  os << ":" << qr.which();
  return os;
}

bool equal(const qr_tuple& qr1, const qr_tuple& qr2) {
  if (qr1.size() != qr2.size())
    return false;
  for (auto i = 0u; i != qr1.size(); i++) 
    if (! qv_equal(qr1[i], qr2[i]))
      return false;

  return true;
}

// ----------- query_result::int -----------
inline bool int_not_equal(const query_result& r1, const query_result& r2) {
    return qv_get_int(r1) != qv_get_int(r2);
}

inline bool int_greater_than(const query_result& r1, const query_result& r2) {
    return qv_get_int(r1) > qv_get_int(r2);
}

inline bool int_less_than(const query_result& r1, const query_result& r2) {
    return qv_get_int(r1) < qv_get_int(r2);
}

inline bool uint64_less_than(const query_result& r1, const query_result& r2) {
    return qv_get_uint64(r1) < qv_get_uint64(r2);
}

inline bool uint64_greater_than(const query_result& r1, const query_result& r2) {
    return qv_get_uint64(r1) > qv_get_uint64(r2);
}

// ----------- query_result::double -----------
inline bool double_not_equal(const query_result& r1, const query_result& r2) {
    return qv_get_double(r1) != qv_get_double(r2);
}

inline bool double_greater_than(const query_result& r1, const query_result& r2) {
    return qv_get_double(r1) > qv_get_double(r2);
}

inline bool double_less_than(const query_result& r1, const query_result& r2) {
    return qv_get_double(r1) < qv_get_double(r2);
}

// ----------- query_result::string -----------
inline bool string_not_equal(const query_result& r1, const query_result& r2) {
    return qv_get_string(r1) != qv_get_string(r2);
}

inline bool string_equal(const query_result& r1, const query_result& r2) {
    return qv_get_string(r1) == qv_get_string(r2);
}

inline bool string_greater_than(const query_result& r1, const query_result& r2) {
    return qv_get_string(r1) > qv_get_string(r2);
}

inline bool string_less_than(const query_result& r1, const query_result& r2) {
    return qv_get_string(r1) < qv_get_string(r2);
}

// ----------- query_result -----------
bool qv_equal(const query_result& qr1, const query_result& qr2) {
    if (qr1.which() == qr2.which()) {
        switch (qr1.which()) {
            case node_ptr_type: // node *
                return qv_get_node(qr1) == qv_get_node(qr2);
            case rship_ptr_type: // relationship *
                return qv_get_relationship(qr1) == qv_get_relationship(qr2);
                break;
            case int_type: // int
                return qv_get_int(qr1) == qv_get_int(qr2);
            case double_type: // double
                return qv_get_double(qr1) == qv_get_double(qr2);
            case string_type: // std::string
                return string_equal(qr1, qr2);
            case uint64_type: // uint64_t
                return qv_get_uint64(qr1) == qv_get_uint64(qr2);
            case ptime_type: 
                return qv_get_ptime(qr1) == qv_get_ptime(qr2);
            case null_type:
                return true;
            default:
                break;
        }
    } 
    else if (qr1.which() == int_type && qr2.which() == uint64_type)
        return (uint64_t)qv_get_int(qr1) == qv_get_uint64(qr2);
    else if (qr1.which() == uint64_type && qr2.which() == int_type)
        return qv_get_uint64(qr1) == (uint64_t)qv_get_int(qr2);
   else if (qr1.which() == double_type && qr2.which() == uint64_type)
        return qv_get_double(qr1) == (double)qv_get_uint64(qr2);
    else if (qr1.which() == uint64_type && qr2.which() == double_type)
        return (double)qv_get_uint64(qr1) == qv_get_double(qr2);
    else if (qr1.which() == double_type && qr2.which() == int_type)
        return qv_get_double(qr1) == (double)qv_get_int(qr2);
    else if (qr1.which() == int_type && qr2.which() == double_type)
        return (double)qv_get_int(qr1) == qv_get_double(qr2);
    return false;
}

bool less_than(const query_result& qr1, const query_result& qr2) {
    if (qr1.which() == qr2.which()) {
        switch (qr1.which()) {
            case node_ptr_type: // node *
            case rship_ptr_type: // relationship *
                break;
            case int_type: // int
                return qv_get_int(qr1) < qv_get_int(qr2);
            case double_type: // double
                return qv_get_double(qr1) < qv_get_double(qr2);
            case string_type: // std::string
                return string_less_than(qr1, qr2);
            case uint64_type: // uint64_t
                return qv_get_uint64(qr1) < qv_get_uint64(qr2);
            case ptime_type:
                return qv_get_ptime(qr1) < qv_get_ptime(qr2);
            default:
                break;
        }
    } 
    else if (qr1.which() == int_type && qr2.which() == uint64_type)
        return (uint64_t)qv_get_int(qr1) <qv_get_uint64(qr2);
    else if (qr1.which() == uint64_type && qr2.which() ==int_type)
        return qv_get_uint64(qr1) < (uint64_t)qv_get_int(qr2);
   else if (qr1.which() == double_type && qr2.which() == uint64_type)
        return qv_get_double(qr1) < (double)qv_get_uint64(qr2);
    else if (qr1.which() == uint64_type && qr2.which() == double_type)
        return (double)qv_get_uint64(qr1) < qv_get_double(qr2);
    else if (qr1.which() == double_type && qr2.which() == int_type)
        return qv_get_double(qr1) < (double)qv_get_int(qr2);
    else if (qr1.which() == int_type && qr2.which() == double_type)
        return (double)qv_get_int(qr1) < qv_get_double(qr2);    
    return false;
}

/*
bool less_or_equal(const query_result& qr1, const query_result& qr2) {
    return less_than(qr1, qr2) || qv_equal(qr1, qr2);
}
*/
bool greater_than(const query_result& qr1, const query_result& qr2) {
    if (qr1.which() == qr2.which()) {
        switch (qr1.which()) {
            case node_ptr_type: // node *
            case rship_ptr_type: // relationship *
                break;
            case int_type: // int
                return qv_get_int(qr1) > qv_get_int(qr2);
            case double_type: // double
               return qv_get_double(qr1) > qv_get_double(qr2);
            case string_type: // std::string
                return string_greater_than(qr1, qr2);
            case uint64_type: // uint64_t
                return qv_get_uint64(qr1) > qv_get_uint64(qr2);
            case ptime_type: // uint64_t
                return qv_get_ptime(qr1) > qv_get_ptime(qr2);
            default:
                break;
        }
    } 
    else if (qr1.which() == int_type && qr2.which() == uint64_type)
        return (uint64_t)qv_get_int(qr1) > qv_get_uint64(qr2);
    else if (qr1.which() == uint64_type && qr2.which() == int_type)
        return qv_get_uint64(qr1) > (uint64_t)qv_get_int(qr2);
    else if (qr1.which() == double_type && qr2.which() == uint64_type)
        return qv_get_double(qr1) > (double)qv_get_uint64(qr2);
    else if (qr1.which() == uint64_type && qr2.which() == double_type)
        return (double)qv_get_uint64(qr1) > qv_get_double(qr2);
    else if (qr1.which() == double_type && qr2.which() == int_type)
        return qv_get_double(qr1) > (double)qv_get_int(qr2);
    else if (qr1.which() == int_type && qr2.which() == double_type)
        return (double)qv_get_int(qr1) > qv_get_double(qr2);
    return false;
}

/*
bool greater_or_equal(const query_result& qr1, const query_result& qr2) {
    return greater_than(qr1, qr2) || qv_equal(qr1, qr2);
}
*/