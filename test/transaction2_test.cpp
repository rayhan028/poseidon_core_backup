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

#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do
                          // this in one cpp file

#include <condition_variable>
#include <iostream>
#include <thread>

#include "catch.hpp"
#include "config.h"
#include "defs.hpp"
#include "graph_db.hpp"
#include "transaction.hpp"
#include "graph_pool.hpp"

const std::string test_path = poseidon::gPmemPath + "transaction2_test";

class barrier {
private:
  std::mutex m;
  std::condition_variable cond_var;
  std::atomic<bool> ready{false};

public:
  barrier() = default;

	/**
	 * Inform the other thread that it can proceed.
	 */
  void notify() {
    std::lock_guard<std::mutex> lock(m);
    ready = true;
    cond_var.notify_one();
  }

	/**
	 * Wait on the barrier until the other threads calls notify.
	 */
  void wait() {
    std::unique_lock<std::mutex> lock(m);
    cond_var.wait(lock, [&] { return ready.load(); });
  }
};

TEST_CASE("Consistency 1", "[transaction]") {

  /**
   * t2 starts before t1
   * t1 updates a node and commits, then t2 reads it 
   * t2 sees the old version
   */
  auto pool = graph_pool::create(test_path);
  auto gdb = pool->create_graph("my_graph");

  node::id_t nid = 0;
  barrier  b1{}, b2{};

  // Just create a node
  gdb->begin_transaction();
  nid = gdb->add_node("Person",
                      {{"name", boost::any(std::string("John Doe"))},
                       {"age", boost::any(42)}});
  gdb->commit_transaction();

  auto t1 = std::thread([&]() {
    b1.wait(); // wait for t2 to start first
    gdb->begin_transaction();
    auto &n = gdb->node_by_id(nid);
    gdb->update_node(n, {{"age", boost::any(47)}}, "Updated Person");
    gdb->commit_transaction();
    b2.notify();  // inform t2 to read the updated node
	});

  auto t2 = std::thread([&]() {
    gdb->begin_transaction();
    b1.notify(); // inform t1 to start
    b2.wait(); // wait for t1 to update the node and commit
    auto nd = gdb->get_node_description(nid);
    REQUIRE(nd.label == "Person");
    REQUIRE(get_property<int>(nd.properties, "age") == 42);
    gdb->commit_transaction();
  });

  t1.join();
  t2.join();

  // check the new version
  gdb->begin_transaction();
  auto nd = gdb->get_node_description(nid);
  REQUIRE(nd.label == "Updated Person");
  REQUIRE(get_property<int>(nd.properties, "age") == 47);
  gdb->commit_transaction();

  graph_pool::destroy(pool);
}

TEST_CASE("Consistency 2", "[transaction]") {

  /**
   * t2 starts before t1
   * t1 deletes a node and commits, then t2 reads it 
   * t2 should see the old version
   */
  auto pool = graph_pool::create(test_path);
  auto gdb = pool->create_graph("my_graph");

  node::id_t nid = 0;
  barrier  b1{}, b2{};

  // Just create a node
  gdb->begin_transaction();
  nid = gdb->add_node("Person",
                      {{"name", boost::any(std::string("John Doe"))},
                       {"age", boost::any(42)}});
  gdb->commit_transaction();

  auto t1 = std::thread([&]() {
    b1.wait(); // wait for t2 to start first
    gdb->begin_transaction();
    gdb->delete_node(nid);
    gdb->commit_transaction();
    b2.notify();  // inform t2 to read the deleted node
	});

  auto t2 = std::thread([&]() {
    gdb->begin_transaction();
    b1.notify(); // inform t1 to start
    b2.wait(); // wait for t1 to delete the node and commit
    auto nd = gdb->get_node_description(nid);
    REQUIRE(nd.label == "Person");
    REQUIRE(get_property<int>(nd.properties, "age") == 42);
    gdb->commit_transaction();
  });

  t1.join();
  t2.join();

  graph_pool::destroy(pool);
}

TEST_CASE("Consistency 3", "[transaction]") {

  /**
   * t2 starts before t1
   * t1 deletes a node and before it commits, t2 reads it 
   * t2 sees the old version
   */
  auto pool = graph_pool::create(test_path);
  auto gdb = pool->create_graph("my_graph");

  node::id_t nid = 0;
  barrier  b1{}, b2{}, b3{};

  // Just create a node
  gdb->begin_transaction();
  nid = gdb->add_node("Person",
                      {{"name", boost::any(std::string("John Doe"))},
                       {"age", boost::any(42)}});
  gdb->commit_transaction();

  auto t1 = std::thread([&]() {
    b1.wait(); // wait for t2 to start first
    gdb->begin_transaction();
    gdb->delete_node(nid);
    b2.notify();  // inform t2 to read the deleted node
    b3.wait(); // wait for t2 to read the deleted node
    gdb->commit_transaction();
	});

  auto t2 = std::thread([&]() {
    gdb->begin_transaction();
    b1.notify(); // inform t1 to start
    b2.wait(); // wait for t1 to delete the node and commit
    auto nd = gdb->get_node_description(nid);
    REQUIRE(nd.label == "Person");
    REQUIRE(get_property<int>(nd.properties, "age") == 42);
    gdb->commit_transaction();
    b3.notify(); // notify t1 to commit
  });

  t1.join();
  t2.join();

  graph_pool::destroy(pool);
}