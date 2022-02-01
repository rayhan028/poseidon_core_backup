#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do
                          // this in one cpp file
#define CATCH_CONFIG_CONSOLE_WIDTH 160

#include <boost/algorithm/string.hpp>

#include "catch.hpp"
#include "config.h"
#include "defs.hpp"
#include "graph_pool.hpp"
#include "qproc.hpp"
#include "qop.hpp"

using namespace boost::posix_time;
namespace dll = boost::dll;

const std::string test_path = poseidon::gPmemPath + "qinterp_test";

void create_data(graph_db_ptr &graph) {
  graph->run_transaction([&]() {
    auto ravalomanana = graph->add_node("Person",
      {{"id", boost::any((uint64_t)65)},
       {"firstName", boost::any(std::string("Marc"))},
       {"lastName", boost::any(std::string("Ravalomanana"))}});
    auto p1 = graph->add_node( "Person",
      {{"id", boost::any((uint64_t)1379)},
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
    auto post1 = graph->add_node("Post",
      {{"id", boost::any((uint64_t)1863)}, 
       {"imageFile", boost::any(std::string(""))},
       {"creationDate", boost::any(time_from_string(std::string("2011-10-17 05:40:34.561")))},      
       {"content", boost::any(std::string("Content of post1"))}});
    auto post2 = graph->add_node("Post",
      {{"id", boost::any((uint64_t)1976)}, 
       {"imageFile", boost::any(std::string(""))},
       {"creationDate", boost::any(time_from_string(std::string("2012-01-14 09:41:00.992")))},      
       {"content", boost::any(std::string("Content of post2"))}});
    auto post3 = graph->add_node("Post",
      {{"id", boost::any((uint64_t)137438956)},
        {"imageFile", boost::any(std::string("photo1374389534791.jpg"))},
        {"creationDate", boost::any(time_from_string(std::string("2011-10-16 15:05:23.955")))},
        {"content", boost::any(std::string(""))}});
    auto post4 = graph->add_node("Post",
      {{"id", boost::any((uint64_t)13743895)},
        {"imageFile", boost::any(std::string("photo1374999534791.jpg"))},
        {"creationDate", boost::any(time_from_string(std::string("2010-03-16 15:05:23.955")))},
        {"content", boost::any(std::string(""))}});
    auto post5 = graph->add_node("Post",
      {{"id", boost::any((uint64_t)13743894)},
        {"imageFile", boost::any(std::string("photo1374991234791.jpg"))},
        {"creationDate", boost::any(time_from_string(std::string("2010-03-16 15:05:23.955")))},
        {"content", boost::any(std::string(""))}});
    auto cmt1 = graph->add_node("Comment",
      {{"id", boost::any((uint64_t)1865)},
        {"creationDate", boost::any(time_from_string(std::string("2013-01-17 09:17:43.567")))},
        {"content", boost::any(std::string("Content of cmt1"))}});
    auto cmt2 = graph->add_node("Comment",
      {{"id", boost::any((uint64_t)1877)},
        {"creationDate", boost::any(time_from_string(std::string("2013-2-17 10:59:33.177")))},
        {"content", boost::any(std::string("Content of cmt2"))}});
    auto cmt3 = graph->add_node("Comment",
      {{"id", boost::any((uint64_t)1978)},
        {"creationDate", boost::any(time_from_string(std::string("2013-03-14 16:57:46.045")))},
        {"content", boost::any(std::string("Content of cmt3"))}});
    auto cmt4 = graph->add_node("Comment",
      {{"id", boost::any((uint64_t)1126)},
        {"creationDate", boost::any(time_from_string(std::string("2013-04-16 21:16:03.354")))},
        {"content", boost::any(std::string("Content of cmt4"))}});
    auto cmt5 = graph->add_node("Comment",
      {{"id", boost::any((uint64_t)1171)},
        {"creationDate", boost::any(time_from_string(std::string("2013-05-17 19:37:26.339")))},
        {"content", boost::any(std::string("Content of cmt5"))}});
    auto cmt6 = graph->add_node("Comment",
      {{"id", boost::any((uint64_t)1768)},
        {"creationDate", boost::any(time_from_string(std::string("2013-06-29 17:57:51.844")))},
        {"content", boost::any(std::string("Content of cmt6"))}});
    auto cmt7 = graph->add_node("Comment",
      {{"id", boost::any((uint64_t)1865)},
        {"creationDate", boost::any(time_from_string(std::string("2013-07-25 07:54:01.976")))},
        {"content", boost::any(std::string("Content of cmt7"))}});
    auto cmt8 = graph->add_node("Comment",
      {{"id", boost::any((uint64_t)1877)},
        {"creationDate", boost::any(time_from_string(std::string("2013-08-25 12:56:57.280")))},
        {"content", boost::any(std::string("Content of cmt8"))}});
    auto cmt9 = graph->add_node("Comment",
      {{"id", boost::any((uint64_t)1978)},
        {"creationDate", boost::any(time_from_string(std::string("2013-09-27 09:41:01.413")))},
        {"content", boost::any(std::string("Content of cmt9"))}});
    auto cmt10 = graph->add_node("Comment",
      {{"id", boost::any((uint64_t)1126)},
        {"creationDate", boost::any(time_from_string(std::string("2013-10-26 23:46:18.580")))},
        {"content", boost::any(std::string("Content of cmt10"))}});
    auto cmt11 = graph->add_node("Comment",
      {{"id", boost::any((uint64_t)1171)},
        {"creationDate", boost::any(time_from_string(std::string("2013-11-26 17:09:07.283")))},
        {"content", boost::any(std::string("Content of cmt11"))}});
    auto cmt12 = graph->add_node("Comment",
      {{"id", boost::any((uint64_t)1768)},
        {"creationDate", boost::any(time_from_string(std::string("2013-12-27 11:32:19.336")))},
        {"content", boost::any(std::string("Content of cmt12"))}});

    auto mahinda = graph->add_node("Person",
      {{"id", boost::any((uint64_t)933)},
       {"firstName", boost::any(std::string("Mahinda"))},
       {"lastName", boost::any(std::string("Perera"))},
       {"gender", boost::any(std::string("male"))},
       {"birthday", boost::any(std::string("1989-12-03"))},
       {"creationDate", boost::any(time_from_string(std::string("2010-02-14 15:32:10.447")))},
       {"locationIP", boost::any(std::string("119.235.7.103"))},
       {"browserUsed", boost::any(std::string("Firefox"))}});
    auto baruch = graph->add_node("Person",
      {{"id", boost::any((uint64_t)4139)},
       {"firstName", boost::any(std::string("Baruch"))},
       {"lastName", boost::any(std::string("Dego"))}});
    auto fritz = graph->add_node("Person",
      {{"id", boost::any((uint64_t)65970697)},
       {"firstName", boost::any(std::string("Fritz"))},
       {"lastName", boost::any(std::string("Muller"))}});
    auto andrei = graph->add_node("Person",
      {{"id", boost::any((uint64_t)10995116)},
       {"firstName", boost::any(std::string("Andrei"))},
       {"lastName", boost::any(std::string("Condariuc"))}});
    auto ottoR = graph->add_node("Person",
      {{"id", boost::any((uint64_t)838375)},
       {"firstName", boost::any(std::string("Otto"))},
       {"lastName", boost::any(std::string("Richter"))}});
    auto ottoB = graph->add_node("Person",
      {{"id", boost::any((uint64_t)833579)},
       {"firstName", boost::any(std::string("Otto"))},
       {"lastName", boost::any(std::string("Becker"))}});
    auto hoChi = graph->add_node("Person",
      {{"id", boost::any((uint64_t)4194)},
       {"firstName", boost::any(std::string("Hồ Chí"))}, 
       {"lastName", boost::any(std::string("Do"))}});
    auto lomana = graph->add_node("Person",
      {{"id", boost::any((uint64_t)15393)},
       {"firstName", boost::any(std::string("Lomana Trésor"))},
       {"lastName", boost::any(std::string("Kanam"))}});
    auto amin = graph->add_node("Person",
      {{"id", boost::any((uint64_t)19791)},
       {"firstName", boost::any(std::string("Amin"))},
       {"lastName", boost::any(std::string("Kamkar"))}});
    auto bingbing = graph->add_node("Person",
      {{"id", boost::any((uint64_t)90796)},
       {"firstName", boost::any(std::string("Bingbing"))},
       {"lastName", boost::any(std::string("Xu"))}});
    auto Kelaniya = graph->add_node("Place", 
      {{"id", boost::any((uint64_t)1353)}});
    auto Gobi = graph->add_node("Place",
      {{"id", boost::any((uint64_t)129)}});
    auto forum_37 = graph->add_node("Forum",
      {{"id", boost::any((uint64_t)37)},
       {"title", boost::any(std::string("Wall of Hồ Chí Do"))}});
    auto post_1374389595 = graph->add_node("Post",
      {{"id", boost::any((uint64_t)1374389595)},
        {"imageFile", boost::any(std::string("photo1374389534791.jpg"))},
        {"creationDate", boost::any(time_from_string(std::string("2011-10-05 14:38:36.019")))},
        {"content", boost::any(std::string(""))}});
    auto post_16492674 = graph->add_node("Post",
      {{"id", boost::any((uint64_t)16492674)},
       {"imageFile", boost::any(std::string(""))},
       {"content", boost::any(std::string("Content of post_16492674"))},
       {"creationDate", boost::any(time_from_string(std::string("2012-01-09 08:05:28.922")))}});
    auto comment_16492676 = graph->add_node("Comment",
      {{"id", boost::any((uint64_t)16492676)},
        {"creationDate", boost::any(time_from_string(std::string("2012-01-10 03:24:33.368")))},
        {"content", boost::any(std::string("Content of comment_16492676"))}});
    auto comment_12362343 = graph->add_node("Comment",
      {{"id", boost::any((uint64_t)12362343)},
        {"creationDate", boost::any(time_from_string(std::string("2011-08-17 14:26:59.961")))},
        {"content", boost::any(std::string("Content of comment_12362343"))}});
    auto comment_16492675 = graph->add_node("Comment",
      {{"id", boost::any((uint64_t)16492675)},
        {"creationDate", boost::any(time_from_string(std::string("2011-10-05 14:38:36.019")))},
        {"content", boost::any(std::string("Content of comment_16492675"))}});
    auto comment_16492677 = graph->add_node("Comment",
      {{"id", boost::any((uint64_t)16492677)},
        {"creationDate", boost::any(time_from_string(std::string("2012-01-10 14:57:10.420")))},
        {"content", boost::any(std::string("Content of comment_16492677"))}});
    auto comment_1642217 = graph->add_node("Comment",
      {{"id", boost::any((uint64_t)1642217)},
        {"creationDate", boost::any(time_from_string(std::string("2012-01-10 06:31:18.533")))},
        {"content", boost::any(std::string("Content of comment_1642217"))}});

    // IS 1
    graph->add_relationship(mahinda, Kelaniya, ":isLocatedIn", {});
    graph->add_relationship(baruch, Gobi, ":isLocatedIn", {});

    // IS 2
    graph->add_relationship(cmt1, ravalomanana, ":hasCreator", {});
    graph->add_relationship(cmt2, ravalomanana, ":hasCreator", {});
    graph->add_relationship(cmt3, ravalomanana, ":hasCreator", {});
    graph->add_relationship(cmt4, ravalomanana, ":hasCreator", {});
    graph->add_relationship(cmt5, ravalomanana, ":hasCreator", {});
    graph->add_relationship(cmt6, ravalomanana, ":hasCreator", {});
    graph->add_relationship(cmt7, ravalomanana, ":hasCreator", {});
    graph->add_relationship(cmt8, ravalomanana, ":hasCreator", {});
    graph->add_relationship(cmt9, ravalomanana, ":hasCreator", {});
    graph->add_relationship(cmt10, ravalomanana, ":hasCreator", {});
    graph->add_relationship(cmt11, ravalomanana, ":hasCreator", {});
    graph->add_relationship(cmt12, ravalomanana, ":hasCreator", {});

    graph->add_relationship(cmt1, post1, ":replyOf", {});
    graph->add_relationship(cmt2, post2, ":replyOf", {});
    graph->add_relationship(cmt3, post2, ":replyOf", {});
    graph->add_relationship(cmt4, post3, ":replyOf", {});
    graph->add_relationship(cmt5, post1, ":replyOf", {});
    graph->add_relationship(cmt6, post1, ":replyOf", {});
    graph->add_relationship(cmt7, post3, ":replyOf", {});
    graph->add_relationship(cmt8, post3, ":replyOf", {});
    graph->add_relationship(cmt9, post2, ":replyOf", {});
    graph->add_relationship(cmt10, post5, ":replyOf", {});
    graph->add_relationship(cmt11, cmt12, ":replyOf", {});
    graph->add_relationship(cmt12, post4, ":replyOf", {});

    graph->add_relationship(post1, ravalomanana, ":hasCreator", {});
    graph->add_relationship(post2, p1, ":hasCreator", {});
    graph->add_relationship(post3, p2, ":hasCreator", {});
    graph->add_relationship(post4, p3, ":hasCreator", {});
    graph->add_relationship(post5, ravalomanana, ":hasCreator", {});

    // IS 3
    graph->add_relationship(mahinda, baruch, ":knows", {
      {"creationDate", boost::any(time_from_string(std::string("2010-03-13 07:37:21.718")))}});
    graph->add_relationship(mahinda, fritz, ":knows", {
          {"creationDate", boost::any(time_from_string(std::string("2010-09-20 09:42:43.187")))}});
    graph->add_relationship(mahinda, andrei, ":knows", {
          {"creationDate", boost::any(time_from_string(std::string("2011-01-02 06:43:41.955")))}});
    graph->add_relationship(mahinda, ottoB, ":knows", {
          {"creationDate", boost::any(time_from_string(std::string("2012-09-07 01:11:30.195")))}});
    graph->add_relationship(mahinda, ottoR, ":knows", {
          {"creationDate", boost::any(time_from_string(std::string("2012-09-07 01:11:30.195")))}});
    
    // IS 5
    graph->add_relationship(post_16492674, mahinda, ":hasCreator", {});
    graph->add_relationship(comment_12362343, andrei, ":hasCreator", {});

    // IS 6
    graph->add_relationship(forum_37, post_16492674, ":containerOf", {});
    graph->add_relationship(forum_37, hoChi, ":hasModerator", {});
    graph->add_relationship(comment_16492675, post_16492674, ":replyOf", {});
    graph->add_relationship(comment_16492676, comment_16492675, ":replyOf", {});
    graph->add_relationship(comment_16492677, comment_16492676, ":replyOf", {});

    // IS 7
    graph->add_relationship(comment_1642217, comment_16492676, ":replyOf", {});
    graph->add_relationship(comment_1642217, lomana, ":hasCreator", {});
    graph->add_relationship(comment_16492677, amin, ":hasCreator", {});
    graph->add_relationship(comment_16492676, bingbing, ":hasCreator", {});
    graph->add_relationship(lomana, bingbing, ":knows", {});

    return true;
  });
}

std::string load_string(const std::string& fname) {
    std::string qstr, line;
    std::ifstream myfile(fname);
    REQUIRE (myfile.is_open());
    while (getline(myfile, line)) {
        qstr.append(line);
        qstr.append("\n");
    }
    myfile.close();
    return qstr;
}

namespace pj = builtin;
void ldbc_is_query_7(graph_db_ptr &gdb, result_set &rs, uint64_t messageId) {
  std::vector<std::string> message = {"Post", "Comment"};

  auto q1 = query(gdb)
                .nodes_where(message, "id",
                            [&](auto &p) { return p.equal(messageId); })
                .from_relationships(":hasCreator")
                .to_node("Person");

  auto q2 = query(gdb)
                .nodes_where(message, "id",
                            [&](auto &p) { return p.equal(messageId); })
                .to_relationships(":replyOf")
                .from_node("Comment")
                .from_relationships(":hasCreator")
                .to_node("Person")
                .outerjoin(q1, [&](const qr_tuple &lv, const qr_tuple &rv) {
                  auto connected = false;
                  auto src = boost::get<node *>(lv[4]);
                  auto des = boost::get<node *>(rv[2]);
                  gdb->foreach_from_relationship_of_node((*src), [&](auto &r) {
                    if (r.to_node_id() == des->id())
                      connected = true;
                  });
                  return connected; })
                .append_to_qr_tuple([&](const qr_tuple &v) {
                  return v[7].type() == typeid(null_val) ?
                    query_result(std::string("false")) : query_result(std::string("true")); })
                .project({PExpr_(2, pj::uint64_property(res, "id")),
                          PExpr_(2, pj::string_property(res, "content")),
                          PExpr_(2, pj::ptime_property(res, "creationDate")),
                          PExpr_(4, pj::uint64_property(res, "id")),
                          PExpr_(4, pj::string_property(res, "firstName")),
                          PExpr_(4, pj::string_property(res, "lastName")),
                          PVar_(8)})
                .orderby([&](const qr_tuple &qr1, const qr_tuple &qr2) {
                        if (boost::get<boost::posix_time::ptime>(qr1[2]) == boost::get<boost::posix_time::ptime>(qr2[2]))
                          return boost::get<uint64_t>(qr1[0]) > boost::get<uint64_t>(qr2[0]);
                        return boost::get<boost::posix_time::ptime>(qr1[2]) < boost::get<boost::posix_time::ptime>(qr2[2]); })
                .collect(rs);
 gdb->run_transaction([&]() {
    query::start({&q1, &q2});
    return true;
  });  

  rs.wait();
}

TEST_CASE("Testing LDBC IS queries in interpreted mode", "[qinterp]") {
    auto pool = graph_pool::create(test_path);
    auto graph = pool->create_graph("my_graph");
    create_data(graph);

    qproc qp(graph);
#ifdef __APPLE__
    REQUIRE(qp.load_library("../libldbc_udf.dylib"));
#else
    REQUIRE(qp.load_library("../libldbc_udf.so"));
#endif

    char buf[1024];
    getcwd(buf, 1024);
    std::string prefix_is(buf); 
    prefix_is += "/../../queries/ldbc/is/is";  

    SECTION("IS #1") {
      spdlog::info("LDBC IS#1"); 
        auto qstr = load_string(prefix_is + "1.q");
        auto res = qp.execute_query(qproc::Interpret, qstr);

        // std::cout << res.result() << std::endl;

        result_set expected;
        expected.append({
          qv_("Mahinda"), qv_("Perera"), qv_("1989-12-03T00:00:00"), qv_("119.235.7.103"),
          qv_("Firefox"), qv_("1353"), qv_("male"), qv_("2010-02-14T15:32:10.447000")
        });

        REQUIRE(res.result() == expected);
    }
    
    SECTION("IS #2") {
      spdlog::info("LDBC IS#2"); 
      auto qstr = load_string(prefix_is + "2.q");
      auto res = qp.execute_query(qproc::Interpret, qstr);
      // std::cout << res.result() << std::endl;

      result_set expected;
      expected.append(
          {qv_("1768"), qv_("Content of cmt12"),
          qv_("2013-12-27T11:32:19.336000"), qv_("13743895"),
          qv_("1121"), qv_("Karl"), qv_("Beran")});
      expected.append(
          {qv_("1171"), qv_("Content of cmt11"),
          qv_("2013-11-26T17:09:07.283000"), qv_("13743895"),
          qv_("1121"), qv_("Karl"), qv_("Beran")});
      expected.append(
        {qv_("1126"), qv_("Content of cmt10"),
        qv_("2013-10-26T23:46:18.580000"), qv_("13743894"),
        qv_("65"), qv_("Marc"), qv_("Ravalomanana")});
      expected.append(
        {qv_("1978"), qv_("Content of cmt9"),
        qv_("2013-09-27T09:41:01.413000"), qv_("1976"),
        qv_("1379"), qv_("Muhammad"), qv_("Iqbal")});
      expected.append(
        {qv_("1877"), qv_("Content of cmt8"),
        qv_("2013-08-25T12:56:57.280000"), qv_("137438956"),
        qv_("1291"), qv_("Wei"), qv_("Li")});
      expected.append(
        {qv_("1865"), qv_("Content of cmt7"),
        qv_("2013-07-25T07:54:01.976000"), qv_("137438956"),
        qv_("1291"), qv_("Wei"), qv_("Li")});
      expected.append(
        {qv_("1768"), qv_("Content of cmt6"),
        qv_("2013-06-29T17:57:51.844000"), qv_("1863"),
        qv_("65"), qv_("Marc"), qv_("Ravalomanana")});
      expected.append(
        {qv_("1171"), qv_("Content of cmt5"),
        qv_("2013-05-17T19:37:26.339000"), qv_("1863"),
        qv_("65"), qv_("Marc"), qv_("Ravalomanana")});
      expected.append(
        {qv_("1126"), qv_("Content of cmt4"),
        qv_("2013-04-16T21:16:03.354000"), qv_("137438956"),
        qv_("1291"), qv_("Wei"), qv_("Li")});
      expected.append(
        {qv_("1978"), qv_("Content of cmt3"),
        qv_("2013-03-14T16:57:46.045000"), qv_("1976"),
        qv_("1379"), qv_("Muhammad"), qv_("Iqbal")});

      REQUIRE(res.result() == expected);
    }
  
    SECTION("IS #3") {
      spdlog::info("LDBC IS#3"); 
      auto qstr = load_string(prefix_is + "3.q");
      auto res = qp.execute_query(qproc::Interpret, qstr);
        // std::cout << res.result() << std::endl;

      result_set expected;
      expected.append({
          qv_("833579"), qv_("Otto"),
          qv_("Becker"), qv_("2012-09-07T01:11:30.195000")});
      expected.append({
          qv_("838375"), qv_("Otto"),
          qv_("Richter"), qv_("2012-09-07T01:11:30.195000")});
      expected.append({
          qv_("10995116"), qv_("Andrei"),
          qv_("Condariuc"), qv_("2011-01-02T06:43:41.955000")});
      expected.append({
          qv_("65970697"), qv_("Fritz"),
          qv_("Muller"), qv_("2010-09-20T09:42:43.187000")});
      expected.append({
          qv_("4139"), qv_("Baruch"),
          qv_("Dego"), qv_("2010-03-13T07:37:21.718000")});

      REQUIRE(res.result() == expected);
    }
    
    SECTION("IS #4") {
      spdlog::info("LDBC IS#4"); 
      auto qstr = load_string(prefix_is + "4.q");
      auto res = qp.execute_query(qproc::Interpret, qstr);
      // std::cout << res.result() << std::endl;

      result_set expected;
      expected.append({
          qv_("2011-10-05T14:38:36.019000"), qv_("photo1374389534791.jpg")
      });

      REQUIRE(res.result() == expected);
    }

    SECTION("IS #5") {
      spdlog::info("LDBC IS#5"); 
      auto qstr = load_string(prefix_is + "5.q");
      auto res = qp.execute_query(qproc::Interpret, qstr);
      // std::cout << res.result() << std::endl;

      result_set expected;
      expected.append({
          qv_("10995116"), qv_("Andrei"), qv_("Condariuc")
      });

      REQUIRE(res.result() == expected);
    }
    
    SECTION("IS #6") {
      spdlog::info("LDBC IS#6"); 
      auto qstr = load_string(prefix_is + "6.q");
      auto res = qp.execute_query(qproc::Interpret, qstr);
       // std::cout << res.result() << std::endl;

      result_set expected;
      expected.append({
          qv_("37"), qv_("Wall of Hồ Chí Do"), qv_("4194"), qv_("Hồ Chí"), qv_("Do")
      });

      REQUIRE(res.result() == expected);
    }

    SECTION("IS #7") {
      spdlog::info("LDBC IS#7"); 
      auto qstr = load_string(prefix_is + "7.q");
      auto res = qp.execute_query(qproc::Interpret, qstr, true);
      std::cout << res.result() << std::endl;

      result_set res2;
      ldbc_is_query_7(graph, res2, 16492676);
      std::cout << res2 << std::endl;

      result_set expected;
      expected.append({
          qv_("1642217"), qv_("Content of comment_1642217"), qv_("2012-01-10T06:31:18.533000"),
          qv_("15393"), qv_("Lomana Trésor"), qv_("Kanam"), qv_("true")
      });
      expected.append({
          qv_("16492677"), qv_("Content of comment_16492677"), qv_("2012-01-10T14:57:10.420000"),
          qv_("19791"), qv_("Amin"), qv_("Kamkar"), qv_("false")
      });
    }
  
  graph_pool::destroy(pool);
}

