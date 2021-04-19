#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do
                          // this in one cpp file

#include <boost/algorithm/string.hpp>

#include "catch.hpp"
#include "config.h"
#include "defs.hpp"
#include "ldbc.hpp"
#include "graph_pool.hpp"

#ifndef USE_PMDK
#undef RUN_INDEXED
#endif

using namespace boost::posix_time;
const std::string test_path = poseidon::gPmemPath + "ldbc_test";

graph_db_ptr create_is_data(graph_db_ptr &graph) {
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

  #ifdef FPTree
  graph->run_transaction([&]() {
    graph->create_index("Person", "id");
    graph->create_index("Post", "id");
    graph->create_index("Comment", "id");
    return true;
  });
  #endif

  return graph;
}

graph_db_ptr create_iu_data(graph_db_ptr &graph) {
  graph->run_transaction([&]() {

    // IU 1
    auto artux = graph->add_node("Place",
      {{"id", boost::any((uint64_t)505)}});
    auto tag_61 = graph->add_node("Tag",
      {{"id", boost::any((uint64_t)61)}});
    auto uni_2213 = graph->add_node( "Organisation",
      {{"id", boost::any((uint64_t)2213)}});
    auto company_915 = graph->add_node("Organisation",
      {{"id", boost::any((uint64_t)915)}});
    
    // IU 2
    auto post_3627 = graph->add_node("Post",
      {{"id", boost::any((uint64_t)3627)}});
    auto meera = graph->add_node("Person",
      {{"id", boost::any((uint64_t)10027)}});
    
    // IU 3
    auto comment_1642250 = graph->add_node("Comment",
      {{"id", boost::any((uint64_t)1642250)}});
    
    // IU 5
    auto forum_71489 = graph->add_node("Forum",
      {{"id", boost::any((uint64_t)71489)}});
    
    // IU 8
    auto ivan = graph->add_node("Person",
      {{"id", boost::any((uint64_t)1043)}});

    return true;
  });

  #ifdef FPTree
  graph->run_transaction([&]() {
    graph->create_index("Place", "id");
    graph->create_index("Tag", "id");
    graph->create_index("Organisation", "id");
    graph->create_index("Post", "id");
    graph->create_index("Person", "id");
    graph->create_index("Comment", "id");
    graph->create_index("Forum", "id");
    return true;
  });
  #endif

  return graph;
}

graph_db_ptr create_bi_data(graph_db_ptr &graph) {
  graph->run_transaction([&]() {

    // Persons
    auto pA = graph->add_node("Person",
      {{"id", boost::any((uint64_t)1)},
       {"firstName", boost::any(std::string("Amelie"))},
       {"lastName", boost::any(std::string(""))},
       {"creationDate", boost::any(time_from_string(std::string("2010-06-10 11:05:56.000")))}});
    auto pB = graph->add_node("Person",
      {{"id", boost::any((uint64_t)2)},
       {"firstName", boost::any(std::string("Bernardo"))},
       {"lastName", boost::any(std::string(""))},
       {"creationDate", boost::any(time_from_string(std::string("2010-06-10 11:05:56.000")))}});
    auto pC = graph->add_node("Person",
      {{"id", boost::any((uint64_t)3)},
       {"firstName", boost::any(std::string("Cedric"))},
       {"lastName", boost::any(std::string(""))},
       {"creationDate", boost::any(time_from_string(std::string("2010-06-10 11:05:56.000")))}});
    auto pD = graph->add_node("Person",
      {{"id", boost::any((uint64_t)4)},
       {"firstName", boost::any(std::string("Diane"))},
       {"lastName", boost::any(std::string(""))},
       {"creationDate", boost::any(time_from_string(std::string("2010-06-10 11:05:56.000")))}});
    auto pE = graph->add_node("Person",
      {{"id", boost::any((uint64_t)5)},
       {"firstName", boost::any(std::string("Eve"))},
       {"lastName", boost::any(std::string(""))},
       {"creationDate", boost::any(time_from_string(std::string("2010-06-10 11:05:56.000")))}});

    graph->add_relationship(pA, pB, ":knows", {{"creationDate", boost::any(time_from_string(std::string("2010-06-10 11:05:56.000")))}});
    graph->add_relationship(pA, pC, ":knows", {{"creationDate", boost::any(time_from_string(std::string("2010-06-10 11:05:56.000")))}});
    graph->add_relationship(pA, pD, ":knows", {{"creationDate", boost::any(time_from_string(std::string("2010-06-10 11:05:56.000")))}});
    graph->add_relationship(pB, pC, ":knows", {{"creationDate", boost::any(time_from_string(std::string("2010-06-10 11:05:56.000")))}});
    graph->add_relationship(pC, pD, ":knows", {{"creationDate", boost::any(time_from_string(std::string("2010-06-10 11:05:56.000")))}});
    graph->add_relationship(pD, pE, ":knows", {{"creationDate", boost::any(time_from_string(std::string("2010-06-10 11:05:56.000")))}});

    // Organisations
    auto cambridge = graph->add_node("Organisation",
      {{"id", boost::any((uint64_t)1)},
        {"type", boost::any(std::string("university"))},
        {"name", boost::any(std::string("Cambridge"))}});
    auto softEngCo = graph->add_node("Organisation",
      {{"id", boost::any((uint64_t)2)},
        {"type", boost::any(std::string("company"))},
        {"name", boost::any(std::string("SoftEngCo"))}});

    graph->add_relationship(pA, cambridge, ":studyAt", {{"classYear", boost::any(2008)}});
    graph->add_relationship(pD, cambridge, ":studyAt", {{"classYear", boost::any(2006)}});
    graph->add_relationship(pE, cambridge, ":studyAt", {{"classYear", boost::any(2008)}});
    graph->add_relationship(pA, softEngCo, ":workAt", {});

    // Places
    auto spain = graph->add_node("Place",
      {{"id", boost::any((uint64_t)1)},
        {"type", boost::any(std::string("country"))},
        {"name", boost::any(std::string("Spain"))}});
    auto madrid = graph->add_node("Place",
      {{"id", boost::any((uint64_t)2)},
        {"type", boost::any(std::string("city"))},
        {"name", boost::any(std::string("Madrid"))}});
    auto france = graph->add_node("Place",
      {{"id", boost::any((uint64_t)3)},
        {"type", boost::any(std::string("country"))},
        {"name", boost::any(std::string("France"))}});
    auto paris = graph->add_node("Place",
      {{"id", boost::any((uint64_t)4)},
        {"type", boost::any(std::string("city"))},
        {"name", boost::any(std::string("Paris"))}});
    auto lyon = graph->add_node("Place",
      {{"id", boost::any((uint64_t)5)},
        {"type", boost::any(std::string("city"))},
        {"name", boost::any(std::string("Lyon"))}});

    graph->add_relationship(madrid, spain, ":isPartOf", {});
    graph->add_relationship(paris, france, ":isPartOf", {});
    graph->add_relationship(lyon, france, ":isPartOf", {});
    graph->add_relationship(pA, paris, ":isLocatedIn", {});
    graph->add_relationship(pB, madrid, ":isLocatedIn", {});
    graph->add_relationship(pC, lyon, ":isLocatedIn", {});
    graph->add_relationship(pD, paris, ":isLocatedIn", {});

    // TagClasses
    auto tc1 = graph->add_node("TagClass",
      {{"id", boost::any((uint64_t)1)},
        {"name", boost::any(std::string("Holiday resorts"))}});
    auto tc2 = graph->add_node("TagClass",
      {{"id", boost::any((uint64_t)2)},
        {"name", boost::any(std::string("Ski resorts"))}});
    auto tc3 = graph->add_node("TagClass",
      {{"id", boost::any((uint64_t)3)},
        {"name", boost::any(std::string("Sports"))}});

    graph->add_relationship(tc2, tc1, ":isSubclassOf", {});

    // Tags
    auto t1 = graph->add_node("Tag",
      {{"id", boost::any((uint64_t)1)},
        {"name", boost::any(std::string("Pyrenees"))}});
    auto t2 = graph->add_node("Tag",
      {{"id", boost::any((uint64_t)2)},
        {"name", boost::any(std::string("Snowboard"))}});

    graph->add_relationship(pB, t1, ":hasInterest", {});
    graph->add_relationship(pD, t2, ":hasInterest", {});
    graph->add_relationship(t1, tc2, ":hasType", {});
    graph->add_relationship(t2, tc3, ":hasType", {});

    // Forums
    auto forum1 = graph->add_node("Forum",
      {{"id", boost::any((uint64_t)1)},
        {"creationDate", boost::any(time_from_string(std::string("2011-10-10 11:01:47.000")))},
        {"title", boost::any(std::string("Skiing trips"))}});
    auto forum2 = graph->add_node("Forum",
      {{"id", boost::any((uint64_t)2)},
        {"creationDate", boost::any(time_from_string(std::string("2012-02-01 13:07:26.000")))},
        {"title", boost::any(std::string("Cinéma"))}});

    graph->add_relationship(forum1, t1, ":hasTag", {});
    graph->add_relationship(forum1, pA, ":hasMember", {});
    graph->add_relationship(forum1, pB, ":hasMember", {});
    graph->add_relationship(forum1, pC, ":hasMember", {});
    graph->add_relationship(forum2, pC, ":hasMember", {});
    graph->add_relationship(forum2, pA, ":hasMember", {});
    graph->add_relationship(forum1, pB, ":hasModerator", {});
    graph->add_relationship(forum2, pC, ":hasModerator", {});

    // Messages
    auto p1 = graph->add_node("Post",
      {{"id", boost::any((uint64_t)10)},
        {"creationDate", boost::any(time_from_string(std::string("2011-10-10 11:05:56.000")))},
        {"length", boost::any(24)},
        {"content", boost::any(std::string("We should go to Hautacam"))},
        {"language", boost::any(std::string("en"))}});
    auto p2 = graph->add_node("Post",
      {{"id", boost::any((uint64_t)20)},
        {"creationDate", boost::any(time_from_string(std::string("2012-03-04 13:41:23.000")))},
        {"length", boost::any(38)},
        {"content", boost::any(std::string("Voici un film de snowboard intéressant"))},
        {"language", boost::any(std::string("fr"))}});
    auto c1 = graph->add_node("Comment",
      {{"id", boost::any((uint64_t)1)},
        {"creationDate", boost::any(time_from_string(std::string("2011-10-10 11:08:01.000")))},
        {"length", boost::any(24)},
        {"content", boost::any(std::string("Yes, I like the Pyrenees"))}});
    auto c2 = graph->add_node("Comment",
      {{"id", boost::any((uint64_t)2)},
        {"creationDate", boost::any(time_from_string(std::string("2011-10-10 11:07:42.000")))},
        {"length", boost::any(57)},
        {"content", boost::any(std::string("We should go to a place with better options for snowboard"))}});
    auto c3 = graph->add_node("Comment",
      {{"id", boost::any((uint64_t)3)},
        {"creationDate", boost::any(time_from_string(std::string("2011-10-10 11:20:37.000")))},
        {"length", boost::any(34)},
        {"content", boost::any(std::string("Hautacam is great for snowboarding"))}});
    auto c4 = graph->add_node("Comment",
      {{"id", boost::any((uint64_t)4)},
        {"creationDate", boost::any(time_from_string(std::string("2012-02-15 09:47:23.000")))},
        {"length", boost::any(58)},
        {"content", boost::any(std::string("It was a great place for snowboarding. Glad we went there!"))}});
    auto c5 = graph->add_node("Comment",
      {{"id", boost::any((uint64_t)5)},
        {"creationDate", boost::any(time_from_string(std::string("2012-02-15 10:24:26.000")))},
        {"length", boost::any(13)},
        {"content", boost::any(std::string("It was great!"))}});
    auto c6 = graph->add_node("Comment",
      {{"id", boost::any((uint64_t)6)},
        {"creationDate", boost::any(time_from_string(std::string("2012-03-04 10:24:26.000")))},
        {"length", boost::any(38)},
        {"content", boost::any(std::string("Merci, j'adore les films de snowboard"))}});

    graph->add_relationship(forum1, p1, ":containerOf", {});
    graph->add_relationship(forum2, p2, ":containerOf", {});
    graph->add_relationship(c1, p1, ":replyOf", {});
    graph->add_relationship(c2, p1, ":replyOf", {});
    graph->add_relationship(c3, c2, ":replyOf", {});
    graph->add_relationship(c4, c3, ":replyOf", {});
    graph->add_relationship(c5, c4, ":replyOf", {});
    graph->add_relationship(c6, p2, ":replyOf", {});
    graph->add_relationship(pA, p1, ":likes", {});
    graph->add_relationship(pB, c2, ":likes", {});
    graph->add_relationship(pB, c3, ":likes", {});
    graph->add_relationship(pC, p1, ":likes", {});
    graph->add_relationship(pC, c4, ":likes", {});
    graph->add_relationship(p1, pB, ":hasCreator", {});
    graph->add_relationship(c1, pC, ":hasCreator", {});
    graph->add_relationship(c2, pA, ":hasCreator", {});
    graph->add_relationship(c3, pC, ":hasCreator", {});
    graph->add_relationship(c4, pA, ":hasCreator", {});
    graph->add_relationship(c5, pD, ":hasCreator", {});
    graph->add_relationship(p2, pC, ":hasCreator", {});
    graph->add_relationship(c6, pA, ":hasCreator", {});
    graph->add_relationship(p1, t1, ":hasTag", {});
    graph->add_relationship(c1, t1, ":hasTag", {});
    graph->add_relationship(c3, t1, ":hasTag", {});
    graph->add_relationship(c2, t2, ":hasTag", {});
    graph->add_relationship(c3, t2, ":hasTag", {});
    graph->add_relationship(c4, t2, ":hasTag", {});
    graph->add_relationship(p2, t2, ":hasTag", {});
    graph->add_relationship(c6, t2, ":hasTag", {});

    return true;
  });
  return graph;
}

TEST_CASE("Testing LDBC IS Query 1", "[ldbc_is_reads]") {
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("snb");
  create_is_data(graph);

  graph->run_transaction([&]() {
    uint64_t pidq1 = 933;
    result_set rs, expected;
    ldbc_is_query_1(graph, rs, pidq1);

    expected.data.push_back(
      {query_result("Mahinda"), query_result("Perera"),
        query_result("1989-12-03"), query_result("119.235.7.103"),
        query_result("Firefox"), query_result("1353"), query_result("male"),
        query_result("2010-02-14T15:32:10.447000")});

    REQUIRE(rs == expected);
    return true;
  });
  graph_pool::destroy(pool);
}

TEST_CASE("Testing LDBC IS Query 2", "[ldbc_is_reads]") {
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("snb");
  create_is_data(graph);

  graph->run_transaction([&]() {
    uint64_t pidq3 = 65;
    result_set rs, expected;
    ldbc_is_query_2(graph, rs, pidq3);

    expected.data.push_back(
        {query_result("1768"), query_result("Content of cmt12"),
        query_result("2013-12-27T11:32:19.336000"), query_result("13743895"),
         query_result("1121"), query_result("Karl"), query_result("Beran")});
    expected.data.push_back(
        {query_result("1171"), query_result("Content of cmt11"),
        query_result("2013-11-26T17:09:07.283000"), query_result("13743895"),
         query_result("1121"), query_result("Karl"), query_result("Beran")});
    expected.data.push_back(
        {query_result("1126"), query_result("Content of cmt10"),
        query_result("2013-10-26T23:46:18.580000"), query_result("13743894"),
        query_result("65"), query_result("Marc"), query_result("Ravalomanana")});
    expected.data.push_back(
        {query_result("1978"), query_result("Content of cmt9"),
        query_result("2013-09-27T09:41:01.413000"), query_result("1976"),
        query_result("1379"), query_result("Muhammad"), query_result("Iqbal")});
    expected.data.push_back(
        {query_result("1877"), query_result("Content of cmt8"),
        query_result("2013-08-25T12:56:57.280000"), query_result("137438956"),
        query_result("1291"), query_result("Wei"), query_result("Li")});
    expected.data.push_back(
        {query_result("1865"), query_result("Content of cmt7"),
        query_result("2013-07-25T07:54:01.976000"), query_result("137438956"),
        query_result("1291"), query_result("Wei"), query_result("Li")});
    expected.data.push_back(
        {query_result("1768"), query_result("Content of cmt6"),
        query_result("2013-06-29T17:57:51.844000"), query_result("1863"),
        query_result("65"), query_result("Marc"), query_result("Ravalomanana")});
    expected.data.push_back(
        {query_result("1171"), query_result("Content of cmt5"),
        query_result("2013-05-17T19:37:26.339000"), query_result("1863"),
        query_result("65"), query_result("Marc"), query_result("Ravalomanana")});
    expected.data.push_back(
        {query_result("1126"), query_result("Content of cmt4"),
        query_result("2013-04-16T21:16:03.354000"), query_result("137438956"),
        query_result("1291"), query_result("Wei"), query_result("Li")});
    expected.data.push_back(
        {query_result("1978"), query_result("Content of cmt3"),
        query_result("2013-03-14T16:57:46.045000"), query_result("1976"),
        query_result("1379"), query_result("Muhammad"), query_result("Iqbal")});

    REQUIRE(rs == expected);
    return true;
  });
  graph_pool::destroy(pool);
}

TEST_CASE("Testing LDBC IS Query 3", "[ldbc_is_reads]") {
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("snb");
  create_is_data(graph);

  graph->run_transaction([&]() {
    uint64_t pidq3 = 933;
    result_set rs, expected;
    ldbc_is_query_3(graph, rs, pidq3);

    expected.data.push_back(
        {query_result("833579"), query_result("Otto"),
         query_result("Becker"), query_result("2012-09-07T01:11:30.195000")});
    expected.data.push_back(
        {query_result("838375"), query_result("Otto"),
         query_result("Richter"), query_result("2012-09-07T01:11:30.195000")});
    expected.data.push_back(
        {query_result("10995116"), query_result("Andrei"),
         query_result("Condariuc"), query_result("2011-01-02T06:43:41.955000")});
    expected.data.push_back(
        {query_result("65970697"), query_result("Fritz"),
         query_result("Muller"), query_result("2010-09-20T09:42:43.187000")});
    expected.data.push_back(
        {query_result("4139"), query_result("Baruch"),
         query_result("Dego"), query_result("2010-03-13T07:37:21.718000")});

    REQUIRE(rs == expected);
    return true;
  });
  graph_pool::destroy(pool);
}

TEST_CASE("Testing LDBC IS Query 4", "[ldbc_is_reads]") {
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("snb");
  create_is_data(graph);

  graph->run_transaction([&]() {
    uint64_t midq4 = 1374389595;
    result_set rs, expected;
    ldbc_is_query_4(graph, rs, midq4);

  expected.data.push_back(
      {query_result("2011-10-05T14:38:36.019000"),
        query_result("photo1374389534791.jpg")});

    REQUIRE(rs == expected);
    midq4 = 16492674;
    ldbc_is_query_4(graph, rs, midq4);

    expected.data.push_back(
        {query_result("2012-01-09T08:05:28.922000"),
         query_result("Content of post_16492674")});

    REQUIRE(rs == expected);
    midq4 = 16492676;
    ldbc_is_query_4(graph, rs, midq4);

    expected.data.push_back(
        {query_result("2012-01-10T03:24:33.368000"),
         query_result("Content of comment_16492676")});

    REQUIRE(rs == expected);
    return true;
  });
  graph_pool::destroy(pool);
}

TEST_CASE("Testing LDBC IS Query 5", "[ldbc_is_reads]") {
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("snb");
  create_is_data(graph);

  graph->run_transaction([&]() {
    uint64_t midq5 = 12362343;
    result_set rs, expected;
    ldbc_is_query_5(graph, rs, midq5);

    expected.data.push_back(
        {query_result("10995116"),
          query_result("Andrei"),
          query_result("Condariuc")});

    REQUIRE(rs == expected);
    midq5 = 16492674;
    ldbc_is_query_5(graph, rs, midq5);

    expected.data.push_back(
        {query_result("933"),
          query_result("Mahinda"),
          query_result("Perera")});

    REQUIRE(rs == expected);
    return true;
  });
  graph_pool::destroy(pool);
}

TEST_CASE("Testing LDBC IS Query 6", "[ldbc_is_reads]") {
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("snb");
  create_is_data(graph);

  graph->run_transaction([&]() {
    uint64_t midq6 = 16492677;
    result_set rs, expected;
    ldbc_is_query_6(graph, rs, midq6);

    expected.data.push_back(
        {query_result("37"),
         query_result("Wall of Hồ Chí Do"),
         query_result("4194"),
         query_result("Hồ Chí"),
         query_result("Do")});

    REQUIRE(rs == expected);
    return true;
  });
  graph_pool::destroy(pool);
}

TEST_CASE("Testing LDBC IS Query 7", "[ldbc_is_reads]") {
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("snb");
  create_is_data(graph);

  graph->run_transaction([&]() {
    uint64_t midq7 = 16492676;
    result_set rs, expected;
    ldbc_is_query_7(graph, rs, midq7);

    expected.data.push_back(
        {query_result("1642217"),
         query_result("Content of comment_1642217"),
         query_result("2012-01-10T06:31:18.533000"),
         query_result("15393"),
         query_result("Lomana Trésor"),
         query_result("Kanam"),
         query_result("true")});
    expected.data.push_back(
        {query_result("16492677"),
         query_result("Content of comment_16492677"),
         query_result("2012-01-10T14:57:10.420000"),
         query_result("19791"),
         query_result("Amin"),
         query_result("Kamkar"),
         query_result("false")});

    REQUIRE(rs == expected);
    return true;
  });
  graph_pool::destroy(pool);
}

TEST_CASE("Testing LDBC IU Query 1", "[ldbc_iu]") {
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("snb");
  create_iu_data(graph);

  graph->run_transaction([&]() {
    std::vector<params_tuple> parameters = 
        {{(uint64_t)505, (uint64_t)61, (uint64_t)2213, (uint64_t)915, (uint64_t)2370,
            "Yang", "Zhu", "male", "1980-11-23", "2010-01-17T07:59:27.746",
            "1.183.127.173", "Internet Explorer", "\"zh\", \"en\"",
            "\"Yang2370@gmail.com\", \"Yang2370@hotmail.com\"",
            2001, 2002}};

    result_set rs, expected;
    ldbc_iu_query_1(graph, rs, parameters[0]);

    expected.append(
        {query_result("Person[9]{birthday: 1980-Nov-23 00:00:00, browserUsed: \"Internet Explorer\", "
          "creationDate: \"2010-01-17T07:59:27.746\", email: \"\"Yang2370@gmail.com\", \"Yang2370@hotmail.com\"\", "
          "firstName: \"Yang\", gender: \"male\", id: 2370, language: \"\"zh\", \"en\"\", lastName: \"Zhu\", "
          "locationIP: \"1.183.127.173\"}"),
        query_result("Place[0]{id: 505}"),
        query_result("::isLocatedIn[0]{}"),
        query_result("Tag[1]{id: 61}"),
        query_result("::hasInterest[1]{}"),
        query_result("Organisation[2]{id: 2213}"),
        query_result("::studyAt[2]{classYear: 2001}"),
        query_result("Organisation[3]{id: 915}"),
        query_result("::workAt[3]{workFrom: 2002}") });

    REQUIRE(rs == expected);
    return true;
  });
  graph_pool::destroy(pool);
}

TEST_CASE("Testing LDBC IU Query 2", "[ldbc_iu]") {
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("snb");
  create_iu_data(graph);

  graph->run_transaction([&]() {
    std::vector<params_tuple> parameters = 
        {{(uint64_t)3627, (uint64_t)10027, "2010-02-14 15:32:10.447"}};

    result_set rs, expected;
    ldbc_iu_query_2(graph, rs, parameters[0]);

    expected.append(
        {query_result("Person[5]{id: 10027}"),
        query_result("Post[4]{id: 3627}"),
         query_result("::likes[0]{creationDate: \"2010-02-14 15:32:10.447\"}")});

    REQUIRE(rs == expected);
    return true;
  });
  graph_pool::destroy(pool);
}

TEST_CASE("Testing LDBC IU Query 3", "[ldbc_iu]") {
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("snb");
  create_iu_data(graph);

  graph->run_transaction([&]() {
    std::vector<params_tuple> parameters = 
        {{(uint64_t)1642250, (uint64_t)10027, "2010-02-14 15:32:10.447"}};

    result_set rs, expected;
    ldbc_iu_query_3(graph, rs, parameters[0]);

    expected.append(
        {query_result("Person[5]{id: 10027}"),
        query_result("Comment[6]{id: 1642250}"),
         query_result("::likes[0]{creationDate: \"2010-02-14 15:32:10.447\"}")});

    REQUIRE(rs == expected);
    return true;
  });
  graph_pool::destroy(pool);
}

TEST_CASE("Testing LDBC IU Query 4", "[ldbc_iu]") {
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("snb");
  create_iu_data(graph);

  graph->run_transaction([&]() {
    std::vector<params_tuple> parameters = 
        {{(uint64_t)10027, (uint64_t)61, (uint64_t)53975,
        "Wall of Emperor of Brazil Silva", "2010-01-02 06:05:05.320"}};

    result_set rs, expected;
    ldbc_iu_query_4(graph, rs, parameters[0]);

    expected.append(
        {query_result("Forum[9]{creationDate: \"2010-01-02 06:05:05.320\", id: 53975, "
            "title: \"Wall of Emperor of Brazil Silva\"}"),
        query_result("Person[5]{id: 10027}"), 
        query_result("::hasModerator[0]{}"),
        query_result("Tag[1]{id: 61}"),
        query_result("::hasTag[1]{}") });

    REQUIRE(rs == expected);
    return true;
  });
  graph_pool::destroy(pool);
}

TEST_CASE("Testing LDBC IU Query 5", "[ldbc_iu]") {
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("snb");
  create_iu_data(graph);

  graph->run_transaction([&]() {
    std::vector<params_tuple> parameters = 
        {{(uint64_t)10027, (uint64_t)71489, "2010-02-23 09:10:25.466"}};

    result_set rs, expected;
    ldbc_iu_query_5(graph, rs, parameters[0]);

    expected.append(
        {query_result("Forum[7]{id: 71489}"),
        query_result("Person[5]{id: 10027}"),
         query_result("::hasMember[0]{creationDate: \"2010-02-23 09:10:25.466\"}")});

    REQUIRE(rs == expected);
    return true;
  });
  graph_pool::destroy(pool);
}

TEST_CASE("Testing LDBC IU Query 6", "[ldbc_iu]") {
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("snb");
  create_iu_data(graph);

  graph->run_transaction([&]() {
    std::vector<params_tuple> parameters = 
        {{(uint64_t)10027, (uint64_t)71489, (uint64_t)505,
        (uint64_t)61, (uint64_t)7146847411148,
        "", "2012-04-06T10:09:52.185+0000", "27.54.182.164", "Firefox", "Acholi", "Content of post_7146847411148",
        197}};

    result_set rs, expected;
    ldbc_iu_query_6(graph, rs, parameters[0]);

    expected.append(
        {query_result("Post[9]{browserUsed: \"Firefox\", content: \"Content of post_7146847411148\", "
            "creationDate: 2012-Apr-06 10:09:52.185000, id: 7146847411148, imageFile: \"\", language: \"Acholi\", "
            "length: 197, locationIP: \"27.54.182.164\"}"),
        query_result("Person[5]{id: 10027}"),
        query_result("::hasCreator[0]{}"),
        query_result("Forum[7]{id: 71489}"),
        query_result("::containerOf[1]{}"),
        query_result("Place[0]{id: 505}"),
        query_result("::isLocatedIn[2]{}"),
        query_result("Tag[1]{id: 61}"),
        query_result("::hasTag[3]{}")});

    REQUIRE(rs == expected);
    return true;
  });
  graph_pool::destroy(pool);
}

TEST_CASE("Testing LDBC IU Query 7", "[ldbc_iu]") {
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("snb");
  create_iu_data(graph);

  graph->run_transaction([&]() {
    std::vector<params_tuple> parameters =
        {{(uint64_t)10027, (uint64_t)3627, (uint64_t)505,
        (uint64_t)61, (uint64_t)442214,
        "2012-01-09 11:49:15.991", "91.149.169.27", "Chrome", "Content of cmt_442214", 4}};

    result_set rs, expected;
    ldbc_iu_query_7(graph, rs, parameters[0]);

    expected.append(
        {query_result("Comment[9]{browserUsed: \"Chrome\", content: \"Content of cmt_442214\", "
            "creationDate: \"2012-01-09 11:49:15.991\", id: 442214, "
            "length: 4, locationIP: \"91.149.169.27\"}"),
        query_result("Person[5]{id: 10027}"),
        query_result("::hasCreator[0]{}"),
        query_result("Post[4]{id: 3627}"),
        query_result("::replyOf[1]{}"),
        query_result("Place[0]{id: 505}"),
        query_result("::isLocatedIn[2]{}"),
        query_result("Tag[1]{id: 61}"),
        query_result("::hasTag[3]{}")});

    REQUIRE(rs == expected);
    return true;
  });
  graph_pool::destroy(pool);
}

TEST_CASE("Testing LDBC IU Query 8", "[ldbc_iu]") {
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("snb");
  create_iu_data(graph);

  graph->run_transaction([&]() {
    std::vector<params_tuple> parameters = 
        {{(uint64_t)1043, (uint64_t)10027, "2010-02-23 09:10:15.466"}};

    result_set rs, expected;
    ldbc_iu_query_8(graph, rs, parameters[0]);

    expected.append(
        {query_result("Person[5]{id: 10027}"),
        query_result("Person[8]{id: 1043}"),
         query_result("::knows[0]{creationDate: \"2010-02-23 09:10:15.466\"}")});

    REQUIRE(rs == expected);
    return true;
  });
  graph_pool::destroy(pool);
}

TEST_CASE("Testing LDBC BI Query 1", "[ldbc_bi]") {
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("snb");
  create_bi_data(graph);

  graph->run_transaction([&]() {
    std::vector<params_tuple> parameters =
        {{time_from_string(std::string("2011-12-01 00:00:00.000"))}};
    result_set rs, expected;
    ldbc_bi_query_1(graph, rs, parameters[0]);

    expected.data.push_back(
      {query_result("2011"), query_result("False"), query_result("0"), query_result("1"),
      query_result("24.000000"), query_result("24"), query_result("25.000000")});
    expected.data.push_back(
      {query_result("2011"), query_result("True"), query_result("0"), query_result("2"),
      query_result("29.000000"), query_result("58"), query_result("50.000000")});
    expected.data.push_back(
      {query_result("2011"), query_result("True"), query_result("1"), query_result("1"),
      query_result("57.000000"), query_result("57"), query_result("25.000000")});

    REQUIRE(rs == expected);
    return true;
  });
  graph_pool::destroy(pool);
}

TEST_CASE("Testing LDBC BI Query 2", "[ldbc_bi]") {
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("snb");
  create_bi_data(graph);

  graph->run_transaction([&]() {
    std::vector<params_tuple> parameters =
        {{time_from_string(std::string("2011-10-01 00:00:00.000"))}};
    result_set rs, expected;
    ldbc_bi_query_2(graph, rs, parameters[0]);

    expected.data.push_back(
      {query_result("Snowboard"), query_result("2"), query_result("3"), query_result("1")});

    REQUIRE(rs == expected);
    return true;
  });
  graph_pool::destroy(pool);
}

TEST_CASE("Testing LDBC BI Query 3", "[ldbc_bi]") {
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("snb");
  create_bi_data(graph);

  graph->run_transaction([&]() {
    std::vector<params_tuple> parameters = {{"Ski resorts", "Spain"}};
    result_set rs, expected;
    ldbc_bi_query_3(graph, rs, parameters[0]);

    expected.data.push_back(
      {query_result("1"), query_result("Skiing trips"),
      query_result("2011-10-10T11:01:47"), query_result("2"),
      query_result("1")});

    REQUIRE(rs == expected);
    return true;
  });
  graph_pool::destroy(pool);
}

TEST_CASE("Testing LDBC BI Query 4", "[ldbc_bi]") {
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("snb");
  create_bi_data(graph);

  graph->run_transaction([&]() {
    std::vector<params_tuple> parameters =
      {{"Spain", time_from_string(std::string("2009-04-14 01:51:21.746"))}};
    result_set rs, expected;
    ldbc_bi_query_4(graph, rs, parameters[0]);

    expected.data.push_back(
      {query_result("2"), query_result("Bernardo"), query_result(""),
      query_result("2010-06-10T11:05:56"), query_result("1")});

    REQUIRE(rs == expected);
    return true;
  });
  graph_pool::destroy(pool);
}

TEST_CASE("Testing LDBC BI Query 5", "[ldbc_bi]") {
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("snb");
  create_bi_data(graph);

  graph->run_transaction([&]() {
    std::vector<params_tuple> parameters = {{"Snowboard"}};
    result_set rs, expected;
    ldbc_bi_query_5(graph, rs, parameters[0]);

    expected.data.push_back(
      {query_result("1"), query_result("2"), query_result("2"),
      query_result("3"), query_result("27")});
    expected.data.push_back(
      {query_result("3"), query_result("2"), query_result("1"),
      query_result("2"), query_result("16")});

    REQUIRE(rs == expected);
    return true;
  });
  graph_pool::destroy(pool);
}

TEST_CASE("Testing LDBC BI Query 6", "[ldbc_bi]") {
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("snb");
  create_bi_data(graph);

  graph->run_transaction([&]() {
    std::vector<params_tuple> parameters = {{"Pyrenees"}};
    result_set rs, expected;
    ldbc_bi_query_6(graph, rs, parameters[0]);

    expected.data.push_back(
      {query_result("2"), query_result("3")});
    expected.data.push_back(
      {query_result("3"), query_result("2")});

    REQUIRE(rs == expected);
    return true;
  });
  graph_pool::destroy(pool);
}

TEST_CASE("Testing LDBC BI Query 7", "[ldbc_bi]") {
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("snb");
  create_bi_data(graph);

  graph->run_transaction([&]() {
    std::vector<params_tuple> parameters = {{"Pyrenees"}};
    result_set rs, expected;
    ldbc_bi_query_7(graph, rs, parameters[0]);

    expected.data.push_back(
      {query_result("Snowboard"), query_result("2")});

    REQUIRE(rs == expected);
    return true;
  });
  graph_pool::destroy(pool);
}

TEST_CASE("Testing LDBC BI Query 8", "[ldbc_bi]") {
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("snb");
  create_bi_data(graph);

  graph->run_transaction([&]() {
    std::vector<params_tuple> parameters = {{"Pyrenees",
        time_from_string(std::string("2010-10-01 00:00:00.000"))}};
    result_set rs, expected;
    ldbc_bi_query_8(graph, rs, parameters[0]);

    expected.data.push_back(
      {query_result("2"), query_result("101"), query_result("2")});
    expected.data.push_back(
      {query_result("3"), query_result("2"), query_result("101")});

    REQUIRE(rs == expected);
    return true;
  });
  graph_pool::destroy(pool);
}

TEST_CASE("Testing LDBC BI Query 9", "[ldbc_bi]") {
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("snb");
  create_bi_data(graph);

  graph->run_transaction([&]() {
    std::vector<params_tuple> parameters =
        {{time_from_string(std::string("2011-10-01 00:00:00.000")),
        time_from_string(std::string("2011-10-15 00:00:00.000"))}};
    result_set rs, expected;
    ldbc_bi_query_9(graph, rs, parameters[0]);

    expected.data.push_back(
      {query_result("2"), query_result("Bernardo"), query_result(""),
      query_result("1"), query_result("4")});

    REQUIRE(rs == expected);
    return true;
  });
  graph_pool::destroy(pool);
}

TEST_CASE("Testing LDBC BI Query 10", "[ldbc_bi]") {
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("snb");
  create_bi_data(graph);

  graph->run_transaction([&]() {
    std::vector<params_tuple> parameters =
        {{(uint64_t)5, "France", "Sports", 2, 3}};
    result_set rs, expected;
    ldbc_bi_query_10(graph, rs, parameters[0]);

    expected.data.push_back(
      {query_result("1"), query_result("Snowboard"), query_result("3")});
    expected.data.push_back(
      {query_result("3"), query_result("Snowboard"), query_result("2")});
    expected.data.push_back(
      {query_result("3"), query_result("Pyrenees"), query_result("1")});

    REQUIRE(rs == expected);
    return true;
  });
  graph_pool::destroy(pool);
}

TEST_CASE("Testing LDBC BI Query 11", "[ldbc_bi]") {
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("snb");
  create_bi_data(graph);

  graph->run_transaction([&]() {
    std::vector<params_tuple> parameters =
        {{"France", time_from_string(std::string("2010-05-01 00:00:00.000"))}};
    result_set rs, expected;
    ldbc_bi_query_11(graph, rs, parameters[0]);

    expected.data.push_back({query_result("1")});

    REQUIRE(rs == expected);
    return true;
  });
  graph_pool::destroy(pool);
}

TEST_CASE("Testing LDBC BI Query 12", "[ldbc_bi]") {
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("snb");
  create_bi_data(graph);

  graph->run_transaction([&]() {
    std::vector<params_tuple> parameters =
      {{time_from_string(std::string("2010-07-22 00:00:00.000")), 50, "en", "fr"}};
    result_set rs, expected;
    ldbc_bi_query_12(graph, rs, parameters[0]);

    expected.data.push_back({query_result("1"), query_result("3")});
    expected.data.push_back({query_result("3"), query_result("1")});
    expected.data.push_back({query_result("0"), query_result("1")});

    REQUIRE(rs == expected);
    return true;
  });
  graph_pool::destroy(pool);
}

TEST_CASE("Testing LDBC BI Query 13", "[ldbc_bi]") {
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("snb");
  create_bi_data(graph);

  graph->run_transaction([&]() {
    std::vector<params_tuple> parameters =
      {{"France", time_from_string(std::string("2013-01-01 00:00:00.000"))}};
    result_set rs, expected;
    ldbc_bi_query_13(graph, rs, parameters[0]);

    expected.data.push_back(
      {query_result("1"), query_result("1"), query_result("2"), query_result("0.500000")});
    expected.data.push_back(
      {query_result("3"), query_result("0"), query_result("1"), query_result("0.000000")});
    expected.data.push_back(
      {query_result("4"), query_result("0"), query_result("0"), query_result("0.000000")});

    REQUIRE(rs == expected);
    return true;
  });
  graph_pool::destroy(pool);
}

TEST_CASE("Testing LDBC BI Query 14", "[ldbc_bi]") {
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("snb");
  create_bi_data(graph);

  graph->run_transaction([&]() {
    std::vector<params_tuple> parameters = {{"France", "Spain"}};
    result_set rs, expected;
    ldbc_bi_query_14(graph, rs, parameters[0]);

    expected.data.push_back(
      {query_result("1"), query_result("2"), query_result("Paris"), query_result("30")});
    expected.data.push_back(
      {query_result("3"), query_result("2"), query_result("Lyon"), query_result("30")});

    REQUIRE(rs == expected);
    return true;
  });
  graph_pool::destroy(pool);
}

TEST_CASE("Testing LDBC BI Query 15", "[ldbc_bi]") {
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("snb");
  create_bi_data(graph);

  graph->run_transaction([&]() {
    std::vector<params_tuple> parameters = {{(uint64_t)2, (uint64_t)4, 
      time_from_string(std::string("2011-06-01 00:00:00.000")),
      time_from_string(std::string("2012-05-31 00:00:00.000"))}};
    result_set rs, expected;
    ldbc_bi_query_15(graph, rs, parameters[0]);

    expected.data.push_back(
      {query_result("[ 2 1 4 ]"), query_result("1.500000")});
    expected.data.push_back(
      {query_result("[ 2 3 4 ]"), query_result("1.000000")});

    REQUIRE(rs == expected);
    return true;
  });
  graph_pool::destroy(pool);
}

TEST_CASE("Testing LDBC BI Query 16", "[ldbc_bi]") {
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("snb");
  create_bi_data(graph);

  graph->run_transaction([&]() {
    std::vector<params_tuple> parameters =
    {{"Pyrenees", time_from_string(std::string("2011-10-10 00:00:00.000")),
    "Snowboard", time_from_string(std::string("2012-03-04 00:00:00.000")), 5}};
    result_set rs, expected;
    ldbc_bi_query_16(graph, rs, parameters[0]);

    expected.data.push_back(
      {query_result("3"), query_result("2"), query_result("1")});

    REQUIRE(rs == expected);
    return true;
  });
  graph_pool::destroy(pool);
}

TEST_CASE("Testing LDBC BI Query 17", "[ldbc_bi]") {
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("snb");
  create_bi_data(graph);

  graph->run_transaction([&]() {
    std::vector<params_tuple> parameters = {{"Snowboard", 10}};
    result_set rs, expected;
    ldbc_bi_query_17(graph, rs, parameters[0]);

    REQUIRE(rs == expected);
    return true;
  });
  graph_pool::destroy(pool);
}

TEST_CASE("Testing LDBC BI Query 18", "[ldbc_bi]") {
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("snb");
  create_bi_data(graph);

  graph->run_transaction([&]() {
    std::vector<params_tuple> parameters = {{(uint64_t)2, "Snowboard"}};
    result_set rs, expected;
    ldbc_bi_query_18(graph, rs, parameters[0]);

    expected.data.push_back(
      {query_result("4"), query_result("2")});

    REQUIRE(rs == expected);
    return true;
  });
  graph_pool::destroy(pool);
}

TEST_CASE("Testing LDBC BI Query 19", "[ldbc_bi]") {
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("snb");
  create_bi_data(graph);

  graph->run_transaction([&]() {
    std::vector<params_tuple> parameters = {{(uint64_t)2, (uint64_t)4}};
    result_set rs, expected;
    ldbc_bi_query_19(graph, rs, parameters[0]);

    expected.data.push_back(
      {query_result("2"), query_result("1"), query_result("1.000000")});
    expected.data.push_back(
      {query_result("2"), query_result("4"), query_result("1.000000")});

    REQUIRE(rs == expected);
    return true;
  });
  graph_pool::destroy(pool);
}

TEST_CASE("Testing LDBC BI Query 20", "[ldbc_bi]") {
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("snb");
  create_bi_data(graph);

  graph->run_transaction([&]() {
    std::vector<params_tuple> parameters = {{"SoftEngCo", (uint64_t)5}};
    result_set rs, expected;
    ldbc_bi_query_20(graph, rs, parameters[0]);

    expected.data.push_back(
      {query_result("1"), query_result("6.000000")});

    REQUIRE(rs == expected);
    return true;
  });
  graph_pool::destroy(pool);
}