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
#include "../qop/qop.hpp"

#ifdef USE_PMDK
#define PMEMOBJ_POOL_SIZE ((size_t)(1024 * 1024 * 80))

namespace nvm = pmem::obj;
const std::string test_path = poseidon::gPmemPath + "graphdb_test";

nvm::pool_base prepare_pool() {
  auto pop = nvm::pool_base::create(test_path, "", PMEMOBJ_POOL_SIZE);
  return pop;
}
#endif

TEST_CASE("Importing a node", "[graph_db]") {
#ifdef USE_PMDK
  auto pop = prepare_pool();
  graph_db_ptr graph;
  nvm::transaction::run(pop, [&] { graph = p_make_ptr<graph_db>(); });
#else
  auto graph = p_make_ptr<graph_db>();
#endif

{
  auto tx = graph->begin_transaction();
  graph->add_node(":Person", {{"name", boost::any(std::string("Anne"))},
                                  {"age", boost::any(28)}});
  graph->commit_transaction();
}
  auto nid = graph->import_node(":Actor", {{"name", boost::any(std::string("John"))},
                                  {"age", boost::any(42)}});

  auto tx = graph->begin_transaction();
  auto& n = graph->node_by_id(nid);
  auto nd = graph->get_node_description(n);
    REQUIRE(nd.label == ":Actor");
    REQUIRE(nd.id == nid);
  REQUIRE(std::string("John") ==
          get_property<const std::string>(nd.properties, "name").value());
  REQUIRE(get_property<int>(nd.properties, "age").value() == 42);

  graph->abort_transaction();

#ifdef USE_PMDK
  nvm::transaction::run(pop, [&] { nvm::delete_persistent<graph_db>(graph); });
  pop.close();
  remove(test_path.c_str());
#endif
}

TEST_CASE("Importing a typed node", "[graph_db]") {
#ifdef USE_PMDK
  auto pop = prepare_pool();
  graph_db_ptr graph;
  nvm::transaction::run(pop, [&] { graph = p_make_ptr<graph_db>(); });
#else
  auto graph = p_make_ptr<graph_db>();
#endif

{
  auto tx = graph->begin_transaction();
  graph->add_node(":Person", {{"name", boost::any(std::string("Anne"))},
                                  {"age", boost::any(28)}});
  graph->commit_transaction();
}

  auto dict = graph->get_dictionary();
  auto label = dict->insert("Actor");
  auto prop1 = dict->insert("name");
  auto prop2 = dict->insert("age");
  auto val = dict->insert("John");

  auto nid = graph->import_typed_node(label, { prop1, prop2 },
    { p_item::p_typecode::p_dcode, p_item::p_typecode::p_int}, 
    { boost::any(val), boost::any(42)});

  auto tx = graph->begin_transaction();
  auto& n = graph->node_by_id(nid);
  auto nd = graph->get_node_description(n);
    REQUIRE(nd.label == "Actor");
    REQUIRE(nd.id == nid);
  REQUIRE(std::string("John") ==
          get_property<const std::string>(nd.properties, "name").value());
  REQUIRE(get_property<int>(nd.properties, "age").value() == 42);

  graph->abort_transaction();

#ifdef USE_PMDK
  nvm::transaction::run(pop, [&] { nvm::delete_persistent<graph_db>(graph); });
  pop.close();
  remove(test_path.c_str());
#endif
}

TEST_CASE("Importing nodes from CSV (old version)", "[graph_db]") {
#ifdef USE_PMDK
  auto pop = prepare_pool();
  graph_db_ptr graph;
  nvm::transaction::run(pop, [&] { graph = p_make_ptr<graph_db>(); });
#else
  auto graph = p_make_ptr<graph_db>();
#endif

  std::string home(".");
  auto h = getenv("TEST_HOME");
  if (h != nullptr)
    home = h;

  graph_db::mapping_t id_map;
  auto num = graph->import_nodes_from_csv("Place", home + "/test/places.csv", '|', id_map);
  REQUIRE(num == 1460);
#ifdef USE_PMDK
  nvm::transaction::run(pop, [&] { nvm::delete_persistent<graph_db>(graph); });
  pop.close();
  remove(test_path.c_str());
#endif
}

#if 0
TEST_CASE("Importing nodes from CSV", "[graph_db]") {
#ifdef USE_PMDK
  auto pop = prepare_pool();
  graph_db_ptr graph;
  nvm::transaction::run(pop, [&] { graph = p_make_ptr<graph_db>(); });
#else
  auto graph = p_make_ptr<graph_db>();
#endif

  std::string home(".");
  auto h = getenv("TEST_HOME");
  if (h != nullptr)
    home = h;

  graph_db::mapping_t id_map;
  auto num = graph->import_typed_nodes_from_csv("Place", home + "/test/places.csv", '|', id_map);
  REQUIRE(num == 1460);
#ifdef USE_PMDK
  nvm::transaction::run(pop, [&] { nvm::delete_persistent<graph_db>(graph); });
  pop.close();
  remove(test_path.c_str());
#endif
}

#endif

TEST_CASE("Importing nodes with many properties from CSV", "[graph_db]") {
#ifdef USE_PMDK
  auto pop = prepare_pool();
  graph_db_ptr graph;
  nvm::transaction::run(pop, [&] { graph = p_make_ptr<graph_db>(); });
#else
  auto graph = p_make_ptr<graph_db>();
#endif

  std::string home(".");
  auto h = getenv("TEST_HOME");
  if (h != nullptr)
    home = h;

  graph_db::mapping_t id_map;
  auto num = graph->import_typed_nodes_from_csv("Post", home + "/test/post.csv", '|', id_map);
  REQUIRE(num == 19);

  auto tx = graph->begin_transaction();
  graph->nodes([&graph](auto &n) {
    auto nd = graph->get_node_description(n);
    std::cout << nd << " ===> " << nd.properties.size() << std::endl;
    // REQUIRE(nd.properties.size() == 8);
  });
  graph->commit_transaction();

#ifdef USE_PMDK
  nvm::transaction::run(pop, [&] { nvm::delete_persistent<graph_db>(graph); });
  pop.close();
  remove(test_path.c_str());
#endif
}