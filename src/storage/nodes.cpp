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
#include "nodes.hpp"
#include <iostream>
#include <sstream>

#include "spdlog/spdlog.h"

template <class T>
bool output_any(std::ostream &os, const boost::any &any_value) {
  try {
    T v = boost::any_cast<T>(any_value);
    os << v;
    return true;
  } catch (boost::bad_any_cast &e) {
    return false;
  }
}

template <>
bool output_any<std::string>(std::ostream &os, const boost::any &any_value) {
  try {
    auto v = boost::any_cast<std::string>(any_value);
    os << '"' << v << '"';
    return true;
  } catch (boost::bad_any_cast &e) {
    return false;
  }
}

template <>
bool output_any<const char *>(std::ostream &os, const boost::any &any_value) {
  try {
    auto v = boost::any_cast<const char *>(any_value);
    os << '"' << v << '"';
    return true;
  } catch (boost::bad_any_cast &e) {
    return false;
  }
}
std::ostream &operator<<(std::ostream &os, const boost::any &any_value) {
  // list all types you want to try
  if (!output_any<int>(os, any_value))
    if (!output_any<double>(os, any_value))
      if (!output_any<bool>(os, any_value))
        if (!output_any<std::string>(os, any_value))
          if (!output_any<const char *>(os, any_value))
            if (!output_any<uint64_t>(os, any_value))
              if (!output_any<boost::posix_time::ptime>(os, any_value))
                os << "{unknown}"; // all cast are failed, we have a unknown type of any
  return os;
}

std::ostream &operator<<(std::ostream &os, const node_description &ndescr) {
  //  os << "(#" << ndescr.id << ":" << ndescr.label << ", {";
  os << ndescr.label << "[" << ndescr.id << "]{"; // OpenCypher 9.0
  bool first = true;
  for (auto &p : ndescr.properties) {
    if (!first)
      os << ", ";
    os << p.first << ": " << p.second;
    first = false;
  }
  //  os << "})";
  os << "}";
  return os;
}

/* ------------------------------------------------------------------------ */

std::string node_description::to_string() const {
  std::ostringstream os;
  os << *this;
  return os.str();
}

/* ------------------------------------------------------------------------ */

node_list::~node_list() {
}

void node_list::runtime_initialize() {
  // make sure that all locks are released and no dirty objects exist
  for (auto &n : nodes_) {
    n.runtime_initialize();
  }
}

node::id_t node_list::append(node &&n, xid_t owner, std::function<void(offset_t)> callback) {
  auto p = nodes_.append(std::move(n), callback);
  p.second->id_ = p.first;
  if (owner != 0) {
    /// spdlog::info("lock node #{} by {}", p.first, owner);
    p.second->lock(owner);
  }

  return p.first;
}

node::id_t node_list::insert(node &&n, xid_t owner, std::function<void(offset_t)> callback) {
  auto p = nodes_.store(std::move(n), callback);
  p.second->id_ = p.first;
  if (owner != 0) {
    /// spdlog::info("lock node #{} by {}", p.first, owner);
    p.second->lock(owner);
  }

  return p.first;
}

node::id_t node_list::add(node &&n, xid_t owner) {
  if (nodes_.is_full())
    nodes_.resize(1);

  auto id = nodes_.first_available();
  assert(id != UNKNOWN);
  n.id_ = id;
  if (owner != 0) {
    /// spdlog::info("lock node #{} by {}", id, owner);
    n.lock(owner);
  }
  nodes_.store_at(id, std::move(n));
  return id;
}

node &node_list::get(node::id_t id) {
  if (nodes_.capacity() <= id)
    throw unknown_id();
  auto &n = nodes_.at(id);
  return n;
}

void node_list::remove(node::id_t id) {
  if (nodes_.capacity() <= id)
    throw unknown_id();
  nodes_.erase(id);
}

void node_list::dump() {
  std::cout << "----------- NODES -----------\n";
  for (auto& n : nodes_) {
    std::cout << "#" << n.id() << ", @" << (unsigned long)&n
              << " [ txn-id=" << short_ts(n.txn_id()) << ", bts=" << short_ts(n.bts())
              << ", cts=" << short_ts(n.cts()) << ", dirty=" << n.d_->is_dirty_ 
              << " ], label=" << n.node_label << ", from="
              << n.from_rship_list << ", to=" << n.to_rship_list << ", props="
              << n.property_list;
    if (n.has_dirty_versions()) {
      // print dirty list
      std::cout << " {\n";
      for (const auto& dn : *(n.dirty_list())) {
        std::cout << "\t( @" << (unsigned long)&(dn->elem_)
                  << ", txn-id=" << short_ts(dn->elem_.txn_id())
                  << ", bts=" << short_ts(dn->elem_.bts()) << ", cts=" << short_ts(dn->elem_.cts())
                  << ", label=" << dn->elem_.node_label
                  << ", dirty=" << dn->elem_.is_dirty()
                  << ", from=" << dn->elem_.from_rship_list
                  << ", to=" << dn->elem_.to_rship_list
                  << ", [";
        for (const auto& pi : dn->properties_) {
          std::cout << " " << pi;
        }
        std::cout << " ])\n";
      }
      std::cout << "}";
    }
    std::cout << "\n";
  }
  std::cout << "-----------------------------\n";
}
