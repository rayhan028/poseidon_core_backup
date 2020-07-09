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

#include "graph_db.hpp"

#ifdef USE_PMDK
namespace nvm = pmem::obj;
#endif

void graph_db::apply_undo_log() {
#if defined USE_PMDK && defined USE_LOGGING
  spdlog::info("checking undo log...");
  ulog_->dump();
  auto pop = pmem::obj::pool_by_vptr(this);

  for (auto li = ulog_->log_begin(); li != ulog_->log_end(); ++li) {
    if (!li.valid())
      continue;

    spdlog::info("recovery: start for log #{}", li.txid());
    for (auto l = li.begin(); l != li.end(); ++l) {
      if (!l.valid())
	      continue;

      if (l.log_type() == pmem_log::log_insert) {
        auto rec = l.get<log_ins_record>();
        if (rec->obj_type == pmem_log::log_node) {
          // delete the node from the nodes_ list
          spdlog::info("recovery: undo insert node {}", rec->oid);
          nvm::transaction::run(pop, [&] { nodes_->remove(rec->oid); });
        } else if (rec->obj_type == pmem_log::log_rship) {
          // delete the relationship from the rships_ list
          spdlog::info("recovery: undo insert rship {}", rec->oid);
          nvm::transaction::run(pop, [&] { rships_->remove(rec->oid); });
        } else if (rec->obj_type == pmem_log::log_property) {
          // TODO
          spdlog::info("recovery: undo insert property {}", rec->oid);
        }
        // mark the log entry as done
        nvm::transaction::run(pop, [&] { l.set_invalid(); });
      } else if (l.log_type() == pmem_log::log_update) {
        if (l.obj_type() == pmem_log::log_node) {
          auto rec = l.get<log_node_record>();
          // TODO
        } else if (l.obj_type() == pmem_log::log_rship) {
          auto rec = l.get<log_rship_record>();
          // TODO
        } else if (l.obj_type() == pmem_log::log_property) {
          auto rec = l.get<log_property_record>();
          // TODO
        }
      } else if (l.log_type() == pmem_log::log_delete) {
      }
    }
    spdlog::info("recovery finished for {}", li.txid());
    // mark li invalid
    li.set_invalid();
  }
  // TODO: cleanup ulog_
#endif
}
