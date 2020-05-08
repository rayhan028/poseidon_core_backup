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

#ifndef graph_pool_hpp_
#define graph_pool_hpp_

#include "defs.hpp"
#include "graph_db.hpp"
#include "config.h"

#include <unordered_map>

class graph_pool;
using graph_pool_ptr = std::unique_ptr<graph_pool>;

class graph_pool {

public:
    static graph_pool_ptr create(const std::string& path, 
	unsigned long long pool_size = 1024*1024*40000ull);
    static graph_pool_ptr open(const std::string& path);
    static void destroy(graph_pool_ptr& p);

    ~graph_pool();

    graph_db_ptr create_graph(const std::string& name);
    graph_db_ptr open_graph(const std::string& name);

    void close();

private:
    graph_pool();

#ifdef USE_PMDK
    using hashmap = pmem::obj::concurrent_hash_map<string_t, graph_db_ptr, string_hasher>;
    p_ptr<hashmap> indexes_;

    struct root {
        p_ptr<hashmap> graphs_;
    };

    pmem::obj::pool<root> pop_;
    std::string path_;
#else
    std::unordered_map<std::string, graph_db_ptr> graphs_;
#endif
};

#endif
