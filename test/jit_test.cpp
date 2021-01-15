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
#include "query.hpp"


const std::string test_path = poseidon::gPmemPath + "jit_test";

#ifdef USE_PMDK

#define PMEMOBJ_POOL_SIZE ((size_t)(1024 * 1024 * 80))

namespace nvm = pmem::obj;

nvm::pool_base prepare_pool() {
	auto pop = nvm::pool_base::create(test_path, "", PMEMOBJ_POOL_SIZE);
	return pop;
}


TEST_CASE("Query the graph", "[jit_query_read]") {
#ifdef USE_PMDK
  auto pop = prepare_pool();
  graph_db_ptr graph;
  nvm::transaction::run(pop, [&] { graph = p_make_ptr<graph_db>(); });
#else
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("my_graph");
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
                               {"num", boost::any(uint64_t(1234567890123412))},
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

  graph->create_index("Person", "id");

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

    SECTION("Using an index to retrieve a certain node") {
        auto expr = IndexScan(Collect());
        arg_builder args;
        args.arg(1, "Person");
        args.arg(2, "id");
        args.arg(3, 42);

        result_set rs;
        queryEngine.generate(expr, false);
        queryEngine.run(&rs, args.args);

        REQUIRE(rs.data.size() == 1);
    }
/*
    SECTION("Find a outgoing relationship from each Person node") {
        auto expr = Scan("Person", ForeachRship(RSHIP_DIR::FROM, {}, ":HAS_READ", Collect()));
        arg_builder args;
        args.arg(1, "Person");
        args.arg(2, ":HAS_READ");

        result_set rs;
        queryEngine.generate(expr, false);
        queryEngine.run(&rs, args.args);

        REQUIRE(rs.data.size() == num_books);
        REQUIRE(boost::get<std::string>(rs.data.front()[0]) == "Person[0]{age: 42, dummy1: \"Dummy\", dummy2: 1.2345, id: 0, name: \"John Doe\", num: 1234567890123412}");
        REQUIRE(boost::get<std::string>(rs.data.front()[1]) == "::HAS_READ[0]{}");
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

    SECTION("Filter a node for a given property condition") {
        auto expr = Scan("Person", Filter(EQ(Key(0, "id"), Int(42)), Collect()));
        arg_builder args;
        args.arg(1, "Person");
        args.arg(2, 42);

        result_set rs;
        queryEngine.generate(expr, false);
        queryEngine.run(&rs, args.args);

        REQUIRE(rs.data.size() == 1);
        REQUIRE(boost::get<std::string>(rs.data.front()[0]) == "Person[42]{age: 42, dummy1: \"Dummy\", dummy2: 1.2345, id: 42, name: \"John Doe\", num: 1234567890123412}");
    }

    SECTION("Apply a Projection to a tuple result") {
        auto expr = Scan("Person", Filter(EQ(Key(0, "id"), Int(42)), Project({{0, "name", FTYPE::STRING}}, Collect())));
        arg_builder args;
        args.arg(1, "Person");
        args.arg(2, 42);

        result_set rs;
        queryEngine.generate(expr, false);
        queryEngine.run(&rs, args.args);

        REQUIRE(rs.data.size() == 1);
        REQUIRE(boost::get<std::string>(rs.data.front()[0]) == "John Doe");
    }

    SECTION("Apply a Projection on all tuple results") {
        auto expr = Scan("Person", Project({{0, "name", FTYPE::STRING}}, Collect()));
        arg_builder args;
        args.arg(1, "Person");

        result_set rs;
        queryEngine.generate(expr, false);
        queryEngine.run(&rs, args.args);

        REQUIRE(rs.data.size() == num_persons);
        REQUIRE(boost::get<std::string>(rs.data.back()[0]) == "John Doe");     
    }

    SECTION("CrossJoin two tuple results") {
        auto rhs = Scan("Book", End());
        auto lhs = Scan("Person", Join(JOIN_OP::CROSS, {}, Collect(), rhs));
        arg_builder args;
        args.arg(1, "Book");
        args.arg(2, "Person");

        result_set rs;
        queryEngine.generate(lhs, false);
        queryEngine.run(&rs, args.args);

        REQUIRE(rs.data.size() == num_persons * num_books);
    }

    TODO: Fix LeftJoin test
    SECTION("Find connected nodes between two results with a LeftJoin") {
        auto rhs = Scan("Book", End());
        auto lhs = Scan("Person", Join(JOIN_OP::LEFT_OUTER, {0,0}, Collect(), rhs));

        arg_builder args;
        args.arg(1, "Book");
        args.arg(2, "Person");

        result_set rs;
        queryEngine.generate(lhs, false);
        queryEngine.run(&rs, args.args);

        REQUIRE(rs.data.size() == num_persons * num_books * 2);
    }   
*/
#ifdef USE_PMDK
	nvm::transaction::run(pop, [&] { nvm::delete_persistent<graph_db>(graph); });
	pop.close();
	remove(test_path.c_str());
#else
   graph_pool::destroy(pool);
#endif

}
/*
TEST_CASE("Test the Projection operator", "[jit_query_projection]") {
#ifdef USE_PMDK
  auto pop = prepare_pool();
  graph_db_ptr graph;
  nvm::transaction::run(pop, [&] { graph = p_make_ptr<graph_db>(); });
#else
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("my_graph");
#endif

#ifdef USE_TX
  auto tx = graph->begin_transaction();
#endif

  auto num_persons = 100;
  auto num_books = 42;    
  for (int i = 0; i < num_persons; i++) {
    auto p = graph->add_node("Person",
                              {{"name", boost::any(std::string("John Doe"))},
                               {"age", boost::any(42)},
                               {"id", boost::any(i)},
                               {"num", boost::any(uint64_t(1234567890123412))},
                               {"dummy1", boost::any(std::string("Dummy"))},
                               {"dummy2", boost::any(1.2345)}},
                              true);
    if(i < num_books) {
      auto b = graph->add_node("Book",
                                {{"title", boost::any(std::string("Book Title"))},
                                {"year", boost::any(1942)},
                                {"id", boost::any(i)}},
                                true);
      graph->add_relationship(p, b, ":HAS_READ", {});
    }
  }

#ifdef USE_TX
  graph->commit_transaction();
#endif

	auto chunks = graph->get_nodes()->num_chunks();

	query_engine queryEngine(graph, 1, chunks);

    SECTION("Single Projection - string type") {
        auto expr = Scan("Person", Project({{0, "name", FTYPE::STRING}}, Collect()));
        arg_builder args;
	      args.arg(1, "Person");

        result_set rs;
        queryEngine.generate(expr, false);
	      queryEngine.run(&rs, args.args);

        REQUIRE(rs.data.size() == num_persons);
        REQUIRE(boost::get<std::string>(rs.data.front()[0]) == "John Doe");
    }

    SECTION("Single Projection - int type") {
        auto expr = Scan("Person", Project({{0, "age", FTYPE::INT}}, Collect()));
        arg_builder args;
        args.arg(1, "Person");

        result_set rs;
        queryEngine.generate(expr, false);
        queryEngine.run(&rs, args.args);

        REQUIRE(rs.data.size() == num_persons);
        REQUIRE(boost::get<int>(rs.data.front()[0]) == 42);
    }

    SECTION("Single Projection - uint64_t type ") {
        auto expr = Scan("Person", Project({{0, "num", FTYPE::UINT64}}, Collect()));
        arg_builder args;
        args.arg(1, "Person");

        result_set rs;
        queryEngine.generate(expr, false);
        queryEngine.run(&rs, args.args);

        REQUIRE(rs.data.size() == num_persons);
        REQUIRE(boost::get<uint64_t>(rs.data.front()[0]) == 1234567890123412);
    }

    SECTION("Multi Projection - string, int, uint64_t ") {
        auto expr = Scan("Person", Project({{0, "name", FTYPE::STRING}, 
                                            {0, "age", FTYPE::INT}, 
                                            {0, "num", FTYPE::UINT64}}, Collect()));
        arg_builder args;
        args.arg(1, "Person");

        result_set rs;
        queryEngine.generate(expr, false);
        queryEngine.run(&rs, args.args);

        REQUIRE(rs.data.size() == num_persons);
        REQUIRE(boost::get<std::string>(rs.data.front()[0]) == "John Doe");
        REQUIRE(boost::get<int>(rs.data.front()[1]) == 42);
        REQUIRE(boost::get<uint64_t>(rs.data.front()[2]) == 1234567890123412);
    }

    SECTION("Project over several query results") {
        auto r_expr = Scan("Book",  End());

        auto l_expr = Scan("Person", ForeachRship(RSHIP_DIR::FROM, {}, ":HAS_READ", 
                       Expand(EXPAND::OUT, "Book", Join(JOIN_OP::CROSS, {}, 
                        Project({{0, "age", FTYPE::INT}, {0, "num", FTYPE::UINT64}, 
                        {3, "name", FTYPE::STRING}}, Collect()), r_expr))));
        arg_builder args;
        args.arg(1, "Person");
        args.arg(2, ":HAS_READ");
        args.arg(3, "Book");
        args.arg(4, "Book");
        args.arg(5, ":HAS_READ");
        args.arg(6, "Person");

        result_set rs;
        queryEngine.generate(l_expr, false);
        queryEngine.run(&rs, args.args);

        REQUIRE(rs.data.front().size() == 3);
    }

#ifdef USE_PMDK
	nvm::transaction::run(pop, [&] { nvm::delete_persistent<graph_db>(graph); });
	pop.close();
	remove(test_path.c_str());
#else
	graph_pool::destroy(pool);
#endif
}

TEST_CASE("Test variable Foreach Relatinship operator", "[jit_query_ForeachVariable]") {
#ifdef USE_PMDK
  auto pop = prepare_pool();
  graph_db_ptr graph;
  nvm::transaction::run(pop, [&] { graph = p_make_ptr<graph_db>(); });
#else
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("my_graph");
#endif

#ifdef USE_TX
  auto tx = graph->begin_transaction();
#endif

  int num_towns = 42;
  
  for(auto i = 0u; i < num_towns; i+=3) {
        auto t1 = graph->add_node("Town",
                              {{"name", boost::any(std::string("Town_")+std::to_string(i))},
                               {"population", boost::any(int(42*i))}},
                              false);
        auto t2 = graph->add_node("Town",
                              {{"name", boost::any(std::string("Town_")+std::to_string(i+1))},
                               {"population", boost::any(int(42*(i+1)))}},
                              false);
        auto t3 = graph->add_node("Town",
                              {{"name", boost::any(std::string("Town_")+std::to_string(i+2))},
                               {"population", boost::any(int(42*(i+2)))}},
                              false);
        if(i>0) {
          graph->add_relationship(t1-3, t1, ":CONNECTED", {});
        }
        graph->add_relationship(t1, t2, ":CONNECTED", {});
        graph->add_relationship(t2, t1, ":CONNECTED", {});
        graph->add_relationship(t2, t3, ":CONNECTED", {});
        graph->add_relationship(t3, t2, ":CONNECTED", {});
  }

#ifdef USE_TX
  graph->commit_transaction();
#endif

	auto chunks = graph->get_nodes()->num_chunks();

	query_engine queryEngine(graph, 1, chunks);

  SECTION("Test the internal variable foreach relationship operator") {
        result_set rs;

        auto t = graph->begin_transaction();
        auto q = query(graph).all_nodes("Town").from_relationships({1, 2}, ":CONNECTED").collect(rs);
        q.start();
        graph->commit_transaction();
        REQUIRE(rs.data.size() == 191);
  }

  SECTION("Test the compiled variable foreach relationship operator") {
        auto fev = Scan("Town", ForeachRship(RSHIP_DIR::FROM, {1, 2}, ":CONNECTED", 
                       Collect()));
        arg_builder args;
        args.arg(1, "Town");
        args.arg(2, ":CONNECTED");

        result_set rs;
        queryEngine.generate(fev, false);
        queryEngine.run(&rs, args.args);

        REQUIRE(rs.data.size() == 191);
    }

#ifdef USE_PMDK
	nvm::transaction::run(pop, [&] { nvm::delete_persistent<graph_db>(graph); });
	pop.close();
	remove(test_path.c_str());
#else
	graph_pool::destroy(pool);
#endif

}*/


#endif