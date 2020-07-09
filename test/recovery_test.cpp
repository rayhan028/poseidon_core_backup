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

#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do
                          // this in one cpp file

#include "catch.hpp"
#include "config.h"
#include "defs.hpp"
#include "graph_db.hpp"
#include "graph_pool.hpp"

#include <boost/process.hpp>

#if defined USE_PMDK && defined USE_LOGGING
#define PMEMOBJ_POOL_SIZE ((size_t)(1024 * 1024 * 80))

// namespace nvm = pmem::obj;
const std::string test_path = poseidon::gPmemPath + "recovery_test";

namespace bp = boost::process;

TEST_CASE("Recovery of aborted inserts", "[graph_db]") {
    char buf[1024];
    remove(test_path.c_str());
    spdlog::info("getcwd {}", getcwd(buf, 1024)); 
    std::string prog(buf);
    prog += "/abort_insert";
    int result = bp::system(prog, test_path, "my_graph");
    REQUIRE(result == 0);

    auto pool = graph_pool::open(test_path);
    auto graph = pool->open_graph("my_graph");

    // check that inserts don't exist anymore
    auto tx = graph->begin_transaction();
    int num_nodes = 0;
    graph->nodes([&](auto& n) { num_nodes++; });
    REQUIRE(num_nodes == 1);

    // but also that the slots are not occupied
    auto nid = graph->add_node(":Person", {{"number", boost::any(56)}});
    REQUIRE(nid == 1);
    graph->abort_transaction();
}

 #endif

