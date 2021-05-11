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
#include "spdlog/spdlog.h"
#include "thread_pool.hpp"

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

#ifdef QOP_RECOVERY

void graph_db::store_query_result(qr_tuple &qr, std::size_t chunk) {
  recovery_results_->add(std::move(qr), chunk);
}

void graph_db::restore_results(std::list<qr_tuple> &result_list) {
  std::map<int, qr_tuple> result_map;

  for(auto & res : recovery_results_->as_vec()) {
    if(res.type_ == 0) {
      auto & n = node_by_id(res.res_);
      result_map[res.tuple_id_].push_back(&n);
    } else if(res.type_ == 1) {
      auto & r = rship_by_id(res.res_);
      result_map[res.tuple_id_].push_back(&r);
    } else if(res.type_ == 2) {
      result_map[res.tuple_id_].push_back((int)res.res_);
    } else if(res.type_ == 3) {
      std::string str = dict_->lookup_code(res.res_);
      result_map[res.tuple_id_].push_back(str);
    } else if(res.type_ == 4) {
      double d;
      std::memcpy(&d, &res.res_, sizeof(d));
      result_map[res.tuple_id_].push_back(d);
    }
  }

  for(auto & res : result_map) {
    result_list.push_back(res.second);
  }
}

void graph_db::store_iter(std::pair<std::size_t, std::size_t> iter_pos) {
  rec_map_t::accessor acc;
  recovery_res_->insert(acc, (int)(iter_pos.first));
  acc->second.get_rw() = iter_pos.second;
  acc.release();
}

std::map<std::size_t, std::size_t> graph_db::restore_positions() {
  std::map<std::size_t, std::size_t> cp;
  for(auto & x : *recovery_res_) {
    std::cout << x.first << " " << x.second << std::endl;
    cp[x.first] = x.second;
  }
  return cp;
}

struct continue_query_task {
  using range = std::pair<std::size_t, std::size_t>;
  continue_query_task(graph_db *gdb, node_list &n, std::size_t first, std::size_t last,
	    graph_db::node_consumer_func c, transaction_ptr tp = nullptr, std::size_t start_pos = 0);

  void operator()();
  
  graph_db *graph_db_;
  node_list &nodes_;
  range range_;
  graph_db::node_consumer_func consumer_;
  transaction_ptr tx_;
  std::size_t start_pos_;
};

continue_query_task::continue_query_task(graph_db *gdb, node_list &n, std::size_t first, std::size_t last, graph_db::node_consumer_func c, transaction_ptr tp, std::size_t start_pos)
	: graph_db_(gdb), nodes_(n), range_(first, last), consumer_(c), tx_(tp), start_pos_(start_pos) {}


void continue_query_task::operator()() {
  xid_t xid = 0;
  if (tx_) { // we need the transaction pointer in thread-local storage
	current_transaction_ = tx_;
	xid = tx_->xid();				    
    }
    auto iter = graph_db_->get_nodes()->range(range_.first, range_.second, start_pos_);
    
    while (iter) {
#ifdef USE_TX
	auto &n = *iter;
	if (n.is_valid()) {
	   auto &nv = graph_db_->get_valid_node_version(n, xid);
		consumer_(nv);
    graph_db_->store_iter({iter.get_cur_chunk(), iter.get_cur_pos()});
	}
#else
	consumer_(*iter);
#endif
	++iter;
    }
}

void graph_db::continue_parallel_nodes(std::map<std::size_t, std::size_t> &check_points, node_consumer_func consumer) {
#ifdef USE_TX
  check_tx_context();
#endif
  auto remaining_chunks = check_points.size();

  spdlog::info("Start parallel query with {} threads",
                remaining_chunks);

  std::vector<std::future<void>> res;
  res.reserve(remaining_chunks);
  thread_pool pool;
  
  for(auto & cp : check_points) {
    if(cp.second == NODE_CHUNK_SIZE) { //TODO: check
      continue;
    }
    res.push_back(
      pool.submit(
        continue_query_task(this, *nodes_, cp.first, cp.first, consumer, current_transaction_, cp.second)
      )
    );
  }

  for(auto &f : res) {
    f.get();
  }
}
#endif