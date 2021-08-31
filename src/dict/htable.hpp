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
#ifndef htable_hpp
#define htable_hpp

#include <string>
#include <limits>
#include "defs.hpp"

class string_pool;

constexpr uint32_t UNKNOWN_CODE = std::numeric_limits<uint32_t>::max();

class htable {
    friend class dict;
public:
    htable(p_ptr<string_pool> pool, uint32_t nb = 1000);
    ~htable();
    
    dcode_t find(const std::string& s);
    dcode_t get(dcode_t id);
    dcode_t insert(const std::string& s, dcode_t id);
    
    void print() const;
    std::size_t size() const { return nelems_; }
    void rebuild();

private:
    dcode_t insert_into_table(uint64_t *tbl, uint32_t tsize, uint64_t hkey, dcode_t id);
    void resize();
    
    p_ptr<string_pool> pool_;
    uint32_t nbuckets_;
    uint64_t *table_;
    uint32_t prime_;
    uint32_t nelems_;
};

#endif /* htable_hpp */
