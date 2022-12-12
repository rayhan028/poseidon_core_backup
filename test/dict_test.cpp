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

#include <boost/filesystem.hpp>
#include "spdlog/fmt/fmt.h"
#include "catch.hpp"
#include "config.h"
#include "defs.hpp"
#include "dict.hpp"

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

#ifdef USE_PMDK
#define PMEMOBJ_POOL_SIZE ((size_t)(1024 * 1024 * 1000))

namespace nvm = pmem::obj;
const std::string test_path = poseidon::gPmemPath + "dict_test";

struct root {
  nvm::persistent_ptr<dict> dict_p;
};
#endif

TEST_CASE("Inserting some strings", "[dict]") {
#ifdef USE_PMDK
  auto pop = nvm::pool<root>::create(test_path, "", PMEMOBJ_POOL_SIZE);
  auto root_obj = pop.root();

  nvm::transaction::run(
      pop, [&] { root_obj->dict_p = nvm::make_persistent<dict>(); });

  dict &d = *(root_obj->dict_p);
  d.initialize();
#elif defined(USE_IN_MEMORY)
  dict d;
#else
  create_dir("dict1");
  bufferpool bpool;
  dict d(bpool, "dict1");
#endif

  REQUIRE(d.size() == 0);
  d.insert("String #1");
  d.insert("String #2");
  d.insert("String #3");
  d.insert("String #4");
  d.insert("String #5");
  d.insert("String #1");
  d.insert("String #2");
  REQUIRE(d.size() == 5);

#ifdef USE_PMDK
  pop.close();
  remove(test_path.c_str());
#else
  delete_dir("dict1");
#endif
}

TEST_CASE("Inserting duplicate strings", "[dict]") {
#ifdef USE_PMDK
  auto pop = nvm::pool<root>::create(test_path, "", PMEMOBJ_POOL_SIZE);
  auto root_obj = pop.root();

  nvm::transaction::run(
      pop, [&] { root_obj->dict_p = nvm::make_persistent<dict>(); });

  dict &d = *(root_obj->dict_p);
  d.initialize();
#elif defined(USE_IN_MEMORY)
  dict d;
#else
  create_dir("dict2");
  bufferpool bpool;
  dict d(bpool, "dict2");
#endif

  REQUIRE(d.size() == 0);
  d.insert("String #1");
  d.insert("String #2");
  d.insert("String #3");
  d.insert("String #3");
  d.insert("String #1");
  REQUIRE(d.size() == 3);

#ifdef USE_PMDK
  pop.close();
  remove(test_path.c_str());
#else
  delete_dir("dict2");  
#endif
}

TEST_CASE("Looking up some strings", "[dict]") {
#ifdef USE_PMDK
  auto pop = nvm::pool<root>::create(test_path, "", PMEMOBJ_POOL_SIZE);
  auto root_obj = pop.root();

  nvm::transaction::run(
      pop, [&] { root_obj->dict_p = nvm::make_persistent<dict>(); });

  dict &d = *(root_obj->dict_p);
  d.initialize();
#elif defined(USE_IN_MEMORY)
  dict d;
#else
  create_dir("dict3");
  bufferpool bpool;
  dict d(bpool, "dict3");
#endif

  REQUIRE(d.size() == 0);
  d.insert("String #1");
  auto c2 = d.insert("String #2");
  d.insert("String #3");
  auto c4 = d.insert("String #4");
  d.insert("String #5");

  REQUIRE(d.lookup_string("String #4") == c4);
  REQUIRE(d.lookup_string("String #2") == c2);

#ifdef USE_PMDK
  pop.close();
  remove(test_path.c_str());
#else
  delete_dir("dict3"); 
#endif
}

TEST_CASE("Looking up some codes", "[dict]") {
#ifdef USE_PMDK
  auto pop = nvm::pool<root>::create(test_path, "", PMEMOBJ_POOL_SIZE);
  auto root_obj = pop.root();

  nvm::transaction::run(
      pop, [&] { root_obj->dict_p = nvm::make_persistent<dict>(); });

  dict &d = *(root_obj->dict_p);
  d.initialize();
#elif defined(USE_IN_MEMORY)
  dict d;
#else
create_dir("dict4");
  bufferpool bpool;
  dict d(bpool, "dict4");
#endif

  REQUIRE(d.size() == 0);
  d.insert("String #1");
  d.insert("String #2");
  auto c3 = d.insert("String #3");
  auto c4 = d.insert("String #4");
  d.insert("String #5");

  REQUIRE(std::string("String #4") == d.lookup_code(c4));
  REQUIRE(std::string("String #3") ==d.lookup_code(c3));

#ifdef USE_PMDK
  pop.close();
  remove(test_path.c_str());
#else
  delete_dir("dict4"); 
#endif
}

TEST_CASE("Looking up some non-existing strings", "[dict]") {
#ifdef USE_PMDK
  auto pop = nvm::pool<root>::create(test_path, "", PMEMOBJ_POOL_SIZE);
  auto root_obj = pop.root();

  nvm::transaction::run(
      pop, [&] { root_obj->dict_p = nvm::make_persistent<dict>(); });

  dict &d = *(root_obj->dict_p);
  d.initialize();
#elif defined(USE_IN_MEMORY)
  dict d;
#else
  create_dir("dict5");
  bufferpool bpool;
  dict d(bpool, "dict5");
#endif

  d.insert("String #1");
  d.insert("String #2");
  d.insert("String #3");
  d.insert("String #4");
  d.insert("String #5");
  REQUIRE(d.lookup_string("Unknown string") == 0);

#ifdef USE_PMDK
  pop.close();
  remove(test_path.c_str());
#else
  delete_dir("dict5"); 
#endif
}

#ifdef USE_PMDK
TEST_CASE("Test persistency of dict", "[dict]") {
  auto pop = nvm::pool<root>::create(test_path, "", PMEMOBJ_POOL_SIZE);
  auto root_obj = pop.root();

  nvm::transaction::run(
      pop, [&] { root_obj->dict_p = nvm::make_persistent<dict>(); });

  dict &d = *(root_obj->dict_p);
  d.initialize();

  d.insert("String #1");
  d.insert("String #2");
  d.insert("String #3");
  auto c = d.insert("String #4");
  d.insert("String #5");

  pop.close();

  pop = nvm::pool<root>::open(test_path, "");
  root_obj = pop.root();

  dict &d2 = *(root_obj->dict_p);
  d2.initialize();

  REQUIRE(d2.lookup_string("String #4") == c);

  pop.close();
  remove(test_path.c_str());
}

#elif !defined(USE_IN_MEMORY)

TEST_CASE("Test persistency of dict", "[dict]") {
  dcode_t c;
  create_dir("dict6");
  {
    bufferpool bpool;
    dict d(bpool, "dict6");

    d.insert("String #1");
    d.insert("String #2");
    d.insert("String #3");
    c = d.insert("String #4");
    d.insert("String #5");

  }

  {
    bufferpool bpool;
    dict d2(bpool, "dict6");

    REQUIRE(d2.lookup_string("String #4") == c);
  }
  delete_dir("dict6");
}
#endif

// * test with a large set of strings
TEST_CASE("Inserting many items", "[dict]") {
#ifdef USE_IN_MEMORY
  dict d;
#endif
  {
#ifdef USE_PMDK
    auto pop = nvm::pool<root>::create(test_path, "", PMEMOBJ_POOL_SIZE);
    auto root_obj = pop.root();

    nvm::transaction::run(
      pop, [&] { root_obj->dict_p = nvm::make_persistent<dict>(); });

    dict &d = *(root_obj->dict_p);
    d.initialize();
#elif !defined(USE_IN_MEMORY)
    create_dir("dict7");
    bufferpool bpool;
    dict d(bpool, "dict7");
#endif

    std::cout << "dict7...insert" << std::endl;
  // max: 4294967295
  // for (uint64_t i = 0u; i < 10000000; i++) {
    for (uint64_t i = 0u; i < 100000; i++) {
//	std::cout << i << std::endl;
      d.insert(fmt::format("DictEntry#{}", i));
    }

    std::cout << "...lookup" << std::endl;
    for (auto i = 1000u; i < 100000; i++) {
      auto str = fmt::format("DictEntry#{}", i);
      auto c = d.lookup_string(str);
      REQUIRE(c != 0);
    }
    std::cout << "finished." << std::endl;
    d.print_table();
  }
  {
    std::cout << "restart...." << std::endl;
#ifdef USE_PMDK
    auto pop = nvm::pool<root>::open(test_path, "");
    auto root_obj = pop.root();

    dict &d = *(root_obj->dict_p);
    d.initialize();

#elif !defined(USE_IN_MEMORY)
    bufferpool bpool;
    dict d(bpool, "dict7");
#endif
    for (auto i = 1000u; i < 100000; i++) {
      auto str = fmt::format("DictEntry#{}", i);
      // std::cout << str << std::endl;
      auto c = d.lookup_string(str);
      REQUIRE(c != 0);
    }
#ifdef USE_PMDK
  pop.close();
  remove(test_path.c_str());
#endif
  }
#if !defined(USE_PMDK) && !defined(USE_IN_MEMORY)
  delete_dir("dict7"); 
#endif
}
