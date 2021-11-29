#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do
                          // this in one cpp file

#include "catch.hpp"
#include "config.h"
#include "graph_pool.hpp"
#include "format_converter.hpp"
#include <chrono>
#include <boost/dynamic_bitset.hpp>

const std::string test_path = poseidon::gPmemPath + "format_converter_test";

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

TEST_CASE("Building CSR from full sequential table scan", "[format_converter]"){
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("my_graph");
  create_known_data(graph);

  std::vector<uint64_t> row_offs = {};
  std::vector<uint64_t> col_inds = {};
  std::vector<float> edge_vals = {};
  auto weight_func = [](auto& r) { return 1.3; };

  graph->run_transaction([&]() {
    poseidon_to_csr(graph, row_offs, col_inds,
                                    edge_vals, weight_func);
    return true;
  });

  std::vector<uint64_t> r = {0, 2, 3, 4, 5, 5, 6, 7};
  std::vector<uint64_t> c = {6, 1, 2, 3, 4, 6, 2};
  std::vector<float> e = {1.3, 1.3, 1.3, 1.3, 1.3, 1.3, 1.3};

  REQUIRE(row_offs == r);
  REQUIRE(col_inds == c);
  REQUIRE(edge_vals == e);

  graph_pool::destroy(pool);
}

TEST_CASE("Building CSR from parallel table scan", "[format_converter]"){
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("my_graph");
  create_known_data(graph);

  std::vector<uint64_t> row_offs = {};
  std::vector<uint64_t> col_inds = {};
  std::vector<float> edge_vals = {};
  auto weight_func = [](auto& r) { return 1.3; };

  graph->run_transaction([&]() {
    parallel_poseidon_to_csr(graph, row_offs, col_inds,
                                    edge_vals, weight_func);
    return true;
  });

  std::vector<uint64_t> r = {0, 2, 3, 4, 5, 5, 6, 7};
  std::vector<uint64_t> c = {6, 1, 2, 3, 4, 6, 2};
  std::vector<float> e = {1.3, 1.3, 1.3, 1.3, 1.3, 1.3, 1.3};

  REQUIRE(row_offs == r);
  REQUIRE(col_inds == c);
  REQUIRE(edge_vals == e);

  graph_pool::destroy(pool);
}

TEST_CASE("Sequential and parallel table scan", "[format_converter]"){
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("my_graph");
  create_known_data(graph);

  std::vector<uint64_t> row_offs1 = {};
  std::vector<uint64_t> col_inds1 = {};
  std::vector<float> edge_vals1 = {};
  auto weight_func = [](auto& r) { return 1.3; };

  graph->run_transaction([&]() {
    poseidon_to_csr(graph, row_offs1, col_inds1,
                                    edge_vals1, weight_func);
    return true;
  });

  std::vector<uint64_t> row_offs2 = {};
  std::vector<uint64_t> col_inds2 = {};
  std::vector<float> edge_vals2 = {};

  graph->run_transaction([&]() {
    parallel_poseidon_to_csr(graph, row_offs2, col_inds2,
                                    edge_vals2, weight_func);
    return true;
  });

  REQUIRE(row_offs1 == row_offs2);
  REQUIRE(col_inds1 == col_inds2);
  REQUIRE(edge_vals1 == edge_vals2);

  graph_pool::destroy(pool);
}