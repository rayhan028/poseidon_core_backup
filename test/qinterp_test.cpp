#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do
                          // this in one cpp file
#define CATCH_CONFIG_CONSOLE_WIDTH 160

#include <boost/algorithm/string.hpp>

#include "config.h"
#include "defs.hpp"
#include "graph_pool.hpp"
#include "qop.hpp"
#include "query_proc.hpp"
#include <catch2/catch_test_macros.hpp>

using namespace boost::posix_time;
namespace dll = boost::dll;

const std::string test_path = PMDK_PATH("qinterp_tst");

void create_data(graph_db_ptr &graph) {
  spdlog::info("create data...");
  graph->run_transaction([&]() {
    auto ravalomanana = graph->add_node(
        "Person", {{"id", boost::any((uint64_t)65)},
                   {"firstName", boost::any(std::string("Marc"))},
                   {"lastName", boost::any(std::string("Ravalomanana"))}});
    auto p1 = graph->add_node(
        "Person", {{"id", boost::any((uint64_t)1379)},
                   {"firstName", boost::any(std::string("Muhammad"))},
                   {"lastName", boost::any(std::string("Iqbal"))}});
    auto p2 = graph->add_node("Person",
                              {{"id", boost::any((uint64_t)1291)},
                               {"firstName", boost::any(std::string("Wei"))},
                               {"lastName", boost::any(std::string("Li"))}});
    auto p3 = graph->add_node("Person",
                              {{"id", boost::any((uint64_t)1121)},
                               {"firstName", boost::any(std::string("Karl"))},
                               {"lastName", boost::any(std::string("Beran"))}});
    auto post1 = graph->add_node(
        "Post",
        {{"id", boost::any((uint64_t)1863)},
         {"imageFile", boost::any(std::string(""))},
         {"creationDate",
          boost::any(time_from_string(std::string("2011-10-17 05:40:34.561")))},
         {"content", boost::any(std::string("Content of post1"))}});
    auto post2 = graph->add_node(
        "Post",
        {{"id", boost::any((uint64_t)1976)},
         {"imageFile", boost::any(std::string(""))},
         {"creationDate",
          boost::any(time_from_string(std::string("2012-01-14 09:41:00.992")))},
         {"content", boost::any(std::string("Content of post2"))}});
    auto post3 = graph->add_node(
        "Post",
        {{"id", boost::any((uint64_t)137438956)},
         {"imageFile", boost::any(std::string("photo1374389534791.jpg"))},
         {"creationDate",
          boost::any(time_from_string(std::string("2011-10-16 15:05:23.955")))},
         {"content", boost::any(std::string(""))}});
    auto post4 = graph->add_node(
        "Post",
        {{"id", boost::any((uint64_t)13743895)},
         {"imageFile", boost::any(std::string("photo1374999534791.jpg"))},
         {"creationDate",
          boost::any(time_from_string(std::string("2010-03-16 15:05:23.955")))},
         {"content", boost::any(std::string(""))}});
    auto post5 = graph->add_node(
        "Post",
        {{"id", boost::any((uint64_t)13743894)},
         {"imageFile", boost::any(std::string("photo1374991234791.jpg"))},
         {"creationDate",
          boost::any(time_from_string(std::string("2010-03-16 15:05:23.955")))},
         {"content", boost::any(std::string(""))}});
    auto cmt1 = graph->add_node(
        "Comment",
        {{"id", boost::any((uint64_t)1865)},
         {"creationDate",
          boost::any(time_from_string(std::string("2013-01-17 09:17:43.567")))},
         {"content", boost::any(std::string("Content of cmt1"))}});
    auto cmt2 = graph->add_node(
        "Comment",
        {{"id", boost::any((uint64_t)1877)},
         {"creationDate",
          boost::any(time_from_string(std::string("2013-2-17 10:59:33.177")))},
         {"content", boost::any(std::string("Content of cmt2"))}});
    auto cmt3 = graph->add_node(
        "Comment",
        {{"id", boost::any((uint64_t)1978)},
         {"creationDate",
          boost::any(time_from_string(std::string("2013-03-14 16:57:46.045")))},
         {"content", boost::any(std::string("Content of cmt3"))}});
    auto cmt4 = graph->add_node(
        "Comment",
        {{"id", boost::any((uint64_t)1126)},
         {"creationDate",
          boost::any(time_from_string(std::string("2013-04-16 21:16:03.354")))},
         {"content", boost::any(std::string("Content of cmt4"))}});
    auto cmt5 = graph->add_node(
        "Comment",
        {{"id", boost::any((uint64_t)1171)},
         {"creationDate",
          boost::any(time_from_string(std::string("2013-05-17 19:37:26.339")))},
         {"content", boost::any(std::string("Content of cmt5"))}});
    auto cmt6 = graph->add_node(
        "Comment",
        {{"id", boost::any((uint64_t)1768)},
         {"creationDate",
          boost::any(time_from_string(std::string("2013-06-29 17:57:51.844")))},
         {"content", boost::any(std::string("Content of cmt6"))}});
    auto cmt7 = graph->add_node(
        "Comment",
        {{"id", boost::any((uint64_t)1865)},
         {"creationDate",
          boost::any(time_from_string(std::string("2013-07-25 07:54:01.976")))},
         {"content", boost::any(std::string("Content of cmt7"))}});
    auto cmt8 = graph->add_node(
        "Comment",
        {{"id", boost::any((uint64_t)1878)},
         {"creationDate",
          boost::any(time_from_string(std::string("2013-08-25 12:56:57.280")))},
         {"content", boost::any(std::string("Content of cmt8"))}});
    auto cmt9 = graph->add_node(
        "Comment",
        {{"id", boost::any((uint64_t)1978)},
         {"creationDate",
          boost::any(time_from_string(std::string("2013-09-27 09:41:01.413")))},
         {"content", boost::any(std::string("Content of cmt9"))}});
    auto cmt10 = graph->add_node(
        "Comment",
        {{"id", boost::any((uint64_t)1126)},
         {"creationDate",
          boost::any(time_from_string(std::string("2013-10-26 23:46:18.580")))},
         {"content", boost::any(std::string("Content of cmt10"))}});
    auto cmt11 = graph->add_node(
        "Comment",
        {{"id", boost::any((uint64_t)1171)},
         {"creationDate",
          boost::any(time_from_string(std::string("2013-11-26 17:09:07.283")))},
         {"content", boost::any(std::string("Content of cmt11"))}});
    auto cmt12 = graph->add_node(
        "Comment",
        {{"id", boost::any((uint64_t)1768)},
         {"creationDate",
          boost::any(time_from_string(std::string("2013-12-27 11:32:19.336")))},
         {"content", boost::any(std::string("Content of cmt12"))}});

    auto mahinda = graph->add_node(
        "Person",
        {{"id", boost::any((uint64_t)933)},
         {"firstName", boost::any(std::string("Mahinda"))},
         {"lastName", boost::any(std::string("Perera"))},
         {"gender", boost::any(std::string("male"))},
         {"birthday", boost::any(std::string("1989-12-03"))},
         {"creationDate",
          boost::any(time_from_string(std::string("2010-02-14 15:32:10.447")))},
         {"locationIP", boost::any(std::string("119.235.7.103"))},
         {"browserUsed", boost::any(std::string("Firefox"))}});
    auto baruch = graph->add_node(
        "Person", {{"id", boost::any((uint64_t)4139)},
                   {"firstName", boost::any(std::string("Baruch"))},
                   {"lastName", boost::any(std::string("Dego"))}});
    auto fritz = graph->add_node(
        "Person", {{"id", boost::any((uint64_t)65970697)},
                   {"firstName", boost::any(std::string("Fritz"))},
                   {"lastName", boost::any(std::string("Muller"))}});
    auto andrei = graph->add_node(
        "Person", {{"id", boost::any((uint64_t)10995116)},
                   {"firstName", boost::any(std::string("Andrei"))},
                   {"lastName", boost::any(std::string("Condariuc"))}});
    auto ottoR = graph->add_node(
        "Person", {{"id", boost::any((uint64_t)838375)},
                   {"firstName", boost::any(std::string("Otto"))},
                   {"lastName", boost::any(std::string("Richter"))}});
    auto ottoB = graph->add_node(
        "Person", {{"id", boost::any((uint64_t)833579)},
                   {"firstName", boost::any(std::string("Otto"))},
                   {"lastName", boost::any(std::string("Becker"))}});
    auto hoChi = graph->add_node(
        "Person", {{"id", boost::any((uint64_t)4194)},
                   {"firstName", boost::any(std::string("Hồ Chí"))},
                   {"lastName", boost::any(std::string("Do"))}});
    auto lomana = graph->add_node(
        "Person", {{"id", boost::any((uint64_t)15393)},
                   {"firstName", boost::any(std::string("Lomana Trésor"))},
                   {"lastName", boost::any(std::string("Kanam"))}});
    auto amin = graph->add_node(
        "Person", {{"id", boost::any((uint64_t)19791)},
                   {"firstName", boost::any(std::string("Amin"))},
                   {"lastName", boost::any(std::string("Kamkar"))}});
    auto bingbing = graph->add_node(
        "Person", {{"id", boost::any((uint64_t)90796)},
                   {"firstName", boost::any(std::string("Bingbing"))},
                   {"lastName", boost::any(std::string("Xu"))}});
    auto Kelaniya =
        graph->add_node("Place", {{"id", boost::any((uint64_t)1353)}});
    auto Gobi = graph->add_node("Place", {{"id", boost::any((uint64_t)129)}});
    auto forum_37 = graph->add_node(
        "Forum", {{"id", boost::any((uint64_t)37)},
                  {"title", boost::any(std::string("Wall of Hồ Chí Do"))}});
    graph->add_node(
        "Post",
        {{"id", boost::any((uint64_t)1374389595)},
         {"imageFile", boost::any(std::string("photo1374389534791.jpg"))},
         {"creationDate",
          boost::any(time_from_string(std::string("2011-10-05 14:38:36.019")))},
         {"content", boost::any(std::string(""))}});
    auto post_16492674 = graph->add_node(
        "Post",
        {{"id", boost::any((uint64_t)16492674)},
         {"imageFile", boost::any(std::string(""))},
         {"content", boost::any(std::string("Content of post_16492674"))},
         {"creationDate", boost::any(time_from_string(
                              std::string("2012-01-09 08:05:28.922")))}});
    auto comment_16492676 = graph->add_node(
        "Comment",
        {{"id", boost::any((uint64_t)16492676)},
         {"creationDate",
          boost::any(time_from_string(std::string("2012-01-10 03:24:33.368")))},
         {"content", boost::any(std::string("Content of comment_16492676"))}});
    auto comment_12362343 = graph->add_node(
        "Comment",
        {{"id", boost::any((uint64_t)12362343)},
         {"creationDate",
          boost::any(time_from_string(std::string("2011-08-17 14:26:59.961")))},
         {"content", boost::any(std::string("Content of comment_12362343"))}});
    auto comment_16492675 = graph->add_node(
        "Comment",
        {{"id", boost::any((uint64_t)16492675)},
         {"creationDate",
          boost::any(time_from_string(std::string("2011-10-05 14:38:36.019")))},
         {"content", boost::any(std::string("Content of comment_16492675"))}});
    auto comment_16492677 = graph->add_node(
        "Comment",
        {{"id", boost::any((uint64_t)16492677)},
         {"creationDate",
          boost::any(time_from_string(std::string("2012-01-10 14:57:10.420")))},
         {"content", boost::any(std::string("Content of comment_16492677"))}});
    auto comment_1642217 = graph->add_node(
        "Comment",
        {{"id", boost::any((uint64_t)1642217)},
         {"creationDate",
          boost::any(time_from_string(std::string("2012-01-10 06:31:18.533")))},
         {"content", boost::any(std::string("Content of comment_1642217"))}});

    // IS 1
    graph->add_relationship(mahinda, Kelaniya, "isLocatedIn", {});
    graph->add_relationship(baruch, Gobi, "isLocatedIn", {});

    // IS 2
    graph->add_relationship(cmt1, ravalomanana, "hasCreator", {});
    graph->add_relationship(cmt2, ravalomanana, "hasCreator", {});
    graph->add_relationship(cmt3, ravalomanana, "hasCreator", {});
    graph->add_relationship(cmt4, ravalomanana, "hasCreator", {});
    graph->add_relationship(cmt5, ravalomanana, "hasCreator", {});
    graph->add_relationship(cmt6, ravalomanana, "hasCreator", {});
    graph->add_relationship(cmt7, ravalomanana, "hasCreator", {});
    graph->add_relationship(cmt8, ravalomanana, "hasCreator", {});
    graph->add_relationship(cmt9, ravalomanana, "hasCreator", {});
    graph->add_relationship(cmt10, ravalomanana, "hasCreator", {});
    graph->add_relationship(cmt11, ravalomanana, "hasCreator", {});
    graph->add_relationship(cmt12, ravalomanana, "hasCreator", {});

    graph->add_relationship(cmt1, post1, "replyOf", {});
    graph->add_relationship(cmt2, post2, "replyOf", {});
    graph->add_relationship(cmt3, post2, "replyOf", {});
    graph->add_relationship(cmt4, post3, "replyOf", {});
    graph->add_relationship(cmt5, post1, "replyOf", {});
    graph->add_relationship(cmt6, post1, "replyOf", {});
    graph->add_relationship(cmt7, post3, "replyOf", {});
    graph->add_relationship(cmt8, post3, "replyOf", {});
    graph->add_relationship(cmt9, post2, "replyOf", {});
    graph->add_relationship(cmt10, post5, "replyOf", {});
    graph->add_relationship(cmt11, cmt12, "replyOf", {});
    graph->add_relationship(cmt12, post4, "replyOf", {});

    graph->add_relationship(post1, ravalomanana, "hasCreator", {});
    graph->add_relationship(post2, p1, "hasCreator", {});
    graph->add_relationship(post3, p2, "hasCreator", {});
    graph->add_relationship(post4, p3, "hasCreator", {});
    graph->add_relationship(post5, ravalomanana, "hasCreator", {});

    // IS 3
    graph->add_relationship(
        mahinda, baruch, "knows",
        {{"creationDate", boost::any(time_from_string(
                              std::string("2010-03-13 07:37:21.718")))}});
    graph->add_relationship(
        mahinda, fritz, "knows",
        {{"creationDate", boost::any(time_from_string(
                              std::string("2010-09-20 09:42:43.187")))}});
    graph->add_relationship(
        mahinda, andrei, "knows",
        {{"creationDate", boost::any(time_from_string(
                              std::string("2011-01-02 06:43:41.955")))}});
    graph->add_relationship(
        mahinda, ottoB, "knows",
        {{"creationDate", boost::any(time_from_string(
                              std::string("2012-09-07 01:11:30.195")))}});
    graph->add_relationship(
        mahinda, ottoR, "knows",
        {{"creationDate", boost::any(time_from_string(
                              std::string("2012-09-07 01:11:30.195")))}});

    // IS 5
    graph->add_relationship(post_16492674, mahinda, "hasCreator", {});
    graph->add_relationship(comment_12362343, andrei, "hasCreator", {});

    // IS 6
    graph->add_relationship(forum_37, post_16492674, ":containerOf", {});
    graph->add_relationship(forum_37, hoChi, ":hasModerator", {});
    graph->add_relationship(comment_16492675, post_16492674, "replyOf", {});
    graph->add_relationship(comment_16492676, comment_16492675, "replyOf", {});
    graph->add_relationship(comment_16492677, comment_16492676, "replyOf", {});

    // IS 7
    graph->add_relationship(comment_1642217, comment_16492676, "replyOf", {});
    graph->add_relationship(comment_1642217, lomana, "hasCreator", {});
    graph->add_relationship(comment_16492677, amin, "hasCreator", {});
    graph->add_relationship(comment_16492676, bingbing, "hasCreator", {});
    graph->add_relationship(lomana, bingbing, "knows", {});

    // IU1
    graph->add_node(
        "Organisation",
        {{"id", boost::any(21)},
         {"type", boost::any(std::string("company"))},
         {"name", boost::any(std::string("Aerolíneas_Argentinas"))},
         {"url", boost::any(std::string(
                     "http://dbpedia.org/resource/Aerolíneas_Argentinas"))}},
        true);
    graph->add_node(
        "Organisation",
        {{"id", boost::any(3985)},
         {"type", boost::any(std::string("company"))},
         {"name", boost::any(std::string("Aerolíneas_Argentinas"))},
         {"url", boost::any(std::string(
                     "http://dbpedia.org/resource/Aerolíneas_Argentinas"))}},
        true);
    graph->add_node(
        "Place",
        {{"id", boost::any(32)},
         {"type", boost::any(std::string("country"))},
         {"name", boost::any(std::string("Norway"))},
         {"url", boost::any(std::string("http://dbpedia.org/resource/Norwa"))}},
        true);
    graph->add_node("Tag",
                    {{"id", boost::any(19)},
                     {"name", boost::any(std::string("José_Acasus"))},
                     {"url", boost::any(std::string(
                                 "http://dbpedia.org/resource/José_Acasuso"))}},
                    true);
    // IU 4
    graph->add_node("Tag",
                    {{"id", boost::any((uint64_t)2)},
                     {"name", boost::any(std::string("Snowboard"))},
                     {"url", boost::any(std::string(
                                 "http://dbpedia.org/resource/Snowboard"))},
                     {"hasType", boost::any((uint64_t)3)}});

    return true;
  });
}

TEST_CASE("Testing queries in interpreted mode", "[qinterp]") {
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("my_qi_graph");
  create_data(graph);
  graph->dump_dot("qinterp.dot");
  query_ctx ctx(graph);
  query_proc qp(ctx);

  SECTION("Testing queries") {
    {
      auto res = qp.execute_query(query_proc::Interpret, "NodeScan()");
      REQUIRE(res.result_size() == 46);
    }

    {
      auto res = qp.execute_query(
          query_proc::Interpret,
          "Project([$0.id:uint64, $0.title:string], NodeScan('Forum'))");
      result_set expected;
      expected.append({qv_("37"), qv_("Wall of Hồ Chí Do")});

      REQUIRE(res.result() == expected);
    }

    {
      auto res = qp.execute_query(query_proc::Interpret,
                                  "NodeScan(['Post', 'Comment'])");
      REQUIRE(res.result_size() == 24);
    }

    {
      auto res = qp.execute_query(query_proc::Interpret,
                                  "Limit(2, NodeScan(['Post', 'Comment']))");
      REQUIRE(res.result_size() == 2);
    }

    {
      auto res = qp.execute_query(
          query_proc::Interpret,
          "Project([$0.id:uint64, $0.firstName:string, $0.lastName:string], "
          "Filter($0.id == 833579, NodeScan('Person')))");
      result_set expected;
      expected.append({qv_("833579"), qv_("Otto"), qv_("Becker")});
      REQUIRE(res.result() == expected);
    }

    {
      auto res =
          qp.execute_query(query_proc::Interpret,
                           "Project([$0.firstName:string, $0.lastName:string], "
                           "Filter($0.firstName == 'Otto' && $0.lastName == "
                           "'Becker', NodeScan('Person')))");
      result_set expected;
      expected.append({qv_("Otto"), qv_("Becker")});

      REQUIRE(res.result() == expected);
    }

    {
      auto res =
          qp.execute_query(query_proc::Interpret,
                           "Project([$0.firstName:string, $0.lastName:string], "
                           "Filter($0.firstName == 'Otto' || $0.lastName == "
                           "'Becker', NodeScan('Person')))");
      result_set expected;
      expected.append({qv_("Otto"), qv_("Richter")});
      expected.append({qv_("Otto"), qv_("Becker")});

      REQUIRE(res.result() == expected);
    }

    {
      auto res =
          qp.execute_query(query_proc::Interpret,
                           "Project([$2.id:uint64], Expand(OUT, 'Person', "
                           "ForeachRelationship(FROM, 'knows', Filter($0.id "
                           "== 933, NodeScan('Person')))))");
      result_set expected;
      expected.append({qv_("838375")});
      expected.append({qv_("833579")});
      expected.append({qv_("10995116")});
      expected.append({qv_("65970697")});
      expected.append({qv_("4139")});

      REQUIRE(res.result() == expected);
    }

    {
      auto res = qp.execute_query(
          query_proc::Interpret,
          "Aggregate([count($0.lastName:string), min($0.id:uint64), "
          "max($0.id:uint64)], NodeScan('Person'))");
      result_set expected;
      expected.append({qv_("14"), qv_("65"), qv_("65970697")});

      REQUIRE(res.result() == expected);
    }

    {
      auto res = qp.execute_query(query_proc::Interpret,
                                  "Project([$2.id:uint64], Match((p1:Person "
                                  "{id: 933})-[:knows]->(p2:Person)))");
      result_set expected;
      expected.append({qv_("838375")});
      expected.append({qv_("833579")});
      expected.append({qv_("10995116")});
      expected.append({qv_("65970697")});
      expected.append({qv_("4139")});

      REQUIRE(res.result() == expected);
    }
  }

  SECTION("Testing creating a node") {
    auto res =
        qp.execute_query(query_proc::Interpret,
                         "Project([$0.id:int, $0.firstName:string, "
                         "$0.lastName:string], Create((p:Person { id: 12345, "
                         "firstName: 'Rocky', lastName: 'Balboa' })))");
    result_set expected;
    expected.append({qv_("12345"), qv_("Rocky"), qv_("Balboa")});

    REQUIRE(res.result() == expected);
  }

  graph_pool::destroy(pool);
}
