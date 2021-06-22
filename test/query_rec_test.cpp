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
#include "graph_db.hpp"
#include "graph_pool.hpp"

#include <set>
#include <iostream>
#include <boost/variant.hpp>

const std::string test_path = poseidon::gPmemPath + "query_recovery_test";

void init_graph(graph_db_ptr &gdb) {
    gdb->begin_transaction();
    for(int i = 0; i < 10; i++) {
        gdb->add_node("Person", {});
    }
    gdb->commit_transaction();
}

TEST_CASE("Store checkpoint test") {
#ifdef QOP_RECOVERY
    auto pool = graph_pool::create(test_path);
    auto graph = pool->create_graph("my_graph");

    init_graph(graph);

    auto iter = graph->get_nodes()->range(0,0);
    for(int i = 0; i < 6; i++) {

        REQUIRE(iter.get_cur_chunk() == 0);
        REQUIRE(iter.get_cur_pos() == i);

        graph->store_iter({iter.get_cur_chunk(), iter.get_cur_pos()});
        auto checkpoints = graph->get_query_checkpoints();

        int j = 0;
        for(auto & cp : *checkpoints) {
            REQUIRE(cp.first == 0);
            REQUIRE(cp.second == i);
        }

        ++iter;
    }

    graph_pool::destroy(pool);

#else
    REQUIRE(true);
#endif
}