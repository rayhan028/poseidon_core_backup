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
