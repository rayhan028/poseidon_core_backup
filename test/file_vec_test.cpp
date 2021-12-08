/*
 * Copyright (C) 2019-2021 DBIS Group - TU Ilmenau, All Rights Reserved.
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

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do
                           // this in one cpp file

#include <cstdio>
#include <vector>

#include "catch.hpp"
#include "file_vec.hpp"

const std::string test_path = "test_file.bin";

struct record {
  record() = default;
  record(record &&r) = default;
  record &operator=(record &&r) = default;

  record &operator=(const record &other) = default;

  uint8_t flag;
  uint32_t head;
  int64_t i;
  char s[44];
};

TEST_CASE("Adding some records", "[file_vec]") {
    file_vec<record> vec(test_path);
 
    // make sure we have enough space for 1000 records
    REQUIRE(vec.capacity() > 1000);

    CHECK_THROWS_AS(vec.at(1000000), index_out_of_range);

    // store 1000 records in the array
    for (offset_t i = 0; i < 1000; i++) {
      record rec;
      rec.head = i + 1;
      rec.i = i * 100 + 1;
      memcpy(rec.s, "##########", 10);
      rec.flag = 1;
      vec.store_at(i, std::move(rec));
    }

    // check whether we can retrieve these records
    for (offset_t o = 0; o < 1000; o++) {
      const auto &rec = vec.const_at(o);
      REQUIRE(rec.flag == 1);
      REQUIRE(rec.head == o + 1);
      REQUIRE(rec.i == (int64_t)(o * 100 + 1));
      REQUIRE(strncmp(rec.s, "##########", 10) == 0);
    }
    vec.close();
    file_vec<record>::remove_file_vec(test_path);
}

TEST_CASE("Adding some records, close the file, and reopen it", "[file_vec]") {
    {
       file_vec<record> vec(test_path);
        // store 1000 records in the array
        for (offset_t i = 0; i < 1000; i++) {
            record rec;
            rec.head = i + 1;
            rec.i = i * 100 + 1;
            memcpy(rec.s, "##########", 10);
            rec.flag = 1;
            vec.store_at(i, std::move(rec));
        }
    }
    {
        file_vec<record> vec2(test_path);
        for (offset_t o = 0; o < 1000; o++) {
            const auto &rec = vec2.const_at(o);
            REQUIRE(rec.flag == 1);
            REQUIRE(rec.head == o + 1);
            REQUIRE(rec.i == (int64_t)(o * 100 + 1));
            REQUIRE(strncmp(rec.s, "##########", 10) == 0);
        }
    }
    file_vec<record>::remove_file_vec(test_path);
}

 TEST_CASE("Adding and deleting some records, close the file, and reopen it", "[file_vec]") {
  std::vector<offset_t> victims = {5, 21, 64, 65, 125, 945};
   {
       file_vec<record> vec(test_path);
      // store 1000 records in the array
      for (offset_t i = 0; i < 1000; i++) {
        record rec;
        rec.head = i + 1;
        rec.i = i * 100 + 1;
        memcpy(rec.s, "##########", 10);
        rec.flag = 1;
        vec.store_at(i, std::move(rec));
      }
   }
   {
      file_vec<record> vec(test_path);
   
      // mark some records as deleted
      for (auto &v : victims) {
        vec.erase(v);
      }
   }

    {
      file_vec<record> vec(test_path);
      // check whether we can retrieve these records
      std::size_t v = 0;
      for (offset_t o = 0; o < 1000; o++) {
        try {
          const auto &rec = vec.const_at(o);
          REQUIRE(rec.flag != 0);
          REQUIRE(rec.head == o + 1);
          REQUIRE(rec.i == (int64_t)(o * 100 + 1));
          REQUIRE(strncmp(rec.s, "##########", 10) == 0);
        } catch (unknown_id &exc) {
          // make sure the record is from the delete list
          std::cout << "out_of_range: " << o << std::endl;
          REQUIRE(victims[v++] == o);
        }
      }
      REQUIRE(v == victims.size());
    }
     file_vec<record>::remove_file_vec(test_path);
}

TEST_CASE("Adding some records at scattered positions", "[file_vev]") {
    file_vec<record> vec(test_path);

    for (offset_t i = 0; i < 100; i++) {
      record rec;
      rec.head = i + 1;
      rec.i = i * 100 + 1;
      memcpy(rec.s, "##########", 10);
      rec.flag = 1;
      vec.store_at(i, std::move(rec));
    }

    for (offset_t i = 102; i < 150; i++) {
      record rec;
      rec.head = i + 1;
      rec.i = i * 100 + 1;
      memcpy(rec.s, "##########", 10);
      rec.flag = 1;
      vec.store_at(i, std::move(rec));
    }

    for (offset_t i = 157; i < 300; i++) {
      record rec;
      rec.head = i + 1;
      rec.i = i * 100 + 1;
      memcpy(rec.s, "##########", 10);
      rec.flag = 1;
      vec.store_at(i, std::move(rec));
    }

    REQUIRE(vec.first_available() == 100);

    {
      auto i = 100;
      record rec;
      rec.head = i + 1;
      rec.i = i * 100 + 1;
      memcpy(rec.s, "##########", 10);
      rec.flag = 1;
      vec.store_at(i, std::move(rec));
    }

    REQUIRE(vec.first_available() == 101);
    {
      auto i = 101;
      record rec;
      rec.head = i + 1;
      rec.i = i * 100 + 1;
      memcpy(rec.s, "##########", 10);
      rec.flag = 1;
      vec.store_at(i, std::move(rec));
    }
    REQUIRE(vec.first_available() == 150);

     file_vec<record>::remove_file_vec(test_path);
  }

   TEST_CASE("Adding many records and iterate over them", "[file_vec]") {
    file_vec<record> vec(test_path);

    // make sure we have enough space for 1000 records
    REQUIRE(vec.capacity() > 1000);

    // store 1000 records in the array
    for (offset_t i = 0; i < 1000; i++) {
      record rec;
      rec.head = i + 1;
      rec.i = i * 100 + 1;
      memcpy(rec.s, "##########", 10);
      rec.flag = 1;
      vec.store_at(i, std::move(rec));
    }

    offset_t o = 0;
    for (auto &rec : vec) {
      REQUIRE(rec.flag == 1);
      REQUIRE(rec.head == o + 1);
      REQUIRE(rec.i == (int64_t)(o * 100 + 1));
      REQUIRE(strncmp(rec.s, "##########", 10) == 0);
      o++;
    }

    std::vector<offset_t> elems(1000);
    for (auto i = 0u; i < 1000; i++) elems[i] = i;

    // delete some records
    std::vector<offset_t> victims = {945, 125, 65, 64, 21, 5};
    for (auto &v : victims) {
      vec.erase(v);
      elems.erase(elems.begin() + v);
    }

    o = 0;
    for (auto &rec : vec) {
      REQUIRE(rec.flag == 1);
      REQUIRE(rec.head == elems[o] + 1);
      REQUIRE(rec.i == (int64_t)(elems[o] * 100 + 1));
      REQUIRE(strncmp(rec.s, "##########", 10) == 0);
      o++;
    }

     file_vec<record>::remove_file_vec(test_path);
  }