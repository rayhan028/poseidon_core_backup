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
#ifdef USE_PFILE
#include <boost/filesystem.hpp>
#endif
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
  // (1) we create a new b+tree
  #if USE_PMDK
    auto pop = pmem::obj::pool_by_vptr(this);
    btree_ptr new_idx;
      pmem::obj::transaction::run(pop, [&] {
        new_idx = p_make_btree();
      });
#elif defined(USE_PFILE)
  auto file_id = index_map_->size() + RPROPS_FILE_ID + 1;
  auto idx_file = std::make_shared<paged_file>();
  std::string prefix = pool_path_;
  if (prefix.length() > 0) prefix += "/";
  prefix += database_name_;
  idx_file->open(prefix + "/" + "idx_" + node_label + "$" + prop_name + ".db", file_id);
  bpool_.register_file(file_id, idx_file);
  index_files_.push_back(idx_file);
  auto new_idx = p_make_btree(bpool_, file_id);
  spdlog::debug("create_index #{}: fill index: {}", file_id, prop_name);
#else
  auto new_idx = p_make_btree();
#endif
  auto pc = dict_->lookup_string(prop_name);

  // (2) we fill the index with (property value, node-id) pairs
  nodes_by_label(node_label, [this, &new_idx, &pc](auto& n) {
    // spdlog::info("get property value for node #{}...", n.id());
    auto val = node_properties_->property_value(n.property_list, pc);
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

void graph_db::index_lookup(std::list<index_id> &idx_ptrs, uint64_t key, node_consumer_func consumer) {
  for (auto &idx_ptr : idx_ptrs) {
    offset_t val = 0;
    if (idx_ptr->lookup(key, &val)) {
      auto& n = node_by_id(val);
      consumer(n);
      break;
    }
  }
}

#ifdef USE_PFILE
void graph_db::restore_indexes(const std::string &pool_path, const std::string &prefix) {
  // forall files in prefix with idx_
  boost::filesystem::path path_obj {pool_path};
  path_obj /= prefix;
  path_obj /= "/";
  for (auto const& dir_entry : boost::filesystem::directory_iterator{path_obj}) {
    auto pname = dir_entry;
    auto file_name = pname.path().filename().string();
    if (! file_name.starts_with("idx_")) {
      continue;
    }
    auto pos = file_name.find("$");
    if (pos == std::string::npos)
      continue;
    std::string node_label = file_name.substr(4, pos - 4);
    auto pos2 = file_name.find(".");
    if (pos2 == std::string::npos)
      continue;
    std::string prop_name = file_name.substr(pos + 1, pos2 - pos - 1);

    auto file_id = index_map_->size() + RPROPS_FILE_ID + 1;
    auto idx_file = std::make_shared<paged_file>();
    idx_file->open(path_obj.string() + file_name, file_id);
    bpool_.register_file(file_id, idx_file);
    index_files_.push_back(idx_file);
    spdlog::debug("restore index {} : {} from {} @{}", node_label, prop_name, file_name, file_id);
    auto new_idx = p_make_btree(bpool_, file_id);
    index_map_->register_index(node_label + ":" + prop_name, new_idx);
  }
}
#endif