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

#include "qoperator.hpp"
#include "queryc.hpp"


const std::string test_path = poseidon::gPmemPath + "jit_test";

#ifdef USE_PMDK

#define PMEMOBJ_POOL_SIZE ((size_t)(1024 * 1024 * 80))

namespace nvm = pmem::obj;

nvm::pool_base prepare_pool() {
	auto pop = nvm::pool_base::create(test_path, "", PMEMOBJ_POOL_SIZE);
	return pop;
}
#endif

TEST_CASE("Query the graph", "[jit_query_read]") {
#ifdef USE_PMDK
  auto pop = prepare_pool();
  graph_db_ptr graph;
  nvm::transaction::run(pop, [&] { graph = p_make_ptr<graph_db>(); });
#else
  auto graph = p_make_ptr<graph_db>();
#endif

#ifdef USE_TX
  auto tx = graph->begin_transaction();
#endif

  auto num_persons = 100;
  auto num_books = 42;

  for (int i = 0; i < num_persons; i++) {
    graph->add_node("Person",
                              {{"name", boost::any(std::string("John Doe"))},
                               {"age", boost::any(42)},
                               {"id", boost::any(i)},
                               {"dummy1", boost::any(std::string("Dummy"))},
                               {"dummy2", boost::any(1.2345)}},
                              true);
  }

  for (int i = 0; i < num_books; i++) {
    auto b = graph->add_node("Book",
                              {{"title", boost::any(std::string("Book Title"))},
                               {"year", boost::any(1942)},
                               {"id", boost::any(i)}},
                              true);
    graph->add_relationship(i, b, ":HAS_READ", {});
  }

#ifdef USE_TX
  graph->commit_transaction();
  tx = graph->begin_transaction();
#endif

  auto idx = graph->create_index("Person", "id");

#ifdef USE_TX
  graph->commit_transaction();
#endif

	auto chunks = graph->get_nodes()->num_chunks();

	query_engine queryEngine(graph, 1, chunks);

    SECTION("Scan all nodes for given label") {
        auto expr = Scan("Person", Collect());
        arg_builder args;
	    args.arg(1, "Person");

        result_set rs;
        queryEngine.generate(expr, false);
	    queryEngine.run(&rs, args.args);

        REQUIRE(rs.data.size() == num_persons);
        //REQUIRE(boost::get<std::string>(rs.data[43][0]) == "Person[42]{age: 42, dummy1: \"Dummy\", dummy2: 1.2345, id: 42, name: \"John Doe\"}");
    }

    SECTION("Find a outgoing relationship from each Person node") {
        auto expr = Scan("Person", ForeachRship(RSHIP_DIR::FROM, {}, ":HAS_READ", Collect()));
        arg_builder args;
        args.arg(1, "Person");
        args.arg(2, ":HAS_READ");

        result_set rs;
        queryEngine.generate(expr, false);
        queryEngine.run(&rs, args.args);

        REQUIRE(rs.data.size() == num_books);
        //REQUIRE(boost::get<std::string>(rs.data[43][0]) == "Person[42]{age: 42, dummy1: \"Dummy\", dummy2: 1.2345, id: 42, name: \"John Doe\"}");
        //REQUIRE(boost::get<std::string>(rs.data[43][1]) == "Book[142]{title: \"Book Title\", year: 1942, id: 42}");
    }

    SECTION("Find a ingoing relationship from each Book node") {
        auto expr = Scan("Book", ForeachRship(RSHIP_DIR::TO, {}, ":HAS_READ", Collect()));
        arg_builder args;
        args.arg(1, "Book");
        args.arg(2, ":HAS_READ");

        result_set rs;
        queryEngine.generate(expr, false);
        queryEngine.run(&rs, args.args);

        REQUIRE(rs.data.size() == num_books);
        //REQUIRE(boost::get<std::string>(rs.data[0][1]) == "Person[0]{age: 42, dummy1: \"Dummy\", dummy2: 1.2345, id: 0, name: \"John Doe\"}");
        //REQUIRE(boost::get<std::string>(rs.data[0][0]) == "Book[100]{title: \"Book Title\", year: 1942, id: 0}");
    }

    SECTION("Find the destination node for each relationship with the given label") {
        auto expr = Scan("Person", ForeachRship(RSHIP_DIR::FROM, {}, ":HAS_READ", Expand(EXPAND::OUT, "Book", Collect())));
        arg_builder args;
        args.arg(1, "Person");
        args.arg(2, ":HAS_READ");
        args.arg(3, "Book");

        result_set rs;
        queryEngine.generate(expr, false);
        queryEngine.run(&rs, args.args);

        REQUIRE(rs.data.size() == num_books);
    }

    SECTION("Find the source node for each relationship with the given label") {
        auto expr = Scan("Book", ForeachRship(RSHIP_DIR::TO, {}, ":HAS_READ", Expand(EXPAND::IN, "Person", Collect())));
        arg_builder args;
        args.arg(1, "Book");
        args.arg(2, ":HAS_READ");
        args.arg(3, "Person");

        result_set rs;
        queryEngine.generate(expr, false);
        queryEngine.run(&rs, args.args);

        REQUIRE(rs.data.size() == num_books);
    }

#ifdef USE_PMDK
	nvm::transaction::run(pop, [&] { nvm::delete_persistent<graph_db>(graph); });
	pop.close();
	remove(test_path.c_str());
#endif

}