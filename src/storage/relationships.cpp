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

#include "relationships.hpp"
#include "thread_pool.hpp"
#include <iostream>
#include <sstream>

#define PARALLEL_INIT

std::ostream &operator<<(std::ostream &os, const rship_description &rdescr) {
  os << ":" << rdescr.label << "[" << rdescr.id << "]{"; // OpenCypher 9.0
  bool first = true;
  for (auto &p : rdescr.properties) {
    if (!first)
      os << ", ";
    os << p.first << ": " << p.second;
    first = false;
  }
  os << "}";
  return os;
}

/* ------------------------------------------------------------------------ */

std::string rship_description::to_string() const {
  std::ostringstream os;
  os << *this;
  return os.str();
}

bool rship_description::has_property(const std::string& pname) const {
  return properties.find(pname) != properties.end();
}

/* ------------------------------------------------------------------------ */


struct init_rship_task {
  using range = std::pair<std::size_t, std::size_t>;
  init_rship_task(relationship_list &r, std::size_t first, std::size_t last)
      : rships_(r), range_(first, last) {}

  void operator()() {
    auto iter = rships_.range(range_.first, range_.second);
    while (iter) {
      auto &r = *iter;
      r.runtime_initialize();
     ++iter;
    }
  }

  relationship_list &rships_;
  range range_;
};

void relationship_list::runtime_initialize() {
  // make sure that all locks are released and no dirty objects exist
#ifdef PARALLEL_INIT
  const int nchunks = 100;
  std::vector<std::future<void>> res;
  res.reserve(num_chunks() / nchunks + 1);
  // spdlog::info("starting {} init tasks for rships...", num_chunks() / nchunks + 1);
  thread_pool pool;
  std::size_t start = 0, end = nchunks - 1;
  while (start < num_chunks()) {
    res.push_back(pool.submit(
        init_rship_task(*this, start, end)));
    // spdlog::info("starting: {}, {}", start, end);
    start = end + 1;
    end += nchunks;
  }
 // std::cout << "waiting ..." << std::endl;
  for (auto &f : res)
    f.get();
#else
  for (auto &r : rships_) {
    r.runtime_initialize();
  }
#endif
}

relationship::id_t relationship_list::append(relationship &&r, xid_t owner, std::function<void(offset_t)> callback) {
  auto p = rships_.append(std::move(r), callback);
  p.second->id_ = p.first;
  if (owner != 0) {
    /// spdlog::info("lock relationship #{} by {}", p.first, owner);
    p.second->lock(owner);
  }
  return p.first;
}

relationship::id_t relationship_list::insert(relationship &&r, xid_t owner, std::function<void(offset_t)> callback) {
  auto p = rships_.store(std::move(r), callback);
  p.second->id_ = p.first;
  if (owner != 0) {
    /// spdlog::info("lock relationship #{} by {}", p.first, owner);
    p.second->lock(owner);
  }
  return p.first;
}

relationship::id_t relationship_list::add(relationship &&r, xid_t owner) {
  if (rships_.is_full())
    rships_.resize(1);

  auto id = rships_.first_available();
  assert(id != UNKNOWN);
  r.id_ = id;
  if (owner != 0) {
    /// spdlog::info("lock relationship #{} by {}", id, owner);
    r.lock(owner);
  }
  rships_.store_at(id, std::move(r));
  return id;
}

relationship &relationship_list::get(relationship::id_t id) {
  if (rships_.capacity() <= id)
    throw unknown_id();
  auto &r = rships_.at(id);
  return r;
}

void relationship_list::remove(relationship::id_t id) {
  if (rships_.capacity() <= id)
    throw unknown_id();
  rships_.erase(id);
}



relationship_list::~relationship_list() {}

relationship &
relationship_list::last_in_from_list(relationship::id_t id) {
  relationship *rship = &get(id);
  while (rship->next_src_rship != UNKNOWN) {
    rship = &get(rship->next_src_rship);
  }
  return *rship;
}

relationship &
relationship_list::last_in_to_list(relationship::id_t id) {
  relationship *rship = &get(id);
  while (rship->next_dest_rship != UNKNOWN) {
    rship = &get(rship->next_dest_rship);
  }
  return *rship;
}

void relationship_list::dump() {
  std::cout << "------- RELATIONSHIPS -------\n";
  for (auto& r : rships_) {
    std::cout << "#" << r.id() 
      << " [ txn-id=" << short_ts(r.txn_id()) << ", bts=" << short_ts(r.bts())
              << ", cts=" << short_ts(r.cts()) << ", dirty=" << r.d_->is_dirty_ 
              << " ], label = " << r.rship_label << ", " << r.src_node
              << "->" << r.dest_node << ", next_src=" << uint64_to_string(r.next_src_rship) << ", next_dest="
              << uint64_to_string(r.next_dest_rship);
    if (r.has_dirty_versions()) {
      // print dirty list
      std::cout << " {\n";
       for (const auto& dr : *(r.dirty_list())) {
        std::cout << "\t( @" << (unsigned long)&(dr->elem_)
                  << ", txn-id=" << short_ts(dr->elem_.txn_id())
                  << ", bts=" << short_ts(dr->elem_.bts()) << ", cts=" << short_ts(dr->elem_.cts())
                  << ", label=" << dr->elem_.rship_label
                  << ", dirty=" << dr->elem_.is_dirty();
        std::cout << " ])\n";
      }
      std::cout << "}";
    }
    std::cout << "\n";
  }
  std::cout << "-----------------------------\n";
}
