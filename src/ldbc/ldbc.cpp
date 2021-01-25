#include "ldbc.hpp"
#include "qop.hpp"
#include "query.hpp"


#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

#define RUN_INDEXED
#define RUN_PARALLEL

#define QP_1

namespace pj = builtin;

// -------------------------------------------------------------------------------------------------------------------------
  #define IS1_QP1a

  #ifdef IS1_QP1a
void ldbc_is_query_1(graph_db_ptr &gdb, result_set &rs, uint64_t personId) {

  auto q = query(gdb)
#ifdef RUN_INDEXED
               .nodes_where_indexed("Person", "id", personId)
#elif defined(RUN_PARALLEL)
               .all_nodes()
               .has_label("Person")
               .property( "id",
                           [&](auto &p) { return p.equal(personId); })
#else
               .nodes_where("Person", "id",
                            [&](auto &p) { return p.equal(personId); })
#endif
               .from_relationships(":isLocatedIn")
               .to_node("Place")
               .project({PExpr_(0, pj::string_property(res, "firstName")),
                         PExpr_(0, pj::string_property(res, "lastName")),
                         PExpr_(0, pj::pr_date(res, "birthday")),
                         PExpr_(0, pj::string_property(res, "locationIP")),
                         PExpr_(0, pj::string_property(res, "browserUsed")),
                         PExpr_(2, pj::uint64_property(res, "id")),
                         PExpr_(0, pj::string_property(res, "gender")),
                         PExpr_(0, pj::ptime_property(res, "creationDate")) })
               .collect(rs);
  q.start();
  rs.wait();
}
  #else
void ldbc_is_query_1(graph_db_ptr &gdb, result_set &rs, uint64_t personId) {
  #define PAR_1

  auto q = query(gdb)
  #ifdef PAR_1
               .all_nodes()
               .has_label("Place")
  #else
               .all_nodes("Place")
  #endif
               .to_relationships(":isLocatedIn")
               .from_node("Person")
               .property( "id",
                           [&](auto &p) { return p.equal(personId); })
               .project({PExpr_(2, pj::string_property(res, "firstName")),
                         PExpr_(2, pj::string_property(res, "lastName")),
                         PExpr_(2, pj::pr_date(res, "birthday")),
                         PExpr_(2, pj::string_property(res, "locationIP")),
                         PExpr_(2, pj::string_property(res, "browserUsed")),
                         PExpr_(0, pj::uint64_property(res, "id")),
                         PExpr_(2, pj::string_property(res, "gender")),
                         PExpr_(2, pj::ptime_property(res, "creationDate")) })
               .collect(rs);
  q.start();
  rs.wait();
}
  #endif

// ------------------------------------------------------------------------------------------------------------------------

void ldbc_is_query_2(graph_db_ptr &gdb, result_set &rs, uint64_t personId) {
  auto maxHops = 100;

  auto q1 = query(gdb)
#ifdef RUN_INDEXED
               .nodes_where_indexed("Person", "id", personId)
#elif defined(RUN_PARALLEL)
               .all_nodes()
               .has_label("Person")
               .property( "id",
                           [&](auto &p) { return p.equal(personId); })
#else
               .nodes_where("Person", "id",
                            [&](auto &p) { return p.equal(personId); })
#endif
               .to_relationships(":hasCreator")
               .from_node("Post")
               .project({PExpr_(2, pj::uint64_property(res, "id")),
                        PExpr_(2, pj::has_property(res, "imageFile") ?
                            pj::string_property(res, "imageFile") : pj::string_property(res, "content")),
                        PExpr_(2, pj::ptime_property(res, "creationDate")),
                        PExpr_(2, pj::uint64_property(res, "id")),
                        PExpr_(0, pj::uint64_property(res, "id")),
                        PExpr_(0, pj::string_property(res, "firstName")),
                        PExpr_(0, pj::string_property(res, "lastName")) });

  auto q2 = query(gdb)
#ifdef RUN_INDEXED
               .nodes_where_indexed("Person", "id", personId)
#elif defined(RUN_PARALLEL)
               .all_nodes()
               .has_label("Person")
               .property( "id",
                           [&](auto &p) { return p.equal(personId); })
#else
               .nodes_where("Person", "id",
                            [&](auto &p) { return p.equal(personId); })
#endif
               .to_relationships(":hasCreator")
               .from_node("Comment")
               .from_relationships({1, maxHops}, ":replyOf")
               .to_node("Post")
               .from_relationships(":hasCreator")
               .to_node("Person")
               .project({PExpr_(2, pj::uint64_property(res, "id")),
                        PExpr_(2, pj::string_property(res, "content")),
                        PExpr_(2, pj::ptime_property(res, "creationDate")),
                        PExpr_(4, pj::uint64_property(res, "id")),
                        PExpr_(6, pj::uint64_property(res, "id")),
                        PExpr_(6, pj::string_property(res, "firstName")),
                        PExpr_(6, pj::string_property(res, "lastName")) })
               .union_all(q1)
               .orderby([&](const qr_tuple &qr1, const qr_tuple &qr2) {
                        if (boost::get<boost::posix_time::ptime>(qr1[2]) == boost::get<boost::posix_time::ptime>(qr2[2]))
                          return boost::get<uint64_t>(qr1[0]) > boost::get<uint64_t>(qr2[0]);
                        return boost::get<boost::posix_time::ptime>(qr1[2]) > boost::get<boost::posix_time::ptime>(qr2[2]); })
               .limit(10)
               .collect(rs);

  query::start({&q1, &q2});
  rs.wait();
}

  #define IS2p_QP1a

  #ifdef IS2p_QP1a
void ldbc_is_query_2_p(graph_db_ptr &gdb, result_set &rs, uint64_t personId) {
  auto q = query(gdb)
#ifdef RUN_INDEXED
               .nodes_where_indexed("Person", "id", personId)
#elif defined(RUN_PARALLEL)
               .all_nodes()
               .has_label("Person")
               .property( "id",
                           [&](auto &p) { return p.equal(personId); })
#else
               .nodes_where("Person", "id",
                            [&](auto &p) { return p.equal(personId); })
#endif
               .to_relationships(":hasCreator")
               .from_node("Post")
               .project({PExpr_(2, pj::uint64_property(res, "id")),
                        PExpr_(2, pj::has_property(res, "imageFile") ?
                            pj::string_property(res, "imageFile") : pj::string_property(res, "content")),
                        PExpr_(2, pj::ptime_property(res, "creationDate")),
                        PExpr_(2, pj::uint64_property(res, "id")),
                        PExpr_(0, pj::uint64_property(res, "id")),
                        PExpr_(0, pj::string_property(res, "firstName")),
                        PExpr_(0, pj::string_property(res, "lastName")) })
               .orderby([&](const qr_tuple &qr1, const qr_tuple &qr2) {
                        if (boost::get<boost::posix_time::ptime>(qr1[2]) == boost::get<boost::posix_time::ptime>(qr2[2]))
                          return boost::get<uint64_t>(qr1[0]) > boost::get<uint64_t>(qr2[0]);
                        return boost::get<boost::posix_time::ptime>(qr1[2]) > boost::get<boost::posix_time::ptime>(qr2[2]); })
               .limit(10)
               .collect(rs);

  q.start();
  rs.wait();
}
  #else
void ldbc_is_query_2_p(graph_db_ptr &gdb, result_set &rs, uint64_t personId) {
  auto q = query(gdb)
#ifdef RUN_INDEXED
               .nodes_where_indexed("Person", "id", personId)
#elif defined(RUN_PARALLEL)
               .all_nodes()
               .has_label("Person")
               .property( "id",
                           [&](auto &p) { return p.equal(personId); })
#else
               .nodes_where("Person", "id",
                            [&](auto &p) { return p.equal(personId); })
#endif
               .to_relationships(":hasCreator")
               .from_node("Post")
               .project({PVar_(0),
                        PVar_(2),
                        PExpr_(2, pj::uint64_property(res, "id")),
                        PExpr_(2, pj::ptime_property(res, "creationDate")) })
               .orderby([&](const qr_tuple &qr1, const qr_tuple &qr2) {
                        if (boost::get<boost::posix_time::ptime>(qr1[3]) == boost::get<boost::posix_time::ptime>(qr2[3]))
                          return boost::get<uint64_t>(qr1[2]) > boost::get<uint64_t>(qr2[2]);
                        return boost::get<boost::posix_time::ptime>(qr1[3]) > boost::get<boost::posix_time::ptime>(qr2[3]); })
               .limit(10)
               .project({PVar_(2),
                        PExpr_(1, pj::has_property(res, "imageFile") ?
                            pj::string_property(res, "imageFile") : pj::string_property(res, "content")),
                        PVar_(3),
                        PExpr_(1, pj::uint64_property(res, "id")),
                        PExpr_(0, pj::uint64_property(res, "id")),
                        PExpr_(0, pj::string_property(res, "firstName")),
                        PExpr_(0, pj::string_property(res, "lastName")) })
               .collect(rs);

  q.start();
  rs.wait();
}
  #endif

// ------------------------------------------------------------------------------------------------------------------------

#ifdef QP_1

  #define IS2c_QP1a

  #ifdef IS2c_QP1a
void ldbc_is_query_2_c(graph_db_ptr &gdb, result_set &rs, uint64_t personId) {
  auto maxHops = 100;

  auto q = query(gdb)
#ifdef RUN_INDEXED
               .nodes_where_indexed("Person", "id", personId)
#elif defined(RUN_PARALLEL)
               .all_nodes()
               .has_label("Person")
               .property( "id",
                           [&](auto &p) { return p.equal(personId); })
#else
               .nodes_where("Person", "id",
                            [&](auto &p) { return p.equal(personId); })
#endif
               .to_relationships(":hasCreator")
               .from_node("Comment")
               .from_relationships({1, maxHops}, ":replyOf")
               .to_node("Post")
               .from_relationships(":hasCreator")
               .to_node("Person")
               .project({PExpr_(2, pj::uint64_property(res, "id")),
                        PExpr_(2, pj::string_property(res, "content")),
                        PExpr_(2, pj::ptime_property(res, "creationDate")),
                        PExpr_(4, pj::uint64_property(res, "id")),
                        PExpr_(6, pj::uint64_property(res, "id")),
                        PExpr_(6, pj::string_property(res, "firstName")),
                        PExpr_(6, pj::string_property(res, "lastName")) })
               .orderby([&](const qr_tuple &qr1, const qr_tuple &qr2) {
                        if (boost::get<boost::posix_time::ptime>(qr1[2]) == boost::get<boost::posix_time::ptime>(qr2[2]))
                          return boost::get<uint64_t>(qr1[0]) > boost::get<uint64_t>(qr2[0]);
                        return boost::get<boost::posix_time::ptime>(qr1[2]) > boost::get<boost::posix_time::ptime>(qr2[2]); })
               .limit(10)
               .collect(rs);

  q.start();
  rs.wait();
}
  #else
void ldbc_is_query_2_c(graph_db_ptr &gdb, result_set &rs, uint64_t personId) {
  auto maxHops = 100;

  auto q = query(gdb)
#ifdef RUN_INDEXED
               .nodes_where_indexed("Person", "id", personId)
#elif defined(RUN_PARALLEL)
               .all_nodes()
               .has_label("Person")
               .property( "id",
                           [&](auto &p) { return p.equal(personId); })
#else
               .nodes_where("Person", "id",
                            [&](auto &p) { return p.equal(personId); })
#endif
               .to_relationships(":hasCreator")
               .from_node("Comment")
               .from_relationships({1, maxHops}, ":replyOf")
               .to_node("Post")
               .from_relationships(":hasCreator")
               .to_node("Person")
               .project({PVar_(2),
                        PVar_(4),
                        PVar_(6),
                        PExpr_(2, pj::uint64_property(res, "id")),
                        PExpr_(2, pj::ptime_property(res, "creationDate")) })
               .orderby([&](const qr_tuple &qr1, const qr_tuple &qr2) {
                        if (boost::get<boost::posix_time::ptime>(qr1[4]) == boost::get<boost::posix_time::ptime>(qr2[4]))
                          return boost::get<uint64_t>(qr1[3]) > boost::get<uint64_t>(qr2[3]);
                        return boost::get<boost::posix_time::ptime>(qr1[4]) > boost::get<boost::posix_time::ptime>(qr2[4]); })
               .limit(10)
               .project({PVar_(3),
                        PExpr_(0, pj::string_property(res, "content")),
                        PVar_(4),
                        PExpr_(1, pj::uint64_property(res, "id")),
                        PExpr_(2, pj::uint64_property(res, "id")),
                        PExpr_(2, pj::string_property(res, "firstName")),
                        PExpr_(2, pj::string_property(res, "lastName")) })
               .collect(rs);

  q.start();
  rs.wait();
}
  #endif

#else

  #define IS2c_QP3c
  #define HASHJOIN

void ldbc_is_query_2_c(graph_db_ptr &gdb, result_set &rs, uint64_t personId) {
  auto maxHops = 100;

  #ifndef IS2c_QP3c
  auto q1 = query(gdb)
#ifdef RUN_INDEXED
               .nodes_where_indexed("Person", "id", personId)
#elif defined(RUN_PARALLEL)
               .all_nodes()
               .has_label("Person")
               .property( "id",
                           [&](auto &p) { return p.equal(personId); })
#else
               .nodes_where("Person", "id",
                            [&](auto &p) { return p.equal(personId); })
#endif
               .to_relationships(":hasCreator")
               .from_node("Comment")
               .from_relationships({1, maxHops}, ":replyOf")
               .to_node("Post");

  #ifdef IS2c_QP3a
  auto q2 = query(gdb)
               .all_nodes()
               .has_label("Person")
               .to_relationships(":hasCreator")
               .from_node("Post")
  #ifdef HASHJOIN
               .hashjoin_on_node({2, 4}, q1)
  #else
               .join_on_node({2, 4}, q1)
  #endif
               .project({PExpr_(5, pj::uint64_property(res, "id")),
                        PExpr_(5, pj::string_property(res, "content")),
                        PExpr_(5, pj::ptime_property(res, "creationDate")),
                        PExpr_(2, pj::uint64_property(res, "id")),
                        PExpr_(0, pj::uint64_property(res, "id")),
                        PExpr_(0, pj::string_property(res, "firstName")),
                        PExpr_(0, pj::string_property(res, "lastName")) })
               .orderby([&](const qr_tuple &qr1, const qr_tuple &qr2) {
                        if (boost::get<boost::posix_time::ptime>(qr1[2]) == boost::get<boost::posix_time::ptime>(qr2[2]))
                          return boost::get<uint64_t>(qr1[0]) > boost::get<uint64_t>(qr2[0]);
                        return boost::get<boost::posix_time::ptime>(qr1[2]) > boost::get<boost::posix_time::ptime>(qr2[2]); })
               .limit(10)
               .collect(rs);

  #else
  auto q2 = query(gdb)
               .all_nodes()
               .has_label("Person")
               .to_relationships(":hasCreator")
               .from_node("Post")
  #ifdef HASHJOIN
               .hashjoin_on_node({2, 4}, q1)
  #else
               .join_on_node({2, 4}, q1)
  #endif
               .project({PVar_(0),
                        PVar_(2),
                        PVar_(5),
                        PExpr_(5, pj::uint64_property(res, "id")),
                        PExpr_(5, pj::ptime_property(res, "creationDate")) })
               .orderby([&](const qr_tuple &qr1, const qr_tuple &qr2) {
                        if (boost::get<boost::posix_time::ptime>(qr1[4]) == boost::get<boost::posix_time::ptime>(qr2[4]))
                          return boost::get<uint64_t>(qr1[3]) > boost::get<uint64_t>(qr2[3]);
                        return boost::get<boost::posix_time::ptime>(qr1[4]) > boost::get<boost::posix_time::ptime>(qr2[4]); })
               .limit(10)
               .project({PVar_(3),
                        PExpr_(2, pj::string_property(res, "content")),
                        PVar_(4),
                        PExpr_(1, pj::uint64_property(res, "id")),
                        PExpr_(0, pj::uint64_property(res, "id")),
                        PExpr_(0, pj::string_property(res, "firstName")),
                        PExpr_(0, pj::string_property(res, "lastName")) })
               .collect(rs);
    #endif

  #else

  auto q1 = query(gdb)
               .all_nodes()
               .has_label("Person")
               .to_relationships(":hasCreator")
               .from_node("Post");

  auto q2 = query(gdb)
#ifdef RUN_INDEXED
               .nodes_where_indexed("Person", "id", personId)
#elif defined(RUN_PARALLEL)
               .all_nodes()
               .has_label("Person")
               .property( "id",
                           [&](auto &p) { return p.equal(personId); })
#else
               .nodes_where("Person", "id",
                            [&](auto &p) { return p.equal(personId); })
#endif
               .to_relationships(":hasCreator")
               .from_node("Comment")
               .from_relationships({1, maxHops}, ":replyOf")
               .to_node("Post")
               .project({PVar_(2),
                        PVar_(4),
                        PExpr_(2, pj::uint64_property(res, "id")),
                        PExpr_(2, pj::ptime_property(res, "creationDate")) })
               .orderby([&](const qr_tuple &qr1, const qr_tuple &qr2) {
                        if (boost::get<boost::posix_time::ptime>(qr1[3]) == boost::get<boost::posix_time::ptime>(qr2[3]))
                          return boost::get<uint64_t>(qr1[2]) > boost::get<uint64_t>(qr2[2]);
                        return boost::get<boost::posix_time::ptime>(qr1[3]) > boost::get<boost::posix_time::ptime>(qr2[3]); })
               .limit(10)
               .join_on_node({1, 2}, q1)
               .project({PVar_(2),
                        PExpr_(0, pj::string_property(res, "content")),
                        PVar_(3),
                        PExpr_(1, pj::uint64_property(res, "id")),
                        PExpr_(4, pj::uint64_property(res, "id")),
                        PExpr_(4, pj::string_property(res, "firstName")),
                        PExpr_(4, pj::string_property(res, "lastName")) })
               .collect(rs);

  #endif
  query::start({&q1, &q2});
  rs.wait();
}

#endif

// ------------------------------------------------------------------------------------------------------------------------

  #define IS3_QP1a

  #ifdef IS3_QP1a
void ldbc_is_query_3(graph_db_ptr &gdb, result_set &rs, uint64_t personId) {

  auto q = query(gdb)
#ifdef RUN_INDEXED
               .nodes_where_indexed("Person", "id", personId)
#elif defined(RUN_PARALLEL)
                .all_nodes()
               .has_label("Person")
               .property( "id",
                           [&](auto &p) { return p.equal(personId); })
#else
                .nodes_where("Person", "id",
                            [&](auto &p) { return p.equal(personId); })
#endif
                .from_relationships(":knows")
                .to_node("Person")
                .project({PExpr_(2, pj::uint64_property(res, "id")),
                          PExpr_(2, pj::string_property(res, "firstName")),
                          PExpr_(2, pj::string_property(res, "lastName")),
                          PExpr_(1, pj::ptime_property(res, "creationDate"))})
                .orderby([&](const qr_tuple &qr1, const qr_tuple &qr2) {
                          if (boost::get<boost::posix_time::ptime>(qr1[3]) == boost::get<boost::posix_time::ptime>(qr2[3]))
                            return boost::get<uint64_t>(qr1[0]) < boost::get<uint64_t>(qr2[0]);
                          return boost::get<boost::posix_time::ptime>(qr1[3]) > boost::get<boost::posix_time::ptime>(qr2[3]); })
                .collect(rs);

  q.start();
  rs.wait();
}
#else
void ldbc_is_query_3(graph_db_ptr &gdb, result_set &rs, uint64_t personId) {

  auto q = query(gdb)
#ifdef RUN_INDEXED
               .nodes_where_indexed("Person", "id", personId)
#elif defined(RUN_PARALLEL)
                .all_nodes()
               .has_label("Person")
               .property( "id",
                           [&](auto &p) { return p.equal(personId); })
#else
                .nodes_where("Person", "id",
                            [&](auto &p) { return p.equal(personId); })
#endif
                .from_relationships(":knows")
                .to_node("Person")
                .project({PVar_(2),
                          PExpr_(2, pj::uint64_property(res, "id")),
                          PExpr_(1, pj::ptime_property(res, "creationDate"))})
                .orderby([&](const qr_tuple &qr1, const qr_tuple &qr2) {
                          if (boost::get<boost::posix_time::ptime>(qr1[2]) == boost::get<boost::posix_time::ptime>(qr2[2]))
                            return boost::get<uint64_t>(qr1[1]) < boost::get<uint64_t>(qr2[1]);
                          return boost::get<boost::posix_time::ptime>(qr1[2]) > boost::get<boost::posix_time::ptime>(qr2[2]); })
                .project({PVar_(1),
                          PExpr_(0, pj::string_property(res, "firstName")),
                          PExpr_(0, pj::string_property(res, "lastName")),
                          PVar_(2)})
                .collect(rs);

  q.start();
  rs.wait();
}
#endif

// ------------------------------------------------------------------------------------------------------------------------

void ldbc_is_query_4(graph_db_ptr &gdb, result_set &rs, uint64_t messageId) {
  std::vector<std::string> message = {"Post", "Comment"};

  auto q = query(gdb)
#ifdef RUN_INDEXED
               .nodes_where_indexed(message, "id", messageId)
#elif defined(RUN_PARALLEL)
               .all_nodes()
               .has_label(message)
               .property( "id",
                           [&](auto &p) { return p.equal(messageId); })
#else
                .nodes_where(message, "id",
                            [&](auto &p) { return p.equal(messageId); })
#endif
                .project({PExpr_(0, pj::ptime_property(res, "creationDate")),
                          PExpr_(0, pj::has_property(res, "imageFile") ?
                            pj::string_property(res, "imageFile") : pj::string_property(res, "content")) })
                .collect(rs);

  q.start();
  rs.wait();
}

void ldbc_is_query_4_p(graph_db_ptr &gdb, result_set &rs, uint64_t messageId) {

  auto q = query(gdb)
#ifdef RUN_INDEXED
               .nodes_where_indexed("Post", "id", messageId)
#elif defined(RUN_PARALLEL)
               .all_nodes()
               .has_label("Post")
               .property( "id",
                           [&](auto &p) { return p.equal(messageId); })
#else
                .nodes_where("Post", "id",
                            [&](auto &p) { return p.equal(messageId); })
#endif
                .project({PExpr_(0, pj::ptime_property(res, "creationDate")),
                          PExpr_(0, pj::has_property(res, "imageFile") ?
                            pj::string_property(res, "imageFile") : pj::string_property(res, "content")) })
                .collect(rs);

  q.start();
  rs.wait();
}

// ------------------------------------------------------------------------------------------------------------------------

void ldbc_is_query_4_c(graph_db_ptr &gdb, result_set &rs, uint64_t messageId) {

  auto q = query(gdb)
#ifdef RUN_INDEXED
               .nodes_where_indexed("Comment", "id", messageId)
#elif defined(RUN_PARALLEL)
               .all_nodes()
               .has_label("Comment")
               .property( "id",
                           [&](auto &p) { return p.equal(messageId); })
#else
                .nodes_where("Comment", "id",
                            [&](auto &p) { return p.equal(messageId); })
#endif
                .project({PExpr_(0, pj::ptime_property(res, "creationDate")),
                          PExpr_(0, pj::string_property(res, "content")) })
                .collect(rs);

  q.start();
  rs.wait();
}

// ------------------------------------------------------------------------------------------------------------------------

void ldbc_is_query_5(graph_db_ptr &gdb, result_set &rs, uint64_t messageId) {
  std::vector<std::string> message = {"Post", "Comment"};

  auto q = query(gdb)
#ifdef RUN_INDEXED
               .nodes_where_indexed(message, "id", messageId)
#elif defined(RUN_PARALLEL)
                .all_nodes()
               .has_label(message)
               .property( "id",
                           [&](auto &p) { return p.equal(messageId); })
#else
                .nodes_where(message, "id",
                            [&](auto &p) { return p.equal(messageId); })
#endif
                .from_relationships(":hasCreator")
                .to_node("Person")
                .project({PExpr_(2, pj::uint64_property(res, "id")),
                          PExpr_(2, pj::string_property(res, "firstName")),
                          PExpr_(2, pj::string_property(res, "lastName")) })
                .collect(rs);

  q.start();
  rs.wait();
}

void ldbc_is_query_5_p(graph_db_ptr &gdb, result_set &rs, uint64_t messageId) {

  auto q = query(gdb)
#ifdef RUN_INDEXED
               .nodes_where_indexed("Post", "id", messageId)
#elif defined(RUN_PARALLEL)
                .all_nodes()
               .has_label("Post")
               .property( "id",
                           [&](auto &p) { return p.equal(messageId); })
#else
                .nodes_where("Post", "id",
                            [&](auto &p) { return p.equal(messageId); })
#endif
                .from_relationships(":hasCreator")
                .to_node("Person")
                .project({PExpr_(2, pj::uint64_property(res, "id")),
                          PExpr_(2, pj::string_property(res, "firstName")),
                          PExpr_(2, pj::string_property(res, "lastName")) })
                .collect(rs);

  q.start();
  rs.wait();
}

// ------------------------------------------------------------------------------------------------------------------------

void ldbc_is_query_5_c(graph_db_ptr &gdb, result_set &rs, uint64_t messageId) {

  auto q = query(gdb)
#ifdef RUN_INDEXED
               .nodes_where_indexed("Comment", "id", messageId)
#elif defined(RUN_PARALLEL)
                .all_nodes()
               .has_label("Comment")
               .property( "id",
                           [&](auto &p) { return p.equal(messageId); })
#else
                .nodes_where("Comment", "id",
                            [&](auto &p) { return p.equal(messageId); })
#endif
                .from_relationships(":hasCreator")
                .to_node("Person")
                .project({PExpr_(2, pj::uint64_property(res, "id")),
                          PExpr_(2, pj::string_property(res, "firstName")),
                          PExpr_(2, pj::string_property(res, "lastName")) })
                .collect(rs);

  q.start();
  rs.wait();
}

// ------------------------------------------------------------------------------------------------------------------------

void ldbc_is_query_6(graph_db_ptr &gdb, result_set &rs, uint64_t messageId) {
  auto maxHops = 100;

    auto q1 = query(gdb)
#ifdef RUN_INDEXED
               .nodes_where_indexed("Post", "id", messageId)
#elif defined(RUN_PARALLEL)
               .all_nodes()
               .has_label("Post")
               .property( "id",
                           [&](auto &p) { return p.equal(messageId); })
#else
                .nodes_where("Post", "id",
                            [&](auto &p) { return p.equal(messageId); })
#endif
                .to_relationships(":containerOf")
                .from_node("Forum")
                .from_relationships(":hasModerator")
                .to_node("Person")
                .project({PExpr_(2, pj::uint64_property(res, "id")),
                          PExpr_(2, pj::string_property(res, "title")),
                          PExpr_(4, pj::uint64_property(res, "id")),
                          PExpr_(4, pj::string_property(res, "firstName")),
                          PExpr_(4, pj::string_property(res, "lastName")) });

  auto q2 = query(gdb)
#ifdef RUN_INDEXED
               .nodes_where_indexed("Comment", "id", messageId)
#elif defined(RUN_PARALLEL)
               .all_nodes()
               .has_label("Comment")
               .property( "id",
                           [&](auto &p) { return p.equal(messageId); })
#else
                .nodes_where("Comment", "id",
                            [&](auto &p) { return p.equal(messageId); })
#endif
                .from_relationships({1, maxHops}, ":replyOf")
                .to_node("Post")
                .to_relationships(":containerOf")
                .from_node("Forum")
                .from_relationships(":hasModerator")
                .to_node("Person")
                .project({PExpr_(4, pj::uint64_property(res, "id")),
                          PExpr_(4, pj::string_property(res, "title")),
                          PExpr_(6, pj::uint64_property(res, "id")),
                          PExpr_(6, pj::string_property(res, "firstName")),
                          PExpr_(6, pj::string_property(res, "lastName")) })
                .union_all(q1)
                .collect(rs);

  query::start({&q1, &q2});
  rs.wait();
}

void ldbc_is_query_6_p(graph_db_ptr &gdb, result_set &rs, uint64_t messageId) {
  auto q = query(gdb)
#ifdef RUN_INDEXED
               .nodes_where_indexed("Post", "id", messageId)
#elif defined(RUN_PARALLEL)
               .all_nodes()
               .has_label("Post")
               .property( "id",
                           [&](auto &p) { return p.equal(messageId); })
#else
                .nodes_where("Post", "id",
                            [&](auto &p) { return p.equal(messageId); })
#endif
                .to_relationships(":containerOf")
                .from_node("Forum")
                .from_relationships(":hasModerator")
                .to_node("Person")
                .project({PExpr_(2, pj::uint64_property(res, "id")),
                          PExpr_(2, pj::string_property(res, "title")),
                          PExpr_(4, pj::uint64_property(res, "id")),
                          PExpr_(4, pj::string_property(res, "firstName")),
                          PExpr_(4, pj::string_property(res, "lastName")) })
                .collect(rs);

  q.start();
  rs.wait();
}

// ------------------------------------------------------------------------------------------------------------------------

void ldbc_is_query_6_c(graph_db_ptr &gdb, result_set &rs, uint64_t messageId) {
  auto maxHops = 100;

  auto q = query(gdb)
#ifdef RUN_INDEXED
               .nodes_where_indexed("Comment", "id", messageId)
#elif defined(RUN_PARALLEL)
               .all_nodes()
               .has_label("Comment")
               .property( "id",
                           [&](auto &p) { return p.equal(messageId); })
#else
                .nodes_where("Comment", "id",
                            [&](auto &p) { return p.equal(messageId); })
#endif
                .from_relationships({1, maxHops}, ":replyOf")
                .to_node("Post")
                .to_relationships(":containerOf")
                .from_node("Forum")
                .from_relationships(":hasModerator")
                .to_node("Person")
                .project({PExpr_(4, pj::uint64_property(res, "id")),
                          PExpr_(4, pj::string_property(res, "title")),
                          PExpr_(6, pj::uint64_property(res, "id")),
                          PExpr_(6, pj::string_property(res, "firstName")),
                          PExpr_(6, pj::string_property(res, "lastName")) })
                .collect(rs);

  q.start();
  rs.wait();
}

// ------------------------------------------------------------------------------------------------------------------------

void ldbc_is_query_7(graph_db_ptr &gdb, result_set &rs, uint64_t messageId) {
  std::vector<std::string> message = {"Post", "Comment"};

  auto q1 = query(gdb)
#ifdef RUN_INDEXED
               .nodes_where_indexed(message, "id", messageId)
#elif defined(RUN_PARALLEL)
                .all_nodes()
               .has_label(message)
               .property( "id",
                           [&](auto &p) { return p.equal(messageId); })
#else
                .nodes_where(message, "id",
                            [&](auto &p) { return p.equal(messageId); })
#endif
                .from_relationships(":hasCreator")
                .to_node("Person");

  auto q2 = query(gdb)
#ifdef RUN_INDEXED
               .nodes_where_indexed(message, "id", messageId)
#elif defined(RUN_PARALLEL)
               .all_nodes()
               .has_label(message)
               .property( "id",
                           [&](auto &p) { return p.equal(messageId); })
#else
                .nodes_where(message, "id",
                            [&](auto &p) { return p.equal(messageId); })
#endif
                .to_relationships(":replyOf")
                .from_node("Comment")
                .from_relationships(":hasCreator")
                .to_node("Person")
                .outerjoin_on_rship({4, 2}, q1)
                .project({PExpr_(2, pj::uint64_property(res, "id")),
                          PExpr_(2, pj::string_property(res, "content")),
                          PExpr_(2, pj::ptime_property(res, "creationDate")),
                          PExpr_(4, pj::uint64_property(res, "id")),
                          PExpr_(4, pj::string_property(res, "firstName")),
                          PExpr_(4, pj::string_property(res, "lastName")),
                          PExpr_(8, pj::string_rep(res) == "NULL" ?
                                      std::string("false") : std::string("true")) })
                .orderby([&](const qr_tuple &qr1, const qr_tuple &qr2) {
                        if (boost::get<boost::posix_time::ptime>(qr1[2]) == boost::get<boost::posix_time::ptime>(qr2[2]))
                          return boost::get<uint64_t>(qr1[0]) > boost::get<uint64_t>(qr2[0]);
                        return boost::get<boost::posix_time::ptime>(qr1[2]) < boost::get<boost::posix_time::ptime>(qr2[2]); })
                .collect(rs);

  query::start({&q1, &q2});
  rs.wait();
}

#ifdef QP_1

void ldbc_is_query_7_p(graph_db_ptr &gdb, result_set &rs, uint64_t messageId) {

  auto q1 = query(gdb)
#ifdef RUN_INDEXED
               .nodes_where_indexed("Post", "id", messageId)
#elif defined(RUN_PARALLEL)
                .all_nodes()
               .has_label("Post")
               .property( "id",
                           [&](auto &p) { return p.equal(messageId); })
#else
                .nodes_where("Post", "id",
                            [&](auto &p) { return p.equal(messageId); })
#endif
                .from_relationships(":hasCreator")
                .to_node("Person");

  #define IS7p_QP1a

  #ifdef IS7p_QP1a
  auto q2 = query(gdb)
#ifdef RUN_INDEXED
               .nodes_where_indexed("Post", "id", messageId)
#elif defined(RUN_PARALLEL)
               .all_nodes()
               .has_label("Post")
               .property( "id",
                           [&](auto &p) { return p.equal(messageId); })
#else
                .nodes_where("Post", "id",
                            [&](auto &p) { return p.equal(messageId); })
#endif
                .to_relationships(":replyOf")
                .from_node("Comment")
                .from_relationships(":hasCreator")
                .to_node("Person")
                .outerjoin_on_rship({4, 2}, q1)
                .project({PExpr_(2, pj::uint64_property(res, "id")),
                          PExpr_(2, pj::string_property(res, "content")),
                          PExpr_(2, pj::ptime_property(res, "creationDate")),
                          PExpr_(4, pj::uint64_property(res, "id")),
                          PExpr_(4, pj::string_property(res, "firstName")),
                          PExpr_(4, pj::string_property(res, "lastName")),
                          PExpr_(8, pj::string_rep(res) == "NULL" ?
                                      std::string("false") : std::string("true")) })
                .orderby([&](const qr_tuple &qr1, const qr_tuple &qr2) {
                        if (boost::get<boost::posix_time::ptime>(qr1[2]) == boost::get<boost::posix_time::ptime>(qr2[2]))
                          return boost::get<uint64_t>(qr1[0]) > boost::get<uint64_t>(qr2[0]);
                        return boost::get<boost::posix_time::ptime>(qr1[2]) < boost::get<boost::posix_time::ptime>(qr2[2]); })
                .collect(rs);

  #elif defined(IS7p_QP1b)
  auto q2 = query(gdb)
#ifdef RUN_INDEXED
               .nodes_where_indexed("Post", "id", messageId)
#elif defined(RUN_PARALLEL)
               .all_nodes()
               .has_label("Post")
               .property( "id",
                           [&](auto &p) { return p.equal(messageId); })
#else
                .nodes_where("Post", "id",
                            [&](auto &p) { return p.equal(messageId); })
#endif
                .to_relationships(":replyOf")
                .from_node("Comment")
                .from_relationships(":hasCreator")
                .to_node("Person")
                .outerjoin_on_rship({4, 2}, q1)
                .project({PVar_(2),
                          PVar_(4),
                          PVar_(8),
                          PExpr_(2, pj::uint64_property(res, "id")),
                          PExpr_(2, pj::ptime_property(res, "creationDate")) })
                .orderby([&](const qr_tuple &qr1, const qr_tuple &qr2) {
                        if (boost::get<boost::posix_time::ptime>(qr1[4]) == boost::get<boost::posix_time::ptime>(qr2[4]))
                          return boost::get<uint64_t>(qr1[3]) > boost::get<uint64_t>(qr2[3]);
                        return boost::get<boost::posix_time::ptime>(qr1[4]) < boost::get<boost::posix_time::ptime>(qr2[4]); })
                .project({PVar_(3),
                          PExpr_(0, pj::string_property(res, "content")),
                          PVar_(4),
                          PExpr_(1, pj::uint64_property(res, "id")),
                          PExpr_(1, pj::string_property(res, "firstName")),
                          PExpr_(1, pj::string_property(res, "lastName")),
                          PExpr_(2, pj::string_rep(res) == "NULL" ?
                                      std::string("false") : std::string("true")) })
                .collect(rs);

  #else
  auto q2 = query(gdb)
#ifdef RUN_INDEXED
               .nodes_where_indexed("Post", "id", messageId)
#elif defined(RUN_PARALLEL)
               .all_nodes()
               .has_label("Post")
               .property( "id",
                           [&](auto &p) { return p.equal(messageId); })
#else
                .nodes_where("Post", "id",
                            [&](auto &p) { return p.equal(messageId); })
#endif
                .to_relationships(":replyOf")
                .from_node("Comment")
                .from_relationships(":hasCreator")
                .to_node("Person")
                .project({PVar_(2),
                          PVar_(4),
                          PExpr_(2, pj::uint64_property(res, "id")),
                          PExpr_(2, pj::ptime_property(res, "creationDate")) })
                .orderby([&](const qr_tuple &qr1, const qr_tuple &qr2) {
                        if (boost::get<boost::posix_time::ptime>(qr1[3]) == boost::get<boost::posix_time::ptime>(qr2[3]))
                          return boost::get<uint64_t>(qr1[2]) > boost::get<uint64_t>(qr2[2]);
                        return boost::get<boost::posix_time::ptime>(qr1[3]) < boost::get<boost::posix_time::ptime>(qr2[3]); })
                .outerjoin_on_rship({1, 2}, q1)
                .project({PVar_(2),
                          PExpr_(0, pj::string_property(res, "content")),
                          PVar_(3),
                          PExpr_(1, pj::uint64_property(res, "id")),
                          PExpr_(1, pj::string_property(res, "firstName")),
                          PExpr_(1, pj::string_property(res, "lastName")),
                          PExpr_(5, pj::string_rep(res) == "NULL" ?
                                      std::string("false") : std::string("true")) })
                .collect(rs);

  #endif
  query::start({&q1, &q2});
  rs.wait();
}

#else

void ldbc_is_query_7_p(graph_db_ptr &gdb, result_set &rs, uint64_t messageId) {
  #define PAR_7p

  auto q = query(gdb)
        #ifdef PAR_7p
                .all_nodes()
                .has_label("Person")
        #else
                .all_nodes("Person")
        #endif
                .to_relationships(":hasCreator")
                .from_node("Post")
                .property( "id",
                           [&](auto &p) { return p.equal(messageId); })
                .to_relationships(":replyOf")
                .from_node("Comment")
                .from_relationships(":hasCreator")
                .to_node("Person")
                .rship_exists({6, 0}, true)
                .project({PExpr_(4, pj::uint64_property(res, "id")),
                          PExpr_(4, pj::string_property(res, "content")),
                          PExpr_(4, pj::ptime_property(res, "creationDate")),
                          PExpr_(6, pj::uint64_property(res, "id")),
                          PExpr_(6, pj::string_property(res, "firstName")),
                          PExpr_(6, pj::string_property(res, "lastName")),
                          PExpr_(7, pj::string_rep(res) == "NULL" ?
                                      std::string("false") : std::string("true")) })
                .orderby([&](const qr_tuple &qr1, const qr_tuple &qr2) {
                        if (boost::get<boost::posix_time::ptime>(qr1[2]) == boost::get<boost::posix_time::ptime>(qr2[2]))
                          return boost::get<uint64_t>(qr1[0]) > boost::get<uint64_t>(qr2[0]);
                        return boost::get<boost::posix_time::ptime>(qr1[2]) < boost::get<boost::posix_time::ptime>(qr2[2]); })
                .collect(rs);

  q.start();
  rs.wait();
}

#endif

// ------------------------------------------------------------------------------------------------------------------------

#ifdef QP_1
void ldbc_is_query_7_c(graph_db_ptr &gdb, result_set &rs, uint64_t messageId) {

  auto q1 = query(gdb)
#ifdef RUN_INDEXED
               .nodes_where_indexed("Comment", "id", messageId)
#elif defined(RUN_PARALLEL)
                .all_nodes()
               .has_label("Comment")
               .property( "id",
                           [&](auto &p) { return p.equal(messageId); })
#else
                .nodes_where("Comment", "id",
                            [&](auto &p) { return p.equal(messageId); })
#endif
                .from_relationships(":hasCreator")
                .to_node("Person");

  #define IS7c_QP1a

  #ifdef IS7c_QP1a
  auto q2 = query(gdb)
#ifdef RUN_INDEXED
               .nodes_where_indexed("Comment", "id", messageId)
#elif defined(RUN_PARALLEL)
               .all_nodes()
               .has_label("Comment")
               .property( "id",
                           [&](auto &p) { return p.equal(messageId); })
#else
                .nodes_where("Comment", "id",
                            [&](auto &p) { return p.equal(messageId); })
#endif
                .to_relationships(":replyOf")
                .from_node("Comment")
                .from_relationships(":hasCreator")
                .to_node("Person")
                .outerjoin_on_rship({4, 2}, q1)
                .project({PExpr_(2, pj::uint64_property(res, "id")),
                          PExpr_(2, pj::string_property(res, "content")),
                          PExpr_(2, pj::ptime_property(res, "creationDate")),
                          PExpr_(4, pj::uint64_property(res, "id")),
                          PExpr_(4, pj::string_property(res, "firstName")),
                          PExpr_(4, pj::string_property(res, "lastName")),
                          PExpr_(8, pj::string_rep(res) == "NULL" ?
                                      std::string("false") : std::string("true")) })
                .orderby([&](const qr_tuple &qr1, const qr_tuple &qr2) {
                        if (boost::get<boost::posix_time::ptime>(qr1[2]) == boost::get<boost::posix_time::ptime>(qr2[2]))
                          return boost::get<uint64_t>(qr1[0]) > boost::get<uint64_t>(qr2[0]);
                        return boost::get<boost::posix_time::ptime>(qr1[2]) < boost::get<boost::posix_time::ptime>(qr2[2]); })
                .collect(rs);

  #elif defined(IS7c_QP1b)
  auto q2 = query(gdb)
#ifdef RUN_INDEXED
               .nodes_where_indexed("Comment", "id", messageId)
#elif defined(RUN_PARALLEL)
               .all_nodes()
               .has_label("Comment")
               .property( "id",
                           [&](auto &p) { return p.equal(messageId); })
#else
                .nodes_where("Comment", "id",
                            [&](auto &p) { return p.equal(messageId); })
#endif
                .to_relationships(":replyOf")
                .from_node("Comment")
                .from_relationships(":hasCreator")
                .to_node("Person")
                .outerjoin_on_rship({4, 2}, q1)
                .project({PVar_(2),
                          PVar_(4),
                          PVar_(8),
                          PExpr_(2, pj::uint64_property(res, "id")),
                          PExpr_(2, pj::ptime_property(res, "creationDate"))})
                .orderby([&](const qr_tuple &qr1, const qr_tuple &qr2) {
                        if (boost::get<boost::posix_time::ptime>(qr1[4]) == boost::get<boost::posix_time::ptime>(qr2[4]))
                          return boost::get<uint64_t>(qr1[3]) > boost::get<uint64_t>(qr2[3]);
                        return boost::get<boost::posix_time::ptime>(qr1[4]) < boost::get<boost::posix_time::ptime>(qr2[4]); })
                .project({PVar_(3),
                          PExpr_(0, pj::string_property(res, "content")),
                          PVar_(4),
                          PExpr_(1, pj::uint64_property(res, "id")),
                          PExpr_(1, pj::string_property(res, "firstName")),
                          PExpr_(1, pj::string_property(res, "lastName")),
                          PExpr_(2, pj::string_rep(res) == "NULL" ?
                                      std::string("false") : std::string("true")) })
                .collect(rs);

  #else
  auto q2 = query(gdb)
#ifdef RUN_INDEXED
               .nodes_where_indexed("Comment", "id", messageId)
#elif defined(RUN_PARALLEL)
               .all_nodes()
               .has_label("Comment")
               .property( "id",
                           [&](auto &p) { return p.equal(messageId); })
#else
                .nodes_where("Comment", "id",
                            [&](auto &p) { return p.equal(messageId); })
#endif
                .to_relationships(":replyOf")
                .from_node("Comment")
                .from_relationships(":hasCreator")
                .to_node("Person")
                .project({PVar_(2),
                          PVar_(4),
                          PExpr_(2, pj::uint64_property(res, "id")),
                          PExpr_(2, pj::ptime_property(res, "creationDate"))})
                .orderby([&](const qr_tuple &qr1, const qr_tuple &qr2) {
                        if (boost::get<boost::posix_time::ptime>(qr1[3]) == boost::get<boost::posix_time::ptime>(qr2[3]))
                          return boost::get<uint64_t>(qr1[2]) > boost::get<uint64_t>(qr2[2]);
                        return boost::get<boost::posix_time::ptime>(qr1[3]) < boost::get<boost::posix_time::ptime>(qr2[3]); })
                .outerjoin_on_rship({1, 2}, q1)
                .project({PVar_(2),
                          PExpr_(0, pj::string_property(res, "content")),
                          PVar_(3),
                          PExpr_(1, pj::uint64_property(res, "id")),
                          PExpr_(1, pj::string_property(res, "firstName")),
                          PExpr_(1, pj::string_property(res, "lastName")),
                          PExpr_(5, pj::string_rep(res) == "NULL" ?
                                      std::string("false") : std::string("true")) })
                .collect(rs);

  #endif
  query::start({&q1, &q2});
  rs.wait();
}

#else

void ldbc_is_query_7_c(graph_db_ptr &gdb, result_set &rs, uint64_t messageId) {
  #define PAR_7c

  auto q = query(gdb)
        #ifdef PAR_7c
                .all_nodes()
                .has_label("Person")
        #else
                .all_nodes("Person")
        #endif
                .to_relationships(":hasCreator")
                .from_node("Comment")
                .property( "id",
                           [&](auto &p) { return p.equal(messageId); })
                .to_relationships(":replyOf")
                .from_node("Comment")
                .from_relationships(":hasCreator")
                .to_node("Person")
                .rship_exists({6, 0}, true)
                .project({PExpr_(4, pj::uint64_property(res, "id")),
                          PExpr_(4, pj::string_property(res, "content")),
                          PExpr_(4, pj::ptime_property(res, "creationDate")),
                          PExpr_(6, pj::uint64_property(res, "id")),
                          PExpr_(6, pj::string_property(res, "firstName")),
                          PExpr_(6, pj::string_property(res, "lastName")),
                          PExpr_(7, pj::string_rep(res) == "NULL" ?
                                      std::string("false") : std::string("true")) })
                .orderby([&](const qr_tuple &qr1, const qr_tuple &qr2) {
                        if (boost::get<boost::posix_time::ptime>(qr1[2]) == boost::get<boost::posix_time::ptime>(qr2[2]))
                          return boost::get<uint64_t>(qr1[0]) > boost::get<uint64_t>(qr2[0]);
                        return boost::get<boost::posix_time::ptime>(qr1[2]) < boost::get<boost::posix_time::ptime>(qr2[2]); })
                .collect(rs);

  q.start();
  rs.wait();
}

#endif

// ------------------------------------------------------------------------------------------------------------------------

void ldbc_iu_query_1(graph_db_ptr &gdb, result_set &rs, params_tuple &params) {

  auto q1 = query(gdb)
#ifdef RUN_INDEXED
               .nodes_where_indexed("Place", "id", boost::get<uint64_t>(params[0]));
#else
#ifdef RUN_PARALLEL
               .all_nodes()
               .has_label("Place")
               .property( "id",
                           [&](auto &p) { return p.equal(boost::get<uint64_t>(params[0])); });
#else
                .nodes_where("Place", "id",
                            [&](auto &p) { return p.equal(boost::get<uint64_t>(params[0])); });
#endif
#endif

  auto q2 = query(gdb)
#ifdef RUN_INDEXED
               .nodes_where_indexed("Tag", "id", boost::get<uint64_t>(params[1]));
#else
#ifdef RUN_PARALLEL
               .all_nodes()
               .has_label("Tag")
               .property( "id",
                           [&](auto &p) { return p.equal(boost::get<uint64_t>(params[1])); });
#else
                .nodes_where("Tag", "id",
                            [&](auto &p) { return p.equal(boost::get<uint64_t>(params[1])); });
#endif
#endif

  auto q3 = query(gdb)
#ifdef RUN_INDEXED
               .nodes_where_indexed("Organisation", "id", boost::get<uint64_t>(params[2]));
#else
#ifdef RUN_PARALLEL
               .all_nodes()
               .has_label("Organisation")
               .property( "id",
                           [&](auto &p) { return p.equal(boost::get<uint64_t>(params[2])); });
#else
                .nodes_where("Organisation", "id",
                            [&](auto &p) { return p.equal(boost::get<uint64_t>(params[2])); });
#endif
#endif

  auto q4 = query(gdb)
#ifdef RUN_INDEXED
               .nodes_where_indexed("Organisation", "id", boost::get<uint64_t>(params[3]));
#else
#ifdef RUN_PARALLEL
               .all_nodes()
               .has_label("Organisation")
               .property( "id",
                           [&](auto &p) { return p.equal(boost::get<uint64_t>(params[3])); });
#else
                .nodes_where("Organisation", "id",
                            [&](auto &p) { return p.equal(boost::get<uint64_t>(params[3])); });
#endif
#endif

  auto q5 = query(gdb).create("Person",
                              {{"id", boost::any(boost::get<uint64_t>(params[4]))},
                              {"firstName", boost::any(boost::get<std::string &>(params[5]))},
                              {"lastName", boost::any(boost::get<std::string &>(params[6]))},
                              {"gender", boost::any(boost::get<std::string &>(params[7]))},
                              {"birthday", boost::any(boost::get<std::string &>(params[8]))},
                              {"creationDate", boost::any(boost::get<std::string &>(params[9]))},
                              {"locationIP", boost::any(boost::get<std::string &>(params[10]))},
                              {"browserUsed", boost::any(boost::get<std::string &>(params[11]))},
                              {"language", boost::any(boost::get<std::string &>(params[12]))},
                              {"email", boost::any(boost::get<std::string &>(params[13]))}})
                      .crossjoin(q1)
                      .create_rship({0, 1}, ":isLocatedIn", {})
                      .crossjoin(q2)
                      .create_rship({0, 3}, ":hasInterest", {})
                      .crossjoin(q3)
                      .create_rship({0, 5}, ":studyAt", {{"classYear", boost::any(boost::get<int>(params[14]))}})
                      .crossjoin(q4)
                      .create_rship({0, 7}, ":workAt", {{"workFrom", boost::any(boost::get<int>(params[15]))}})
#ifdef IU_RESULT
                      .collect(rs);
#else
                      .finish();
#endif

  query::start({&q1, &q2, &q3, &q4, &q5});
}

// ------------------------------------------------------------------------------------------------------------------------

void ldbc_iu_query_2(graph_db_ptr &gdb, result_set &rs, params_tuple &params) {

  auto q1 = query(gdb)
#ifdef RUN_INDEXED
               .nodes_where_indexed("Post", "id", boost::get<uint64_t>(params[0]));
#else
#ifdef RUN_PARALLEL
               .all_nodes()
               .has_label("Post")
               .property( "id",
                           [&](auto &p) { return p.equal(boost::get<uint64_t>(params[0])); });
#else
                .nodes_where("Post", "id",
                            [&](auto &p) { return p.equal(boost::get<uint64_t>(params[0])); });
#endif
#endif

  auto q2 = query(gdb)
#ifdef RUN_INDEXED
               .nodes_where_indexed("Person", "id", boost::get<uint64_t>(params[1]))
#else
#ifdef RUN_PARALLEL
               .all_nodes()
               .has_label("Person")
               .property( "id",
                           [&](auto &p) { return p.equal(boost::get<uint64_t>(params[1])); })
#else
                .nodes_where("Person", "id",
                                  [&](auto &p) { return p.equal(boost::get<uint64_t>(params[1])); })
#endif
#endif
                .crossjoin(q1)
                .create_rship({0, 1}, ":likes", {{"creationDate", boost::any(boost::get<std::string &>(params[2]))}})
#ifdef IU_RESULT
                .collect(rs);
#else
                .finish();
#endif

  query::start({&q1, &q2});
}

// ------------------------------------------------------------------------------------------------------------------------

void ldbc_iu_query_3(graph_db_ptr &gdb, result_set &rs, params_tuple &params) {

  auto q1 = query(gdb)
#ifdef RUN_INDEXED
               .nodes_where_indexed("Comment", "id", boost::get<uint64_t>(params[0]));
#else
#ifdef RUN_PARALLEL
               .all_nodes()
               .has_label("Comment")
               .property( "id",
                           [&](auto &p) { return p.equal(boost::get<uint64_t>(params[0])); });
#else
                .nodes_where("Comment", "id",
                            [&](auto &p) { return p.equal(boost::get<uint64_t>(params[0])); });
#endif
#endif

  auto q2 =
      query(gdb)
#ifdef RUN_INDEXED
               .nodes_where_indexed("Person", "id", boost::get<uint64_t>(params[1]))
#else
#ifdef RUN_PARALLEL
               .all_nodes()
               .has_label("Person")
               .property( "id",
                           [&](auto &p) { return p.equal(boost::get<uint64_t>(params[1])); })
#else
          .nodes_where("Person", "id",
                            [&](auto &p) { return p.equal(boost::get<uint64_t>(params[1])); })
#endif
#endif
          .crossjoin(q1)
          .create_rship({0, 1}, ":likes", {{"creationDate", boost::any(boost::get<std::string &>(params[2]))}})
#ifdef IU_RESULT
          .collect(rs);
#else
          .finish();
#endif

  query::start({&q1, &q2});
}

// ------------------------------------------------------------------------------------------------------------------------

void ldbc_iu_query_4(graph_db_ptr &gdb, result_set &rs, params_tuple &params) {

  auto q1 = query(gdb)
#ifdef RUN_INDEXED
               .nodes_where_indexed("Person", "id", boost::get<uint64_t>(params[0]));
#else
#ifdef RUN_PARALLEL
               .all_nodes()
               .has_label("Person")
               .property( "id",
                           [&](auto &p) { return p.equal(boost::get<uint64_t>(params[0])); });
#else
                .nodes_where("Person", "id",
                            [&](auto &p) { return p.equal(boost::get<uint64_t>(params[0])); });
#endif
#endif

  auto q2 = query(gdb)
#ifdef RUN_INDEXED
               .nodes_where_indexed("Tag", "id", boost::get<uint64_t>(params[1]));
#else
#ifdef RUN_PARALLEL
               .all_nodes()
               .has_label("Tag")
               .property( "id",
                           [&](auto &p) { return p.equal(boost::get<uint64_t>(params[1])); });
#else
                .nodes_where("Tag", "id",
                            [&](auto &p) { return p.equal(boost::get<uint64_t>(params[1])); });
#endif
#endif

  auto q3 = query(gdb).create("Forum",
                              {{"id", boost::any(boost::get<uint64_t>(params[2]))},
                              {"title", boost::any(boost::get<std::string &>(params[3]))},
                              {"creationDate", boost::any(boost::get<std::string &>(params[4]))} })
                      .crossjoin(q1)
                      .create_rship({0, 1}, ":hasModerator", {})
                      .crossjoin(q2)
                      .create_rship({0, 3}, ":hasTag", {})
#ifdef IU_RESULT
                      .collect(rs);
#else
                      .finish();
#endif

  query::start({&q1, &q2, &q3});
}

// ------------------------------------------------------------------------------------------------------------------------

void ldbc_iu_query_5(graph_db_ptr &gdb, result_set &rs, params_tuple &params) {

  auto q1 =
      query(gdb)
#ifdef RUN_INDEXED
               .nodes_where_indexed("Person", "id", boost::get<uint64_t>(params[0]));
#else
#ifdef RUN_PARALLEL
               .all_nodes()
               .has_label("Person")
               .property( "id",
                           [&](auto &p) { return p.equal(boost::get<uint64_t>(params[0])); });
#else
          .nodes_where("Person", "id",
                            [&](auto &p) { return p.equal(boost::get<uint64_t>(params[0])); });
#endif
#endif

  auto q2 =
      query(gdb)
#ifdef RUN_INDEXED
               .nodes_where_indexed("Forum", "id", boost::get<uint64_t>(params[1]))
#else
#ifdef RUN_PARALLEL
               .all_nodes()
               .has_label("Forum")
               .property( "id",
                           [&](auto &p) { return p.equal(boost::get<uint64_t>(params[1])); })
#else
          .nodes_where("Forum", "id",
                            [&](auto &p) { return p.equal(boost::get<uint64_t>(params[1])); })
#endif
#endif
          .crossjoin(q1)
          .create_rship({0, 1}, ":hasMember", {{"creationDate", boost::any(boost::get<std::string &>(params[2]))}})
#ifdef IU_RESULT
          .collect(rs);
#else
          .finish();
#endif

  query::start({&q1, &q2});
}

// ------------------------------------------------------------------------------------------------------------------------

void ldbc_iu_query_6(graph_db_ptr &gdb, result_set &rs, params_tuple &params) {

  auto q1 = query(gdb)
#ifdef RUN_INDEXED
               .nodes_where_indexed("Person", "id", boost::get<uint64_t>(params[0]));
#else
#ifdef RUN_PARALLEL
               .all_nodes()
               .has_label("Person")
               .property( "id",
                           [&](auto &p) { return p.equal(boost::get<uint64_t>(params[0])); });
#else
                .nodes_where("Person", "id",
                            [&](auto &p) { return p.equal(boost::get<uint64_t>(params[0])); });
#endif
#endif

  auto q2 = query(gdb)
#ifdef RUN_INDEXED
               .nodes_where_indexed("Forum", "id", boost::get<uint64_t>(params[1]));
#else
#ifdef RUN_PARALLEL
               .all_nodes()
               .has_label("Forum")
               .property( "id",
                           [&](auto &p) { return p.equal(boost::get<uint64_t>(params[1])); });
#else
                .nodes_where("Forum", "id",
                            [&](auto &p) { return p.equal(boost::get<uint64_t>(params[1])); });
#endif
#endif

  auto q3 = query(gdb)
#ifdef RUN_INDEXED
               .nodes_where_indexed("Place", "id", boost::get<uint64_t>(params[2]));
#else
#ifdef RUN_PARALLEL
               .all_nodes()
               .has_label("Place")
               .property( "id",
                           [&](auto &p) { return p.equal(boost::get<uint64_t>(params[2])); });
#else
                .nodes_where("Place", "id",
                            [&](auto &p) { return p.equal(boost::get<uint64_t>(params[2])); });
#endif
#endif

  auto q4 = query(gdb)
#ifdef RUN_INDEXED
               .nodes_where_indexed("Tag", "id", boost::get<uint64_t>(params[3]));
#else
#ifdef RUN_PARALLEL
               .all_nodes()
               .has_label("Tag")
               .property( "id",
                           [&](auto &p) { return p.equal(boost::get<uint64_t>(params[3])); });
#else
                .nodes_where("Tag", "id",
                            [&](auto &p) { return p.equal(boost::get<uint64_t>(params[3])); });
#endif
#endif

  auto q5 = query(gdb).create("Post",
                            {{"id", boost::any(boost::get<uint64_t>(params[4]))},
                              {"imageFile", boost::any(boost::get<std::string &>(params[5]))},
                              {"creationDate", boost::any(boost::get<std::string &>(params[6]))},
                              {"locationIP", boost::any(boost::get<std::string &>(params[7]))},
                              {"browserUsed", boost::any(boost::get<std::string &>(params[8]))},
                              {"language", boost::any(boost::get<std::string &>(params[9]))},
                              {"content", boost::any(boost::get<std::string &>(params[10]))},
                              {"length", boost::any(boost::get<int>(params[11]))} })
                      .crossjoin(q1)
                      .create_rship({0, 1}, ":hasCreator", {})
                      .crossjoin(q2)
                      .create_rship({3, 0}, ":containerOf", {})
                      .crossjoin(q3)
                      .create_rship({0, 5}, ":isLocatedIn", {})
                      .crossjoin(q4)
                      .create_rship({0, 7}, ":hasTag", {})
#ifdef IU_RESULT
                      .collect(rs);
#else
                      .finish();
#endif

  query::start({&q1, &q2, &q3, &q4, &q5});
}

// ------------------------------------------------------------------------------------------------------------------------

void ldbc_iu_query_7(graph_db_ptr &gdb, result_set &rs, params_tuple &params) {

  auto q1 = query(gdb)
#ifdef RUN_INDEXED
               .nodes_where_indexed("Person", "id", boost::get<uint64_t>(params[0]));
#else
#ifdef RUN_PARALLEL
               .all_nodes()
               .has_label("Person")
               .property( "id",
                           [&](auto &p) { return p.equal(boost::get<uint64_t>(params[0])); });
#else
                .nodes_where("Person", "id",
                            [&](auto &p) { return p.equal(boost::get<uint64_t>(params[0])); });
#endif
#endif

  auto q2 = query(gdb)
#ifdef RUN_INDEXED
               .nodes_where_indexed("Post", "id", boost::get<uint64_t>(params[1]));
#else
#ifdef RUN_PARALLEL
               .all_nodes()
               .has_label("Post")
               .property( "id",
                           [&](auto &p) { return p.equal(boost::get<uint64_t>(params[1])); });
#else
                .nodes_where("Post", "id",
                            [&](auto &p) { return p.equal(boost::get<uint64_t>(params[1])); });
#endif
#endif

  auto q3 = query(gdb)
#ifdef RUN_INDEXED
               .nodes_where_indexed("Place", "id", boost::get<uint64_t>(params[2]));
#else
#ifdef RUN_PARALLEL
               .all_nodes()
               .has_label("Place")
               .property( "id",
                           [&](auto &p) { return p.equal(boost::get<uint64_t>(params[2])); });
#else
                .nodes_where("Place", "id",
                            [&](auto &p) { return p.equal(boost::get<uint64_t>(params[2])); });
#endif
#endif

  auto q4 = query(gdb)
#ifdef RUN_INDEXED
               .nodes_where_indexed("Tag", "id", boost::get<uint64_t>(params[3]));
#else
#ifdef RUN_PARALLEL
               .all_nodes()
               .has_label("Tag")
               .property( "id",
                           [&](auto &p) { return p.equal(boost::get<uint64_t>(params[3])); });
#else
                .nodes_where("Tag", "id",
                            [&](auto &p) { return p.equal(boost::get<uint64_t>(params[3])); });
#endif
#endif

  auto q5 = query(gdb).create("Comment",
                              {{"id", boost::any(boost::get<uint64_t>(params[4]))},
                              {"creationDate", boost::any(boost::get<std::string &>(params[5]))},
                              {"locationIP", boost::any(boost::get<std::string &>(params[6]))},
                              {"browserUsed", boost::any(boost::get<std::string &>(params[7]))},
                              {"content", boost::any(boost::get<std::string &>(params[8]))},
                              {"length", boost::any(boost::get<int>(params[9]))} })
                      .crossjoin(q1)
                      .create_rship({0, 1}, ":hasCreator", {})
                      .crossjoin(q2)
                      .create_rship({0, 3}, ":replyOf", {})
                      .crossjoin(q3)
                      .create_rship({0, 5}, ":isLocatedIn", {})
                      .crossjoin(q4)
                      .create_rship({0, 7}, ":hasTag", {})
#ifdef IU_RESULT
                      .collect(rs);
#else
                      .finish();
#endif

  query::start({&q1, &q2, &q3, &q4, &q5});
}

// ------------------------------------------------------------------------------------------------------------------------

void ldbc_iu_query_8(graph_db_ptr &gdb, result_set &rs, params_tuple &params) {

  auto q1 =
      query(gdb)
#ifdef RUN_INDEXED
               .nodes_where_indexed("Person", "id", boost::get<uint64_t>(params[0]));
#else
#ifdef RUN_PARALLEL
               .all_nodes()
               .has_label("Person")
               .property( "id",
                           [&](auto &p) { return p.equal(boost::get<uint64_t>(params[0])); });
#else
          .nodes_where("Person", "id",
                            [&](auto &p) { return p.equal(boost::get<uint64_t>(params[0])); });
#endif
#endif

  auto q2 =
      query(gdb)
#ifdef RUN_INDEXED
               .nodes_where_indexed("Person", "id", boost::get<uint64_t>(params[1]))
#else
#ifdef RUN_PARALLEL
               .all_nodes()
               .has_label("Person")
               .property( "id",
                           [&](auto &p) { return p.equal(boost::get<uint64_t>(params[1])); })
#else
          .nodes_where("Person", "id",
                            [&](auto &p) { return p.equal(boost::get<uint64_t>(params[1])); })
#endif
#endif
          .crossjoin(q1)
          .create_rship({0, 1}, ":knows", {{"creationDate", boost::any(boost::get<std::string &>(params[2]))}})
#ifdef IU_RESULT
          .collect(rs);
#else
          .finish();
#endif

  query::start({&q1, &q2});
}


// ------------------------------------------------------------------------------------------------------------------------

void bfs(graph_db_ptr &gdb, result_set &rs, uint64_t id){
  std::cout << "BFS \n";
  constexpr int inf = std::numeric_limits<int>::max();
  node::id_t size = gdb->get_nodes()->as_vec().first_available();
  std::vector<int>* depth = new std::vector<int>(size, inf);
  std::queue<uint64_t>* q = new std::queue<uint64_t>();
  q->push(id);
  (*depth)[id] = 0;

  while (!q->empty()){
    uint64_t uid = q->front();
    auto &u = gdb->node_by_id(uid);
    gdb->foreach_from_relationship_of_node(u, [&](auto &r) {
      auto &v = gdb->node_by_id(r.to_node_id());
      auto vid = v.id();
      if ((*depth)[vid] == inf){
        (*depth)[vid] = (*depth)[uid] + 1;
        q->push(vid);
      }
    });
    q->pop();
  }

  for (auto i = 930; i < 939; i++)
    std::cout << (*depth)[i] << " ";

}

void load_snb_data(graph_db_ptr &graph, const std::string& path, bool strict) {
  std::string snb_sta = path + "/static/";
  std::string snb_dyn = path + "/dynamic/";

  std::vector<std::string> node_files =
    {snb_dyn + "comment_0_0.csv", snb_dyn + "comment_1_0.csv",
    snb_dyn + "comment_2_0.csv", snb_dyn + "comment_3_0.csv",
    snb_dyn + "forum_0_0.csv", snb_dyn + "forum_1_0.csv",
    snb_dyn + "forum_2_0.csv", snb_dyn + "forum_3_0.csv",
    snb_sta + "organisation_0_0.csv", snb_sta + "organisation_1_0.csv",
    snb_sta + "organisation_2_0.csv", snb_sta + "organisation_3_0.csv",
    snb_dyn + "person_0_0.csv", snb_dyn + "person_1_0.csv",
    snb_dyn + "person_2_0.csv", snb_dyn + "person_3_0.csv",
    snb_sta + "place_0_0.csv", snb_sta + "place_1_0.csv",
    snb_sta + "place_2_0.csv", snb_sta + "place_3_0.csv",
    snb_dyn + "post_0_0.csv", snb_dyn + "post_1_0.csv",
    snb_dyn + "post_2_0.csv", snb_dyn + "post_3_0.csv",
    snb_sta + "tag_0_0.csv", snb_sta + "tag_1_0.csv",
    snb_sta + "tag_2_0.csv", snb_sta + "tag_3_0.csv",
    snb_sta + "tagclass_0_0.csv", snb_sta + "tagclass_1_0.csv",
    snb_sta + "tagclass_2_0.csv", snb_sta + "tagclass_3_0.csv"};

  std::vector<std::string> rship_files =
    {snb_dyn + "comment_hasCreator_person_0_0.csv",
    snb_dyn + "comment_hasCreator_person_1_0.csv",
    snb_dyn + "comment_hasCreator_person_2_0.csv",
    snb_dyn + "comment_hasCreator_person_3_0.csv",
    snb_dyn + "comment_hasTag_tag_0_0.csv",
    snb_dyn + "comment_hasTag_tag_1_0.csv",
    snb_dyn + "comment_hasTag_tag_2_0.csv",
    snb_dyn + "comment_hasTag_tag_3_0.csv",
    snb_dyn + "comment_isLocatedIn_place_0_0.csv",
    snb_dyn + "comment_isLocatedIn_place_1_0.csv",
    snb_dyn + "comment_isLocatedIn_place_2_0.csv",
    snb_dyn + "comment_isLocatedIn_place_3_0.csv",
    snb_dyn + "comment_replyOf_comment_0_0.csv",
    snb_dyn + "comment_replyOf_comment_1_0.csv",
    snb_dyn + "comment_replyOf_comment_2_0.csv",
    snb_dyn + "comment_replyOf_comment_3_0.csv",
    snb_dyn + "comment_replyOf_post_0_0.csv",
    snb_dyn + "comment_replyOf_post_1_0.csv",
    snb_dyn + "comment_replyOf_post_2_0.csv",
    snb_dyn + "comment_replyOf_post_3_0.csv",
    snb_dyn + "forum_containerOf_post_0_0.csv",
    snb_dyn + "forum_containerOf_post_1_0.csv",
    snb_dyn + "forum_containerOf_post_2_0.csv",
    snb_dyn + "forum_containerOf_post_3_0.csv",
    snb_dyn + "forum_hasMember_person_0_0.csv",
    snb_dyn + "forum_hasMember_person_1_0.csv",
    snb_dyn + "forum_hasMember_person_2_0.csv",
    snb_dyn + "forum_hasMember_person_3_0.csv",
    snb_dyn + "forum_hasModerator_person_0_0.csv",
    snb_dyn + "forum_hasModerator_person_1_0.csv",
    snb_dyn + "forum_hasModerator_person_2_0.csv",
    snb_dyn + "forum_hasModerator_person_3_0.csv",
    snb_dyn + "forum_hasTag_tag_0_0.csv",
    snb_dyn + "forum_hasTag_tag_1_0.csv",
    snb_dyn + "forum_hasTag_tag_2_0.csv",
    snb_dyn + "forum_hasTag_tag_3_0.csv",
    snb_sta + "organisation_isLocatedIn_place_0_0.csv",
    snb_sta + "organisation_isLocatedIn_place_1_0.csv",
    snb_sta + "organisation_isLocatedIn_place_2_0.csv",
    snb_sta + "organisation_isLocatedIn_place_3_0.csv",
    snb_dyn + "person_hasInterest_tag_0_0.csv",
    snb_dyn + "person_hasInterest_tag_1_0.csv",
    snb_dyn + "person_hasInterest_tag_2_0.csv",
    snb_dyn + "person_hasInterest_tag_3_0.csv",
    snb_dyn + "person_isLocatedIn_place_0_0.csv",
    snb_dyn + "person_isLocatedIn_place_1_0.csv",
    snb_dyn + "person_isLocatedIn_place_2_0.csv",
    snb_dyn + "person_isLocatedIn_place_3_0.csv",
    snb_dyn + "person_knows_person_0_0.csv",
    snb_dyn + "person_knows_person_1_0.csv",
    snb_dyn + "person_knows_person_2_0.csv",
    snb_dyn + "person_knows_person_3_0.csv",
    snb_dyn + "person_likes_comment_0_0.csv",
    snb_dyn + "person_likes_comment_1_0.csv",
    snb_dyn + "person_likes_comment_2_0.csv",
    snb_dyn + "person_likes_comment_3_0.csv",
    snb_dyn + "person_likes_post_0_0.csv",
    snb_dyn + "person_likes_post_1_0.csv",
    snb_dyn + "person_likes_post_2_0.csv",
    snb_dyn + "person_likes_post_3_0.csv",
    snb_dyn + "person_studyAt_organisation_0_0.csv",
    snb_dyn + "person_studyAt_organisation_1_0.csv",
    snb_dyn + "person_studyAt_organisation_2_0.csv",
    snb_dyn + "person_studyAt_organisation_3_0.csv",
    snb_dyn + "person_workAt_organisation_0_0.csv",
    snb_dyn + "person_workAt_organisation_1_0.csv",
    snb_dyn + "person_workAt_organisation_2_0.csv",
    snb_dyn + "person_workAt_organisation_3_0.csv",
    snb_sta + "place_isPartOf_place_0_0.csv",
    snb_sta + "place_isPartOf_place_1_0.csv",
    snb_sta + "place_isPartOf_place_2_0.csv",
    snb_sta + "place_isPartOf_place_3_0.csv",
    snb_dyn + "post_hasCreator_person_0_0.csv",
    snb_dyn + "post_hasCreator_person_1_0.csv",
    snb_dyn + "post_hasCreator_person_2_0.csv",
    snb_dyn + "post_hasCreator_person_3_0.csv",
    snb_dyn + "post_hasTag_tag_0_0.csv",
    snb_dyn + "post_hasTag_tag_1_0.csv",
    snb_dyn + "post_hasTag_tag_2_0.csv",
    snb_dyn + "post_hasTag_tag_3_0.csv",
    snb_dyn + "post_isLocatedIn_place_0_0.csv",
    snb_dyn + "post_isLocatedIn_place_1_0.csv",
    snb_dyn + "post_isLocatedIn_place_2_0.csv",
    snb_dyn + "post_isLocatedIn_place_3_0.csv",
    snb_sta + "tag_hasType_tagclass_0_0.csv",
    snb_sta + "tag_hasType_tagclass_1_0.csv",
    snb_sta + "tag_hasType_tagclass_2_0.csv",
    snb_sta + "tag_hasType_tagclass_3_0.csv",
    snb_sta + "tagclass_isSubclassOf_tagclass_0_0.csv",
    snb_sta + "tagclass_isSubclassOf_tagclass_1_0.csv",
    snb_sta + "tagclass_isSubclassOf_tagclass_2_0.csv",
    snb_sta + "tagclass_isSubclassOf_tagclass_3_0.csv"};

  spdlog::info("trying to load data from {} and {}", snb_sta, snb_dyn);
  load_snb_data(graph, node_files, rship_files, strict);
}


void load_snb_data(graph_db_ptr &graph,
                    std::vector<std::string> &node_files,
                    std::vector<std::string> &rship_files, bool strict) {
  if (strict)
    spdlog::info("using strict mode for loading");

  auto delim = '|';
  graph_db::mapping_t mapping;
  std::mutex imtx;

  if (!node_files.empty()) {
    spdlog::info("--------- Importing nodes...");

#ifdef PARALLEL_LOAD
    std::vector<std::future<std::pair<std::string, std::size_t>>> res;
    res.reserve(node_files.size());
    thread_pool pool;

    for (auto &file : node_files) {
      res.push_back(pool.submit([&](){
        std::vector<std::string> fp;
        boost::split(fp, file, boost::is_any_of("/"));
         assert(fp.back().find(".csv",fp.size()-4) != std::string::npos);
        auto pos = fp.back().find("_");
        auto label = fp.back().substr(0, pos);
        if (label[0] >= 'a' && label[0] <= 'z')
          label[0] -= 32;

        // spdlog::info("processing file '{}'...", file);
        auto num_nodes = strict
          ? graph->import_typed_nodes_from_csv(label, file, delim, mapping, &imtx)
          : graph->import_nodes_from_csv(label, file, delim, mapping, &imtx);
        // spdlog::info("file '{}' finished.", file);
        return std::make_pair(label, num_nodes);
      }));
    }
    for (auto &f : res) {
      auto resp = f.get();
      if (resp.second > 0)
        spdlog::info("{} '{}' node objects imported", resp.second, resp.first);
    }
#else
    for (auto &file : node_files) {
      auto start_tm = std::chrono::steady_clock::now();

      std::vector<std::string> fp;
      boost::split(fp, file, boost::is_any_of("/"));
       assert(fp.back().find(".csv",fp.size()-4) != std::string::npos);
      auto pos = fp.back().find("_");
      auto label = fp.back().substr(0, pos);
      if (label[0] >= 'a' && label[0] <= 'z')
        label[0] -= 32;

      auto num_nodes = strict
        ? graph->import_typed_nodes_from_csv(label, file, delim, mapping)
        : graph->import_nodes_from_csv(label, file, delim, mapping);
      auto end_tm = std::chrono::steady_clock::now();
      auto runtime = std::chrono::duration_cast<std::chrono::milliseconds>(end_tm -
                                                                       start_tm).count();

      if (num_nodes > 0) {
        spdlog::info("{} '{}' nodes imported in {} secs", num_nodes, label, runtime / 1000.0);
        // graph->print_stats();
      }
    }
#endif
  }

  if (!rship_files.empty()) {
    spdlog::info("--------- Importing relationships ...");

#ifdef PARALLEL_RSHIP_LOAD
    std::vector<std::future<std::pair<std::string, std::size_t>>> res;
    res.reserve(rship_files.size());
    thread_pool pool;

    for (auto &file : rship_files) {
      res.push_back(pool.submit([&](){
        std::vector<std::string> fp;
        boost::split(fp, file, boost::is_any_of("/"));
        assert(fp.back().find(".csv",fp.size()-4) != std::string::npos);
        std::vector<std::string> fn;
        boost::split(fn, fp.back(), boost::is_any_of("_"));
        auto label = ":" + fn[1];

        auto num_rships = strict
          ? graph->import_typed_relationships_from_csv(file, delim, mapping, &imtx)
          : graph->import_relationships_from_csv(file, delim, mapping, &imtx);
        char buf[100];
        sprintf(buf, "(%s)-[%s]-(%s)", fn[0].c_str(), label.c_str(), fn[2].c_str());
        return std::make_pair(std::string(buf), num_rships);
      }));
    }
    for (auto &f : res) {
      auto resp = f.get();
      if (resp.second > 0)
        spdlog::info("{} '{}' relationships imported", resp.second, resp.first);
    }
#else
    for (auto &file : rship_files) {
      auto start_tm = std::chrono::steady_clock::now();

      std::vector<std::string> fp;
      boost::split(fp, file, boost::is_any_of("/"));
      assert(fp.back().find(".csv",fp.size()-4) != std::string::npos);
      std::vector<std::string> fn;
      boost::split(fn, fp.back(), boost::is_any_of("_"));
      auto label = ":" + fn[1];

      auto num_rships = strict
      ? graph->import_typed_relationships_from_csv(file, delim, mapping)
      : graph->import_relationships_from_csv(file, delim, mapping);
      auto end_tm = std::chrono::steady_clock::now();
      auto runtime = std::chrono::duration_cast<std::chrono::milliseconds>(end_tm -
                                                                       start_tm).count();
      if (num_rships > 0) {
        spdlog::info("{} ({})-[{}]-({}) relationships imported in {} secs",
          num_rships, fn[0], label, fn[2], runtime / 1000.0);
        // graph->print_stats();
      }
    }
    #endif
  }

#ifdef CREATE_INDEX
  auto tx = graph->begin_transaction();
  graph->create_index("Person", "id");
  graph->create_index("Post", "id");
  graph->create_index("Comment", "id");
  graph->create_index("Place", "id");
  graph->create_index("Tag", "id");
  graph->create_index("Tagclass", "id");
  graph->create_index("Organisation", "id");
  graph->create_index("Forum", "id");
  graph->commit_transaction();
#endif
}

void fptree_recovery(graph_db_ptr &graph){

  std::vector<index_id> indexes;
#ifdef FPTree
  auto tx = graph->begin_transaction();
  indexes.push_back(graph->get_index("Person", "id"));
  indexes.push_back(graph->get_index("Post", "id"));
  indexes.push_back(graph->get_index("Comment", "id"));
  indexes.push_back(graph->get_index("Place", "id"));
  indexes.push_back(graph->get_index("Tag", "id"));
  indexes.push_back(graph->get_index("Tagclass", "id"));
  indexes.push_back(graph->get_index("Organisation", "id"));
  indexes.push_back(graph->get_index("Forum", "id"));
  graph->commit_transaction();
  for (auto idx_ptr : indexes)
    idx_ptr->recover();
#endif
}
