/*
 * Copyright (C) 2019-2021 DBIS Group - TU Ilmenau, All Rights Reserved.
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
#include <iostream>
#include "hdict.hpp"

dict::dict(const std::string& prefix, uint32_t init_pool_size)
#ifdef USE_MMFILE
   : dict_file_(prefix + "dict.db", init_pool_size)
#endif
 {
#ifdef USE_MMFILE
    uint8_t *base_addr = static_cast<uint8_t *>(dict_file_.base_address());
    std::size_t region_size = dict_file_.size();
    pool_ = std::make_shared<string_pool>(base_addr, region_size);
#else
    pool_ = p_make_ptr<string_pool>(init_pool_size);
#endif
    table_ = std::make_unique<htable>(pool_, 50000);
    table_->rebuild();
}

dict::~dict() {
#ifdef USE_MMFILE
    dict_file_.close();
#elif defined(USE_PMDK)
 auto pop = pmem::obj::pool_by_vptr(this);
  pmem::obj::transaction::run(pop, [&] {
    pmem::obj::delete_persistent<string_pool>(pool_);
#endif
}

void dict::initialize() {
    table_->rebuild();
}

std::size_t dict::size() const {
    return table_->size();    
}

dcode_t dict::insert(const std::string& s) {
    auto pos = table_->find(s);
    if (pos != UNKNOWN_CODE) {
        return table_->get(pos);
    }
    auto id = pool_->add(s);
    table_->insert(s, id);
    return id;
}

dcode_t dict::lookup_string(const std::string& s) const {
    auto k = table_->find(s);
    return k != UNKNOWN_CODE ? table_->get(k) : 0;
}

const char* dict::lookup_code(dcode_t code) const {
    return  pool_->extract(code);
}

void dict::print_pool() const {
    pool_->print();
    // table_->print();
}

void dict::resize() {
    table_->resize();
}
