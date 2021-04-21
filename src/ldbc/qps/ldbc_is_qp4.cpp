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

void ldbc_is_qp4_query_2_c(graph_db_ptr &gdb, result_set &rs, uint64_t personId) {
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
               .from_node("Comment")
               .from_relationships({1, maxHops}, ":replyOf")
               .to_node("Post");

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

  query::start({&q1, &q2});
  rs.wait();
}

// -------------------------------------------------------------------------------------------------------------------------

void ldbc_is_qp4_query_7_p(graph_db_ptr &gdb, result_set &rs, uint64_t messageId) {
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
                .append_to_qr_tuple([&](const qr_tuple &v) {
                  return v[7].type() == typeid(null_val) ?
                    query_result(std::string("false")) : query_result(std::string("true")); })
                .project({PExpr_(4, pj::uint64_property(res, "id")),
                          PExpr_(4, pj::string_property(res, "content")),
                          PExpr_(4, pj::ptime_property(res, "creationDate")),
                          PExpr_(6, pj::uint64_property(res, "id")),
                          PExpr_(6, pj::string_property(res, "firstName")),
                          PExpr_(6, pj::string_property(res, "lastName")),
                          PVar_(7) })
                .orderby([&](const qr_tuple &qr1, const qr_tuple &qr2) {
                        if (boost::get<boost::posix_time::ptime>(qr1[2]) == boost::get<boost::posix_time::ptime>(qr2[2]))
                          return boost::get<uint64_t>(qr1[0]) > boost::get<uint64_t>(qr2[0]);
                        return boost::get<boost::posix_time::ptime>(qr1[2]) < boost::get<boost::posix_time::ptime>(qr2[2]); })
                .collect(rs);

  q.start();
  rs.wait();
}

// ------------------------------------------------------------------------------------------------------------------------

void ldbc_is_qp4_query_7_c(graph_db_ptr &gdb, result_set &rs, uint64_t messageId) {
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
                .append_to_qr_tuple([&](const qr_tuple &v) {
                  return v[7].type() == typeid(null_val) ?
                    query_result(std::string("false")) : query_result(std::string("true")); })
                .project({PExpr_(4, pj::uint64_property(res, "id")),
                          PExpr_(4, pj::string_property(res, "content")),
                          PExpr_(4, pj::ptime_property(res, "creationDate")),
                          PExpr_(6, pj::uint64_property(res, "id")),
                          PExpr_(6, pj::string_property(res, "firstName")),
                          PExpr_(6, pj::string_property(res, "lastName")),
                          PVar_(7) })
                .orderby([&](const qr_tuple &qr1, const qr_tuple &qr2) {
                        if (boost::get<boost::posix_time::ptime>(qr1[2]) == boost::get<boost::posix_time::ptime>(qr2[2]))
                          return boost::get<uint64_t>(qr1[0]) > boost::get<uint64_t>(qr2[0]);
                        return boost::get<boost::posix_time::ptime>(qr1[2]) < boost::get<boost::posix_time::ptime>(qr2[2]); })
                .collect(rs);

  q.start();
  rs.wait();
}