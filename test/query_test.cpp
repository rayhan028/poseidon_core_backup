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

// -------

TEST_CASE("Projecting dtimestring property of node", "[graph_db]") {
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("my_graph");

  auto tx = graph->begin_transaction();

auto post_id = graph->add_node(
    "Post",
    {
        {"id", boost::any(13743895)},
        {"creationDate",
          boost::any(builtin::dtimestring_to_int("2011-Oct-05 14:38:36.019"))}});

  graph->commit_transaction();
  tx = graph->begin_transaction();

  auto &post = graph->node_by_id(post_id);
  auto post_descr = graph->get_node_description(post_id);
  auto pr_property = std::string("creationDate");
  auto sec = get_property<int>(post_descr.properties, pr_property).value();
  assert(std::floor(sec) == sec);
  auto date = builtin::int_to_dtimestring(sec); // this works here -  but it does NOT work in the test below (relationsip)
  //auto test_date = builtin::int_to_dtimestring(1317825516); // this does not work here - but it works in the test below (relationsip)

  REQUIRE(sec == 1317825516);
  REQUIRE(date == "2011-Oct-05 14:38:36");

  graph->commit_transaction();

  graph_pool::destroy(pool);
}


TEST_CASE("Projecting only PExpr_ of higher indexes", "[graph_db]") {
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("my_graph");

  auto tx = graph->begin_transaction();

  auto hoChi_id = graph->add_node(
      "Person",
      {{"id", boost::any(4194)},
       {"firstName", boost::any(std::string("Hồ Chí"))}, 
       {"lastName", boost::any(std::string("Do"))}});
  auto forum_id = graph->add_node(
      "Forum",
      {{"id", boost::any(37)},
       {"title", boost::any(std::string("Wall of Hồ Chí Do"))}});
  auto post_id = graph->add_node(
      "Post",
      {{"id", boost::any(16492674)}});
  auto comment1_id = graph->add_node(
      "Comment",
      {
          {"id", boost::any(16492675)}});
  auto comment2_id = graph->add_node(
      "Comment",
      {
          {"id", boost::any(16492676)}});
  auto comment3_id = graph->add_node(
      "Comment",
      {
          {"id", boost::any(16492677)}});

  graph->add_relationship(forum_id, post_id, ":containerOf", {});
  graph->add_relationship(forum_id, hoChi_id, ":hasModerator", {});
  graph->add_relationship(comment1_id, post_id, ":replyOf", {});
  graph->add_relationship(comment2_id, comment1_id, ":replyOf", {});
  graph->add_relationship(comment3_id, comment2_id, ":replyOf", {});

  graph->commit_transaction();
  tx = graph->begin_transaction();

  std::set<int> qr_result_f_id;
  std::set<int> qr_result_modrt_id;
  std::set<std::string> qr_result_f_title;
  std::set<std::string> qr_result_modrt_fName;
  std::set<std::string> qr_result_modrt_lName;
  
  auto &comment3 = graph->node_by_id(comment3_id);
  graph->foreach_variable_from_relationship_of_node(comment3, 1, 5, [&](auto &r1) {
    auto r1_label = std::string(graph->get_string(r1.rship_label));
    
    if (r1_label == ":replyOf"){
      auto &msg = graph->node_by_id(r1.to_node_id());
      auto msg_label = std::string(graph->get_string(msg.node_label));
      
      if (msg_label == "Post"){
        graph->foreach_to_relationship_of_node(msg, [&](auto &r2) {
          auto r2_label = std::string(graph->get_string(r2.rship_label));
          
          if (r2_label == ":containerOf"){
            auto &forum = graph->node_by_id(r2.from_node_id());
            auto forum_label = std::string(graph->get_string(forum.node_label));
            
            if (forum_label == "Forum"){
              graph->foreach_from_relationship_of_node(forum, [&](auto &r3) {
                auto r3_label = std::string(graph->get_string(r3.rship_label));
                
                if (r3_label == ":hasModerator"){
                  auto &modrt = graph->node_by_id(r3.to_node_id());
                  auto modrt_label = std::string(graph->get_string(modrt.node_label));
                  
                  if (modrt_label == "Person"){
                    auto forum_descr = graph->get_node_description(forum.id());
                    auto modrt_descr = graph->get_node_description(modrt.id());
                    auto f_id = get_property<int>(forum_descr.properties, 
                                                  std::string("id")).value();
                    auto f_title = get_property<std::string>(forum_descr.properties, 
                                                  std::string("title")).value();
                    auto modrt_id = get_property<int>(modrt_descr.properties, 
                                                  std::string("id")).value();
                    auto modrt_fName = get_property<std::string>(modrt_descr.properties, 
                                                  std::string("firstName")).value();
                    auto modrt_lName = get_property<std::string>(modrt_descr.properties, 
                                                  std::string("lastName")).value();
                    
                    qr_result_f_id.insert(f_id);
                    qr_result_modrt_id.insert(modrt_id);
                    qr_result_f_title.insert(f_title);
                    qr_result_modrt_fName.insert(modrt_fName);
                    qr_result_modrt_lName.insert(modrt_lName);

                  }
                }
              });
            }
          }
        });
      }
    }
  });

  REQUIRE(qr_result_f_id == std::set<int>({37}));
  REQUIRE(qr_result_modrt_id == std::set<int>({4194}));
  REQUIRE(qr_result_f_title == std::set<std::string>({"Wall of Hồ Chí Do"}));
  REQUIRE(qr_result_modrt_fName == std::set<std::string>({"Hồ Chí"}));
  REQUIRE(qr_result_modrt_lName == std::set<std::string>({"Do"}));

  graph->commit_transaction();

  graph_pool::destroy(pool);
}

TEST_CASE("Projecting PExpr_", "[graph_db]") {
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("my_graph");

  auto tx = graph->begin_transaction();

auto lomana_id = graph->add_node(
    "Person",
    {{"id", boost::any(15393)},
      {"firstName", boost::any(std::string("Lomana Trésor"))},
      {"lastName", boost::any(std::string("Kanam"))},
      {"gender", boost::any(std::string("male"))},
      {"birthday", boost::any(builtin::datestring_to_int("1986-09-22"))},
      {"creationDate",
      boost::any(builtin::dtimestring_to_int("2011-04-02 23:53:29.932"))},
      {"locationIP", boost::any(std::string("41.76.137.230"))},
      {"browser", boost::any(std::string("Chrome"))}});
auto amin_id = graph->add_node(
    "Person",
    {{"id", boost::any(19791)},
      {"firstName", boost::any(std::string("Amin"))},
      {"lastName", boost::any(std::string("Kamkar"))},
      {"gender", boost::any(std::string("male"))},
      {"birthday", boost::any(builtin::datestring_to_int("1989-05-24"))},
      {"creationDate",
      boost::any(builtin::dtimestring_to_int("2011-08-30 05:41:09.519"))},
      {"locationIP", boost::any(std::string("81.28.60.168"))},
      {"browser", boost::any(std::string("Internet Explorer"))}});
auto comment1_id = graph->add_node(
    "Comment",
    {
        {"id", boost::any(16492676)},
        {"creationDate", boost::any(builtin::dtimestring_to_int("2012-01-10 03:24:33.368"))},
        {"locationIP", boost::any(std::string("14.196.249.198"))},
        {"browser", boost::any(std::string("Firefox"))},
        {"content", boost::any(std::string("About Bruce Lee,  sources, in the spirit of "
        "his personal martial arts philosophy, whic"))},
        {"length", boost::any(86)}
    });
auto comment2_id = graph->add_node(
    "Comment",
    {
        {"id", boost::any(1642217)},
        {"creationDate", boost::any(builtin::dtimestring_to_int("2012-01-10 06:31:18.533"))},
        {"locationIP", boost::any(std::string("41.76.137.230"))},
        {"browser", boost::any(std::string("Chrome"))},
        {"content", boost::any(std::string("maybe"))},
        {"length", boost::any(5)}
    });
auto comment3_id = graph->add_node(
    "Comment",
    {
        {"id", boost::any(16492677)},
        {"creationDate", boost::any(builtin::dtimestring_to_int("2012-01-10 14:57:10.420"))},
        {"locationIP", boost::any(std::string("81.28.60.168"))},
        {"browser", boost::any(std::string("Internet Explorer"))},
        {"content", boost::any(std::string("I see"))},
        {"length", boost::any(5)}
    });

graph->add_relationship(comment2_id, comment1_id, ":replyOf", {});
graph->add_relationship(comment3_id, comment1_id, ":replyOf", {});
graph->add_relationship(comment2_id, lomana_id, ":hasCreator", {});
graph->add_relationship(comment3_id, amin_id, ":hasCreator", {});

  graph->commit_transaction();
  tx = graph->begin_transaction();

  std::set<int> qr_result_cmnt_id;
  std::set<int> qr_result_author_id;
  std::set<std::string> qr_result_cmnt_content;
  std::set<std::string> qr_result_author_fName;
  std::set<std::string> qr_result_author_lName;

  auto &comment1 = graph->node_by_id(comment1_id);
  graph->foreach_to_relationship_of_node(comment1, [&](auto &r1) {
    auto r1_label = std::string(graph->get_string(r1.rship_label));
    
    if (r1_label == ":replyOf"){
      auto &msg = graph->node_by_id(r1.from_node_id());
      auto msg_label = std::string(graph->get_string(msg.node_label));
      
      if (msg_label == "Comment"){
        graph->foreach_from_relationship_of_node(msg, [&](auto &r2) {
          auto r2_label = std::string(graph->get_string(r2.rship_label));
          
          if (r2_label == ":hasCreator"){
            auto &creator = graph->node_by_id(r2.to_node_id());
            auto creator_label = std::string(graph->get_string(creator.node_label));
            
            if (creator_label == "Person"){
              auto msg_descr = graph->get_node_description(msg.id());
              auto creator_descr = graph->get_node_description(creator.id());
              auto cmnt_id = get_property<int>(msg_descr.properties, 
                                            std::string("id")).value();
              auto cmnt_content = get_property<std::string>(msg_descr.properties, 
                                            std::string("content")).value();
              auto author_id = get_property<int>(creator_descr.properties, 
                                            std::string("id")).value();
              auto author_fName = get_property<std::string>(creator_descr.properties, 
                                            std::string("firstName")).value();
              auto author_lName = get_property<std::string>(creator_descr.properties, 
                                            std::string("lastName")).value();
              
              qr_result_cmnt_id.insert(cmnt_id);
              qr_result_author_id.insert(author_id);
              qr_result_cmnt_content.insert(cmnt_content);
              qr_result_author_fName.insert(author_fName);
              qr_result_author_lName.insert(author_lName);
            }
          }
        });
      }
    }
  });

  REQUIRE(qr_result_cmnt_id == std::set<int>({1642217, 16492677}));
  REQUIRE(qr_result_author_id == std::set<int>({15393, 19791}));
  REQUIRE(qr_result_cmnt_content == std::set<std::string>({"I see", "maybe"}));
  REQUIRE(qr_result_author_fName == std::set<std::string>({"Amin", "Lomana Trésor"}));
  REQUIRE(qr_result_author_lName == std::set<std::string>({"Kamkar", "Kanam"}));

  graph->commit_transaction();

  graph_pool::destroy(pool);
} 

TEST_CASE("Finding Unweighted Shortest Path", "[shortest_path]") {
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("my_graph");

  path_item ss_path;
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
  bool found = unweighted_shortest_path(graph, a, d, true, rpred, path_vis, ss_path);

  REQUIRE(found);
  REQUIRE(ss_path.get_hops() == 2);
  REQUIRE(ss_path.get_path() == exp_path);

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