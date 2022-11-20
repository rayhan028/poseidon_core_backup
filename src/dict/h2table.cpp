/*
 * Copyright (C) 2019-2022 DBIS Group - TU Ilmenau, All Rights Reserved.
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
#include <functional>
#include <math.h>
#include "h2table.hpp"
#if defined(USE_PMDK) || defined(USE_IN_MEMORY)
#include "string_pool.hpp"
#else
#include "paged_string_pool.hpp"
#endif
#include "spdlog/spdlog.h"

void h2table::rebuild() {
    pool_->scan([this](const char *s, dcode_t c) {
        // std::cout << s << " -> " << c << std::endl;
        auto d = insert(std::string(s), c);
    });
}

void h2table::print() const {
}

dcode_t h2table::find(const std::string& s) {
    auto key = std::hash<std::string>{}(s);
    auto res = map_.find(key);
    return res != map_.end() ? res->second : UNKNOWN_CODE;
}

dcode_t h2table::get(dcode_t id) {
    return id;
}

dcode_t h2table::insert(const std::string& s, dcode_t id) {
    auto key = std::hash<std::string>{}(s);
    map_.emplace(key, id);
    return id;
}
