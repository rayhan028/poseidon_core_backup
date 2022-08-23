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
#include "exceptions.hpp"
#include "index_map.hpp"
#include "spdlog/spdlog.h"

index_map::index_map() {
#ifdef USE_PMDK
    auto pop = pmem::obj::pool_by_vptr(this);
    pmem::obj::transaction::run(pop, [&] {
        indexes_ = pmem::obj::make_persistent<hashmap>();
    });
#endif
}

index_map::~index_map() {
#ifdef USE_PMDK
    auto pop = pmem::obj::pool_by_vptr(this);
    pmem::obj::transaction::run(pop, [&] {
        pmem::obj::delete_persistent<hashmap>(indexes_);
    });
#endif
}

void index_map::register_index(const std::string& idx_name, index_id idx) {
#ifdef USE_PMDK
    string_t str(idx_name);
    hashmap::accessor ac;
    indexes_->insert(ac, str);
    ac->second = idx;
    ac.release();
#else
    indexes_.insert({ idx_name, idx });
#endif
}

void index_map::unregister_index(const std::string& idx_name) {
#ifdef USE_PMDK
    // TODO
#else
    auto it = indexes_.find(idx_name);
    if (it == indexes_.end())
        throw unknown_index();
    indexes_.erase(it);
#endif
}

index_id index_map::get_index(const std::string& idx_name) {
#ifdef USE_PMDK
    hashmap::const_accessor ac;
    if (indexes_->find(ac, string_t(idx_name)))
        return ac->second;
    else
        throw unknown_index();
#else
    auto it = indexes_.find(idx_name);
    if (it == indexes_.end())
        throw unknown_index();
    return it->second;
#endif
}

index_id index_map::get_index_id(const std::string& idx_name) {
#ifdef USE_PMDK
    hashmap::const_accessor ac;
    if (indexes_->find(ac, string_t(idx_name)))
        return ac->second;
    else
        return boost::blank{};
#else
    auto it = indexes_.find(idx_name);
    if (it == indexes_.end())
        return boost::blank{};
    return it->second;
#endif    
}

bool index_map::has_index(const std::string& idx_name) {
#ifdef USE_PMDK
    hashmap::const_accessor ac;
    return indexes_->find(ac, string_t(idx_name));
#else
    auto it = indexes_.find(idx_name);
    return it != indexes_.end();
#endif
}
