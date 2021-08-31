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
#include "nodes.hpp"

#include <sstream>

#ifdef USE_PMDK
#define PMEMOBJ_POOL_SIZE ((size_t)(1024 * 1024 * 80))

namespace nvm = pmem::obj;
const std::string test_path = poseidon::gPmemPath + "node_list_test";

struct root {
  pmem::obj::persistent_ptr<node_list> nlist_p;
};

#endif

TEST_CASE("Testing output functions", "[nodes]") {
  {
    std::ostringstream os;
    boost::any v(12);
    os << v;
    REQUIRE(os.str() == "12");
  }
  {
    std::ostringstream os;
    boost::any v(12.34);
    os << v;
    REQUIRE(os.str() == "12.34");
  }
  {
    std::ostringstream os;
    boost::any v(true);
    os << v;
    REQUIRE(os.str() == "1");
  }
  {
    std::ostringstream os;
    boost::any v((uint64_t)1234);
    os << v;
    REQUIRE(os.str() == "1234");
  }
  {
    std::ostringstream os;
    boost::posix_time::ptime pt{ boost::gregorian::date{2014, 5, 12}, 
      boost::posix_time::time_duration{12, 0, 0}};
    boost::any v(pt);
    os << v;
    REQUIRE(os.str() == "2014-May-12 12:00:00");
  }
}

TEST_CASE("Creating a node", "[nodes]") {
  node n(42);
  REQUIRE(n.node_label == 42);
  REQUIRE(n.from_rship_list == UNKNOWN);
  REQUIRE(n.to_rship_list == UNKNOWN);
  REQUIRE(n.property_list == UNKNOWN);
  REQUIRE(n.id() == UNKNOWN);
  n.property_list = 66;
  REQUIRE(n.property_list == 66);
}

TEST_CASE("Creating a few nodes in the node list", "[nodes]") {
#ifdef USE_PMDK
  auto pop = nvm::pool<root>::create(test_path, "", PMEMOBJ_POOL_SIZE);
  auto root_obj = pop.root();

  nvm::transaction::run(
      pop, [&] { root_obj->nlist_p = nvm::make_persistent<node_list>(); });
#endif

  SECTION("Creating nodes") {
#ifdef USE_PMDK
  node_list &nlist = *(root_obj->nlist_p);
#else
  node_list nlist("nodes1.db");
#endif

  auto n1 = nlist.add(node(62));
  auto n2 = nlist.add(node(63));
  auto n3 = nlist.add(node(64));

  REQUIRE(nlist.get(n1).id() == n1);
  REQUIRE(nlist.get(n1).node_label == 62);

  REQUIRE(nlist.get(n2).id() == n2);
  REQUIRE(nlist.get(n2).node_label == 63);

  REQUIRE(nlist.get(n3).id() == n3);
  REQUIRE(nlist.get(n3).node_label == 64);

  REQUIRE(nlist.num_chunks() == 1);

  CHECK_THROWS_AS(nlist.get(47), unknown_id);
  CHECK_THROWS_AS(nlist.get(10000), unknown_id);

  nlist.remove(n3);
  CHECK_THROWS_AS(nlist.get(n3), unknown_id);
  // if the capacity is larger than 10000 then no exception is raised
  // CHECK_THROWS_AS(nlist.remove(10000), unknown_id); 
  }

#ifdef USE_PMDK
  pop.close();
  remove(test_path.c_str());
#elif USE_MMFILE
  remove("nodes1.db");
  remove("slots_nodes1.db");
#endif
}

#if defined(USE_PMDK) || defined(USE_MMFILE) 
TEST_CASE("Creating and restoring a persistent node list", "[nodes]") {
  node::id_t n1 = 0, n2 = 0, n3 = 0;
#ifdef USE_PMDK
  auto pop = nvm::pool<root>::create(test_path, "", PMEMOBJ_POOL_SIZE);
  auto root_obj = pop.root();
#endif
  {
#ifdef USE_PMDK

  nvm::transaction::run(
      pop, [&] { root_obj->nlist_p = nvm::make_persistent<node_list>(); });
  node_list &nlist = *(root_obj->nlist_p);
#else
  node_list nlist("nodes2.db");
#endif
  n1 = nlist.add(node(62));
  n2 = nlist.add(node(63));
  n3 = nlist.add(node(64));

  REQUIRE(nlist.get(n1).id() == n1);
  REQUIRE(nlist.get(n1).node_label == 62);

  REQUIRE(nlist.get(n2).id() == n2);
  REQUIRE(nlist.get(n2).node_label == 63);

  REQUIRE(nlist.get(n3).id() == n3);
  REQUIRE(nlist.get(n3).node_label == 64);

#ifdef USE_PMDK
  pop.close();
#endif
  }

  {
#ifdef USE_PMDK
  pop = nvm::pool<root>::open(test_path, "");
  root_obj = pop.root();

  node_list &nlist2 = *(root_obj->nlist_p);
#else
  node_list nlist2("nodes2.db");
#endif

  REQUIRE(nlist2.get(n1).id() == n1);
  REQUIRE(nlist2.get(n1).node_label == 62);

  REQUIRE(nlist2.get(n2).id() == n2);
  REQUIRE(nlist2.get(n2).node_label == 63);

  REQUIRE(nlist2.get(n3).id() == n3);
  REQUIRE(nlist2.get(n3).node_label == 64);

#ifdef USE_PMDK
  pop.close();
#endif
  }

#ifdef USE_PMDK
  remove(test_path.c_str());
#else
  remove("nodes2.db");
  remove("slots_nodes2.db");
#endif
}
#endif

TEST_CASE("Deleting a node", "[nodes]") {
#ifdef USE_PMDK
  auto pop = nvm::pool<root>::create(test_path, "", PMEMOBJ_POOL_SIZE);
  auto root_obj = pop.root();

  nvm::transaction::run(
      pop, [&] { root_obj->nlist_p = nvm::make_persistent<node_list>(); });
#endif

  SECTION("Delete") {
#ifdef USE_PMDK
  node_list &nlist = *(root_obj->nlist_p);
#else
  node_list nlist("nodes3.db");
#endif
  auto n1 = nlist.add(node(62));
  auto n2 = nlist.add(node(63));
  auto n3 = nlist.add(node(64));

  nlist.remove(n2);

  REQUIRE(nlist.get(n1).id() == n1);
  REQUIRE(nlist.get(n1).node_label == 62);

  REQUIRE(nlist.get(n3).id() == n3);
  REQUIRE(nlist.get(n3).node_label == 64);

  // nlist.get(n2) should raise an exception
  REQUIRE_THROWS_AS(nlist.get(n2), unknown_id);
  }

#ifdef USE_PMDK
  pop.close();
  remove(test_path.c_str());
#elif USE_MMFILE
  remove("nodes3.db");
  remove("slots_nodes3.db");
#endif
}

TEST_CASE("Checking reuse of space in node list", "[nodes]") {
  // TODO
}
