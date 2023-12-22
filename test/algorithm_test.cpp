#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do
                          // this in one cpp file
#define CATCH_CONFIG_CONSOLE_WIDTH 160

#include <boost/algorithm/string.hpp>

#include "config.h"
#include "defs.hpp"
#include "graph_pool.hpp"
#include "qop.hpp"
#include "qop_algorithm.hpp"
#include "query_proc.hpp"

#include <catch2/catch_test_macros.hpp>

using namespace boost::posix_time;
namespace dll = boost::dll;

const std::string test_path = PMDK_PATH("algo_tst");

TEST_CASE("Testing queries with algorithms", "[algorithm]") {
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("my_algo_graph");

  query_ctx ctx(graph);
  query_proc qp(ctx);

   ctx.run_transaction([&]() {
      auto frankfurt = graph->add_node(
          "City", {{"name", std::any(std::string("Frankfurt"))}});
      auto mannheim = graph->add_node(
          "City", {{"name", std::any(std::string("Mannheim"))}});
      auto wuerzburg = graph->add_node(
          "City", {{"name", std::any(std::string("Würzburg"))}});
      auto stuttgart = graph->add_node(
          "City", {{"name", std::any(std::string("Stuttgart"))}});
      auto karlsruhe = graph->add_node(
          "City", {{"name", std::any(std::string("Karslruhe"))}});
      auto erfurt =
          graph->add_node("City", {{"name", std::any(std::string("Erfurt"))}});
      auto nuernberg = graph->add_node(
          "City", {{"name", std::any(std::string("Nürnberg"))}});
      auto kassel =
          graph->add_node("City", {{"name", std::any(std::string("Kassel"))}});
      auto augsburg = graph->add_node(
          "City", {{"name", std::any(std::string("Augsburg"))}});
      auto muenchen =
          graph->add_node("City", {{"name", std::any(std::string("München"))}});

      graph->add_relationship(frankfurt, mannheim, "connects",
                              {{"distance", std::any(85.0)}});
      graph->add_relationship(frankfurt, wuerzburg, "connects",
                              {{"distance", std::any(217.0)}});
      graph->add_relationship(frankfurt, kassel, "connects",
                              {{"distance", std::any(173.0)}});
      graph->add_relationship(mannheim, karlsruhe, "connects",
                              {{"distance", std::any(80.0)}});
      graph->add_relationship(wuerzburg, erfurt, "connects",
                              {{"distance", std::any(186.0)}});
      graph->add_relationship(wuerzburg, nuernberg, "connects",
                              {{"distance", std::any(103.0)}});
      graph->add_relationship(stuttgart, nuernberg, "connects",
                              {{"distance", std::any(183.0)}});
      graph->add_relationship(karlsruhe, augsburg, "connects",
                              {{"distance", std::any(250.0)}});
      graph->add_relationship(augsburg, muenchen, "connects",
                              {{"distance", std::any(84.0)}});
      graph->add_relationship(nuernberg, muenchen, "connects",
                              {{"distance", std::any(167.0)}});
      graph->add_relationship(kassel, muenchen, "connects",
                              {{"distance", std::any(502.0)}});

      return true;
    });

 
  SECTION("Shortest Path") {
    auto res = qp.execute_query(
        query_proc::Interpret,
        "Algorithm([ShortestPath, TUPLE, 'connects', 0, 0], CrossJoin( "
        "Filter($0.name == 'Frankfurt', NodeScan('City')), Filter($0.name == "
        "'München', NodeScan('City'))))",
        true);

    result_set expected;
    expected.append({qv_("City[0]{name: \"Frankfurt\"}"),
                     qv_("City[9]{name: \"München\"}"), qv_("[ 0 7 9 ]")});

    REQUIRE(res.result() == expected);
  }

  SECTION("Weighted Shortest Path") {
    auto res = qp.execute_query(
        query_proc::Interpret,
        "Algorithm([WeightedShortestPath, TUPLE, 'connects', 'distance', 0, 0], CrossJoin( "
        "Filter($0.name == 'Frankfurt', NodeScan('City')), Filter($0.name == "
        "'München', NodeScan('City'))))",
        true);
    std::cout << res << std::endl;
    result_set expected;
    expected.append({qv_("City[0]{name: \"Frankfurt\"}"),
                     qv_("City[9]{name: \"München\"}"), qv_("[ 0 2 6 9 ]")});

    REQUIRE(res.result() == expected);
  }

  
  graph_pool::destroy(pool);
}