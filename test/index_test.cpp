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

#include <set>

#include "catch.hpp"
#include "config.h"
#include "graph_db.hpp"
#include "graph_pool.hpp"
#include "../qop/qop.hpp"

const std::string test_path = poseidon::gPmemPath + "index_test";

TEST_CASE("Creating an index on nodes", "[index]") {
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("my_graph");

  graph->run_transaction([&]() {
    for (int i = 0; i < 100; i++) {
      graph->add_node("Person",
                              {{"name", boost::any(std::string("John Doe"))},
                               {"age", boost::any(42)},
                               {"id", boost::any(i)},
                               {"dummy1", boost::any(std::string("Dummy"))},
                               {"dummy2", boost::any(1.2345)}},
                              true);
    }
    return true;
  });

  index_id idx; 
  graph->run_transaction([&]() {
    idx = graph->create_index("Person", "id");
    return true;
  });
  
  graph->begin_transaction();

  REQUIRE(graph->get_index("Person", "id"));
  CHECK_THROWS_AS(graph->get_index("Actor", "id"), unknown_index);

  bool found = false;
  graph->index_lookup(idx, 55u, [&found](auto& n) {
    found = true;
  });
  REQUIRE(found);

  graph->commit_transaction();

  graph_pool::destroy(pool);
}
