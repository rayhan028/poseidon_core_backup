#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do
                          // this in one cpp file

#include "catch.hpp"
#include "config.h"
#include "graph_pool.hpp"
#include "format_converter.hpp"
#include "csr_delta.hpp"
#include <chrono>
#include <boost/dynamic_bitset.hpp>

const std::string test_path = poseidon::gPmemPath + "csr_delta_test";

void create_known_data(graph_db_ptr graph) {
  graph->begin_transaction();

    //
    //  0 --> 1 --> 2 --> 3 --> 4
    //  |           ^
    //  |           |
    //  +---------> 6
    //              ^
    //              |
    //             5
    //

  for (int i = 0; i < 7; i++) {
    graph->add_node("Person",
                                  {{"name", boost::any(std::string("John Doe"))},
                                  {"age", boost::any(42)},
                                  {"id", boost::any(i)},
                                  {"dummy1", boost::any(std::string("Dummy"))},
                                  {"dummy2", boost::any(1.2345)}},
                                  true);
  }
  graph->add_relationship(0, 1, ":knows", {});
  graph->add_relationship(1, 2, ":knows", {});
  graph->add_relationship(2, 3, ":knows", {});
  graph->add_relationship(3, 4, ":knows", {});
  graph->add_relationship(5, 6, ":knows", {});
  graph->add_relationship(6, 2, ":knows", {});
  graph->add_relationship(0, 6, ":likes", {});
  graph->commit_transaction();
}

TEST_CASE("Adding entries to deltas", "[csr_delta]"){
// #if defined CSR_DELTA_STORE && defined USE_TX
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("my_graph");
  create_known_data(graph);

  auto &delta = *(graph->get_csr_delta());
  delta.add_update_delta(5, {6, 4, 1}, {1.3, 1.3, 1.3});
  delta.add_update_delta(0, {1}, {1.3});
  delta.add_append_delta(10, {2, 0, 1}, {1.3, 1.3, 1.3});
  delta.add_append_delta(8, {6, 5}, {1.3, 1.3});
  delta.add_append_delta(9, {0}, {1.3});

  auto &update_deltas = delta.get_update_deltas();
  auto &append_deltas = delta.get_append_deltas();

  SECTION("Update delta") {
    auto iter = update_deltas->begin();
    auto &a = *(iter->second->first);
    auto &b = *(iter->second->second);

    std::vector<uint64_t> a_ = {1};
    std::vector<double> b_ = {1.3};

    REQUIRE(iter->first == 0);
    REQUIRE(a == a_);
    REQUIRE(b == b_);

    iter++;
    auto &c = *(iter->second->first);
    auto &d = *(iter->second->second);

    std::vector<uint64_t> c_ = {6, 4, 1};
    std::vector<double> d_ = {1.3, 1.3, 1.3};

    REQUIRE(iter->first == 5);
    REQUIRE(c == c_);
    REQUIRE(d == d_);
  }

  SECTION("Append delta") {
    auto iter = append_deltas->begin();
    auto &a = *(iter->second->first);
    auto &b = *(iter->second->second);

    std::vector<uint64_t> a_ = {6, 5};
    std::vector<double> b_ = {1.3, 1.3};

    REQUIRE(iter->first == 8);
    REQUIRE(a == a_);
    REQUIRE(b == b_);

    iter++;
    auto &c = *(iter->second->first);
    auto &d = *(iter->second->second);

    std::vector<uint64_t> c_ = {0};
    std::vector<double> d_ = {1.3};

    REQUIRE(iter->first == 9);
    REQUIRE(c == c_);
    REQUIRE(d == d_);

    iter++;
    auto &e = *(iter->second->first);
    auto &f = *(iter->second->second);

    std::vector<uint64_t> e_ = {2, 0, 1};
    std::vector<double> f_ = {1.3, 1.3, 1.3};

    REQUIRE(iter->first == 10);
    REQUIRE(e == e_);
    REQUIRE(f == f_);
  }

  graph_pool::destroy(pool);
// #endif
}