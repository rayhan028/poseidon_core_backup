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

#include <boost/algorithm/string.hpp>

#include "graph_db.hpp"
#include "chunked_vec.hpp"
#include "parser.hpp"
#include "spdlog/spdlog.h"
#include "thread_pool.hpp"
#include <iostream>

#ifdef USE_PMDK
namespace nvm = pmem::obj;
#endif

index_id graph_db::create_index(const std::string& node_label, const std::string& prop_name) {
  // spdlog::info("create_index...");
  // (1) we create a new b+tree
  #if USE_PMDK
    auto pop = pmem::obj::pool_by_vptr(this);
    btree_ptr new_idx;
      pmem::obj::transaction::run(pop, [&] {
        new_idx = p_make_btree();
      });
#else
  auto new_idx = p_make_btree();
#endif
  auto pc = dict_->lookup_string(prop_name);

  // (2) we fill the index with (property value, node-id) pairs
  // spdlog::info("create_index: fill index: {} => {}", prop_name, pc);
  nodes_by_label(node_label, [this, &new_idx, &pc](auto& n) {
    // spdlog::info("get property value for node #{}...", n.id());
    auto val = properties_->property_value(n.property_list, pc);
    if (!val.empty()) {
      // because we don't distinguish differently typed indexes we use the raw value here
      auto v = val.get_raw(); // val.template get<int>();
      // spdlog::info("create_index: {} -> {}", v, n.id());      
      new_idx->insert(v, n.id());
    }
  });

  // (3) and register the index
  index_map_->register_index(node_label + ":" + prop_name, new_idx);

  return new_idx;
}

index_id graph_db::get_index(const std::string& node_label, const std::string& prop_name) {
  return index_map_->get_index(node_label + ":" + prop_name);
}

void graph_db::drop_index(const std::string& node_label, const std::string& prop_name) {
  auto idx_name = node_label + ":" + prop_name;
  auto idx = index_map_->get_index(idx_name);
  // TODO: delete idx
  index_map_->unregister_index(idx_name);
}

void graph_db::index_lookup(index_id idx_ptr, uint64_t key, node_consumer_func consumer) {
  offset_t val = 0;
  if (idx_ptr->lookup(key, &val)) {
    auto& n = node_by_id(val);
    consumer(n);
  }
}
