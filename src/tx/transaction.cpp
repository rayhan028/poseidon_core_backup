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

#include <cassert>
#include <chrono>

#include "spdlog/spdlog.h"

#include "transaction.hpp"

thread_local transaction_ptr current_transaction_;

constexpr auto max_logid = std::numeric_limits<std::size_t>::max();

// TODO: replace by atomic counter
xid_t now2Xid() {
  using namespace std::chrono;
  // Get current time with native precision
  auto now = system_clock::now();
  // Convert time_point to signed integral type
  return now.time_since_epoch().count();
}

transaction::transaction() : xid_(now2Xid()), logid_(max_logid) {}

void transaction::add_dirty_node(offset_t id) {
	dirty_nodes_.push_back(id);
}


void transaction::add_dirty_relationship(offset_t id) {
	dirty_rships_.push_back(id);
}

void check_tx_context() {
#ifdef USE_TX
  if (!current_transaction_)
    throw out_of_transaction_scope();
#endif
}

transaction_ptr current_transaction() { return current_transaction_; }

#if defined CSR_DELTA && defined USE_TX
  void transaction::add_deleted_node(offset_t nid) {
    delta_ids_.deleted_nodes_.push_back(nid);
    // nodes in deleted_nodes_ have all their neighbours deleted
    delta_ids_.deleted_neighbours_.erase(nid);
  }

  void transaction::add_deleted_neighbour(offset_t nid, offset_t id) {
    delta_ids_.deleted_neighbours_[nid].push_back(id);
  }

  void transaction::add_inserted_node(offset_t nid) {
    delta_ids_.inserted_neighbours_[nid];
  }

  void transaction::add_inserted_neighbour(offset_t nid, offset_t id, double weight) {
    delta_ids_.inserted_neighbours_[nid].first.push_back(id);
    delta_ids_.inserted_neighbours_[nid].second.push_back(weight);
  }

#if 0
  void transaction::add_updated_node(offset_t nid) {
    delta_ids_.updated_nodes_.push_back(nid);
  }

  void transaction::add_deleted_node(offset_t nid) {
    delta_ids_.deleted_nodes_.insert(nid);
  }

  void transaction::add_deleted_rship(offset_t rid) {
    delta_ids_.deleted_rships_.insert(rid);
  }
#endif

#endif