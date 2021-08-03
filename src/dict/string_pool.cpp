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
#include <cstdlib>
#include "string_pool.hpp"

string_pool::string_pool(uint32_t init_size, uint32_t exp_size) : size_(init_size), expand_(exp_size), last_(1ul) {
#ifdef USE_PMDK
    auto pop = pmem::obj::pool_by_vptr(this);
    pmem::obj::transaction::run(pop, [&] {
        pool_ = pmem::obj::make_persistent<char[]>(init_size);
      });
#else
    pool_ = static_cast<char *>(malloc(init_size));
    memset(pool_, 0, init_size);
#endif
    pool_[0] = '#';
}

#ifdef USE_MMFILE
string_pool::string_pool(uint8_t *base_addr, std::size_t sz) : expand_(10000), base_addr_(base_addr) {
    // std::cout << "create string pool from mm_file...\n";
    memcpy(&size_, base_addr, sizeof(uint32_t));
    memcpy(&last_, base_addr + sizeof(uint32_t), sizeof(uint32_t));
    pool_ = (char *)base_addr + 2 * sizeof(uint32_t);
    if (pool_[0] != '#' && size_ == 0) {
        size_ = sz;
        last_ = 1;
    }
    pool_[0] = '#';
}
#endif

string_pool::~string_pool() {
#ifdef USE_MMFILE
    memcpy(base_addr_, &size_, sizeof(uint32_t));
    memcpy(base_addr_ + sizeof(uint32_t), &last_, sizeof(uint32_t));
#elif !defined(USE_PMDK)
    free(pool_);
#endif
}

void string_pool::scan(std::function<void(const char *s, dcode_t c)> cb) {
    auto pos = 1u;
    for (auto p = 1u; p < last_; p++) {
        if (pool_[p] == '\0') {
            cb(&pool_[pos], pos);
            pos = p + 1;
        }
    }    
}

const char *string_pool::extract(dcode_t pos) const {
    assert(pos < last_);
    auto p = pos;
    while (p < last_ && pool_[p++] != '\0') ;
    // return std::string(&pool_[pos], p - pos - 1);
    return &pool_[pos];
}

bool string_pool::equal(dcode_t pos, const std::string& s) const {
    auto i = 0u;
    for (; i < s.length() && i + pos < last_; i++)
        if (pool_[pos + i] != s.at(i))
            return false;
    return pool_[pos + i] == '\0';
}

dcode_t string_pool::add(const std::string& str) {
    auto pos = last_;
    if (last_ + str.length() + 1 >= size_) {
#if USE_MMFILE
        std::cerr << "FATAL: cannot grow memory mapped file for dictionary - aborting." << std::endl;
#else
        auto old_size = size_;
        size_ += expand_;
        // std::cout << "expand to " << size_ << std::endl;
#ifdef USE_PMDK
    auto pop = pmem::obj::pool_by_vptr(this);
    p_ptr<char []> new_pool;
    pmem::obj::transaction::run(pop, [&] {
        new_pool = pmem::obj::make_persistent<char[]>(size_);
        pmemobj_memcpy_persist(pop.handle(), new_pool, pool_, old_size);
        pmem::obj::delete_persistent<char[]>(pool_);
    });
    pool_  = new_pool;
#else
    pool_ = static_cast<char *>(realloc(pool_, size_));
#endif
#endif
    }
    memcpy(&pool_[last_], str.c_str(), str.length());
    last_ += str.length() + 1;
    pool_[last_ - 1] = '\0';
    return pos;
}

void string_pool::print() const {
#ifdef USE_PMDK
#else
    std::cout << std::string(pool_, last_) << std::endl;
#endif
}
