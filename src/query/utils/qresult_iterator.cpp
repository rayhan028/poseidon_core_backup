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

#include <boost/hana.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "defs.hpp"
#include "nodes.hpp"
#include "relationships.hpp"
#include "qresult_iterator.hpp"

using namespace boost::posix_time;

void result_set::wait() {
  std::unique_lock<std::mutex> lock(m);
  cond_var.wait(lock, [&] { return ready.load(); });
}

void result_set::notify() {
  std::lock_guard<std::mutex> lock(m);
  ready = true;
  cond_var.notify_one();
}

bool result_set::operator==(const result_set &other) const {
  return data == other.data;
}

bool result_set::qr_compare(const qr_tuple &qr1, const qr_tuple &qr2,
                            const sort_spec &spec) {
  // TODO
  return true;
}

void result_set::sort(const sort_spec &spec) {
  data.sort([&](const qr_tuple &v1, const qr_tuple &v2) {
    return qr_compare(v1, v2, spec);
  });
}

void result_set::sort(
    std::function<bool(const qr_tuple &, const qr_tuple &)> cmp) {
  data.sort(cmp);
}

std::ostream &operator<<(std::ostream &os, const result_set &rs) {
  auto my_visitor = boost::hana::overload(
      [&](node *n) { /*os << gdb->get_node_description(*n); */ },
      [&](relationship *r) { /* os << gdb->get_relationship_label(*r); */ },
      [&](int i) { os << i; }, [&](double d) { os << d; },
      [&](const std::string &s) { os << s; }, [&](uint64_t ll) { os << ll; },
      [&](null_t n) { os << "NULL"; },
      [&](array_t arr) {
        os << "[ ";
        for (auto elem : arr.elems)
          os << elem << " ";
        os << " ]"; },
      [&](ptime dt) { os << dt; }); 

  for (const qr_tuple &qv : rs.data) {
    os << "{ ";

    auto i = 0u;
    for (const auto &qr : qv) {
      boost::apply_visitor(my_visitor, qr);
      if (++i < qv.size())
        os << ", ";
    }
    os << " }" << std::endl;
  }
  return os;
}
