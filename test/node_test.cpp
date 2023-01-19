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

#include <catch2/catch_test_macros.hpp>
#include "config.h"
#include "nodes.hpp"

#include <sstream>
#include <boost/filesystem.hpp>

void create_dir(const std::string& path) {
    boost::filesystem::path path_obj(path);
    // check if path exists and is of a regular file
    if (! boost::filesystem::exists(path_obj))
        boost::filesystem::create_directory(path_obj);
}

void delete_dir(const std::string& path) {
    boost::filesystem::path path_obj(path);
    boost::filesystem::remove_all(path_obj);
}

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

#ifdef USE_PMDK

// ----------------------------------------------------------------------------
//       node_list with nvm_chunked_vec
// ----------------------------------------------------------------------------

#define PMEMOBJ_POOL_SIZE ((size_t)(1024 * 1024 * 80))


namespace nvm = pmem::obj;
const std::string test_path = poseidon::gPmemPath + "node_list_test";

struct root {
  pmem::obj::persistent_ptr<node_list<nvm_chunked_vec> > nlist_p;
};

TEST_CASE("Creating a few nodes in the nvm_node_list", "[nvm_node_list]") {
  auto pop = nvm::pool<root>::create(test_path, "", PMEMOBJ_POOL_SIZE);
  auto root_obj = pop.root();

  nvm::transaction::run(
      pop, [&] { root_obj->nlist_p = nvm::make_persistent<node_list<nvm_chunked_vec> >(); });

  SECTION("Creating nodes") {
  auto &nlist = *(root_obj->nlist_p);

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

  pop.close();
  remove(test_path.c_str());
}

TEST_CASE("Creating and restoring a persistent nvm_node_list", "[nvm_node_list]") {
  node::id_t n1 = 0, n2 = 0, n3 = 0;
  auto pop = nvm::pool<root>::create(test_path, "", PMEMOBJ_POOL_SIZE);
  auto root_obj = pop.root();
  {
  nvm::transaction::run(
      pop, [&] { root_obj->nlist_p = nvm::make_persistent<node_list<nvm_chunked_vec> >(); });
  auto &nlist = *(root_obj->nlist_p);

  n1 = nlist.add(node(62));
  n2 = nlist.add(node(63));
  n3 = nlist.add(node(64));

  REQUIRE(nlist.get(n1).id() == n1);
  REQUIRE(nlist.get(n1).node_label == 62);

  REQUIRE(nlist.get(n2).id() == n2);
  REQUIRE(nlist.get(n2).node_label == 63);

  REQUIRE(nlist.get(n3).id() == n3);
  REQUIRE(nlist.get(n3).node_label == 64);

  nlist.dump();
  pop.close();
  }

  {
  pop = nvm::pool<root>::open(test_path, "");
  root_obj = pop.root();

  auto &nlist2 = *(root_obj->nlist_p);
  nlist2.dump();

  REQUIRE(nlist2.get(n1).id() == n1);
  REQUIRE(nlist2.get(n1).node_label == 62);

  REQUIRE(nlist2.get(n2).id() == n2);
  REQUIRE(nlist2.get(n2).node_label == 63);

  REQUIRE(nlist2.get(n3).id() == n3);
  REQUIRE(nlist2.get(n3).node_label == 64);

  pop.close();
  }

  remove(test_path.c_str());
}

TEST_CASE("Deleting a node in the nvm_node_list", "[nvm_node_list]") {
  auto pop = nvm::pool<root>::create(test_path, "", PMEMOBJ_POOL_SIZE);
  auto root_obj = pop.root();

  nvm::transaction::run(
      pop, [&] { root_obj->nlist_p = nvm::make_persistent<node_list<nvm_chunked_vec> >(); });

  auto &nlist = *(root_obj->nlist_p);

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

  pop.close();
  remove(test_path.c_str());
}

TEST_CASE("Appending a node to a nvm_node_list", "[nvm_node_list]") {
  auto pop = nvm::pool<root>::create(test_path, "", PMEMOBJ_POOL_SIZE);
  auto root_obj = pop.root();

  nvm::transaction::run(
      pop, [&] { root_obj->nlist_p = nvm::make_persistent<node_list<nvm_chunked_vec> >(); });
  auto &nlist = *(root_obj->nlist_p);

  auto n1 = nlist.append(node(62));
  REQUIRE(n1 == 0);

  REQUIRE(nlist.get(n1).id() == n1);
  REQUIRE(nlist.get(n1).node_label == 62);

  for (auto i = 0u; i < 100; i++)
    nlist.append(node(100 + i));

  int num = 0;
  for (auto& ni : nlist.as_vec()) {
    num++;
  }
  REQUIRE(num == 101);
  
  pop.close();
  remove(test_path.c_str());
}

TEST_CASE("Checking reuse of space in a nvm_node_list", "[nvm_node_list]") {
  // TODO
}
#endif // USE_PMDK

// ----------------------------------------------------------------------------
//       node_list with mem_chunked_vec
// ----------------------------------------------------------------------------

TEST_CASE("Creating a few nodes in the mem_node_list", "[mem_node_list]") {

  SECTION("Creating nodes") {
  node_list<mem_chunked_vec> nlist;

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
}

TEST_CASE("Deleting a node in the mem_node_list", "[mem_node_list]") {
  node_list<mem_chunked_vec> nlist;
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

TEST_CASE("Appending a node to a mem_node_list", "[mem_node_list]") {
  node_list<mem_chunked_vec> nlist;

  auto n1 = nlist.append(node(62));
  REQUIRE(n1 == 0);

  REQUIRE(nlist.get(n1).id() == n1);
  REQUIRE(nlist.get(n1).node_label == 62);

  for (auto i = 0u; i < 100; i++)
    nlist.append(node(100 + i));

  int num = 0;
  for (auto& ni : nlist.as_vec()) {
    num++;
  }
  REQUIRE(num == 101);
}

TEST_CASE("Checking reuse of space in a mem_node_list", "[mem_node_list]") {
  // TODO
}

// ----------------------------------------------------------------------------
//       node_list with buffered_vec
// ----------------------------------------------------------------------------

TEST_CASE("Creating a few nodes in the pfile_node_list", "[pfile_node_list]") {
  create_dir("my_ntest1");

  SECTION("Creating nodes") {
  auto test_file = std::make_shared<paged_file>();
  test_file->open("my_ntest1/nodes1.db", 1);
  bufferpool bpool;
  bpool.register_file(1, test_file);

  node_list<buffered_vec> nlist(bpool, 1);

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
  delete_dir("my_ntest1");
}
 
TEST_CASE("Creating and restoring a persistent pfile_node_list", "[pfile_node_list]") {
  node::id_t n1 = 0, n2 = 0, n3 = 0;
  {
  create_dir("my_ntest2");
  auto test_file = std::make_shared<paged_file>();
  test_file->open("my_ntest2/nodes2.db", 1);
  bufferpool bpool;
  bpool.register_file(1, test_file);

  node_list<buffered_vec> nlist(bpool, 1);

  n1 = nlist.add(node(62));
  n2 = nlist.add(node(63));
  n3 = nlist.add(node(64));

  REQUIRE(nlist.get(n1).id() == n1);
  REQUIRE(nlist.get(n1).node_label == 62);

  REQUIRE(nlist.get(n2).id() == n2);
  REQUIRE(nlist.get(n2).node_label == 63);

  REQUIRE(nlist.get(n3).id() == n3);
  REQUIRE(nlist.get(n3).node_label == 64);

  nlist.dump();
  }

  { 
  auto test_file = std::make_shared<paged_file>();
  test_file->open("my_ntest2/nodes2.db", 1);
  bufferpool bpool;
  bpool.register_file(1, test_file);

  node_list<buffered_vec> nlist2(bpool, 1);

  nlist2.dump();

  REQUIRE(nlist2.get(n1).id() == n1);
  REQUIRE(nlist2.get(n1).node_label == 62);

  REQUIRE(nlist2.get(n2).id() == n2);
  REQUIRE(nlist2.get(n2).node_label == 63);

  REQUIRE(nlist2.get(n3).id() == n3);
  REQUIRE(nlist2.get(n3).node_label == 64);

  }
  delete_dir("my_ntest2");
}

TEST_CASE("Deleting a node in a pfile_node_list", "[pfile_node_list]") {
  create_dir("my_ntest3");
  auto test_file = std::make_shared<paged_file>();
  test_file->open("my_ntest3/nodes3.db", 1);
  bufferpool bpool;
  bpool.register_file(1, test_file);

  node_list<buffered_vec> nlist(bpool, 1);

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

  delete_dir("my_ntest3");
}

TEST_CASE("Appending a node to a pfile_node_list", "[pfile_node_list]") {
  create_dir("my_ntest4");
  auto test_file = std::make_shared<paged_file>();
  test_file->open("my_ntest4/nodes4.db", 1);
  bufferpool bpool;
  bpool.register_file(1, test_file);

  node_list<buffered_vec> nlist(bpool, 1);

  auto n1 = nlist.append(node(62));
  REQUIRE(n1 == 0);

  REQUIRE(nlist.get(n1).id() == n1);
  REQUIRE(nlist.get(n1).node_label == 62);

  for (auto i = 0u; i < 100; i++)
    nlist.append(node(100 + i));

  int num = 0;
  for (auto& ni : nlist.as_vec()) {
    num++;
  }
  REQUIRE(num == 101);
  
  delete_dir("my_ntest4");
}

TEST_CASE("Checking reuse of space in a pfile_node_list", "[pfile_node_list]") {
  // TODO
}

