#include <iostream>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>

#include "defs.hpp"
#include "graph_db.hpp"
#include "graph_pool.hpp"
#include "ldbc.hpp"
#include "config.h"

#include "threadsafe_queue.hpp"

#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"

// ------------------------------------------------------------------------------------------------------------------------

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
                        PExpr_(2, boost::get<std::string>(pj::string_property(res, "imageFile")).empty() ?
                          pj::string_property(res, "content") : pj::string_property(res, "imageFile")),
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
                        PExpr_(2, boost::get<std::string>(pj::string_property(res, "imageFile")).empty() ?
                          pj::string_property(res, "content") : pj::string_property(res, "imageFile")),
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

// ------------------------------------------------------------------------------------------------------------------------

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

// ------------------------------------------------------------------------------------------------------------------------

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
                          PExpr_(0, !pj::has_property(res, "imageFile") ? pj::string_property(res, "content") :
                          boost::get<std::string>(pj::string_property(res, "imageFile")).empty() ?
                          pj::string_property(res, "content") : pj::string_property(res, "imageFile")) })
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
                          PExpr_(0, boost::get<std::string>(pj::string_property(res, "imageFile")).empty() ?
                          pj::string_property(res, "content") : pj::string_property(res, "imageFile")) })
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
                .append_to_qr_tuple([&](qr_tuple &v) {
                  return v[8].type() == typeid(null_val) ?
                    query_result(std::string("false")) : query_result(std::string("true")); })
                .project({PExpr_(2, pj::uint64_property(res, "id")),
                          PExpr_(2, pj::string_property(res, "content")),
                          PExpr_(2, pj::ptime_property(res, "creationDate")),
                          PExpr_(4, pj::uint64_property(res, "id")),
                          PExpr_(4, pj::string_property(res, "firstName")),
                          PExpr_(4, pj::string_property(res, "lastName")),
                          PVar_(9)})
                .orderby([&](const qr_tuple &qr1, const qr_tuple &qr2) {
                        if (boost::get<boost::posix_time::ptime>(qr1[2]) == boost::get<boost::posix_time::ptime>(qr2[2]))
                          return boost::get<uint64_t>(qr1[0]) > boost::get<uint64_t>(qr2[0]);
                        return boost::get<boost::posix_time::ptime>(qr1[2]) < boost::get<boost::posix_time::ptime>(qr2[2]); })
                .collect(rs);

  query::start({&q1, &q2});
  rs.wait();
}

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
                .append_to_qr_tuple([&](qr_tuple &v) {
                  return v[8].type() == typeid(null_val) ?
                    query_result(std::string("false")) : query_result(std::string("true")); })
                .project({PExpr_(2, pj::uint64_property(res, "id")),
                          PExpr_(2, pj::string_property(res, "content")),
                          PExpr_(2, pj::ptime_property(res, "creationDate")),
                          PExpr_(4, pj::uint64_property(res, "id")),
                          PExpr_(4, pj::string_property(res, "firstName")),
                          PExpr_(4, pj::string_property(res, "lastName")),
                          PVar_(9)})
                .orderby([&](const qr_tuple &qr1, const qr_tuple &qr2) {
                        if (boost::get<boost::posix_time::ptime>(qr1[2]) == boost::get<boost::posix_time::ptime>(qr2[2]))
                          return boost::get<uint64_t>(qr1[0]) > boost::get<uint64_t>(qr2[0]);
                        return boost::get<boost::posix_time::ptime>(qr1[2]) < boost::get<boost::posix_time::ptime>(qr2[2]); })
                .collect(rs);

  query::start({&q1, &q2});
  rs.wait();
}

// ------------------------------------------------------------------------------------------------------------------------

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
                .append_to_qr_tuple([&](qr_tuple &v) {
                  return v[8].type() == typeid(null_val) ?
                    query_result(std::string("false")) : query_result(std::string("true")); })
                .project({PExpr_(2, pj::uint64_property(res, "id")),
                          PExpr_(2, pj::string_property(res, "content")),
                          PExpr_(2, pj::ptime_property(res, "creationDate")),
                          PExpr_(4, pj::uint64_property(res, "id")),
                          PExpr_(4, pj::string_property(res, "firstName")),
                          PExpr_(4, pj::string_property(res, "lastName")),
                          PVar_(9)})
                .orderby([&](const qr_tuple &qr1, const qr_tuple &qr2) {
                        if (boost::get<boost::posix_time::ptime>(qr1[2]) == boost::get<boost::posix_time::ptime>(qr2[2]))
                          return boost::get<uint64_t>(qr1[0]) > boost::get<uint64_t>(qr2[0]);
                        return boost::get<boost::posix_time::ptime>(qr1[2]) < boost::get<boost::posix_time::ptime>(qr2[2]); })
                .collect(rs);

  query::start({&q1, &q2});
  rs.wait();
}