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
#include "qop.hpp"
#include "query.hpp"
#include "graph_pool.hpp"

using namespace boost::posix_time;
const std::string test_path = poseidon::gPmemPath + "query_test";

void create_data(graph_db_ptr graph) {
  auto tx = graph->begin_transaction();

      graph->add_node("Node", {{"id", boost::any(7)},
                               {"name", boost::any(std::string("aaa7"))},
                               {"other", boost::any(std::string("BBB7"))}});

      graph->add_node("Node", {{"id", boost::any(6)},
                               {"name", boost::any(std::string("aaa6"))},
                               {"other", boost::any(std::string("BBB6"))}});

      graph->add_node("Node", {{"id", boost::any(5)},
                               {"name", boost::any(std::string("aaa5"))},
                               {"other", boost::any(std::string("BBB5"))}});

      graph->add_node("Node", {{"id", boost::any(4)},
                               {"name", boost::any(std::string("aaa4"))},
                               {"other", boost::any(std::string("BBB4"))}});

      graph->add_node("Node", {{"id", boost::any(3)},
                               {"name", boost::any(std::string("aaa3"))},
                               {"other", boost::any(std::string("BBB3"))}});

      graph->add_node("Node", {{"id", boost::any(2)},
                               {"name", boost::any(std::string("aaa2"))},
                               {"other", boost::any(std::string("BBB2"))}});

      graph->add_node("Node", {{"id", boost::any(1)},
                               {"name", boost::any(std::string("aaa1"))},
                               {"other", boost::any(std::string("BBB1"))}});

  graph->commit_transaction();
}

void create_join_data(graph_db_ptr graph) {
  auto tx = graph->begin_transaction();

  graph->add_node("Node1", {{"id", boost::any(1)}});
  graph->add_node("Node1", {{"id", boost::any(2)}});
  graph->add_node("Node2", {{"id", boost::any(3)}});
  graph->add_node("Node2", {{"id", boost::any(4)}});

  graph->commit_transaction();
}


TEST_CASE("Testing query operators", "[qop]") {
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("my_graph");

  create_data(graph);

  auto tx = graph->begin_transaction();

  namespace pj = builtin;

  SECTION("limit") {
    result_set rs, expected;
    auto q = query(graph).all_nodes("Node").limit(3).collect(rs);
    q.start();

    rs.wait();

    REQUIRE(rs.data.size() == 3);
    q.print_plan();
  }

  SECTION("count") {
    result_set rs, expected;
    auto q = query(graph).all_nodes("Node").count().collect(rs);
    q.start();

    rs.wait();
    expected.data.push_back({query_result("7")});

    REQUIRE(rs == expected);
    q.print_plan();
  }

  SECTION("order by") {
    result_set rs, expected;
    auto q = query(graph)
                 .all_nodes("Node")
                 .project({PExpr_(0, pj::int_property(res, "id"))})
                 .orderby([&](const qr_tuple &qr1, const qr_tuple &qr2) {
                   return boost::get<int>(qr1[0]) < boost::get<int>(qr2[0]);
                 })
                 .collect(rs);
    q.start();

    rs.wait();
    for (int i = 1; i <= 7; i++) {
      expected.data.push_back({query_result(std::to_string(i))});
    }
    REQUIRE(rs == expected);
    q.print_plan();
  }

  SECTION("has string property") {
    result_set rs, expected;
    auto dc = graph->get_code("aaa4");
    REQUIRE(dc != 0);
    auto q = query(graph)
                 .all_nodes("Node")
                 .property("name", [dc](auto &p) { return p.equal(dc); })
                 .project({PExpr_(0, pj::int_property(res, "id")),
                           PExpr_(0, pj::string_property(res, "name"))})
                 .collect(rs);
    q.start();

    rs.wait();
    expected.append({query_result(std::to_string(4)), query_result("aaa4")});
    REQUIRE(rs == expected);
    q.print_plan();
  }

  SECTION("has label") {
    graph->add_node("Movie", {{"title", boost::any(std::string("m1"))}});
    graph->add_node("Movie", {{"title", boost::any(std::string("m2"))}});
    graph->add_node("Actor", {{"name", boost::any(std::string("p1"))}});
    graph->add_node("Actor", {{"name", boost::any(std::string("p2"))}});

    result_set rs, expected;
    auto q = query(graph)
                 .all_nodes()
                 .has_label("Movie")
                 .project({PExpr_(0, pj::string_property(res, "title"))})
                 .collect(rs);
    q.start();

    rs.wait();
    expected.append({query_result("m1")});
    expected.append({query_result("m2")});
    REQUIRE(rs == expected);
    q.print_plan();
  }

  SECTION("node scan with label and property predicate") {
    result_set rs, expected;
    auto dc = graph->get_code("aaa4");
    REQUIRE(dc != 0);
    auto q = query(graph)
                 .nodes_where("Node", "name", [dc](auto &p) { return p.equal(dc); })
                 .project({PExpr_(0, pj::int_property(res, "id")),
                           PExpr_(0, pj::string_property(res, "name"))})
                 .collect(rs);
    q.start();

    rs.wait();
    expected.append({query_result(std::to_string(4)), query_result("aaa4")});
    REQUIRE(rs == expected);
    q.print_plan();
  }

  SECTION("use index") {
    graph->commit_transaction();
    tx = graph->begin_transaction();

    // create index
    auto idx = graph->create_index("Node", "id");
 
    result_set rs, expected;
    auto q = query(graph)
              .nodes_where_indexed("Node", "id", 3)
              .project({PExpr_(0, pj::int_property(res, "id")),
                        PExpr_(0, pj::string_property(res, "name"))})
              .collect(rs);
    q.start();

    rs.wait();
    expected.append({query_result(std::to_string(3)), query_result("aaa3")});
    REQUIRE(rs == expected);
    q.print_plan();
  }

  SECTION("where_qr_tuple") {
    result_set rs, expected;
    auto q = query(graph)
                 .all_nodes("Node")
                 .project({PExpr_(0, pj::int_property(res, "id"))})
                 .where_qr_tuple([&](const auto &v) {
                   return boost::get<int>(v[0]) > 4; })
                 .collect(rs);
    q.start();

    rs.wait();
    expected.append({query_result("7")});
    expected.append({query_result("6")});
    expected.append({query_result("5")});
    REQUIRE(rs == expected);
    q.print_plan();
  }

  SECTION("append_to_qr_tuple") {
    result_set rs, expected;
    auto q = query(graph)
                 .all_nodes("Node")
                 .project({PExpr_(0, pj::int_property(res, "id"))})
                 .append_to_qr_tuple([&](const auto &v) {
                   auto val = boost::get<int>(v[0]) + 10;
                   return query_result(val); })
                 .collect(rs);
    q.start();

    rs.wait();
    expected.append({query_result("7"), query_result("17")});
    expected.append({query_result("6"), query_result("16")});
    expected.append({query_result("5"), query_result("15")});
    expected.append({query_result("4"), query_result("14")});
    expected.append({query_result("3"), query_result("13")});
    expected.append({query_result("2"), query_result("12")});
    expected.append({query_result("1"), query_result("11")});
    REQUIRE(rs == expected);
    q.print_plan();
  }
  graph->abort_transaction();

  graph_pool::destroy(pool);
}

TEST_CASE("Testing join operators", "[qop]") {
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("my_graph");

  // prepare some data
  create_join_data(graph);

  auto tx = graph->begin_transaction();

  namespace pj = builtin;

  SECTION("cross join") {
    result_set rs, expected;
    auto q1 = query(graph).all_nodes("Node2");

    auto q2 = query(graph)
                  .all_nodes("Node1")
                  .crossjoin(q1)
                  .project({PExpr_(0, pj::int_property(res, "id")),
                            PExpr_(1, pj::int_property(res, "id"))})
                  .collect(rs);
    query::start({&q1, &q2});

    rs.wait();
    expected.data.push_back({query_result("1"), query_result("3")});
    expected.data.push_back({query_result("1"), query_result("4")});
    expected.data.push_back({query_result("2"), query_result("3")});
    expected.data.push_back({query_result("2"), query_result("4")});
    REQUIRE(rs == expected);
    query::print_plans({&q1, &q2});
  }

  graph->abort_transaction();

  graph_pool::destroy(pool);
}

TEST_CASE("Projecting node and relationship datetime properties", "[graph_db]") {
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("my_graph");

  auto tx = graph->begin_transaction();

  auto a = graph->add_node("Person",
      {{"id", boost::any(1)},
      {"creationDate", boost::any(time_from_string(std::string("2011-11-01 00:05:56.000")))}});
  auto b = graph->add_node("Place",
      {{"id", boost::any(24)},
      {"creationDate", boost::any(time_from_string(std::string("2010-06-10 11:50:26.000")))}});

  graph->add_relationship(a, b, ":isLocatedIn",
      {{"creationDate", boost::any(time_from_string(std::string("2011-11-02 13:00:00.000")))}});

  graph->commit_transaction();
  tx = graph->begin_transaction();

  namespace pj = builtin;
  result_set rs, expected;
  auto q = query(graph)
                  .all_nodes("Person")
                  .from_relationships(":isLocatedIn")
                  .to_node("Place")
                  .project({PExpr_(0, pj::ptime_property(res, "creationDate")),
                            PExpr_(2, pj::ptime_property(res, "creationDate")),
                            PExpr_(1, pj::ptime_property(res, "creationDate")) })
                  .collect(rs);
    q.start();
    rs.wait();

  expected.data.push_back(
    {query_result("2011-11-01T00:05:56"),
    query_result("2010-06-10T11:50:26"),
    query_result("2011-11-02T13:00:00")});
  REQUIRE(rs == expected);

  graph->commit_transaction();

  graph_pool::destroy(pool);
}

TEST_CASE("Finding Unweighted Shortest Path", "[shortest_path]") {
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("my_graph");

  std::list<path_item> ss_paths;
  path_visitor path_vis = [&](node &n, const path &p) { return; };
  auto rpred = [&](relationship &r) {
                return std::string(graph->get_string(r.rship_label)) == ":knows"; };

  auto tx = graph->begin_transaction();

  auto a = graph->add_node(":Person", {{"name",
            boost::any(std::string("John"))}, {"age", boost::any(42)}});
  auto b = graph->add_node(":Person", {{"name", boost::any(std::string("Ann"))},
                                {"age", boost::any(36)}});
  auto c = graph->add_node(":Person", {{"name", boost::any(std::string("Pete"))},
                                {"age", boost::any(58)}});
  auto d = graph->add_node(":Person", {{"name", boost::any(std::string("Han"))},
                                {"age", boost::any(13)}});
  auto e = graph->add_node(":Person", {{"name", boost::any(std::string("Zaki"))},
                                {"age", boost::any(47)}});

  graph->add_relationship(a, b, ":knows", {});
  graph->add_relationship(b, c, ":knows", {});
  graph->add_relationship(c, d, ":knows", {});
  graph->add_relationship(a, e, ":knows", {});
  graph->add_relationship(d, e, ":knows", {});

  std::vector<uint64_t> exp_path = {0, 4, 3};
  bool found = unweighted_shortest_path(graph, a, d, true, rpred, path_vis, ss_paths);

  REQUIRE(found);
  REQUIRE(ss_paths.front().get_hops() == 2);
  REQUIRE(ss_paths.front().get_path() == exp_path);

  graph->commit_transaction();

  graph_pool::destroy(pool);
}

TEST_CASE("Weighted Shortest Path", "[shortest_path]") {
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("my_graph");

  path_item ss_path;
  path_visitor path_vis = [&](node &n, const path &p) { return; };
  auto rpred = [&](relationship &r) {
                return std::string(graph->get_string(r.rship_label)) == ":knows"; };

  auto rweight = [&](relationship &r) {
        auto &src = graph->node_by_id(r.from_node_id());
        auto &des = graph->node_by_id(r.to_node_id());
        auto src_descr = graph->get_node_description(r.from_node_id());
        auto des_descr = graph->get_node_description(r.to_node_id());
        auto src_age = get_property<int>(src_descr.properties, 
                                      std::string("age")).value();
        auto des_age = get_property<int>(des_descr.properties, 
                                      std::string("age")).value();
        return (double)(src_age + des_age); };

  auto tx = graph->begin_transaction();

  auto a = graph->add_node(":Person", {{"name",
            boost::any(std::string("John"))}, {"age", boost::any(42)}});
  auto b = graph->add_node(":Person", {{"name", boost::any(std::string("Ann"))},
                                {"age", boost::any(36)}});
  auto c = graph->add_node(":Person", {{"name", boost::any(std::string("Pete"))},
                                {"age", boost::any(58)}});
  auto d = graph->add_node(":Person", {{"name", boost::any(std::string("Han"))},
                                {"age", boost::any(13)}});
  auto e = graph->add_node(":Person", {{"name", boost::any(std::string("Zaki"))},
                                {"age", boost::any(47)}});

  graph->add_relationship(a, b, ":knows", {});
  graph->add_relationship(b, c, ":knows", {});
  graph->add_relationship(c, d, ":knows", {});
  graph->add_relationship(a, e, ":knows", {});
  graph->add_relationship(d, e, ":knows", {});

  weighted_shortest_path(graph, a, d, true, rpred, rweight, path_vis, ss_path);

  REQUIRE(ss_path.get_weight() == 238.0);

  graph->commit_transaction();

  graph_pool::destroy(pool);
}

TEST_CASE("Top K Weighted Shortest Paths", "[shortest_path]") {
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("my_graph");

  std::size_t k = 2;
  std::vector<path_item> k_spath;
  path_visitor path_vis = [&](node &n, const path &p) { return; };
  auto rpred = [&](relationship &r) {
                return std::string(graph->get_string(r.rship_label)) == ":knows"; };

  auto rweight = [&](relationship &r) {
        auto &src = graph->node_by_id(r.from_node_id());
        auto &des = graph->node_by_id(r.to_node_id());
        auto src_descr = graph->get_node_description(r.from_node_id());
        auto des_descr = graph->get_node_description(r.to_node_id());
        auto src_age = get_property<int>(src_descr.properties, 
                                      std::string("age")).value();
        auto des_age = get_property<int>(des_descr.properties, 
                                      std::string("age")).value();
        return (double)(src_age + des_age); };

  auto tx = graph->begin_transaction();

  auto a = graph->add_node(":Person", {{"name",
            boost::any(std::string("John"))}, {"age", boost::any(42)}});
  auto b = graph->add_node(":Person", {{"name", boost::any(std::string("Ann"))},
                                {"age", boost::any(36)}});
  auto c = graph->add_node(":Person", {{"name", boost::any(std::string("Pete"))},
                                {"age", boost::any(58)}});
  auto d = graph->add_node(":Person", {{"name", boost::any(std::string("Han"))},
                                {"age", boost::any(13)}});
  auto e = graph->add_node(":Person", {{"name", boost::any(std::string("Zaki"))},
                                {"age", boost::any(47)}});
  auto f = graph->add_node(":Person", {{"name", boost::any(std::string("Zaki"))},
                                {"age", boost::any(81)}});
  auto g = graph->add_node(":Person", {{"name", boost::any(std::string("Zaki"))},
                                {"age", boost::any(23)}});

  graph->add_relationship(a, b, ":knows", {});
  graph->add_relationship(b, c, ":knows", {});
  graph->add_relationship(c, d, ":knows", {});
  graph->add_relationship(e, d, ":knows", {});
  graph->add_relationship(f, e, ":knows", {});
  graph->add_relationship(a, g, ":knows", {});
  graph->add_relationship(g, f, ":knows", {});

  k_weighted_shortest_path(graph, a, d, k, true, rpred, rweight, path_vis, k_spath);

  REQUIRE(k_spath.size() == 2);

  graph->commit_transaction();

  graph_pool::destroy(pool);
}

TEST_CASE("Testing query profiling", "[qop]") {
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("my_graph");

  create_data(graph);
  auto dc = graph->get_code("aaa3");
  graph->run_transaction([&]() {
    result_set rs;
    auto q = query(graph)
              .all_nodes("Node")
              .property("name", [&](auto &p) { return p.equal(dc); })
              .collect(rs);
    q.start();
    rs.wait();
    q.print_plan();
    return true;
  });
  graph_pool::destroy(pool);
}

TEST_CASE("Testing union_all operator", "[qop]") {
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("my_graph");

  namespace pj = builtin;

  create_data(graph);
  auto ab = graph->get_code("aaa3");
  auto cd = graph->get_code("aaa7");
  graph->run_transaction([&]() {
    result_set rs, expected;
    expected.append({query_result("aaa3")});
    expected.append({query_result("aaa7")});

    auto q1 = query(graph)
              .all_nodes("Node")
              .property("name", [&](auto &p) { return p.equal(ab); })
              .project({PExpr_(0, pj::string_property(res, "name"))});
    
    auto q2 = query(graph)
              .all_nodes("Node")
              .property("name", [&](auto &p) { return p.equal(cd); })
              .project({PExpr_(0, pj::string_property(res, "name"))})
              .union_all(q1)
              .collect(rs);

    query::start({&q1, &q2});
    rs.wait();
    query::print_plans({&q1, &q2});

    REQUIRE(rs == expected);
    return true;
  });
  graph_pool::destroy(pool);
}

TEST_CASE("Testing union_all operator 2", "[qop]") {
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("my_graph");

  namespace pj = builtin;

  create_data(graph);
  auto a = graph->get_code("aaa1");
  auto b = graph->get_code("aaa2");
  auto c = graph->get_code("aaa3");
  auto d = graph->get_code("aaa4");
  graph->run_transaction([&]() {
    result_set rs, expected;
    expected.append({query_result("aaa1")});
    expected.append({query_result("aaa2")});
    expected.append({query_result("aaa3")});
    expected.append({query_result("aaa4")});

    auto q1 = query(graph)
              .all_nodes("Node")
              .property("name", [&](auto &p) { return p.equal(a); })
              .project({PExpr_(0, pj::string_property(res, "name"))});

    auto q2 = query(graph)
              .all_nodes("Node")
              .property("name", [&](auto &p) { return p.equal(b); })
              .project({PExpr_(0, pj::string_property(res, "name"))});

    auto q3 = query(graph)
              .all_nodes("Node")
              .property("name", [&](auto &p) { return p.equal(c); })
              .project({PExpr_(0, pj::string_property(res, "name"))});
    
    auto q4 = query(graph)
              .all_nodes("Node")
              .property("name", [&](auto &p) { return p.equal(d); })
              .project({PExpr_(0, pj::string_property(res, "name"))})
              .union_all({&q1, &q2, &q3})
              .collect(rs);

    query::start({&q1, &q2, &q3, &q4});
    rs.wait();
    query::print_plans({&q1, &q2, &q3, &q4});

    REQUIRE(rs == expected);
    return true;
  });
  graph_pool::destroy(pool);
}

TEST_CASE("Testing outgoing traversal operators", "[qop]") {
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("my_graph");

  namespace pj = builtin;

  graph->run_transaction([&]() {

    auto a = graph->add_node("Person", {{"id", boost::any(1)},
                                        {"firstName", boost::any(std::string("A"))}});
    auto b = graph->add_node("Person", {{"id", boost::any(2)},
                                        {"firstName", boost::any(std::string("B"))}});
    auto c = graph->add_node("Person", {{"id", boost::any(3)},
                                        {"firstName", boost::any(std::string("C"))}});
    auto d = graph->add_node("Person", {{"id", boost::any(4)},
                                        {"firstName", boost::any(std::string("D"))}});
    auto e = graph->add_node("Person", {{"id", boost::any(5)},
                                        {"firstName", boost::any(std::string("E"))}});
    auto f = graph->add_node("Person", {{"id", boost::any(6)},
                                        {"firstName", boost::any(std::string("F"))}});

    graph->add_relationship(a, b, ":knows", {});
    graph->add_relationship(a, c, ":knows", {});
    graph->add_relationship(a, d, ":knows", {});
    graph->add_relationship(b, e, ":knows", {});
    graph->add_relationship(e, f, ":knows", {});
    return true;
  });

  graph->run_transaction([&]() {

    SECTION("foreach_from_relationship") {
      result_set rs, expected;
      auto q = query(graph)
                .all_nodes("Person")
                .property("firstName", [&](auto &p) { return p.equal(graph->get_code("A")); })
                .from_relationships(":knows")
                .to_node("Person")
                .project({PExpr_(2, pj::string_property(res, "firstName"))})
                .collect(rs);

      q.start();
      rs.wait();
      q.print_plan();

      expected.data.push_back({query_result("D")});
      expected.data.push_back({query_result("C")});
      expected.data.push_back({query_result("B")});

      REQUIRE(rs == expected);
    }

    SECTION("foreach_variable_from_relationship") {
      result_set rs, expected;
      auto q = query(graph)
                .all_nodes("Person")
                .property("firstName", [&](auto &p) { return p.equal(graph->get_code("A")); })
                .from_relationships({1, 3}, ":knows")
                .to_node("Person")
                .project({PExpr_(2, pj::string_property(res, "firstName"))})
                .collect(rs);

      q.start();
      rs.wait();
      q.print_plan();

      expected.data.push_back({query_result("D")});
      expected.data.push_back({query_result("C")});
      expected.data.push_back({query_result("B")});
      expected.data.push_back({query_result("E")});
      expected.data.push_back({query_result("F")});

      REQUIRE(rs == expected);
    }

    return true;
  });
  graph_pool::destroy(pool);
}

TEST_CASE("Testing incoming traversal operators", "[qop]") {
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("my_graph");

  namespace pj = builtin;

  graph->run_transaction([&]() {

    auto a = graph->add_node("Person", {{"id", boost::any(1)},
                                        {"firstName", boost::any(std::string("A"))}});
    auto b = graph->add_node("Person", {{"id", boost::any(2)},
                                        {"firstName", boost::any(std::string("B"))}});
    auto c = graph->add_node("Person", {{"id", boost::any(3)},
                                        {"firstName", boost::any(std::string("C"))}});
    auto d = graph->add_node("Person", {{"id", boost::any(4)},
                                        {"firstName", boost::any(std::string("D"))}});
    auto e = graph->add_node("Person", {{"id", boost::any(5)},
                                        {"firstName", boost::any(std::string("E"))}});
    auto f = graph->add_node("Person", {{"id", boost::any(6)},
                                        {"firstName", boost::any(std::string("F"))}});

    graph->add_relationship(a, b, ":knows", {});
    graph->add_relationship(a, c, ":knows", {});
    graph->add_relationship(a, d, ":knows", {});
    graph->add_relationship(b, e, ":knows", {});
    graph->add_relationship(e, f, ":knows", {});
    return true;
  });

  graph->run_transaction([&]() {

    SECTION("foreach_from_relationship") {
      result_set rs, expected;
      auto q = query(graph)
                .all_nodes("Person")
                .property("firstName", [&](auto &p) { return p.equal(graph->get_code("F")); })
                .to_relationships(":knows")
                .from_node("Person")
                .project({PExpr_(2, pj::string_property(res, "firstName"))})
                .collect(rs);

      q.start();
      rs.wait();
      q.print_plan();

      expected.data.push_back({query_result("E")});

      REQUIRE(rs == expected);
    }

    SECTION("foreach_variable_from_relationship") {
      result_set rs, expected;
      auto q = query(graph)
                .all_nodes("Person")
                .property("firstName", [&](auto &p) { return p.equal(graph->get_code("F")); })
                .to_relationships({1, 3}, ":knows")
                .from_node("Person")
                .project({PExpr_(2, pj::string_property(res, "firstName"))})
                .collect(rs);

      q.start();
      rs.wait();
      q.print_plan();

      expected.data.push_back({query_result("E")});
      expected.data.push_back({query_result("B")});
      expected.data.push_back({query_result("A")});

      REQUIRE(rs == expected);
    }

    return true;
  });
  graph_pool::destroy(pool);
}

TEST_CASE("Testing other Join operators", "[qop]") {
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("my_graph");

  namespace pj = builtin;

  graph->run_transaction([&]() {

    auto a = graph->add_node("Person", {{"id", boost::any(1)},
                                        {"firstName", boost::any(std::string("A"))}});
    auto b = graph->add_node("Person", {{"id", boost::any(2)},
                                        {"firstName", boost::any(std::string("B"))}});
    auto c = graph->add_node("Person", {{"id", boost::any(3)},
                                        {"firstName", boost::any(std::string("C"))}});
    auto d = graph->add_node("Person", {{"id", boost::any(4)},
                                        {"firstName", boost::any(std::string("D"))}});
    auto e = graph->add_node("Person", {{"id", boost::any(5)},
                                        {"firstName", boost::any(std::string("E"))}});
    auto f = graph->add_node("Person", {{"id", boost::any(6)},
                                        {"firstName", boost::any(std::string("F"))}});

    graph->add_relationship(a, b, ":knows", {});
    graph->add_relationship(a, c, ":knows", {});
    graph->add_relationship(a, d, ":knows", {});
    graph->add_relationship(b, e, ":knows", {});
    graph->add_relationship(e, f, ":knows", {});
    return true;
  });

  graph->run_transaction([&]() {

    SECTION("hashjoin_on_node") {
      result_set rs, expected;
      auto q1 = query(graph)
                .all_nodes("Person")
                .property("firstName", [&](auto &p) { return p.equal(graph->get_code("A")); })
                .from_relationships(":knows")
                .to_node("Person");

      auto q2 = query(graph)
                .all_nodes("Person")
                .property("firstName", [&](auto &p) { return p.equal(graph->get_code("A")); })
                .from_relationships({1, 3}, ":knows")
                .to_node("Person")
                .hashjoin_on_node({2, 2}, q1)
                .project({PExpr_(0, pj::string_property(res, "firstName")),
                          PExpr_(2, pj::string_property(res, "firstName")),
                          PExpr_(3, pj::string_property(res, "firstName")),
                          PExpr_(5, pj::string_property(res, "firstName"))})
                .collect(rs);

      query::start({&q1, &q2});
      rs.wait();
      query::print_plans({&q1, &q2});

      expected.data.push_back({query_result("A"), query_result("D"),
                              query_result("A"), query_result("D")});
      expected.data.push_back({query_result("A"), query_result("C"),
                              query_result("A"), query_result("C")});
      expected.data.push_back({query_result("A"), query_result("B"),
                              query_result("A"), query_result("B")});

      REQUIRE(rs == expected);
    }

    SECTION("join_on_node") {
      result_set rs, expected;
      auto q1 = query(graph)
                .all_nodes("Person")
                .property("firstName", [&](auto &p) { return p.equal(graph->get_code("A")); })
                .from_relationships(":knows")
                .to_node("Person");

      auto q2 = query(graph)
                .all_nodes("Person")
                .property("firstName", [&](auto &p) { return p.equal(graph->get_code("A")); })
                .from_relationships({1, 3}, ":knows")
                .to_node("Person")
                .join_on_node({2, 2}, q1)
                .project({PExpr_(0, pj::string_property(res, "firstName")),
                          PExpr_(2, pj::string_property(res, "firstName")),
                          PExpr_(3, pj::string_property(res, "firstName")),
                          PExpr_(5, pj::string_property(res, "firstName"))})
                .collect(rs);

      query::start({&q1, &q2});
      rs.wait();
      query::print_plans({&q1, &q2});

      expected.data.push_back({query_result("A"), query_result("D"),
                              query_result("A"), query_result("D")});
      expected.data.push_back({query_result("A"), query_result("C"),
                              query_result("A"), query_result("C")});
      expected.data.push_back({query_result("A"), query_result("B"),
                              query_result("A"), query_result("B")});

      REQUIRE(rs == expected);
    }

    SECTION("outerjoin_on_node") {
      result_set rs, expected;
      auto q1 = query(graph)
                .all_nodes("Person")
                .property("firstName", [&](auto &p) { return p.equal(graph->get_code("A")); })
                .from_relationships(":knows")
                .to_node("Person");

      auto q2 = query(graph)
                .all_nodes("Person")
                .property("firstName", [&](auto &p) { return p.equal(graph->get_code("A")); })
                .from_relationships({1, 3}, ":knows")
                .to_node("Person")
                .outerjoin_on_node({2, 2}, q1)
                .project({PExpr_(0, pj::string_property(res, "firstName")),
                          PExpr_(2, pj::string_property(res, "firstName")),
                          PExpr_(3, pj::string_property(res, "firstName")),
                          PExpr_(5, pj::string_property(res, "firstName"))})
                .collect(rs);

      query::start({&q1, &q2});
      rs.wait();
      query::print_plans({&q1, &q2});

      expected.data.push_back({query_result("A"), query_result("D"),
                              query_result("A"), query_result("D")});
      expected.data.push_back({query_result("A"), query_result("C"),
                              query_result("A"), query_result("C")});
      expected.data.push_back({query_result("A"), query_result("B"),
                              query_result("A"), query_result("B")});
      expected.data.push_back({query_result("A"), query_result("E"),
                              query_result("NULL"), query_result("NULL")});
      expected.data.push_back({query_result("A"), query_result("F"),
                              query_result("NULL"), query_result("NULL")});

      REQUIRE(rs == expected);
    }

    SECTION("join_on_rship") {
      result_set rs, expected;
      auto q1 = query(graph)
                .all_nodes("Person");

      auto q2 = query(graph)
                .all_nodes("Person")
                .join_on_rship({0, 0}, q1)
                .project({PExpr_(0, pj::string_property(res, "firstName")),
                          PVar_(2),
                          PExpr_(1, pj::string_property(res, "firstName"))})
                .collect(rs);

      query::start({&q1, &q2});
      rs.wait();
      query::print_plans({&q1, &q2});

      expected.data.push_back({query_result("A"), query_result("::knows[0]{}"), query_result("B")});
      expected.data.push_back({query_result("A"), query_result("::knows[1]{}"), query_result("C")});
      expected.data.push_back({query_result("A"), query_result("::knows[2]{}"), query_result("D")});
      expected.data.push_back({query_result("B"), query_result("::knows[3]{}"), query_result("E")});
      expected.data.push_back({query_result("E"), query_result("::knows[4]{}"), query_result("F")});

      REQUIRE(rs == expected);
    }

    SECTION("outerjoin_on_rship") {
      result_set rs, expected;
      auto q1 = query(graph)
                .all_nodes("Person");

      auto q2 = query(graph)
                .all_nodes("Person")
                .outerjoin_on_rship({0, 0}, q1)
                .project({PExpr_(0, pj::string_property(res, "firstName")),
                          PVar_(2),
                          PExpr_(1, pj::string_property(res, "firstName"))})
                .collect(rs);

      query::start({&q1, &q2});
      rs.wait();
      query::print_plans({&q1, &q2});

      expected.data.push_back({query_result("A"), query_result("::knows[0]{}"), query_result("B")});
      expected.data.push_back({query_result("A"), query_result("::knows[1]{}"), query_result("C")});
      expected.data.push_back({query_result("A"), query_result("::knows[2]{}"), query_result("D")});
      expected.data.push_back({query_result("B"), query_result("::knows[3]{}"), query_result("E")});
      expected.data.push_back({query_result("C"), query_result("NULL"), query_result("NULL")});
      expected.data.push_back({query_result("D"), query_result("NULL"), query_result("NULL")});
      expected.data.push_back({query_result("E"), query_result("::knows[4]{}"), query_result("F")});
      expected.data.push_back({query_result("F"), query_result("NULL"), query_result("NULL")});

      REQUIRE(rs == expected);
    }

    return true;
  });
  graph_pool::destroy(pool);
}

TEST_CASE("Testing the existence of relationship", "[qop]") {
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("my_graph");

  namespace pj = builtin;

  graph->run_transaction([&]() {

    auto a = graph->add_node("Person", {{"id", boost::any(1)},
                                        {"firstName", boost::any(std::string("A"))}});
    auto b = graph->add_node("Person", {{"id", boost::any(2)},
                                        {"firstName", boost::any(std::string("B"))}});
    auto c = graph->add_node("Person", {{"id", boost::any(3)},
                                        {"firstName", boost::any(std::string("C"))}});
    auto d = graph->add_node("Person", {{"id", boost::any(4)},
                                        {"firstName", boost::any(std::string("D"))}});
    auto e = graph->add_node("Person", {{"id", boost::any(5)},
                                        {"firstName", boost::any(std::string("E"))}});
    auto f = graph->add_node("Person", {{"id", boost::any(6)},
                                        {"firstName", boost::any(std::string("F"))}});

    graph->add_relationship(a, b, ":knows", {});
    graph->add_relationship(a, c, ":knows", {});
    graph->add_relationship(a, d, ":knows", {});
    graph->add_relationship(b, e, ":knows", {});
    graph->add_relationship(e, f, ":knows", {});
    return true;
  });

  graph->run_transaction([&]() {

    SECTION("rship_exists without NULL append") {
      result_set rs, expected;
      auto q1 = query(graph)
                .all_nodes("Person");

      auto q2 = query(graph)
                .all_nodes("Person")
                .property("firstName", [&](auto &p) { return p.equal(graph->get_code("A")); })
                .crossjoin(q1)
                .rship_exists({0,1}, false)
                .project({PExpr_(0, pj::string_property(res, "firstName")),
                          PVar_(2),
                          PExpr_(1, pj::string_property(res, "firstName"))})
                .collect(rs);

      query::start({&q1, &q2});
      rs.wait();
      query::print_plans({&q1, &q2});

      expected.data.push_back({query_result("A"), query_result("::knows[0]{}"), query_result("B")});
      expected.data.push_back({query_result("A"), query_result("::knows[1]{}"), query_result("C")});
      expected.data.push_back({query_result("A"), query_result("::knows[2]{}"), query_result("D")});

      REQUIRE(rs == expected);
    }

    SECTION("rship_exists with NULL append") {
      result_set rs, expected;
      auto q1 = query(graph)
                .all_nodes("Person");

      auto q2 = query(graph)
                .all_nodes("Person")
                .property("firstName", [&](auto &p) { return p.equal(graph->get_code("A")); })
                .crossjoin(q1)
                .rship_exists({0,1}, true)
                .project({PExpr_(0, pj::string_property(res, "firstName")),
                          PVar_(2),
                          PExpr_(1, pj::string_property(res, "firstName"))})
                .collect(rs);

      query::start({&q1, &q2});
      rs.wait();
      query::print_plans({&q1, &q2});

      expected.data.push_back({query_result("A"), query_result("NULL"), query_result("A")});
      expected.data.push_back({query_result("A"), query_result("::knows[0]{}"), query_result("B")});
      expected.data.push_back({query_result("A"), query_result("::knows[1]{}"), query_result("C")});
      expected.data.push_back({query_result("A"), query_result("::knows[2]{}"), query_result("D")});
      expected.data.push_back({query_result("A"), query_result("NULL"), query_result("E")});
      expected.data.push_back({query_result("A"), query_result("NULL"), query_result("F")});

      REQUIRE(rs == expected);
    }

    return true;
  });
  graph_pool::destroy(pool);
}

TEST_CASE("Testing Groupby operator", "[qop]") {
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("my_graph");

  namespace pj = builtin;

  graph->run_transaction([&]() {

    graph->add_node("Person", {{"age", boost::any(42)},
                              {"firstName", boost::any(std::string("John"))},
                              {"lastName", boost::any(std::string("Doe"))}});

    graph->add_node("Person", {{"age", boost::any(77)},
                              {"firstName", boost::any(std::string("Michael"))},
                              {"lastName", boost::any(std::string("Stonebreaker"))}});

    graph->add_node("Person", {{"age", boost::any(48)},
                              {"firstName", boost::any(std::string("Anastasia"))},
                              {"lastName", boost::any(std::string("Ailamaki"))}});

    graph->add_node("Person", {{"age", boost::any(37)},
                              {"firstName", boost::any(std::string("John"))},
                              {"lastName", boost::any(std::string("Jones"))}});

    graph->add_node("Person", {{"age", boost::any(20)},
                              {"firstName", boost::any(std::string("John"))},
                              {"lastName", boost::any(std::string(""))}});

    graph->add_node("Person", {{"age", boost::any(100)},
                              {"firstName", boost::any(std::string("Michael"))},
                              {"lastName", boost::any(std::string("G."))}});

    result_set rs, expected;
    auto q = query(graph)
              .all_nodes("Person")
              .project({PExpr_(0, pj::int_property(res, "age")),
                        PExpr_(0, pj::string_property(res, "firstName")),
                        PExpr_(0, pj::string_property(res, "lastName"))})
              .groupby({1}, {{"count", 0}, {"pcount", 0}, {"avg", 0}, {"sum", 0}})
              .collect(rs);

    q.start();
    rs.wait();
    q.print_plan();

    expected.data.push_back(
      {query_result("John"), query_result("3"), query_result("50.000000"),
        query_result("33.000000"), query_result("99")});
    expected.data.push_back(
      {query_result("Michael"), query_result("2"), query_result("33.333333"),
        query_result("88.500000"), query_result("177")});
    expected.data.push_back(
      {query_result("Anastasia"), query_result("1"), query_result("16.666667"),
        query_result("48.000000"), query_result("48")});

    REQUIRE(rs == expected);
    return true;
  });
  graph_pool::destroy(pool);
}

TEST_CASE("Testing Bi-directional traversal operator", "[qop]") {
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("my_graph");

  namespace pj = builtin;

  graph->run_transaction([&]() {

    auto A = graph->add_node("Person", {{"age", boost::any(42)},
                              {"firstName", boost::any(std::string("AAA"))},
                              {"lastName", boost::any(std::string("aaa"))}});

    auto B = graph->add_node("Person", {{"age", boost::any(77)},
                              {"firstName", boost::any(std::string("BBB"))},
                              {"lastName", boost::any(std::string("bbb"))}});

    auto C = graph->add_node("Person", {{"age", boost::any(48)},
                              {"firstName", boost::any(std::string("CCC"))},
                              {"lastName", boost::any(std::string("ccc"))}});

    auto D = graph->add_node("Person", {{"age", boost::any(37)},
                              {"firstName", boost::any(std::string("DDD"))},
                              {"lastName", boost::any(std::string("ddd"))}});

    auto E = graph->add_node("Person", {{"age", boost::any(20)},
                              {"firstName", boost::any(std::string("EEE"))},
                              {"lastName", boost::any(std::string("eee"))}});
    
    graph->add_relationship(A, C, ":knows", {});
    graph->add_relationship(B, C, ":knows", {});
    graph->add_relationship(C, D, ":knows", {});
    graph->add_relationship(C, E, ":knows", {});

    return true;
  });

  graph->run_transaction([&]() {

    result_set rs, expected;
    auto q = query(graph)
              .nodes_where("Person", "age",
                          [&](auto &p) { return p.equal(48); })
              .all_relationships(":knows")
              .project({PExpr_(2, pj::string_property(res, "firstName"))})
              .collect(rs);

    q.start();
    rs.wait();
    q.print_plan();

    expected.data.push_back({query_result("EEE")});
    expected.data.push_back({query_result("DDD")});
    expected.data.push_back({query_result("BBB")});
    expected.data.push_back({query_result("AAA")});

    REQUIRE(rs == expected);
    return true;
  });
  graph_pool::destroy(pool);
}