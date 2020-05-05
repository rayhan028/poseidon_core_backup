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
#include "graph_pool.hpp"
#include "graph_db.hpp"
#include "../qop/qop.hpp"

const std::string test_path = poseidon::gPmemPath + "graphdb_test";

TEST_CASE("Creating nodes", "[graph_db]") {
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("my_graph");

  auto tx = graph->begin_transaction();

  for (int i = 0; i < 100; i++) {
    graph->add_node("Person",
                              {{"name", boost::any(std::string("John Doe"))},
                               {"age", boost::any(42)},
                               {"number", boost::any(i)},
                               {"dummy1", boost::any(std::string("Dummy"))},
                               {"dummy2", boost::any(1.2345)}},
                              true);
  }
  graph->commit_transaction();

  graph_pool::destroy(pool);
}

TEST_CASE("Creating some nodes and relationships", "[graph_db]") {
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("my_graph");

  auto tx = graph->begin_transaction();

  auto p1 = graph->add_node(":Person", {});
  auto p2 = graph->add_node(":Person", {});
  auto b1 = graph->add_node(":Book", {});
  auto b2 = graph->add_node(":Book", {});
  auto b3 = graph->add_node(":Book", {});

  graph->add_relationship(p1, b1, ":HAS_READ", {});
  graph->add_relationship(p1, b2, ":HAS_READ", {});
  graph->add_relationship(p1, b3, ":HAS_READ", {});
  graph->add_relationship(p2, b3, ":HAS_READ", {});
  graph->add_relationship(p1, p2, ":IS_FRIENDS_WITH", {});

  // check if we have all nodes
  {
    auto &n = graph->node_by_id(p1);
    REQUIRE(n.id() == p1);
    auto n1 = graph->get_node_description(n);
    REQUIRE(n1.label == ":Person");
    REQUIRE(n1.id == p1);

    auto n2 = graph->get_node_description(graph->node_by_id(p2));
    REQUIRE(n2.label == ":Person");
    REQUIRE(n2.id == p2);

    auto n3 = graph->get_node_description(graph->node_by_id(b1));
    REQUIRE(n3.label == ":Book");
    REQUIRE(n3.id == b1);

    auto n4 = graph->get_node_description(graph->node_by_id(b2));
    REQUIRE(n4.label == ":Book");
    REQUIRE(n4.id == b2);

    auto n5 = graph->get_node_description(graph->node_by_id(b3));
    REQUIRE(n5.label == ":Book");
    REQUIRE(n5.id == b3);
  }
  graph->commit_transaction();

  graph_pool::destroy(pool);
}

TEST_CASE("Checking FROM relationships", "[graph_db]") {
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("my_graph");

  auto tx = graph->begin_transaction();

  auto p1 = graph->add_node(":Person", {});
  auto p2 = graph->add_node(":Person", {});
  auto b1 = graph->add_node(":Book", {});
  auto b2 = graph->add_node(":Book", {});
  auto b3 = graph->add_node(":Book", {});

  graph->add_relationship(p1, b1, ":HAS_READ", {});
  graph->add_relationship(p1, b2, ":HAS_READ", {});
  graph->add_relationship(p1, b3, ":HAS_READ", {});
  graph->add_relationship(p2, b3, ":HAS_READ", {});
  graph->add_relationship(p1, p2, ":IS_FRIENDS_WITH", {});

  graph->commit_transaction();
  tx = graph->begin_transaction();

  // check if we have all relationships for each node
  {
    int hasReadCnt = 0;
    int isFriendsCnt = 0;
    auto &n1 = graph->node_by_id(p1);
    graph->foreach_from_relationship_of_node(n1, [&](auto &r) {
      auto s = std::string(graph->get_string(r.rship_label));
      if (s == ":HAS_READ")
        hasReadCnt++;
      else if (s == ":IS_FRIENDS_WITH")
        isFriendsCnt++;
      auto &n2 = graph->node_by_id(r.to_node_id());
      std::cout << n1.id() << "-[" << s << "]->" << n2.id() << std::endl;
    });
    REQUIRE(hasReadCnt == 3);
    REQUIRE(isFriendsCnt == 1);
  }

  graph->commit_transaction();

  graph_pool::destroy(pool);
}

TEST_CASE("Checking TO relationships", "[graph_db]") {
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("my_graph");

  auto tx = graph->begin_transaction();

  auto p1 = graph->add_node(":Person", {});
  auto p2 = graph->add_node(":Person", {});
  auto b1 = graph->add_node(":Book", {});
  auto b2 = graph->add_node(":Book", {});
  auto b3 = graph->add_node(":Book", {});

  graph->add_relationship(p1, b1, ":HAS_READ", {});
  graph->add_relationship(p1, b2, ":HAS_READ", {});
  graph->add_relationship(p1, b3, ":HAS_READ", {});
  graph->add_relationship(p2, b3, ":HAS_READ", {});
  graph->add_relationship(p1, p2, ":IS_FRIENDS_WITH", {});

  graph->commit_transaction();
  tx = graph->begin_transaction();

  {
    int hasReadCnt = 0;
    auto &n1 = graph->node_by_id(b3);
    graph->foreach_to_relationship_of_node(n1, [&](auto &r) {
      auto s = std::string(graph->get_string(r.rship_label));
      if (s == ":HAS_READ")
        hasReadCnt++;
      // auto& n2 = graph->node_by_id(r.from_node_id());
    });
    REQUIRE(hasReadCnt == 2);
  }
  graph->commit_transaction();

  graph_pool::destroy(pool);
}

TEST_CASE("Checking recursive FROM relationships", "[graph_db]") {
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("my_graph");

  auto tx = graph->begin_transaction();

  /*
    p1[0]-->p2[1]-->p3[2]-->p4[3]-->p5[4]
    |       |
    |       +-->p7[6]-->p8[7]
    |       |
    |       +->p10[9]
    |       |
    |       +->p9[8]
    |
    +-->p6[5]-->p11[10]
   */
  auto p1 = graph->add_node(":Person", {});
  auto p2 = graph->add_node(":Person", {});
  auto p3 = graph->add_node(":Person", {});
  auto p4 = graph->add_node(":Person", {});
  auto p5 = graph->add_node(":Person", {});
  auto p6 = graph->add_node(":Person", {});
  auto p7 = graph->add_node(":Person", {});
  auto p8 = graph->add_node(":Person", {});
  auto p9 = graph->add_node(":Person", {});
  auto p10 = graph->add_node(":Person", {});
  auto p11 = graph->add_node(":Person", {});

  graph->add_relationship(p1, p2, ":KNOWS", {});
  graph->add_relationship(p2, p3, ":KNOWS", {});
  graph->add_relationship(p3, p4, ":KNOWS", {});
  graph->add_relationship(p4, p5, ":KNOWS", {});
  graph->add_relationship(p2, p7, ":KNOWS", {});
  graph->add_relationship(p7, p8, ":KNOWS", {});
  graph->add_relationship(p2, p10, ":KNOWS", {});
  graph->add_relationship(p2, p9, ":KNOWS", {});
  graph->add_relationship(p1, p6, ":KNOWS", {});
  graph->add_relationship(p6, p11, ":KNOWS", {});

  graph->commit_transaction();
  tx = graph->begin_transaction();

  std::set<node::id_t> reachable_nodes;
  auto &n1 = graph->node_by_id(p1);
  graph->foreach_variable_from_relationship_of_node(n1, 1, 3, [&](auto &r) {
    auto &n2 = graph->node_by_id(r.to_node_id());
    reachable_nodes.insert(n2.id());
  });

  REQUIRE(reachable_nodes ==
          std::set<node::id_t>({1, 2, 3, 6, 7, 8, 9, 5, 10}));

  reachable_nodes.clear();
  auto &n2 = graph->node_by_id(p2);
  graph->foreach_variable_from_relationship_of_node(n2, 1, 3, [&](auto &r) {
    auto &n3 = graph->node_by_id(r.to_node_id());
    reachable_nodes.insert(n3.id());
  });

  REQUIRE(reachable_nodes == std::set<node::id_t>({2, 3, 4, 6, 7, 8, 9}));

  graph->commit_transaction();

  graph_pool::destroy(pool);
}

TEST_CASE("Checking adding a node with properties", "[graph_db]") {
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("my_graph");

  auto tx = graph->begin_transaction();

  auto p1 =
      graph->add_node(":Person", {{"name", boost::any(std::string("John"))},
                                  {"age", boost::any(42)}});

  auto &n = graph->node_by_id(p1);
  REQUIRE(n.id() == p1);

  auto ndescr = graph->get_node_description(n);
  REQUIRE(ndescr.id == p1);
  REQUIRE(ndescr.label == ":Person");
  REQUIRE(ndescr.properties.find("name") != ndescr.properties.end());
  REQUIRE(ndescr.properties.find("age") != ndescr.properties.end());

  REQUIRE(std::string("John") ==
          get_property<const std::string>(ndescr.properties, "name").value());
  REQUIRE(get_property<int>(ndescr.properties, "age").value() == 42);

  graph->commit_transaction();

  graph_pool::destroy(pool);
}

TEST_CASE("Checking node with properties", "[graph_db]") {
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("my_graph");

  auto tx = graph->begin_transaction();

  auto p1 =
      graph->add_node(":Person", {{"name", boost::any(std::string("John"))},
                                  {"age", boost::any(42)}});

  graph->commit_transaction();
  tx = graph->begin_transaction();

  auto &n1 = graph->node_by_id(p1);
  auto ndescr = graph->get_node_description(n1);

  REQUIRE(ndescr.id == p1);
  REQUIRE(ndescr.label == ":Person");
  REQUIRE(ndescr.properties.find("name") != ndescr.properties.end());
  REQUIRE(ndescr.properties.find("age") != ndescr.properties.end());

  REQUIRE(std::string("John") ==
          get_property<const std::string>(ndescr.properties, "name").value());
  REQUIRE(get_property<int>(ndescr.properties, "age").value() == 42);

  graph->commit_transaction();

  graph_pool::destroy(pool);
}

TEST_CASE("Checking a dirty node with properties", "[graph_db]") {
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("my_graph");

  auto tx = graph->begin_transaction();

  auto p1 =
      graph->add_node(":Person", {{"name", boost::any(std::string("John"))},
                                  {"age", boost::any(42)}});

  auto &n = graph->node_by_id(p1);
  REQUIRE(n.id() == p1);

  auto ndescr = graph->get_node_description(n);
  REQUIRE(ndescr.id == p1);
  REQUIRE(ndescr.label == ":Person");
  REQUIRE(ndescr.properties.find("name") != ndescr.properties.end());
  REQUIRE(ndescr.properties.find("age") != ndescr.properties.end());

  REQUIRE(std::string("John") ==
          get_property<const std::string>(ndescr.properties, "name").value());
  REQUIRE(get_property<int>(ndescr.properties, "age").value() == 42);

  graph->commit_transaction();

  graph_pool::destroy(pool);
}

TEST_CASE("Checking a node update", "[graph_db]") {
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("my_graph");

  node::id_t p1;

#ifdef USE_TX
  {
    // add a new node
    auto tx = graph->begin_transaction();
#endif

    p1 = graph->add_node(":Person",
                         {{"name", boost::any(std::string("John"))},
                          {"age", boost::any(42)},
                          {"dummy1", boost::any(10)},
                          {"dummy2", boost::any(11)},
                          {"dummy3", boost::any(12)},
                          {"city", boost::any(std::string("Berlin"))}});
#ifdef USE_TX
    graph->commit_transaction();
  }
  {
    REQUIRE_THROWS(check_tx_context());
    // perform an update
    auto tx = graph->begin_transaction();
#endif

    auto &n1 = graph->node_by_id(p1);
    graph->update_node(n1, {{"name", boost::any(std::string("Anne"))},
                            {"age", boost::any(43)},
                            {"city", boost::any(std::string("Munich"))},
                            {"zipcode", boost::any(12345)}});
    // and check whether the updates are available within the transaction
    auto &n2 = graph->node_by_id(p1);
    auto ndescr = graph->get_node_description(n2);

    REQUIRE(ndescr.id == p1);
    REQUIRE(ndescr.label == ":Person");

    REQUIRE(std::string("Anne") ==
            get_property<const std::string>(ndescr.properties, "name").value());
    REQUIRE(get_property<int>(ndescr.properties, "age").value() == 43);
    REQUIRE(std::string("Munich") ==
            get_property<const std::string>(ndescr.properties, "city").value());
    REQUIRE(ndescr.properties.find("zipcode") != ndescr.properties.end());
    REQUIRE(get_property<int>(ndescr.properties, "zipcode").value() == 12345);
#ifdef USE_TX
    graph->commit_transaction();
  }
  {
    // now we check whether the updates are available also outside the
    // transaction
    REQUIRE_THROWS(check_tx_context());
    auto tx = graph->begin_transaction();

    auto &n2 = graph->node_by_id(p1);
    auto ndescr = graph->get_node_description(n2);

    REQUIRE(ndescr.id == p1);
    REQUIRE(ndescr.label == ":Person");

    REQUIRE(std::string("Anne") ==
            get_property<const std::string>(ndescr.properties, "name").value());
    REQUIRE(get_property<int>(ndescr.properties, "age") == 43);
    REQUIRE(std::string("Munich") ==
            get_property<const std::string>(ndescr.properties, "city").value());
    REQUIRE(ndescr.properties.find("zipcode") != ndescr.properties.end());
    REQUIRE(get_property<int>(ndescr.properties, "zipcode") == 12345);

    graph->commit_transaction();
  }
#endif

  graph_pool::destroy(pool);
}

TEST_CASE("Checking multiple node updates", "[graph_db]") {
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("my_graph");

  node::id_t p1;

#ifdef USE_TX
  {
    // add a new node
    auto tx = graph->begin_transaction();
#endif

    p1 = graph->add_node(":Person",
                         {{"name", boost::any(std::string("John"))},
                          {"age", boost::any(42)},
                          {"dummy1", boost::any(10)},
                          {"dummy2", boost::any(11)},
                          {"dummy3", boost::any(12)},
                          {"city", boost::any(std::string("Berlin"))}});
#ifdef USE_TX
    graph->commit_transaction();
  }
  {
    REQUIRE_THROWS(check_tx_context());
    // perform an update
    auto tx = graph->begin_transaction();
#endif

    auto &n1 = graph->node_by_id(p1);
    graph->update_node(n1, {{"name", boost::any(std::string("Anne"))},
                            {"age", boost::any(43)},
                            {"city", boost::any(std::string("Munich"))},
                            {"zipcode", boost::any(12345)}});
    // and check whether the updates are available within the transaction
    {
    auto &n2 = graph->node_by_id(p1);
    auto ndescr = graph->get_node_description(n2);

    REQUIRE(ndescr.id == p1);
    REQUIRE(ndescr.label == ":Person");

    REQUIRE(std::string("Anne") ==
            get_property<const std::string>(ndescr.properties, "name").value());
    REQUIRE(get_property<int>(ndescr.properties, "age").value() == 43);
    REQUIRE(std::string("Munich") ==
            get_property<const std::string>(ndescr.properties, "city").value());
    REQUIRE(ndescr.properties.find("zipcode") != ndescr.properties.end());
    REQUIRE(get_property<int>(ndescr.properties, "zipcode").value() == 12345);
    }

    // second update
    graph->update_node(n1, {{"age", boost::any(46)},
                            {"city", boost::any(std::string("Munich"))},
                            {"zipcode", boost::any(12346)}}, ":Actor");
    // and check whether the updates are available within the transaction
    {
    auto &n2 = graph->node_by_id(p1);
    auto ndescr = graph->get_node_description(n2);

    REQUIRE(ndescr.id == p1);
    REQUIRE(ndescr.label == ":Actor");

    REQUIRE(std::string("Anne") ==
            get_property<const std::string>(ndescr.properties, "name").value());
    REQUIRE(get_property<int>(ndescr.properties, "age").value() == 46);
    REQUIRE(std::string("Munich") ==
            get_property<const std::string>(ndescr.properties, "city").value());
    REQUIRE(ndescr.properties.find("zipcode") != ndescr.properties.end());
    REQUIRE(get_property<int>(ndescr.properties, "zipcode").value() == 12346);
    }
#ifdef USE_TX
    graph->commit_transaction();
  }
  {
    // now we check whether the updates are available also outside the
    // transaction
    REQUIRE_THROWS(check_tx_context());
    auto tx = graph->begin_transaction();

    auto &n2 = graph->node_by_id(p1);
    auto ndescr = graph->get_node_description(n2);

    REQUIRE(ndescr.id == p1);
    REQUIRE(ndescr.label == ":Actor");

    REQUIRE(std::string("Anne") ==
            get_property<const std::string>(ndescr.properties, "name").value());
    REQUIRE(get_property<int>(ndescr.properties, "age").value() == 46);
    REQUIRE(std::string("Munich") ==
            get_property<const std::string>(ndescr.properties, "city").value());
    REQUIRE(ndescr.properties.find("zipcode") != ndescr.properties.end());
    REQUIRE(get_property<int>(ndescr.properties, "zipcode").value() == 12346);

    graph->commit_transaction();
  }
#endif

  graph_pool::destroy(pool);
}

TEST_CASE("Checking a relationship update", "[graph_db]") {
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("my_graph");

  node::id_t p1;
  relationship::id_t r;
#ifdef USE_TX
  {
    auto tx = graph->begin_transaction();
#endif

    // add two nodes and one relationship
    p1 = graph->add_node("Person", {});
    auto p2 = graph->add_node("Person", {});
    r = graph->add_relationship(
        p1, p2, "KNOWS",
        {{"p1", boost::any(std::string("val"))}, {"p2", boost::any(10)}});

#ifdef USE_TX
    graph->commit_transaction();
  }
  {
    REQUIRE_THROWS(check_tx_context());
    // perform an update
    auto tx = graph->begin_transaction();
#endif

    auto &n1 = graph->node_by_id(p1);
    graph->foreach_from_relationship_of_node(n1, [&](relationship &rel) {
      std::cout << "update relationship: " << graph->get_relationship_label(rel)
                << std::endl;
      graph->update_relationship(rel, {{"p1", boost::any(std::string("val2"))},
                                       {"p2", boost::any(20)},
                                       {"p3", boost::any(30)}});
    });

    // and check whether the updates are available within the transaction
    auto &n2 = graph->node_by_id(p1);
    graph->foreach_from_relationship_of_node(n2, [&](relationship &rel) {
      auto reldesc = graph->get_rship_description(rel);
      std::cout << reldesc << std::endl;

      REQUIRE(reldesc.id == r);
      REQUIRE(reldesc.label == "KNOWS");

      REQUIRE(std::string("val2") ==
              get_property<const std::string>(reldesc.properties, "p1").value());
      REQUIRE(get_property<int>(reldesc.properties, "p2").value() == 20);
      REQUIRE(get_property<int>(reldesc.properties, "p3").value() == 30);
    });

#ifdef USE_TX
    graph->commit_transaction();
  }
  {
    // now we check whether the updates are available also outside the
    // transaction
    REQUIRE_THROWS(check_tx_context());
    auto tx = graph->begin_transaction();

    auto &n2 = graph->node_by_id(p1);
    graph->foreach_from_relationship_of_node(n2, [&](relationship &rel) {
      auto reldesc = graph->get_rship_description(rel);
      std::cout << reldesc << std::endl;

      REQUIRE(reldesc.id == r);
      REQUIRE(reldesc.label == "KNOWS");

      REQUIRE(std::string("val2") ==
              get_property<const std::string>(reldesc.properties, "p1").value());
      REQUIRE(get_property<int>(reldesc.properties, "p2").value() == 20);
      REQUIRE(get_property<int>(reldesc.properties, "p3").value() == 30);
    });

    graph->commit_transaction();
  }
#endif

  graph_pool::destroy(pool);
}

TEST_CASE("Adding a larger number of nodes", "[graph_db]") {
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("my_graph");

  auto tx = graph->begin_transaction();
  for (int i = 0u; i < 10000; i++) {
    graph->add_node("Person",
                              {{"name", boost::any(std::string("John Doe"))},
                               {"age", boost::any(42)},
                               {"number", boost::any(i)},
                               {"dummy1", boost::any(std::string("Dummy"))},
                               {"dummy2", boost::any(1.2345)}},
                              true);
  }
  graph->commit_transaction();

  graph_pool::destroy(pool);
} 

TEST_CASE("Deleting all inserted nodes and relationships", "[graph_db]") {
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("my_graph");

  auto tx = graph->begin_transaction();
  
  auto i = 1;
  
  auto p1 = graph->add_node(":Person", 
                              {{"name", boost::any(std::string("John"))},
                               {"age", boost::any(42)},
                               {"number", boost::any(i++)},
                               {"dummy1", boost::any(std::string("Dummy"))},
                               {"dummy2", boost::any(1.2345)}}, true);

  auto p2 = graph->add_node(":Person", 
                              {{"name", boost::any(std::string("Doe"))},
                               {"age", boost::any(42)},
                               {"number", boost::any(i++)},
                               {"dummy1", boost::any(std::string("Dummy"))},
                               {"dummy2", boost::any(1.2345)}}, true);

  auto b1 = graph->add_node(":Book", 
                              {{"name", boost::any(std::string("Text Book"))},
                               {"ISBN", boost::any(i++)},
                               {"dummy1", boost::any(std::string("Dummy"))},
                               {"dummy2", boost::any(1.2345)}}, true);

  auto b2 = graph->add_node(":Book", 
                              {{"name", boost::any(std::string("e-Book"))},
                               {"age", boost::any(42)},
                               {"ISBN", boost::any(i++)},
                               {"dummy1", boost::any(std::string("Dummy"))},
                               {"dummy2", boost::any(1.2345)}}, true);

  auto b3 = graph->add_node(":Book", 
                              {{"name", boost::any(std::string("Manuscript"))},
                               {"ISBN", boost::any(i++)},
                               {"dummy1", boost::any(std::string("Dummy"))},
                               {"dummy2", boost::any(1.2345)}}, true);

  graph->add_relationship(p1, b1, ":HAS_READ", {{"dummy1", boost::any(std::string("Dummy"))}}, true);
  graph->add_relationship(p1, b2, ":HAS_READ", {{"dummy1", boost::any(std::string("Dummy"))}}, true);
  graph->add_relationship(p1, b3, ":HAS_READ", {{"dummy1", boost::any(std::string("Dummy"))}}, true);
  graph->add_relationship(p2, b3, ":HAS_READ", {{"dummy1", boost::any(std::string("Dummy"))}}, true);
  graph->add_relationship(p1, p2, ":IS_FRIENDS_WITH", {{"dummy1", boost::any(std::string("Dummy"))}}, true);
  
  graph->commit_transaction();
  tx = graph->begin_transaction();
  
  node::id_t next_node = graph->get_nodes()->as_vec().first_available();
  relationship::id_t next_rship = graph->get_relationships()->as_vec().first_available();
  REQUIRE(next_node == 5);
  REQUIRE(next_rship == 5);  

  // delete all nodes and relationships
  for (node::id_t i = 0; i < next_node; i++)
    graph->delete_node(i);
  
  // deleting a node deletes all relationships associated with the node
  for (relationship::id_t i = 0; i < next_rship; i++)
    graph->delete_relationship(i);

  graph->commit_transaction();

  tx = graph->begin_transaction();

  int num = 0;
  graph->nodes([&num](node& n) {
    num++;
  });
  REQUIRE(num == 0);
  num = 0;
  /*
  spdlog::info("scan relationships...");
  graph->relationships([&num](relationship& r) {
    spdlog::info("---> {}", r.id());
    num++;
  });
  REQUIRE(num == 0);
  */

  graph->commit_transaction();

  graph_pool::destroy(pool);
} 

TEST_CASE("Deleting some nodes and relationships", "[graph_db]") {
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("my_graph");

  auto tx = graph->begin_transaction();  
  auto i = 1;
  
  auto p1 = graph->add_node(":Person", {{"number", boost::any(i++)}}, true);
  auto p2 = graph->add_node(":Person", {{"number", boost::any(i++)}}, true);
  auto b1 = graph->add_node(":Person", {{"number", boost::any(i++)}}, true);
  auto b2 = graph->add_node(":Person", {{"number", boost::any(i++)}}, true);
  auto b3 = graph->add_node(":Person", {{"number", boost::any(i++)}}, true);

  graph->add_relationship(p1, b1, ":HAS_READ", {{"dummy1", boost::any(std::string("Dummy"))}}, true);
  graph->add_relationship(p1, b2, ":HAS_READ", {{"dummy1", boost::any(std::string("Dummy"))}}, true);
  graph->add_relationship(p1, b3, ":HAS_READ", {{"dummy1", boost::any(std::string("Dummy"))}}, true);
  graph->add_relationship(p2, b3, ":HAS_READ", {{"dummy1", boost::any(std::string("Dummy"))}}, true);
  graph->add_relationship(p1, p2, ":IS_FRIENDS_WITH", {{"dummy1", boost::any(std::string("Dummy"))}}, true);
  
  graph->commit_transaction();
  tx = graph->begin_transaction();
  
  node::id_t next_node = graph->get_nodes()->as_vec().first_available();
  for (node::id_t i = 1; i < next_node; i++)
    graph->delete_node(i); 
  relationship::id_t next_rship = graph->get_relationships()->as_vec().first_available();
  for (node::id_t i = 1; i < next_rship; i++)
    graph->delete_relationship(i); 
  
  int num = 0;
  graph->nodes([&num](node& n) {
    num++;
  });
  REQUIRE(num == 1);
  /*
  next_node = graph->get_nodes()->as_vec().first_available();
  next_rship = graph->get_relationships()->as_vec().first_available();
  REQUIRE(next_node == 1);
  REQUIRE(next_rship == 1);
  */

  graph->commit_transaction();

  graph_pool::destroy(pool);
}
