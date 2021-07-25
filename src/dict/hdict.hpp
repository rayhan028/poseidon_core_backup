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
#ifndef dict_hpp
#define dict_hpp

#include <string>
#include <memory>
#include "defs.hpp"
#include "string_pool.hpp"
#include "htable.hpp"

#ifdef USE_MMFILE
#include "mm_file.hpp"
#endif

class dict {
public:
    dict(const std::string& prefix = "", uint32_t init_pool_size = 100000);
    ~dict();
    
    void initialize();

    dcode_t insert(const std::string& s);
    dcode_t lookup_string(const std::string& s) const;
    const char* lookup_code(dcode_t code) const;
    
    void print_pool() const;
    void resize();
    
    std::size_t size() const;

private:
#ifdef USE_MMFILE
    mm_file dict_file_;
#endif
    p_ptr<string_pool> pool_;
    std::unique_ptr<htable> table_;
    std::mutex m_;
};

#endif /* dict_hpp */
