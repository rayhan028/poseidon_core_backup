#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do
                          // this in one cpp file

#include "catch.hpp"
#include "config.h"
#include "graph_pool.hpp"
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

TEST_CASE("Building CSR from Sequential and parallel table scan", "[format_converter]"){
#ifndef USE_GUNROCK
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("my_graph");
  create_known_data(graph);

  csr_arrays csr1;
  auto weight_func = [](auto& r) { return 1.3; };
  graph->run_transaction([&]() {
    graph->host_csr_build(csr1, weight_func);
    return true;
  });

  std::vector<uint64_t> row_offs = {0, 2, 3, 4, 5, 5, 6, 7};
  std::vector<uint64_t> col_inds = {6, 1, 2, 3, 4, 6, 2};
  std::vector<float> edge_vals = {1.3, 1.3, 1.3, 1.3, 1.3, 1.3, 1.3};

  REQUIRE(csr1.row_offsets == row_offs);
  REQUIRE(csr1.col_indices == col_inds);
  REQUIRE(csr1.edge_values == edge_vals);

#ifdef USE_PMDK
  csr_arrays csr2;
  graph->run_transaction([&]() {
    graph->parallel_host_csr_build(csr2, weight_func);
    return true;
  });

  REQUIRE(csr2.row_offsets == row_offs);
  REQUIRE(csr2.col_indices == col_inds);
  REQUIRE(csr2.edge_values == edge_vals);
#endif

  graph_pool::destroy(pool);
#endif
}