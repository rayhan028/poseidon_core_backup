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

void update_data(graph_db_ptr graph) {
  graph->begin_transaction();

  //
  //         +--- 10 ---+
  //         |     |     |
  //         |     |     |
  //         v     v     v
  //         0 --> 1 --> 2 --> 3 --> 4
  //         ^     ^     ^           ^
  //         |     |     |           |
  //         |     |     6 <-- 8     |
  //         9     |     ^     |     |
  //               |     |     |     |
  //               +---- 5 ----------+
  //                     ^     |
  //                     |     |
  //                     ------+
  //

  graph->add_relationship(5, 4, ":knows", {}); // adds nodes 5 and 4 to update delta
  graph->add_relationship(5, 1, ":knows", {}); // adds node 1 to update delta
  graph->delete_relationship(0, 6); // adds nodes 0 and 6 to update delta

  for (int i = 7; i < 11; i++) { // adds nodes 7, 8, 9 and 10 to append delta
    graph->add_node("Person",
              {{"name", boost::any(std::string("John Doe"))},
              {"age", boost::any(42)},
              {"id", boost::any(i)},
              {"dummy1", boost::any(std::string("Dummy"))},
              {"dummy2", boost::any(1.2345)}},
              true);
  }
  graph->add_relationship(8, 6, ":knows", {});
  graph->add_relationship(8, 5, ":knows", {});
  graph->add_relationship(9, 0, ":knows", {});
  graph->add_relationship(10, 2, ":knows", {}); // adds node 2 to update delta
  graph->add_relationship(10, 0, ":knows", {});
  graph->add_relationship(10, 1, ":knows", {});

  graph->commit_transaction();
}

TEST_CASE("Storing delta of transactional updates", "[csr_delta]"){
#if defined CSR_DELTA_STORE && defined USE_TX
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("my_graph");
  create_known_data(graph);

  csr_delta::delta_map_t update_deltas, append_deltas;
  graph->restore_csr_delta(update_deltas, append_deltas);

  csr_delta::delta_map_t update_map, append_map;

  append_map[0] = {{6, 1}, {1.3, 1.3}};
  append_map[1] = {{2}, {1.3}};
  append_map[2] = {{3}, {1.3}};
  append_map[3] = {{4}, {1.3}};
  append_map[4] = {{}, {}};
  append_map[5] = {{6}, {1.3}};
  append_map[6] = {{2}, {1.3}};

  REQUIRE(update_deltas.size() == update_map.size());
  REQUIRE(std::equal(update_deltas.begin(), update_deltas.end(),
                      update_map.begin()));

  REQUIRE(append_deltas.size() == append_deltas.size());
  REQUIRE(std::equal(append_deltas.begin(), append_deltas.end(),
                      append_map.begin()));

  update_deltas.clear();
  append_deltas.clear();
  update_map.clear();
  append_map.clear();

  update_data(graph);

  graph->restore_csr_delta(update_deltas, append_deltas);

  update_map[0] = {{1}, {1.3}};
  update_map[1] = {{2}, {1.3}};
  update_map[2] = {{3}, {1.3}};
  update_map[4] = {{}, {}};
  update_map[5] = {{1, 4, 6}, {1.3, 1.3, 1.3}};
  update_map[6] = {{2}, {1.3}};

  append_map[7] = {{}, {}};
  append_map[8] = {{5, 6}, {1.3, 1.3}};
  append_map[9] = {{0}, {1.3}};
  append_map[10] = {{1, 0, 2}, {1.3, 1.3, 1.3}};

  REQUIRE(update_deltas.size() == update_map.size());
  REQUIRE(std::equal(update_deltas.begin(), update_deltas.end(),
                      update_map.begin()));

  REQUIRE(append_deltas.size() == append_deltas.size());
  REQUIRE(std::equal(append_deltas.begin(), append_deltas.end(),
                      append_map.begin()));

  graph_pool::destroy(pool);
#endif
}