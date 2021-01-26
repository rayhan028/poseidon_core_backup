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

// -------------------------------------------------------------------------------------------------------------------------

void ldbc_is_qp2_query_1(graph_db_ptr &gdb, result_set &rs, uint64_t personId) {
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

// ------------------------------------------------------------------------------------------------------------------------

void ldbc_is_qp2_query_2_p(graph_db_ptr &gdb, result_set &rs, uint64_t personId) {
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

// ------------------------------------------------------------------------------------------------------------------------

void ldbc_is_qp2_query_2_c(graph_db_ptr &gdb, result_set &rs, uint64_t personId) {
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

// ------------------------------------------------------------------------------------------------------------------------

void ldbc_is_qp2_query_3(graph_db_ptr &gdb, result_set &rs, uint64_t personId) {

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

// ------------------------------------------------------------------------------------------------------------------------

void ldbc_is_qp2_query_7_p(graph_db_ptr &gdb, result_set &rs, uint64_t messageId) {

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

  query::start({&q1, &q2});
  rs.wait();
}

// ------------------------------------------------------------------------------------------------------------------------

void ldbc_is_qp2_query_7_c(graph_db_ptr &gdb, result_set &rs, uint64_t messageId) {

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

  query::start({&q1, &q2});
  rs.wait();
}