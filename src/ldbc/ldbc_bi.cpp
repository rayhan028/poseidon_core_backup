#include <iostream>
#include <regex>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>

#include "defs.hpp"
#include "graph_db.hpp"
#include "graph_pool.hpp"
#include "ldbc.hpp"
#include "config.h"
#include "qop.hpp"
#include "query.hpp"
#include "shortest_path.hpp"

#include "threadsafe_queue.hpp"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"

namespace pj = builtin;
using namespace boost::posix_time;

static std::vector<std::string> message = {"Post", "Comment"};
#define RUN_PARALLEL
/* ------------------------------------------------------------------------ */

void ldbc_bi_query_1(graph_db_ptr &gdb, result_set &rs, params_tuple &params) {

    auto q = query(gdb)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label(message)
              .property( "creationDate", [&](auto &prop) {
                return (*(reinterpret_cast<const ptime *>(prop.value_))) <
                    boost::get<ptime>(params[0]); })
#else
              .nodes_where(message, "creationDate", [&](auto &prop) {
                return (*(reinterpret_cast<const ptime *>(prop.value_))) <
                    boost::get<ptime>(params[0]); })
#endif
              .project({PExpr_(0, pj::pr_year(res, "creationDate")),
                        PExpr_(0, (pj::has_label(res, "Comment") ?
                                    std::string("True") : std::string("False"))),
                        PExpr_(0, pj::int_property(res, "length")),
                        projection::expr(0, [&](auto res) {
                          auto len = boost::get<int>(pj::int_property(res, "length"));
                          return (len >= 0 && len < 40) ? query_result(std::string("0")) :
                                  (len >= 40 && len < 80) ? query_result(std::string("1")) :
                                  (len >= 80 && len < 160) ? query_result(std::string("2")) :
                                  query_result(std::string("3")); }) })
              .groupby({0, 1, 3}, {{"count", 0}, {"avg", 2}, {"sum", 2}, {"pcount", 0}})
              .orderby([&](const qr_tuple &qr1, const qr_tuple &qr2) {
                if (boost::get<int>(qr1[0]) == boost::get<int>(qr2[0])) {
                  if (boost::get<std::string>(qr1[1]) == boost::get<std::string>(qr2[1]))
                    return boost::get<std::string>(qr1[2]) < boost::get<std::string>(qr2[2]);
                  else
                    return boost::get<std::string>(qr1[1]) < boost::get<std::string>(qr2[1]);
                }
                return boost::get<int>(qr1[0]) > boost::get<int>(qr2[0]); })
              .collect(rs);
    q.start();
    rs.wait();
}

void recover_ldbc_bi_query_1(graph_db_ptr &gdb, result_set &rs, std::map<std::size_t, std::size_t> &cp, 
std::map<std::size_t, std::vector<std::size_t>> &range_map,
std::list<qr_tuple> rec, params_tuple &params) {
    auto q = query(gdb)
#ifdef RUN_PARALLEL
              .all_nodes(range_map)
              .has_label(message)
              .property( "creationDate", [&](auto &prop) {
                return (*(reinterpret_cast<const ptime *>(prop.value_))) <
                    boost::get<ptime>(params[0]); })
#else
              .nodes_where(message, "creationDate", [&](auto &prop) {
                return (*(reinterpret_cast<const ptime *>(prop.value_))) <
                    boost::get<ptime>(params[0]); })
#endif
              .project({PExpr_(0, pj::pr_year(res, "creationDate")),
                        PExpr_(0, (pj::has_label(res, "Comment") ?
                                    std::string("True") : std::string("False"))),
                        PExpr_(0, pj::int_property(res, "length")),
                        projection::expr(0, [&](auto res) {
                          auto len = boost::get<int>(pj::int_property(res, "length"));
                          return (len >= 0 && len < 40) ? query_result(std::string("0")) :
                                  (len >= 40 && len < 80) ? query_result(std::string("1")) :
                                  (len >= 80 && len < 160) ? query_result(std::string("2")) :
                                  query_result(std::string("3")); }) })
              .groupby(rec, {0, 1, 3}, {{"count", 0}, {"avg", 2}, {"sum", 2}, {"pcount", 0}})
              .orderby([&](const qr_tuple &qr1, const qr_tuple &qr2) {
                if (boost::get<int>(qr1[0]) == boost::get<int>(qr2[0])) {
                  if (boost::get<std::string>(qr1[1]) == boost::get<std::string>(qr2[1]))
                    return boost::get<std::string>(qr1[2]) < boost::get<std::string>(qr2[2]);
                  else
                    return boost::get<std::string>(qr1[1]) < boost::get<std::string>(qr2[1]);
                }
                return boost::get<int>(qr1[0]) > boost::get<int>(qr2[0]); })
              .collect(rs);
    q.start();
    rs.wait();
}

void ldbc_bi_query_2(graph_db_ptr &gdb, result_set &rs, params_tuple &params) {

    auto q1 = query(gdb)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label(message)
              .property( "creationDate", [&](auto &p) {
                auto d = *(reinterpret_cast<const ptime *>(p.value_));
                auto dt1 = boost::get<ptime>(params[0]);
                time_period duration1(dt1, hours(24*100));
                auto dt2 = duration1.last();
                time_period duration2(dt2, hours(24*100));
                return duration2.contains(d) ? true : false; })
#else
              .nodes_where(message, "creationDate", [&](auto &p) {
                auto d = *(reinterpret_cast<const ptime *>(p.value_));
                auto dt1 = boost::get<ptime>(params[0]);
                time_period duration1(dt1, hours(24*100));
                auto dt2 = duration1.last();
                time_period duration2(dt2, hours(24*100));
                return duration2.contains(d) ? true : false; })
#endif
              .from_relationships(":hasTag", 0)
              .to_node("Tag")
              .groupby({2}, {{"count", 0}});

    auto q2 = query(gdb)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label(message)
              .property( "creationDate", [&](auto &p) {
                auto d = *(reinterpret_cast<const ptime *>(p.value_));
                auto dt = boost::get<ptime>(params[0]);
                time_period duration(dt, hours(24*100));
                return duration.contains(d) ? true : false; })
#else
              .nodes_where(message, "creationDate", [&](auto &p) {
                auto d = *(reinterpret_cast<const ptime *>(p.value_));
                auto dt = boost::get<ptime>(params[0]);
                time_period duration(dt, hours(24*100));
                return duration.contains(d) ? true : false; })
#endif
              .from_relationships(":hasTag", 0)
              .to_node("Tag")
              .groupby({2}, {{"count", 0}})
              .hashjoin_on_node({0, 0}, q1)
              .project({PExpr_(0, pj::string_property(res, "name")),
                        PVar_(1),
                        PVar_(3) })
              .append_to_qr_tuple([&](const qr_tuple &v) {
                auto cnt = boost::get<uint64_t>(v[1]);
                auto nxt_cnt = boost::get<uint64_t>(v[2]);
                uint64_t diff = cnt > nxt_cnt ? cnt - nxt_cnt : nxt_cnt - cnt;
                return query_result(diff); })
              .orderby([&](const qr_tuple &q1, const qr_tuple &q2) {
                if (boost::get<uint64_t>(q1[3]) == boost::get<uint64_t>(q2[3]))
                  return boost::get<std::string>(q1[0]) < boost::get<std::string>(q2[0]);
                return boost::get<uint64_t>(q1[3]) > boost::get<uint64_t>(q2[3]); })
              .limit(100)
              .collect(rs);

    query::start({&q1, &q2});
    rs.wait();
}

void ldbc_bi_query_3(graph_db_ptr &gdb, result_set &rs, params_tuple &params) {

    auto q1 = query(gdb)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Place")
              .property("name", [&](auto &prop) {
                auto c1 = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto c2 = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[1]));
                return c1 == c2; })
#else
              .nodes_where("Place", "name", [&](auto &prop) {
                auto c1 = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto c2 = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[1]));
                return c1 == c2; })
#endif
              .to_relationships(":isPartOf")
              .from_node("Place")
              .to_relationships(":isLocatedIn")
              .from_node("Person")
              .to_relationships(":hasModerator")
              .from_node("Forum")
              .from_relationships(":containerOf")
              .to_node("Post")
              .from_relationships(":hasTag")
              .to_node("Tag")
              .from_relationships(":hasType")
              .to_node("TagClass")
              .property("name", [&](auto &prop) {
                auto c1 = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto c2 = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0]));
                return c1 == c2; })
              .groupby({4, 6, 8});

    auto q2 = query(gdb)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Place")
              .property("name", [&](auto &prop) {
                auto c1 = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto c2 = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[1]));
                return c1 == c2; })
#else
              .nodes_where("Place", "name", [&](auto &prop) {
                auto c1 = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto c2 = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[1]));
                return c1 == c2; })
#endif
              .to_relationships(":isPartOf")
              .from_node("Place")
              .to_relationships(":isLocatedIn")
              .from_node("Person")
              .to_relationships(":hasModerator")
              .from_node("Forum")
              .from_relationships(":containerOf")
              .to_node("Post")
              .to_relationships({1, 100}, ":replyOf")
              .from_node("Comment")
              .from_relationships(":hasTag")
              .to_node("Tag")
              .from_relationships(":hasType")
              .to_node("TagClass")
              .property("name", [&](auto &prop) {
                auto c1 = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto c2 = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0]));
                return c1 == c2; })
              .groupby({4, 6, 10})
              .union_all({&q1})
              .groupby({0, 1}, {{"count", 0}})
              .project({PExpr_(1, pj::uint64_property(res, "id")),
                        PExpr_(1, pj::string_property(res, "title")),
                        PExpr_(1, pj::ptime_property(res, "creationDate")),
                        PExpr_(0, pj::uint64_property(res, "id")),
                        PVar_(2) })
              .orderby([&](const qr_tuple &q1, const qr_tuple &q2) {
                if (boost::get<uint64_t>(q1[4]) == boost::get<uint64_t>(q2[4]))
                  return boost::get<uint64_t>(q1[0]) < boost::get<uint64_t>(q2[0]);
                return boost::get<uint64_t>(q1[4]) > boost::get<uint64_t>(q2[4]); })
              .limit(20)
              .collect(rs);

    query::start({&q1, &q2});
    rs.wait();
}

void ldbc_bi_query_4(graph_db_ptr &gdb, result_set &rs, params_tuple &params) {

        auto q1 = query(gdb)
  #ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Place")
              .property("name", [&](auto &prop) {
                auto c1 = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto c2 = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0]));
                return c1 == c2; })
  #else
              .nodes_where("Place", "name", [&](auto &prop) {
                auto c1 = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto c2 = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0]));
                return c1 == c2; })
  #endif
              .to_relationships(":isPartOf")
              .from_node("Place")
              .to_relationships(":isLocatedIn")
              .from_node("Person")
              .to_relationships(":hasMember")
              .from_node("Forum")
              .project({PVar_(6),
                        PExpr_(6, pj::uint64_property(res, "id")),
                        PExpr_(6, pj::ptime_property(res, "creationDate")) })
              .where_qr_tuple([&](auto &v) {
                return boost::get<ptime>(v[2]) > boost::get<ptime>(params[1]);})
              .groupby({0, 1}, {{"count", 0}})
              .orderby([&](const qr_tuple &q1, const qr_tuple &q2) {
                if (boost::get<uint64_t>(q1[2]) == boost::get<uint64_t>(q2[2]))
                  return boost::get<uint64_t>(q1[1]) < boost::get<uint64_t>(q2[1]);
                return boost::get<uint64_t>(q1[2]) > boost::get<uint64_t>(q2[2]); })
              .limit(100);

        auto q2 = query(gdb)
  #ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Place")
              .property("name", [&](auto &prop) {
                auto c1 = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto c2 = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0]));
                return c1 == c2; })
  #else
              .nodes_where("Place", "name", [&](auto &prop) {
                auto c1 = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto c2 = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0]));
                return c1 == c2; })
  #endif
              .to_relationships(":isPartOf")
              .from_node("Place")
              .to_relationships(":isLocatedIn")
              .from_node("Person")
              .to_relationships(":hasMember")
              .from_node("Forum")
              .project({PVar_(6),
                        PExpr_(6, pj::uint64_property(res, "id")),
                        PExpr_(6, pj::ptime_property(res, "creationDate")) })
              .where_qr_tuple([&](auto &v) {
                return boost::get<ptime>(v[2]) > boost::get<ptime>(params[1]);})
              .groupby({0, 1}, {{"count", 0}})
              .orderby([&](const qr_tuple &q1, const qr_tuple &q2) {
                if (boost::get<uint64_t>(q1[2]) == boost::get<uint64_t>(q2[2]))
                  return boost::get<uint64_t>(q1[1]) < boost::get<uint64_t>(q2[1]);
                return boost::get<uint64_t>(q1[2]) > boost::get<uint64_t>(q2[2]); })
              .limit(100)
              .from_relationships(":hasMember", 0)
              .to_node("Person")
              .to_relationships(":hasCreator")
              .from_node("Post")
              .to_relationships(":containerOf")
              .from_node("Forum")
              .project({PVar_(4),
                        PExpr_(6, pj::ptime_property(res, "creationDate")),
                        PVar_(8) })
              .where_qr_tuple([&](auto &v) {
                return boost::get<ptime>(v[1]) > boost::get<ptime>(params[1]); })
              .hashjoin_on_node({2, 0}, q1);

       auto q3 = query(gdb)
  #ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Place")
              .property("name", [&](auto &prop) {
                auto c1 = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto c2 = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0]));
                return c1 == c2; })
  #else
              .nodes_where("Place", "name", [&](auto &prop) {
                auto c1 = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto c2 = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0]));
                return c1 == c2; })
  #endif
              .to_relationships(":isPartOf")
              .from_node("Place")
              .to_relationships(":isLocatedIn")
              .from_node("Person")
              .to_relationships(":hasMember")
              .from_node("Forum")
              .project({PVar_(6),
                        PExpr_(6, pj::uint64_property(res, "id")),
                        PExpr_(6, pj::ptime_property(res, "creationDate")) })
              .where_qr_tuple([&](auto &v) {
                return boost::get<ptime>(v[2]) > boost::get<ptime>(params[1]);})
              .groupby({0, 1}, {{"count", 0}})
              .orderby([&](const qr_tuple &q1, const qr_tuple &q2) {
                if (boost::get<uint64_t>(q1[2]) == boost::get<uint64_t>(q2[2]))
                  return boost::get<uint64_t>(q1[1]) < boost::get<uint64_t>(q2[1]);
                return boost::get<uint64_t>(q1[2]) > boost::get<uint64_t>(q2[2]); })
              .limit(100);

        auto q4 = query(gdb)
  #ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Place")
              .property("name", [&](auto &prop) {
                auto c1 = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto c2 = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0]));
                return c1 == c2; })
  #else
              .nodes_where("Place", "name", [&](auto &prop) {
                auto c1 = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto c2 = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0]));
                return c1 == c2; })
  #endif
              .to_relationships(":isPartOf")
              .from_node("Place")
              .to_relationships(":isLocatedIn")
              .from_node("Person")
              .to_relationships(":hasMember")
              .from_node("Forum")
              .project({PVar_(6),
                        PExpr_(6, pj::uint64_property(res, "id")),
                        PExpr_(6, pj::ptime_property(res, "creationDate")) })
              .where_qr_tuple([&](auto &v) {
                return boost::get<ptime>(v[2]) > boost::get<ptime>(params[1]);})
              .groupby({0, 1}, {{"count", 0}})
              .orderby([&](const qr_tuple &q1, const qr_tuple &q2) {
                if (boost::get<uint64_t>(q1[2]) == boost::get<uint64_t>(q2[2]))
                  return boost::get<uint64_t>(q1[1]) < boost::get<uint64_t>(q2[1]);
                return boost::get<uint64_t>(q1[2]) > boost::get<uint64_t>(q2[2]); })
              .limit(100)
              .from_relationships(":hasMember", 0)
              .to_node("Person")
              .to_relationships(":hasCreator")
              .from_node("Comment")
              .from_relationships({1, 100}, ":replyOf")
              .to_node("Post")
              .to_relationships(":containerOf")
              .from_node("Forum")
              .project({PVar_(4),
                        PExpr_(6, pj::ptime_property(res, "creationDate")),
                        PVar_(10) })
              .where_qr_tuple([&](auto &v) {
                return boost::get<ptime>(v[1]) > boost::get<ptime>(params[1]); })
              .hashjoin_on_node({2, 0}, q3)
              .union_all({&q2})
              .groupby({0}, {{"count", 0}})
              .project({PExpr_(0, pj::uint64_property(res, "id")),
                        PExpr_(0, pj::string_property(res, "firstName")),
                        PExpr_(0, pj::string_property(res, "lastName")),
                        PExpr_(0, pj::ptime_property(res, "creationDate")),
                        PVar_(1) })
              .orderby([&](const qr_tuple &q1, const qr_tuple &q2) {
                if (boost::get<uint64_t>(q1[4]) == boost::get<uint64_t>(q2[4]))
                  return boost::get<uint64_t>(q1[0]) < boost::get<uint64_t>(q2[0]);
                return boost::get<uint64_t>(q1[4]) > boost::get<uint64_t>(q2[4]); })
              .limit(100)
              .collect(rs);

    query::start({&q1, &q2, &q3, &q4});
    rs.wait();
}

void ldbc_bi_query_5(graph_db_ptr &gdb, result_set &rs, params_tuple &params) {

    auto q1 = query(gdb)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Tag")
              .property("name", [&](auto &prop) {
                auto c1 = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto c2 = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0]));
                return c1 == c2; })
#else
              .nodes_where("Tag", "name", [&](auto &prop) {
                auto c1 = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto c2 = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0]));
                return c1 == c2; })
#endif
              .to_relationships(":hasTag")
              .from_node(message)
              .from_relationships(":hasCreator")
              .to_node("Person")
              .groupby({4}, {{"count", 0}});

    auto q2 = query(gdb)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Tag")
              .property("name", [&](auto &prop) {
                auto c1 = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto c2 = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0]));
                return c1 == c2; })
#else
              .nodes_where("Tag", "name", [&](auto &prop) {
                auto c1 = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto c2 = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0]));
                return c1 == c2; })
#endif
              .to_relationships(":hasTag")
              .from_node(message)
              .from_relationships(":hasCreator")
              .to_node("Person")
              .to_relationships(":likes", 2)
              .from_node("Person")
              .groupby({4}, {{"count", 0}})
              .join_on_node({0, 0}, q1);

    auto q3 = query(gdb)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Tag")
              .property("name", [&](auto &prop) {
                auto c1 = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto c2 = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0]));
                return c1 == c2; })
#else
              .nodes_where("Tag", "name", [&](auto &prop) {
                auto c1 = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto c2 = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0]));
                return c1 == c2; })
#endif
              .to_relationships(":hasTag")
              .from_node(message)
              .from_relationships(":hasCreator")
              .to_node("Person")
              .to_relationships(":replyOf", 2)
              .from_node("Comment")
              .groupby({4}, {{"count", 0}})
              .join_on_node({0, 0}, q2)
              .append_to_qr_tuple([&](const qr_tuple &v) {
                auto reply_cnt = boost::get<uint64_t>(v[1]);
                auto like_cnt = boost::get<uint64_t>(v[3]);
                auto msg_cnt = boost::get<uint64_t>(v[5 ]);
                auto score = msg_cnt + 2 * reply_cnt + 10 * like_cnt;
                return query_result(score); })
              .project({PExpr_(0, pj::uint64_property(res, "id")),
                        PVar_(1),
                        PVar_(3),
                        PVar_(5),
                        PVar_(6) })
              .orderby([&](const qr_tuple &q1, const qr_tuple &q2) {
                if (boost::get<uint64_t>(q1[4]) == boost::get<uint64_t>(q2[4]))
                  return boost::get<uint64_t>(q1[0]) < boost::get<uint64_t>(q2[0]);
                return boost::get<uint64_t>(q1[4]) > boost::get<uint64_t>(q2[4]); })
              .limit(100)
              .collect(rs);

    query::start({&q1, &q2, &q3});
    rs.wait();
}

void ldbc_bi_query_6(graph_db_ptr &gdb, result_set &rs, params_tuple &params) {

    auto q = query(gdb)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Tag")
              .property("name", [&](auto &prop) {
                auto c1 = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto c2 = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0]));
                return c1 == c2; })
#else
              .nodes_where("Tag", "name", [&](auto &prop) {
                auto c1 = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto c2 = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0]));
                return c1 == c2; })
#endif
              .to_relationships(":hasTag")
              .from_node(message)
              .from_relationships(":hasCreator")
              .to_node("Person")
              .to_relationships(":likes", 2)
              .from_node("Person")
              .groupby({4, 6})
              .to_relationships(":hasCreator")
              .from_node(message)
              .to_relationships(":likes")
              .from_node("Person")
              .groupby({0, 1}, {{"count", 0}})
              .groupby({0}, {{"sum", 2}})
              .project({PExpr_(0, pj::uint64_property(res, "id")),
                        PVar_(1) })
              .orderby([&](const qr_tuple &q1, const qr_tuple &q2) {
                if (boost::get<uint64_t>(q1[1]) == boost::get<uint64_t>(q2[1]))
                  return boost::get<uint64_t>(q1[0]) < boost::get<uint64_t>(q2[0]);
                return boost::get<uint64_t>(q1[1]) > boost::get<uint64_t>(q2[1]); })
              .limit(100)
              .collect(rs);

    q.start();
    rs.wait();
}

void ldbc_bi_query_7(graph_db_ptr &gdb, result_set &rs, params_tuple &params) {

    auto q = query(gdb)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Tag")
              .property("name", [&](auto &prop) {
                auto c1 = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto c2 = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0]));
                return c1 == c2; })
#else
              .nodes_where("Tag", "name", [&](auto &prop) {
                auto c1 = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto c2 = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0]));
                return c1 == c2; })
#endif
              .to_relationships(":hasTag")
              .from_node(message)
              .to_relationships(":replyOf")
              .from_node("Comment")
              .from_relationships(":hasTag")
              .to_node("Tag")
              .property("name", [&](auto &prop) {
                auto c1 = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto c2 = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0]));
                return c1 != c2; })
              .groupby({6}, {{"count", 0}})
              .project({PExpr_(0, pj::string_property(res, "name")),
                        PVar_(1) })
              .orderby([&](const qr_tuple &q1, const qr_tuple &q2) {
                if (boost::get<uint64_t>(q1[1]) == boost::get<uint64_t>(q2[1]))
                  return boost::get<std::string>(q1[0]) < boost::get<std::string>(q2[0]);
                return boost::get<uint64_t>(q1[1]) > boost::get<uint64_t>(q2[1]); })
              .collect(rs);

    q.start();
    rs.wait();
}

void ldbc_bi_query_8(graph_db_ptr &gdb, result_set &rs, params_tuple &params) {

    auto q1 = query(gdb)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Tag")
              .property("name", [&](auto &prop) {
                auto c1 = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto c2 = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0]));
                return c1 == c2; })
#else
              .nodes_where("Tag", "name", [&](auto &prop) {
                auto c1 = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto c2 = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0]));
                return c1 == c2; })
#endif
              .to_relationships(":hasTag")
              .from_node(message)
              .property("creationDate", [&](auto &prop) {
                auto date = (*(reinterpret_cast<const ptime *>(prop.value_)));
                return date > boost::get<ptime>(params[1]); })
              .from_relationships(":hasCreator")
              .to_node("Person")
              .groupby({4}, {{"count", 0}});

    auto q2 = query(gdb)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Tag")
              .property("name", [&](auto &prop) {
                auto c1 = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto c2 = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0]));
                return c1 == c2; })
#else
              .nodes_where("Tag", "name", [&](auto &prop) {
                auto c1 = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto c2 = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0]));
                return c1 == c2; })
#endif
              .to_relationships(":hasInterest")
              .from_node("Person")
              .project({PVar_(2)})
              .append_to_qr_tuple([&](const qr_tuple &v) {
                uint64_t intrst = 100;
                return query_result(intrst); })
              .union_all({&q1})
              .groupby({0}, {{"sum", 1}});

    auto q3 = query(gdb)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Tag")
              .property("name", [&](auto &prop) {
                auto c1 = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto c2 = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0]));
                return c1 == c2; })
#else
              .nodes_where("Tag", "name", [&](auto &prop) {
                auto c1 = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto c2 = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0]));
                return c1 == c2; })
#endif
              .to_relationships(":hasTag")
              .from_node(message)
              .property("creationDate", [&](auto &prop) {
                auto date = (*(reinterpret_cast<const ptime *>(prop.value_)));
                return date > boost::get<ptime>(params[1]); })
              .from_relationships(":hasCreator")
              .to_node("Person")
              .groupby({4}, {{"count", 0}});

    auto q4 = query(gdb)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Tag")
              .property("name", [&](auto &prop) {
                auto c1 = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto c2 = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0]));
                return c1 == c2; })
#else
              .nodes_where("Tag", "name", [&](auto &prop) {
                auto c1 = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto c2 = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0]));
                return c1 == c2; })
#endif
              .to_relationships(":hasInterest")
              .from_node("Person")
              .project({PVar_(2)})
              .append_to_qr_tuple([&](const qr_tuple &v) {
                uint64_t intrst = 100;
                return query_result(intrst); })
              .union_all({&q3})
              .groupby({0}, {{"sum", 1}})
              .all_relationships(":knows", 0)
              .hashjoin_on_node({3, 0}, q2)
              .groupby({0, 1}, {{"sum", 5}})
              .project({PExpr_(0, pj::uint64_property(res, "id")),
                        PVar_(1),
                        PVar_(2) })
              .orderby([&](const qr_tuple &q1, const qr_tuple &q2) {
                uint64_t s1 = boost::get<uint64_t>(q1[1]) + boost::get<uint64_t>(q1[2]);
                uint64_t s2 = boost::get<uint64_t>(q2[1]) + boost::get<uint64_t>(q2[2]);
                if (s1 == s2)
                  return boost::get<uint64_t>(q1[0]) < boost::get<uint64_t>(q2[0]);
                return s1 > s2; })
              .limit(100)
              .collect(rs);

    query::start({&q1, &q2, &q3, &q4});
    rs.wait();
}

void ldbc_bi_query_9(graph_db_ptr &gdb, result_set &rs, params_tuple &params) {

  auto q = query(gdb)
#ifdef RUN_PARALLEL
              .all_nodes()
              
              .has_label("Person")
#else
              .all_nodes("Person")
#endif
              .to_relationships(":hasCreator")
              .from_node("Post")
              .project({PVar_(0),
                        PVar_(2),
                        PExpr_(2, pj::ptime_property(res, "creationDate")) })
              .where_qr_tuple([&](const qr_tuple &v) {
                return boost::get<ptime>(v[2]) >= boost::get<ptime>(params[0]) &&
                        boost::get<ptime>(v[2]) <= boost::get<ptime>(params[1]); })
              .to_relationships({1, 100}, ":replyOf", 1)
              .from_node("Comment")
              .project({PVar_(0),
                        PVar_(1),
                        PVar_(4),
                        PExpr_(4, pj::ptime_property(res, "creationDate")) })
              .where_qr_tuple([&](const qr_tuple &v) {
                return boost::get<ptime>(v[3]) >= boost::get<ptime>(params[0]) &&
                        boost::get<ptime>(v[3]) <= boost::get<ptime>(params[1]); })
              .groupby({0, 1}, {{"count", 0}})
              .append_to_qr_tuple([&](const qr_tuple &v) {
                auto msg_cnt = boost::get<uint64_t>(v[2]);
                msg_cnt++;
                return query_result(msg_cnt); })
              .groupby({0}, {{"count", 0}, {"sum", 3}})
              .project({PExpr_(0, pj::uint64_property(res, "id")),
                        PExpr_(0, pj::string_property(res, "firstName")),
                        PExpr_(0, pj::string_property(res, "lastName")),
                        PVar_(1),
                        PVar_(2) })
              .orderby([&](const qr_tuple &q1, const qr_tuple &q2) {
                if (boost::get<uint64_t>(q1[4]) == boost::get<uint64_t>(q2[4]))
                  return boost::get<uint64_t>(q1[0]) < boost::get<uint64_t>(q2[0]);
                return boost::get<uint64_t>(q1[4]) > boost::get<uint64_t>(q2[4]); })
              .limit(100)
              .collect(rs);

  q.start();
  rs.wait();
}

void recover_ldbc_bi_query_9(graph_db_ptr &gdb, result_set &rs, std::map<std::size_t, std::size_t> &cp, 
    std::map<std::size_t, std::vector<std::size_t>> &range_map, 
    std::list<qr_tuple> rec, params_tuple &params) {

  auto q = query(gdb)
#ifdef RUN_PARALLEL
              .all_nodes(range_map)
              .has_label("Person")
#else
              .all_nodes("Person")
#endif
              .to_relationships(":hasCreator")
              .from_node("Post")
              .project({PVar_(0),
                        PVar_(2),
                        PExpr_(2, pj::ptime_property(res, "creationDate")) })
              .where_qr_tuple([&](const qr_tuple &v) {
                return boost::get<ptime>(v[2]) >= boost::get<ptime>(params[0]) &&
                        boost::get<ptime>(v[2]) <= boost::get<ptime>(params[1]); })
              .to_relationships({1, 100}, ":replyOf", 1)
              .from_node("Comment")
              .project({PVar_(0),
                        PVar_(1),
                        PVar_(4),
                        PExpr_(4, pj::ptime_property(res, "creationDate")) })
              .where_qr_tuple([&](const qr_tuple &v) {
                return boost::get<ptime>(v[3]) >= boost::get<ptime>(params[0]) &&
                        boost::get<ptime>(v[3]) <= boost::get<ptime>(params[1]); })
              .groupby(rec, {0, 1}, {{"count", 0}})
              .append_to_qr_tuple([&](qr_tuple &v) {
                auto msg_cnt = boost::get<uint64_t>(v[2]);
                msg_cnt++;
                return query_result(msg_cnt); })
              .groupby({0}, {{"count", 0}, {"sum", 3}})
              .project({PExpr_(0, pj::uint64_property(res, "id")),
                        PExpr_(0, pj::string_property(res, "firstName")),
                        PExpr_(0, pj::string_property(res, "lastName")),
                        PVar_(1),
                        PVar_(2) })
              .orderby([&](const qr_tuple &q1, const qr_tuple &q2) {
                if (boost::get<uint64_t>(q1[4]) == boost::get<uint64_t>(q2[4]))
                  return boost::get<uint64_t>(q1[0]) < boost::get<uint64_t>(q2[0]);
                return boost::get<uint64_t>(q1[4]) > boost::get<uint64_t>(q2[4]); })
              .limit(100)
              .collect(rs);

  q.start();
  rs.wait();
}


void ldbc_bi_query_10(graph_db_ptr &gdb, result_set &rs, params_tuple &params) {

  auto q = query(gdb)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Person")
              .property("id", [&](auto &p) {
                return p.equal(boost::get<uint64_t>(params[0])); })
#else
              .nodes_where("Person", "id", [&](auto &p) {
                return p.equal(boost::get<uint64_t>(params[0])); })
#endif
              .all_relationships({boost::get<int>(params[3]), boost::get<int>(params[4])}, ":knows")
              .groupby({2})
              .from_relationships(":isLocatedIn")
              .to_node("Place")
              .from_relationships(":isPartOf")
              .to_node("Place")
              .property("name", [&](auto &prop) {
                return *(reinterpret_cast<const dcode_t *>(prop.value_)) ==
                        gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[1])); })
              .project({PVar_(0)})
              .to_relationships(":hasCreator")
              .from_node(message)
              .from_relationships(":hasTag")
              .to_node("Tag")
              .from_relationships(":hasType")
              .to_node("TagClass")
              .property("name", [&](auto &prop) {
                return *(reinterpret_cast<const dcode_t *>(prop.value_)) ==
                        gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[2])); })
              .project({PVar_(0),
                        PVar_(2) })
              .from_relationships(":hasTag")
              .to_node("Tag")
              .groupby({0, 3}, {{"count", 0}})
              .project({PExpr_(0, pj::uint64_property(res, "id")),
                        PExpr_(1, pj::string_property(res, "name")),
                        PVar_(2) })
              .orderby([&](const qr_tuple &q1, const qr_tuple &q2) {
                if (boost::get<uint64_t>(q1[2]) == boost::get<uint64_t>(q2[2])) {
                  if (boost::get<std::string>(q1[1]) == boost::get<std::string>(q2[1]))
                    return boost::get<uint64_t>(q1[0]) < boost::get<uint64_t>(q2[0]);
                  return boost::get<std::string>(q1[1]) < boost::get<std::string>(q2[1]);
                }
                return boost::get<uint64_t>(q1[2]) > boost::get<uint64_t>(q2[2]); })
              .limit(100)
              .collect(rs);

  q.start();
  rs.wait();
}

void ldbc_bi_query_11(graph_db_ptr &gdb, result_set &rs, params_tuple &params) {

  auto q = query(gdb)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Place")
              .property("name", [&](auto &prop) {
                return *(reinterpret_cast<const dcode_t *>(prop.value_)) ==
                        gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0])); })
#else
              .nodes_where("Place", "name", [&](auto &prop) {
                return *(reinterpret_cast<const dcode_t *>(prop.value_)) ==
                        gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0])); })
#endif
              .to_relationships(":isPartOf")
              .from_node("Place")
              .to_relationships(":isLocatedIn")
              .from_node("Person")
              .all_relationships(":knows")
              .project({PVar_(4),
                        PExpr_(5, pj::ptime_property(res, "creationDate")),
                        PVar_(6) })
              .where_qr_tuple([&](auto &v) {
                return boost::get<ptime>(v[1]) > boost::get<ptime>(params[1]); })
              .from_relationships(":isLocatedIn")
              .to_node("Place")
              .from_relationships(":isPartOf")
              .to_node("Place")
              .property("name", [&](auto &prop) {
                return *(reinterpret_cast<const dcode_t *>(prop.value_)) ==
                        gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0])); })
              .all_relationships(":knows", 2)
              .project({PVar_(0),
                        PVar_(2),
                        PExpr_(7, pj::ptime_property(res, "creationDate")),
                        PVar_(8) })
              .where_qr_tuple([&](auto &v) {
                return boost::get<ptime>(v[2]) > boost::get<ptime>(params[1]); })
              .from_relationships(":isLocatedIn")
              .to_node("Place")
              .from_relationships(":isPartOf")
              .to_node("Place")
              .property("name", [&](auto &prop) {
                return *(reinterpret_cast<const dcode_t *>(prop.value_)) ==
                        gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0])); })
              .all_relationships(":knows", 3)
              .project({PVar_(0),
                        PVar_(1),
                        PVar_(3),
                        PExpr_(8, pj::ptime_property(res, "creationDate")),
                        PVar_(9) })
              .where_qr_tuple([&](auto &v) {
                return boost::get<ptime>(v[3]) > boost::get<ptime>(params[1]) &&
                        boost::get<node *>(v[4])->id() == boost::get<node *>(v[0])->id(); })
              .append_to_qr_tuple([&](const qr_tuple &v) {
                auto a = boost::get<node *>(v[0])->id();
                auto b = boost::get<node *>(v[1])->id();
                auto c = boost::get<node *>(v[2])->id();
                if (a > c)
                  std::swap(a, c);
                if (a > b)
                  std::swap(a, b);
                if (b > c)
                  std::swap(b, c);
                std::string key = std::to_string(a)+ "_" + std::to_string(b) + "_" + std::to_string(c);
                return query_result(key); })
              .groupby({5})
              .count()
              .collect(rs);

  q.start();
  rs.wait();
}

void ldbc_bi_query_12(graph_db_ptr &gdb, result_set &rs, params_tuple &params) {

  auto q1 = query(gdb)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Person")
#else
              .all_nodes("Person")
#endif
              .to_relationships(":hasCreator")
              .from_node("Post")
              .project({PVar_(0),
                        PExpr_(2, pj::has_property(res, "content") ? 1 : 0 ),
                        PExpr_(2, pj::int_property(res, "length")),
                        PExpr_(2, pj::ptime_property(res, "creationDate")),
                        PExpr_(2, pj::has_property(res, "content") ?
                                    pj::string_property(res, "language") : std::string("n/a")) })
              .where_qr_tuple([&](const qr_tuple &v) {
                if (boost::get<int>(v[1]) == 0 ||
                    boost::get<int>(v[2]) >= boost::get<int>(params[1]) ||
                    boost::get<ptime>(v[3]) <= boost::get<ptime>(params[0]))
                      return false;
                for (std::size_t i = 2; i < params.size(); i++)
                  if (boost::get<std::string>(v[4]) == boost::get<std::string>(params[i]))
                    return true;
                return false; })
              .groupby({0}, {{"count", 0}});

  auto q2 = query(gdb)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Person")
#else
              .all_nodes("Person")
#endif
              .to_relationships(":hasCreator")
              .from_node("Comment")
              .project({PVar_(0),
                        PVar_(2),
                        PExpr_(2, pj::has_property(res, "content") ? 1 : 0 ),
                        PExpr_(2, pj::int_property(res, "length")),
                        PExpr_(2, pj::ptime_property(res, "creationDate")) })
              .where_qr_tuple([&](const qr_tuple &v) {
                if (boost::get<int>(v[2]) == 0 ||
                    boost::get<int>(v[3]) >= boost::get<int>(params[1]) ||
                    boost::get<ptime>(v[4]) <= boost::get<ptime>(params[0]))
                      return false;
                else
                  return true; })
              .from_relationships({1, 100}, ":replyOf", 1)
              .to_node("Post")
              .project({PVar_(0),
                        PExpr_(6, pj::string_property(res, "language")) })
              .where_qr_tuple([&](const qr_tuple &v) {
                for (std::size_t i = 2; i < params.size(); i++)
                  if (boost::get<std::string>(v[1]) == boost::get<std::string>(params[i]))
                    return true;
                return false; })
              .groupby({0}, {{"count", 0}})
              .union_all({&q1})
              .groupby({0}, {{"sum", 1}});

  auto q3 = query(gdb)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Person")
#else
              .all_nodes("Person")
#endif
              .outerjoin(q2, [&](const qr_tuple &lv, const qr_tuple &rv) {
                return boost::get<node *>(lv[0])->id() == boost::get<node *>(rv[0])->id(); })
              .append_to_qr_tuple([&](const qr_tuple &v) {
                return v[1].type() == typeid(null_val) ?
                  query_result((uint64_t)0) : query_result(boost::get<uint64_t>(v[2])); })
              .groupby({3}, {{"count", 0}})
              .orderby([&](const qr_tuple &q1, const qr_tuple &q2) {
                if (boost::get<uint64_t>(q1[1]) == boost::get<uint64_t>(q2[1]))
                  return boost::get<uint64_t>(q1[0]) > boost::get<uint64_t>(q2[0]);
                return boost::get<uint64_t>(q1[1]) > boost::get<uint64_t>(q2[1]); })
              .collect(rs);

  query::start({&q1, &q2, &q3});
  rs.wait();
}

void ldbc_bi_query_13(graph_db_ptr &gdb, result_set &rs, params_tuple &params) {

    auto q1 = query(gdb)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Place")
              .property("name", [&](auto &prop) {
                return *(reinterpret_cast<const dcode_t *>(prop.value_)) ==
                        gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0])); })
#else
              .nodes_where("Place", "name", [&](auto &prop) {
                return *(reinterpret_cast<const dcode_t *>(prop.value_)) ==
                        gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0])); })
#endif
              .to_relationships(":isPartOf")
              .from_node("Place")
              .to_relationships(":isLocatedIn")
              .from_node("Person")
              .property("creationDate", [&](auto &p) {
                return (*(reinterpret_cast<const ptime *>(p.value_))) <
                        boost::get<ptime>(params[1]); })
              .to_relationships(":hasCreator")
              .from_node(message)
              .property("creationDate", [&](auto &p) {
                return (*(reinterpret_cast<const ptime *>(p.value_))) <
                        boost::get<ptime>(params[1]); })
              .groupby({4}, {{"count", 0}})
              .project({PVar_(0),
                        PVar_(1),
                        PExpr_(0, pj::ptime_property(res, "creationDate")) })
              .append_to_qr_tuple([&](const qr_tuple &v) {
                auto sdt = to_iso_extended_string(boost::get<ptime>(v[2]));
                auto ssyr = sdt.substr(0, sdt.find("-"));
                auto ssmo = sdt.substr(5, 2);
                auto syr = std::stoi(ssyr);
                auto smo = std::stoi(ssmo);
                auto edt = to_iso_extended_string(boost::get<ptime>(params[1]));
                auto esyr = edt.substr(0, edt.find("-"));
                auto esmo = edt.substr(5, 2);
                auto eyr = std::stoi(esyr);
                auto emo = std::stoi(esmo);
                auto msgs = boost::get<uint64_t>(v[1]);
                uint64_t months =
                  (syr == eyr) ? (emo - smo + 1) : (12 - smo + 1) + ((eyr - syr) * 12) + emo;
                uint64_t avg_msg = msgs / months;
                return query_result(avg_msg); });

    auto q2 = query(gdb)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Place")
              .property("name", [&](auto &prop) {
                return *(reinterpret_cast<const dcode_t *>(prop.value_)) ==
                        gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0])); })
#else
              .nodes_where("Place", "name", [&](auto &prop) {
                return *(reinterpret_cast<const dcode_t *>(prop.value_)) ==
                        gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0])); })
#endif
              .to_relationships(":isPartOf")
              .from_node("Place")
              .to_relationships(":isLocatedIn")
              .from_node("Person")
              .property("creationDate", [&](auto &p) {
                return (*(reinterpret_cast<const ptime *>(p.value_))) <
                        boost::get<ptime>(params[1]); })
              .project({PVar_(4) })
              .outerjoin(q1, [&](const qr_tuple &lv, const qr_tuple &rv) {
                return boost::get<node *>(lv[0])->id() == boost::get<node *>(rv[0])->id(); })
              .where_qr_tuple([&](const qr_tuple &v) {
                return v[4].type() == typeid(null_val) ? true :
                        boost::get<uint64_t>(v[4]) < 1 ? true : false; })
              .project({PVar_(0) }); // zombies

    auto q3 = query(gdb)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Place")
              .property("name", [&](auto &prop) {
                return *(reinterpret_cast<const dcode_t *>(prop.value_)) ==
                        gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0])); })
#else
              .nodes_where("Place", "name", [&](auto &prop) {
                return *(reinterpret_cast<const dcode_t *>(prop.value_)) ==
                        gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0])); })
#endif
              .to_relationships(":isPartOf")
              .from_node("Place")
              .to_relationships(":isLocatedIn")
              .from_node("Person")
              .property("creationDate", [&](auto &p) {
                return (*(reinterpret_cast<const ptime *>(p.value_))) <
                        boost::get<ptime>(params[1]); })
              .to_relationships(":hasCreator")
              .from_node(message)
              .property("creationDate", [&](auto &p) {
                return (*(reinterpret_cast<const ptime *>(p.value_))) <
                        boost::get<ptime>(params[1]); })
              .groupby({4}, {{"count", 0}})
              .project({PVar_(0),
                        PVar_(1),
                        PExpr_(0, pj::ptime_property(res, "creationDate")) })
              .append_to_qr_tuple([&](const qr_tuple &v) {
                auto sdt = to_iso_extended_string(boost::get<ptime>(v[2]));
                auto ssyr = sdt.substr(0, sdt.find("-"));
                auto ssmo = sdt.substr(5, 2);
                auto syr = std::stoi(ssyr);
                auto smo = std::stoi(ssmo);
                auto edt = to_iso_extended_string(boost::get<ptime>(params[1]));
                auto esyr = edt.substr(0, edt.find("-"));
                auto esmo = edt.substr(5, 2);
                auto eyr = std::stoi(esyr);
                auto emo = std::stoi(esmo);
                auto msgs = boost::get<uint64_t>(v[1]);
                uint64_t months =
                  (syr == eyr) ? (emo - smo + 1) : (12 - smo + 1) + ((eyr - syr) * 12) + emo;
                uint64_t avg_msg = msgs / months;
                return query_result(avg_msg); });

    auto q4 = query(gdb)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Place")
              .property("name", [&](auto &prop) {
                return *(reinterpret_cast<const dcode_t *>(prop.value_)) ==
                        gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0])); })
#else
              .nodes_where("Place", "name", [&](auto &prop) {
                return *(reinterpret_cast<const dcode_t *>(prop.value_)) ==
                        gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0])); })
#endif
              .to_relationships(":isPartOf")
              .from_node("Place")
              .to_relationships(":isLocatedIn")
              .from_node("Person")
              .property("creationDate", [&](auto &p) {
                return (*(reinterpret_cast<const ptime *>(p.value_))) <
                        boost::get<ptime>(params[1]); })
              .project({PVar_(4) })
              .outerjoin(q3, [&](const qr_tuple &lv, const qr_tuple &rv) {
                return boost::get<node *>(lv[0])->id() == boost::get<node *>(rv[0])->id(); })
              .where_qr_tuple([&](const qr_tuple &v) {
                return v[4].type() == typeid(null_val) ? true :
                        boost::get<uint64_t>(v[4]) < 1 ? true : false; })
              .project({PVar_(0) })
              .to_relationships(":hasCreator")
              .from_node(message)
              .to_relationships(":likes")
              .from_node("Person")
              .groupby({0}, {{"count", 0}}); // total_like_count

    auto q5 = query(gdb)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Place")
              .property("name", [&](auto &prop) {
                return *(reinterpret_cast<const dcode_t *>(prop.value_)) ==
                        gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0])); })
#else
              .nodes_where("Place", "name", [&](auto &prop) {
                return *(reinterpret_cast<const dcode_t *>(prop.value_)) ==
                        gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0])); })
#endif
              .to_relationships(":isPartOf")
              .from_node("Place")
              .to_relationships(":isLocatedIn")
              .from_node("Person")
              .property("creationDate", [&](auto &p) {
                return (*(reinterpret_cast<const ptime *>(p.value_))) <
                        boost::get<ptime>(params[1]); })
              .to_relationships(":hasCreator")
              .from_node(message)
              .property("creationDate", [&](auto &p) {
                return (*(reinterpret_cast<const ptime *>(p.value_))) <
                        boost::get<ptime>(params[1]); })
              .groupby({4}, {{"count", 0}})
              .project({PVar_(0),
                        PVar_(1),
                        PExpr_(0, pj::ptime_property(res, "creationDate")) })
              .append_to_qr_tuple([&](const qr_tuple &v) {
                auto sdt = to_iso_extended_string(boost::get<ptime>(v[2]));
                auto ssyr = sdt.substr(0, sdt.find("-"));
                auto ssmo = sdt.substr(5, 2);
                auto syr = std::stoi(ssyr);
                auto smo = std::stoi(ssmo);
                auto edt = to_iso_extended_string(boost::get<ptime>(params[1]));
                auto esyr = edt.substr(0, edt.find("-"));
                auto esmo = edt.substr(5, 2);
                auto eyr = std::stoi(esyr);
                auto emo = std::stoi(esmo);
                auto msgs = boost::get<uint64_t>(v[1]);
                uint64_t months =
                  (syr == eyr) ? (emo - smo + 1) : (12 - smo + 1) + ((eyr - syr) * 12) + emo;
                uint64_t avg_msg = msgs / months;
                return query_result(avg_msg); });

    auto q6 = query(gdb)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Place")
              .property("name", [&](auto &prop) {
                return *(reinterpret_cast<const dcode_t *>(prop.value_)) ==
                        gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0])); })
#else
              .nodes_where("Place", "name", [&](auto &prop) {
                return *(reinterpret_cast<const dcode_t *>(prop.value_)) ==
                        gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0])); })
#endif
              .to_relationships(":isPartOf")
              .from_node("Place")
              .to_relationships(":isLocatedIn")
              .from_node("Person")
              .property("creationDate", [&](auto &p) {
                return (*(reinterpret_cast<const ptime *>(p.value_))) <
                        boost::get<ptime>(params[1]); })
              .project({PVar_(4) })
              .outerjoin(q5, [&](const qr_tuple &lv, const qr_tuple &rv) {
                return boost::get<node *>(lv[0])->id() == boost::get<node *>(rv[0])->id(); })
              .where_qr_tuple([&](const qr_tuple &v) {
                return v[4].type() == typeid(null_val) ? true :
                        boost::get<uint64_t>(v[4]) < 1 ? true : false; })
              .project({PVar_(0) })
              .to_relationships(":hasCreator")
              .from_node(message)
              .to_relationships(":likes")
              .from_node("Person")
              .hashjoin_on_node({4, 0}, q2)
              .groupby({0}, {{"count", 0}}); // zombie_like_count

    auto q7 = query(gdb)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Place")
              .property("name", [&](auto &prop) {
                return *(reinterpret_cast<const dcode_t *>(prop.value_)) ==
                        gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0])); })
#else
              .nodes_where("Place", "name", [&](auto &prop) {
                return *(reinterpret_cast<const dcode_t *>(prop.value_)) ==
                        gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0])); })
#endif
              .to_relationships(":isPartOf")
              .from_node("Place")
              .to_relationships(":isLocatedIn")
              .from_node("Person")
              .property("creationDate", [&](auto &p) {
                return (*(reinterpret_cast<const ptime *>(p.value_))) <
                        boost::get<ptime>(params[1]); })
              .to_relationships(":hasCreator")
              .from_node(message)
              .property("creationDate", [&](auto &p) {
                return (*(reinterpret_cast<const ptime *>(p.value_))) <
                        boost::get<ptime>(params[1]); })
              .groupby({4}, {{"count", 0}})
              .project({PVar_(0),
                        PVar_(1),
                        PExpr_(0, pj::ptime_property(res, "creationDate")) })
              .append_to_qr_tuple([&](const qr_tuple &v) {
                auto sdt = to_iso_extended_string(boost::get<ptime>(v[2]));
                auto ssyr = sdt.substr(0, sdt.find("-"));
                auto ssmo = sdt.substr(5, 2);
                auto syr = std::stoi(ssyr);
                auto smo = std::stoi(ssmo);
                auto edt = to_iso_extended_string(boost::get<ptime>(params[1]));
                auto esyr = edt.substr(0, edt.find("-"));
                auto esmo = edt.substr(5, 2);
                auto eyr = std::stoi(esyr);
                auto emo = std::stoi(esmo);
                auto msgs = boost::get<uint64_t>(v[1]);
                uint64_t months =
                  (syr == eyr) ? (emo - smo + 1) : (12 - smo + 1) + ((eyr - syr) * 12) + emo;
                uint64_t avg_msg = msgs / months;
                return query_result(avg_msg); });

    auto q8 = query(gdb)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Place")
              .property("name", [&](auto &prop) {
                return *(reinterpret_cast<const dcode_t *>(prop.value_)) ==
                        gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0])); })
#else
              .nodes_where("Place", "name", [&](auto &prop) {
                return *(reinterpret_cast<const dcode_t *>(prop.value_)) ==
                        gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0])); })
#endif
              .to_relationships(":isPartOf")
              .from_node("Place")
              .to_relationships(":isLocatedIn")
              .from_node("Person")
              .property("creationDate", [&](auto &p) {
                return (*(reinterpret_cast<const ptime *>(p.value_))) <
                        boost::get<ptime>(params[1]); })
              .project({PVar_(4) })
              .outerjoin(q7, [&](const qr_tuple &lv, const qr_tuple &rv) {
                return boost::get<node *>(lv[0])->id() == boost::get<node *>(rv[0])->id(); })
              .where_qr_tuple([&](const qr_tuple &v) {
                return v[4].type() == typeid(null_val) ? true :
                        boost::get<uint64_t>(v[4]) < 1 ? true : false; })
              .project({PVar_(0) })
              .outerjoin(q6, [&](const qr_tuple &lv, const qr_tuple &rv) {
                return boost::get<node *>(lv[0])->id() == boost::get<node *>(rv[0])->id(); })
              .outerjoin(q4, [&](const qr_tuple &lv, const qr_tuple &rv) {
                return boost::get<node *>(lv[0])->id() == boost::get<node *>(rv[0])->id(); })
              .append_to_qr_tuple([&](const qr_tuple &v) {
                return v[2].type() == typeid(null_val) ?
                  query_result((uint64_t)0) : query_result(boost::get<uint64_t>(v[2])); })
              .append_to_qr_tuple([&](const qr_tuple &v) {
                return v[4].type() == typeid(null_val) ?
                  query_result((uint64_t)0) : query_result(boost::get<uint64_t>(v[4])); })
              .append_to_qr_tuple([&](const qr_tuple &v) {
                double score = boost::get<uint64_t>(v[6]) == 0 ? 0.0 :
                                boost::get<uint64_t>(v[5]) / (double)boost::get<uint64_t>(v[6]); 
                return query_result(score); })
              .project({PExpr_(0, pj::uint64_property(res, "id")),
                        PVar_(5),
                        PVar_(6),
                        PVar_(7) })
              .orderby([&](const qr_tuple &q1, const qr_tuple &q2) {
                if (boost::get<double>(q1[3]) == boost::get<double>(q2[3]))
                  return boost::get<uint64_t>(q1[0]) < boost::get<uint64_t>(q2[0]);
                return boost::get<double>(q1[3]) > boost::get<double>(q2[3]); })
              .limit(100)
              .collect(rs);

    query::start({&q1, &q2, &q3, &q4, &q5, &q6, &q7, &q8});
    rs.wait();
}

void ldbc_bi_query_14(graph_db_ptr &gdb, result_set &rs, params_tuple &params) {

    auto q1 = query(gdb)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Place")
              .property("name", [&](auto &prop) {
                return *(reinterpret_cast<const dcode_t *>(prop.value_)) ==
                        gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[1])); })
#else
              .nodes_where("Place", "name", [&](auto &prop) {
                return *(reinterpret_cast<const dcode_t *>(prop.value_)) ==
                        gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[1])); })
#endif
              .to_relationships(":isPartOf")
              .from_node("Place")
              .to_relationships(":isLocatedIn")
              .from_node("Person")
              .project({PVar_(2),
                        PVar_(4) });

    auto q2 = query(gdb)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Place")
              .property("name", [&](auto &prop) {
                return *(reinterpret_cast<const dcode_t *>(prop.value_)) ==
                        gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0])); })
#else
              .nodes_where("Place", "name", [&](auto &prop) {
                return *(reinterpret_cast<const dcode_t *>(prop.value_)) ==
                        gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0])); })
#endif
              .to_relationships(":isPartOf")
              .from_node("Place")
              .to_relationships(":isLocatedIn")
              .from_node("Person")
              .project({PVar_(2),
                        PVar_(4) })
              .crossjoin(q1)
              .to_relationships(":hasCreator", 1)
              .from_node("Comment")
              .to_relationships(":hasCreator", 3)
              .from_node(message)
              .rship_exists({5, 7})
              .where_qr_tuple([&](const qr_tuple &v) {
                if (v[8].type() == typeid(null_val)) 
                  return false;
                auto r = boost::get<relationship *>(v[8]);
                return r->rship_label == gdb->get_code(":replyOf") ? true : false;  })
              .project({PVar_(1),
                        PVar_(3),
                        PVar_(0) })
              .append_to_qr_tuple([&](const qr_tuple &v) { return query_result((uint64_t)4);  }); // case 1

    auto q3 = query(gdb)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Place")
              .property("name", [&](auto &prop) {
                return *(reinterpret_cast<const dcode_t *>(prop.value_)) ==
                        gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[1])); })
#else
              .nodes_where("Place", "name", [&](auto &prop) {
                return *(reinterpret_cast<const dcode_t *>(prop.value_)) ==
                        gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[1])); })
#endif
              .to_relationships(":isPartOf")
              .from_node("Place")
              .to_relationships(":isLocatedIn")
              .from_node("Person")
              .project({PVar_(2),
                        PVar_(4) });

    auto q4 = query(gdb)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Place")
              .property("name", [&](auto &prop) {
                return *(reinterpret_cast<const dcode_t *>(prop.value_)) ==
                        gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0])); })
#else
              .nodes_where("Place", "name", [&](auto &prop) {
                return *(reinterpret_cast<const dcode_t *>(prop.value_)) ==
                        gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0])); })
#endif
              .to_relationships(":isPartOf")
              .from_node("Place")
              .to_relationships(":isLocatedIn")
              .from_node("Person")
              .project({PVar_(2),
                        PVar_(4) })
              .crossjoin(q3)
              .to_relationships(":hasCreator", 1)
              .from_node(message)
              .to_relationships(":hasCreator", 3)
              .from_node("Comment")
              .rship_exists({7, 5})
              .where_qr_tuple([&](const qr_tuple &v) {
                if (v[8].type() == typeid(null_val)) 
                  return false;
                auto r = boost::get<relationship *>(v[8]);
                return r->rship_label == gdb->get_code(":replyOf") ? true : false;  })
              .project({PVar_(1),
                        PVar_(3),
                        PVar_(0) })
              .append_to_qr_tuple([&](const qr_tuple &v) { return query_result((uint64_t)1);  }); // case 2

    auto q5 = query(gdb)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Place")
              .property("name", [&](auto &prop) {
                return *(reinterpret_cast<const dcode_t *>(prop.value_)) ==
                        gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[1])); })
#else
              .nodes_where("Place", "name", [&](auto &prop) {
                return *(reinterpret_cast<const dcode_t *>(prop.value_)) ==
                        gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[1])); })
#endif
              .to_relationships(":isPartOf")
              .from_node("Place")
              .to_relationships(":isLocatedIn")
              .from_node("Person")
              .project({PVar_(2),
                        PVar_(4) });

    auto q6 = query(gdb)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Place")
              .property("name", [&](auto &prop) {
                return *(reinterpret_cast<const dcode_t *>(prop.value_)) ==
                        gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0])); })
#else
              .nodes_where("Place", "name", [&](auto &prop) {
                return *(reinterpret_cast<const dcode_t *>(prop.value_)) ==
                        gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0])); })
#endif
              .to_relationships(":isPartOf")
              .from_node("Place")
              .to_relationships(":isLocatedIn")
              .from_node("Person")
              .project({PVar_(2),
                        PVar_(4) })
              .crossjoin(q5)
              .rship_exists({1, 3})
              .rship_exists({3, 1})
              .where_qr_tuple([&](const qr_tuple &v) {
                auto r = v[4].type() == typeid(relationship *) ? boost::get<relationship *>(v[4]) :
                          v[5].type() == typeid(relationship *) ? boost::get<relationship *>(v[5]) : nullptr;
                return !r ? false : r->rship_label == gdb->get_code(":knows") ? true : false;  })
              .project({PVar_(1),
                        PVar_(3),
                        PVar_(0) })
              .append_to_qr_tuple([&](const qr_tuple &v) { return query_result((uint64_t)15);  }); // case 3

    auto q7 = query(gdb)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Place")
              .property("name", [&](auto &prop) {
                return *(reinterpret_cast<const dcode_t *>(prop.value_)) ==
                        gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[1])); })
#else
              .nodes_where("Place", "name", [&](auto &prop) {
                return *(reinterpret_cast<const dcode_t *>(prop.value_)) ==
                        gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[1])); })
#endif
              .to_relationships(":isPartOf")
              .from_node("Place")
              .to_relationships(":isLocatedIn")
              .from_node("Person")
              .project({PVar_(2),
                        PVar_(4) });

    auto q8 = query(gdb)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Place")
              .property("name", [&](auto &prop) {
                return *(reinterpret_cast<const dcode_t *>(prop.value_)) ==
                        gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0])); })
#else
              .nodes_where("Place", "name", [&](auto &prop) {
                return *(reinterpret_cast<const dcode_t *>(prop.value_)) ==
                        gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0])); })
#endif
              .to_relationships(":isPartOf")
              .from_node("Place")
              .to_relationships(":isLocatedIn")
              .from_node("Person")
              .project({PVar_(2),
                        PVar_(4) })
              .crossjoin(q7)
              .to_relationships(":hasCreator", 3)
              .from_node(message)
              .rship_exists({1, 5})
              .where_qr_tuple([&](const qr_tuple &v) {
                if (v[6].type() == typeid(null_val)) 
                  return false;
                auto r = boost::get<relationship *>(v[6]);
                return r->rship_label == gdb->get_code(":likes") ? true : false;  })
              .project({PVar_(1),
                        PVar_(3),
                        PVar_(0) })
              .append_to_qr_tuple([&](const qr_tuple &v) { return query_result((uint64_t)10);  }); // case 4

    auto q9 = query(gdb)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Place")
              .property("name", [&](auto &prop) {
                return *(reinterpret_cast<const dcode_t *>(prop.value_)) ==
                        gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[1])); })
#else
              .nodes_where("Place", "name", [&](auto &prop) {
                return *(reinterpret_cast<const dcode_t *>(prop.value_)) ==
                        gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[1])); })
#endif
              .to_relationships(":isPartOf")
              .from_node("Place")
              .to_relationships(":isLocatedIn")
              .from_node("Person")
              .project({PVar_(2),
                        PVar_(4) });

    auto q10 = query(gdb)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Place")
              .property("name", [&](auto &prop) {
                return *(reinterpret_cast<const dcode_t *>(prop.value_)) ==
                        gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0])); })
#else
              .nodes_where("Place", "name", [&](auto &prop) {
                return *(reinterpret_cast<const dcode_t *>(prop.value_)) ==
                        gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0])); })
#endif
              .to_relationships(":isPartOf")
              .from_node("Place")
              .to_relationships(":isLocatedIn")
              .from_node("Person")
              .project({PVar_(2),
                        PVar_(4) })
              .crossjoin(q9)
              .to_relationships(":hasCreator", 1)
              .from_node(message)
              .rship_exists({3, 5})
              .where_qr_tuple([&](const qr_tuple &v) {
                if (v[6].type() == typeid(null_val)) 
                  return false;
                auto r = boost::get<relationship *>(v[6]);
                return r->rship_label == gdb->get_code(":likes") ? true : false;  })
              .project({PVar_(1),
                        PVar_(3),
                        PVar_(0) })
              .append_to_qr_tuple([&](const qr_tuple &v) { return query_result((uint64_t)1);  }) // case 5
              .union_all({&q2, &q4, &q6, &q8})
              .groupby({0, 1, 2}, {{"sum", 3}})
              .project({PExpr_(0, pj::uint64_property(res, "id")),
                        PExpr_(1, pj::uint64_property(res, "id")),
                        PExpr_(2, pj::string_property(res, "name")),
                        PVar_(3) })
              .orderby([&](const qr_tuple &q1, const qr_tuple &q2) {
                if (boost::get<uint64_t>(q1[3]) == boost::get<uint64_t>(q2[3])) {
                  if (boost::get<uint64_t>(q1[0]) == boost::get<uint64_t>(q2[0]))
                    return boost::get<uint64_t>(q1[1]) < boost::get<uint64_t>(q2[1]);
                  return boost::get<uint64_t>(q1[0]) < boost::get<uint64_t>(q2[0]);
                }
                return boost::get<uint64_t>(q1[3]) > boost::get<uint64_t>(q2[3]); })
              .collect(rs);

    query::start({&q1, &q2, &q3, &q4, &q5, &q6, &q7, &q8, &q9, &q10});
    rs.wait();
}

void ldbc_bi_query_15(graph_db_ptr &gdb, result_set &rs, params_tuple &params) {

    auto edge_weight =
      [&](const node &n1, const node &n2) {
        double w = 0.0;
        gdb->foreach_to_relationship_of_node(n1, ":hasCreator", [&](relationship &r1) {
          auto &msg1 = gdb->node_by_id(r1.from_node_id());
          if (msg1.node_label == gdb->get_code("Post")) {
            gdb->foreach_to_relationship_of_node(msg1, ":containerOf", [&](relationship &r2) {
              auto &forum = gdb->node_by_id(r2.from_node_id());
              if (forum.node_label == gdb->get_code("Forum")) {
                auto forum_descr = gdb->get_node_description(forum.id());
                ptime dt = get_property<ptime>(forum_descr.properties, 
                                                  std::string("creationDate")).value();
                ptime gdt1 = boost::get<ptime>(params[2]);
                ptime gdt2 = boost::get<ptime>(params[3]);
                if (gdt1 <= dt && dt <= gdt2) {
                  gdb->foreach_to_relationship_of_node(msg1, ":replyOf", [&](relationship &r3) {
                    auto &msg2 = gdb->node_by_id(r3.from_node_id());
                    if (msg2.node_label == gdb->get_code("Comment")) {
                      gdb->foreach_from_relationship_of_node(msg2, ":hasCreator", [&](relationship &r4) {
                        if (r4.to_node_id() == n2.id())
                          w += 1.0;
                      });
                    }
                  });
                }
              }
            });
          }
          else if (msg1.node_label == gdb->get_code("Comment")) {
            auto reply_code = gdb->get_code(":replyOf");
            gdb->foreach_variable_from_relationship_of_node(msg1, reply_code, 1, 100, [&](relationship &r2) {
              auto &post = gdb->node_by_id(r2.to_node_id());
              if (post.node_label == gdb->get_code("Post")) {
                gdb->foreach_to_relationship_of_node(post, ":containerOf", [&](relationship &r3) {
                  auto &forum = gdb->node_by_id(r3.from_node_id());
                  if (forum.node_label == gdb->get_code("Forum")) {
                    auto forum_descr = gdb->get_node_description(forum.id());
                    ptime dt = get_property<ptime>(forum_descr.properties, 
                                                      std::string("creationDate")).value();
                    ptime gdt1 = boost::get<ptime>(params[2]);
                    ptime gdt2 = boost::get<ptime>(params[3]);
                    if (gdt1 <= dt && dt <= gdt2) {
                      gdb->foreach_to_relationship_of_node(msg1, ":replyOf", [&](relationship &r4) {
                        auto &msg2 = gdb->node_by_id(r4.from_node_id());
                        if (msg1.node_label == gdb->get_code("Comment")) {
                          gdb->foreach_from_relationship_of_node(msg2, ":hasCreator", [&](relationship &r5) {
                            if (r5.to_node_id() == n2.id())
                              w += 0.5;
                          });
                        }
                      });
                    }
                  }
                });
              }
            });
          }
        });
        return w;
      };

    auto q = query(gdb)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Person")
              .property("id", [&](auto &prop) {
                return prop.equal(boost::get<uint64_t>(params[0])); })
#else
              .nodes_where("Person", "id", [&](auto &prop) {
                return prop.equal(boost::get<uint64_t>(params[0])); })
#endif
              .from_relationships({1, 100}, ":knows")
              .to_node("Person")
              .property("id", [&](auto &prop) {
                return prop.equal(boost::get<uint64_t>(params[1])); })
              .limit(1)
              .algo_shortest_path({0, 2}, [&](relationship &r) {
                return std::string(gdb->get_string(r.rship_label)) == ":knows"; }, true, true)
              .append_to_qr_tuple([&](qr_tuple v) {
                auto arr = boost::get<array_t>(v[3]).elems;
                std::vector<offset_t> ids;
                for (auto id : arr) {
                  auto &n = gdb->node_by_id(id);
                  auto nd = gdb->get_node_description(n.id());
                  auto p = get_property<uint64_t>(nd.properties, "id");
                  ids.push_back(p.value());
                }
                array_t nids(ids);
                return query_result(nids); })
              .append_to_qr_tuple([&](qr_tuple v) {
                double weight = 0.0;
                auto nids = boost::get<array_t>(v[3]).elems;
                for (std::size_t i = 0; i < (nids.size() - 1); i++) {
                  auto &n1 = gdb->node_by_id(nids[i]);
                  auto &n2 = gdb->node_by_id(nids[i + 1]);
                  weight += edge_weight(n1, n2);
                  weight += edge_weight(n2, n1);
                }
                return query_result(weight); })
              .orderby([&](const qr_tuple &q1, const qr_tuple &q2) {
                if (boost::get<double>(q1[5]) == boost::get<double>(q2[5])) {
                  auto a = boost::get<array_t>(q1[4]).elems;
                  auto b = boost::get<array_t>(q2[4]).elems;
                  auto m = std::mismatch(a.begin(), a.end(), b.begin(), b.end());
                  return *(m.first) < *(m.second);
                }
                return boost::get<double>(q1[5]) > boost::get<double>(q2[5]); })
              .project({PVar_(4),
                        PVar_(5) })
              .collect(rs);

    q.start();
    rs.wait();
}

void ldbc_bi_query_16(graph_db_ptr &gdb, result_set &rs, params_tuple &params) {

    auto q1 = query(gdb)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Tag")
              .property("name", [&](auto &prop) {
                auto gtg = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto etg = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[2]));
                return gtg == etg; })
#else
              .nodes_where("Tag", "name", [&](auto &prop) {
                auto gtg = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto etg = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[2]));
                return gtg == etg; })
#endif
              .to_relationships(":hasTag")
              .from_node(message)
              .property("creationDate", [&](auto &prop) {
                auto gdt = (*(reinterpret_cast<const ptime *>(prop.value_)));
                auto edt = boost::get<ptime>(params[3]);
                return gdt.date() == edt.date(); })
              .from_relationships(":hasCreator")
              .to_node("Person")
              .groupby({4}, {{"count", 0}})
              .all_relationships(":knows", 0)
              .to_relationships(":hasCreator")
              .from_node(message)
              .property("creationDate", [&](auto &prop) {
                auto gdt = (*(reinterpret_cast<const ptime *>(prop.value_)));
                auto edt = boost::get<ptime>(params[3]);
                return gdt.date() == edt.date(); })
              .from_relationships(":hasTag")
              .to_node("Tag")
              .property("name", [&](auto &prop) {
                auto gtg = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto etg = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[2]));
                return gtg == etg; })
              .groupby({0}, {{"count", 0}});

    auto q2 = query(gdb)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Tag")
              .property("name", [&](auto &prop) {
                auto gtg = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto etg = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[2]));
                return gtg == etg; })
#else
              .nodes_where("Tag", "name", [&](auto &prop) {
                auto gtg = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto etg = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[2]));
                return gtg == etg; })
#endif
              .to_relationships(":hasTag")
              .from_node(message)
              .property("creationDate", [&](auto &prop) {
                auto gdt = (*(reinterpret_cast<const ptime *>(prop.value_)));
                auto edt = boost::get<ptime>(params[3]);
                return gdt.date() == edt.date(); })
              .from_relationships(":hasCreator")
              .to_node("Person")
              .groupby({4}, {{"count", 0}})
              .outerjoin(q1, [&](const qr_tuple &lv, const qr_tuple &rv) {
                return boost::get<node *>(lv[0])->id() == boost::get<node *>(rv[0])->id(); })
              .where_qr_tuple([&](const qr_tuple &v) {
                if (v[3].type() == typeid(null_val)) 
                  return true;
                return boost::get<uint64_t>(v[3]) <= (uint64_t)boost::get<int>(params[4]); });

    auto q3 = query(gdb)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Tag")
              .property("name", [&](auto &prop) {
                auto gtg = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto etg = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0]));
                return gtg == etg; })
#else
              .nodes_where("Tag", "name", [&](auto &prop) {
                auto gtg = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto etg = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0]));
                return gtg == etg; })
#endif
              .to_relationships(":hasTag")
              .from_node(message)
              .property("creationDate", [&](auto &prop) {
                auto gdt = (*(reinterpret_cast<const ptime *>(prop.value_)));
                auto edt = boost::get<ptime>(params[1]);
                return gdt.date() == edt.date(); })
              .from_relationships(":hasCreator")
              .to_node("Person")
              .groupby({4}, {{"count", 0}})
              .all_relationships(":knows", 0)
              .to_relationships(":hasCreator")
              .from_node(message)
              .property("creationDate", [&](auto &prop) {
                auto gdt = (*(reinterpret_cast<const ptime *>(prop.value_)));
                auto edt = boost::get<ptime>(params[1]);
                return gdt.date() == edt.date(); })
              .from_relationships(":hasTag")
              .to_node("Tag")
              .property("name", [&](auto &prop) {
                auto gtg = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto etg = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0]));
                return gtg == etg; })
              .groupby({0}, {{"count", 0}});

    auto q4 = query(gdb)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Tag")
              .property("name", [&](auto &prop) {
                auto gtg = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto etg = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0]));
                return gtg == etg; })
#else
              .nodes_where("Tag", "name", [&](auto &prop) {
                auto gtg = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto etg = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0]));
                return gtg == etg; })
#endif
              .to_relationships(":hasTag")
              .from_node(message)
              .property("creationDate", [&](auto &prop) {
                auto gdt = (*(reinterpret_cast<const ptime *>(prop.value_)));
                auto edt = boost::get<ptime>(params[1]);
                return gdt.date() == edt.date(); })
              .from_relationships(":hasCreator")
              .to_node("Person")
              .groupby({4}, {{"count", 0}})
              .outerjoin(q3, [&](const qr_tuple &lv, const qr_tuple &rv) {
                return boost::get<node *>(lv[0])->id() == boost::get<node *>(rv[0])->id(); })
              .where_qr_tuple([&](const qr_tuple &v) {
                if (v[3].type() == typeid(null_val)) 
                  return true;
                return boost::get<uint64_t>(v[3]) <= (uint64_t)boost::get<int>(params[4]); })
              .hashjoin_on_node({0, 0}, q2)
              .project({PExpr_(0, pj::uint64_property(res, "id")),
                        PVar_(1),
                        PVar_(5) })
              .limit(20)
              .collect(rs);

    query::start({&q1, &q2, &q3, &q4});
    rs.wait();
}

void ldbc_bi_query_17(graph_db_ptr &gdb, result_set &rs, params_tuple &params) {

    auto q1 = query(gdb)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Tag")
              .property("name", [&](auto &prop) {
                auto gtg = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto etg = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0]));
                return gtg == etg; })
#else
              .nodes_where("Tag", "name", [&](auto &prop) {
                auto gtg = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto etg = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0]));
                return gtg == etg; })
#endif
              .to_relationships(":hasTag")
              .from_node("Post") // message 1
              .from_relationships(":hasCreator")
              .to_node("Person")
              .to_relationships(":containerOf", 2)
              .from_node("Forum")
              .from_relationships(":hasMember")
              .to_node("Person")
              .where_qr_tuple([&](const qr_tuple &v) {
                return boost::get<node *>(v[4])->id() != boost::get<node *>(v[8])->id(); })
              .to_relationships(":hasCreator")
              .from_node("Post") // message1 2
              .project({PExpr_(2, pj::ptime_property(res, "creationDate")),
                        PVar_(4),
                        PVar_(6),
                        PVar_(8),
                        PVar_(10),
                        PExpr_(10, pj::ptime_property(res, "creationDate")) })
              .where_qr_tuple([&] (const qr_tuple v) {
                auto msg1_dt = boost::get<ptime>(v[0]);
                auto msg2_dt = boost::get<ptime>(v[5]);
                auto hrs = boost::get<int>(params[1]);
                return (msg1_dt + hours(hrs)) < msg2_dt; })
              .from_relationships(":hasTag", 4)
              .to_node("Tag")
              .property("name", [&](auto &prop) {
                auto gtg = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto etg = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0]));
                return gtg == etg; })
              .to_relationships(":containerOf", 4)
              .from_node("Forum")
              .where_qr_tuple([&](const qr_tuple &v) {
                return boost::get<node *>(v[2])->id() != boost::get<node *>(v[9])->id(); })
              .rship_exists({9, 1})
              .where_qr_tuple([&](const qr_tuple &v) {
                if (v[10].type() == typeid(null_val))
                  return true;
                auto r = boost::get<relationship *>(v[10]);
                return r->rship_label == gdb->get_code(":hasMember") ? false : true; })
              .to_relationships(":replyOf", 4)
              .from_node("Comment")
              .from_relationships(":hasTag")
              .to_node("Tag")
              .property("name", [&](auto &prop) {
                auto gtg = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto etg = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0]));
                return gtg == etg; })
              .from_relationships(":hasCreator", 9)
              .to_node("Person")
              .rship_exists({2, 16})
              .where_qr_tuple([&](const qr_tuple &v) {
                if (v[17].type() == typeid(null_val))
                  return false;
                auto r = boost::get<relationship *>(v[17]);
                return r->rship_label == gdb->get_code(":hasMember") ? true : false; })
              .groupby({1}, {{"count", 0}});

    auto q2 = query(gdb)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Tag")
              .property("name", [&](auto &prop) {
                auto gtg = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto etg = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0]));
                return gtg == etg; })
#else
              .nodes_where("Tag", "name", [&](auto &prop) {
                auto gtg = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto etg = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0]));
                return gtg == etg; })
#endif
              .to_relationships(":hasTag")
              .from_node("Post") // message 1
              .from_relationships(":hasCreator")
              .to_node("Person")
              .to_relationships(":containerOf", 2)
              .from_node("Forum")
              .from_relationships(":hasMember")
              .to_node("Person")
              .where_qr_tuple([&](const qr_tuple &v) {
                return boost::get<node *>(v[4])->id() != boost::get<node *>(v[8])->id(); })
              .to_relationships(":hasCreator")
              .from_node("Comment") // message 2
              .from_relationships({1, 100}, ":replyOf")
              .to_node("Post")
              .project({PExpr_(2, pj::ptime_property(res, "creationDate")),
                        PVar_(4),
                        PVar_(6),
                        PVar_(8),
                        PVar_(10),
                        PExpr_(10, pj::ptime_property(res, "creationDate")),
                        PVar_(12) })
              .where_qr_tuple([&] (const qr_tuple v) {
                auto msg1_dt = boost::get<ptime>(v[0]);
                auto msg2_dt = boost::get<ptime>(v[5]);
                auto hrs = boost::get<int>(params[1]);
                return (msg1_dt + hours(hrs)) < msg2_dt; })
              .from_relationships(":hasTag", 4)
              .to_node("Tag")
              .property("name", [&](auto &prop) {
                auto gtg = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto etg = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0]));
                return gtg == etg; })
              .to_relationships(":containerOf", 6)
              .from_node("Forum")
              .where_qr_tuple([&](const qr_tuple &v) {
                return boost::get<node *>(v[2])->id() != boost::get<node *>(v[10])->id(); })
              .rship_exists({10, 1})
              .where_qr_tuple([&](const qr_tuple &v) {
                if (v[11].type() == typeid(null_val))
                  return true;
                auto r = boost::get<relationship *>(v[11]);
                return r->rship_label == gdb->get_code(":hasMember") ? false : true; })
              .to_relationships(":replyOf", 4)
              .from_node("Comment")
              .from_relationships(":hasTag")
              .to_node("Tag")
              .property("name", [&](auto &prop) {
                auto gtg = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto etg = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0]));
                return gtg == etg; })
              .from_relationships(":hasCreator", 13)
              .to_node("Person")
              .rship_exists({2, 17})
              .where_qr_tuple([&](const qr_tuple &v) {
                if (v[18].type() == typeid(null_val))
                  return false;
                auto r = boost::get<relationship *>(v[18]);
                return r->rship_label == gdb->get_code(":hasMember") ? true : false; })
              .groupby({1}, {{"count", 0}});

    auto q3 = query(gdb)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Tag")
              .property("name", [&](auto &prop) {
                auto gtg = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto etg = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0]));
                return gtg == etg; })
#else
              .nodes_where("Tag", "name", [&](auto &prop) {
                auto gtg = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto etg = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0]));
                return gtg == etg; })
#endif
              .to_relationships(":hasTag")
              .from_node("Comment") // message 1
              .from_relationships({1, 100}, ":replyOf")
              .to_node("Post")
              .from_relationships(":hasCreator", 2)
              .to_node("Person")
              .to_relationships(":containerOf", 4)
              .from_node("Forum")
              .from_relationships(":hasMember")
              .to_node("Person")
              .where_qr_tuple([&](const qr_tuple &v) {
                return boost::get<node *>(v[6])->id() != boost::get<node *>(v[10])->id(); })
              .to_relationships(":hasCreator")
              .from_node("Post") // message 2
              .project({PExpr_(2, pj::ptime_property(res, "creationDate")),
                        PVar_(6),
                        PVar_(8),
                        PVar_(10),
                        PVar_(12),
                        PExpr_(12, pj::ptime_property(res, "creationDate")) })
              .where_qr_tuple([&] (const qr_tuple v) {
                auto msg1_dt = boost::get<ptime>(v[0]);
                auto msg2_dt = boost::get<ptime>(v[5]);
                auto hrs = boost::get<int>(params[1]);
                return (msg1_dt + hours(hrs)) < msg2_dt; })
              .from_relationships(":hasTag", 4)
              .to_node("Tag")
              .property("name", [&](auto &prop) {
                auto gtg = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto etg = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0]));
                return gtg == etg; })
              .to_relationships(":containerOf", 4)
              .from_node("Forum")
              .where_qr_tuple([&](const qr_tuple &v) {
                return boost::get<node *>(v[2])->id() != boost::get<node *>(v[9])->id(); })
              .rship_exists({9, 1})
              .where_qr_tuple([&](const qr_tuple &v) {
                if (v[10].type() == typeid(null_val))
                  return true;
                auto r = boost::get<relationship *>(v[10]);
                return r->rship_label == gdb->get_code(":hasMember") ? false : true; })
              .to_relationships(":replyOf", 4)
              .from_node("Comment")
              .from_relationships(":hasTag")
              .to_node("Tag")
              .property("name", [&](auto &prop) {
                auto gtg = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto etg = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0]));
                return gtg == etg; })
              .from_relationships(":hasCreator", 12)
              .to_node("Person")
              .rship_exists({2, 16})
              .where_qr_tuple([&](const qr_tuple &v) {
                if (v[17].type() == typeid(null_val))
                  return false;
                auto r = boost::get<relationship *>(v[17]);
                return r->rship_label == gdb->get_code(":hasMember") ? true : false; })
              .groupby({1}, {{"count", 0}});

    auto q4 = query(gdb)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Tag")
              .property("name", [&](auto &prop) {
                auto gtg = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto etg = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0]));
                return gtg == etg; })
#else
              .nodes_where("Tag", "name", [&](auto &prop) {
                auto gtg = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto etg = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0]));
                return gtg == etg; })
#endif
              .to_relationships(":hasTag")
              .from_node("Comment") // message 1
              .from_relationships({1, 100}, ":replyOf")
              .to_node("Post")
              .from_relationships(":hasCreator", 2)
              .to_node("Person")
              .to_relationships(":containerOf", 4)
              .from_node("Forum")
              .from_relationships(":hasMember")
              .to_node("Person")
              .where_qr_tuple([&](const qr_tuple &v) {
                return boost::get<node *>(v[6])->id() != boost::get<node *>(v[10])->id(); })
              .to_relationships(":hasCreator")
              .from_node("Comment") // message 2
              .from_relationships({1, 100}, ":replyOf")
              .to_node("Post")
              .project({PExpr_(2, pj::ptime_property(res, "creationDate")),
                        PVar_(6),
                        PVar_(8),
                        PVar_(10),
                        PVar_(12),
                        PExpr_(12, pj::ptime_property(res, "creationDate")),
                        PVar_(14) })
              .where_qr_tuple([&] (const qr_tuple v) {
                auto msg1_dt = boost::get<ptime>(v[0]);
                auto msg2_dt = boost::get<ptime>(v[5]);
                auto hrs = boost::get<int>(params[1]);
                return (msg1_dt + hours(hrs)) < msg2_dt; })
              .from_relationships(":hasTag", 4)
              .to_node("Tag")
              .property("name", [&](auto &prop) {
                auto gtg = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto etg = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0]));
                return gtg == etg; })
              .to_relationships(":containerOf", 6)
              .from_node("Forum")
              .where_qr_tuple([&](const qr_tuple &v) {
                return boost::get<node *>(v[2])->id() != boost::get<node *>(v[10])->id(); })
              .rship_exists({10, 1})
              .where_qr_tuple([&](const qr_tuple &v) {
                if (v[11].type() == typeid(null_val))
                  return true;
                auto r = boost::get<relationship *>(v[11]);
                return r->rship_label == gdb->get_code(":hasMember") ? false : true; })
              .to_relationships(":replyOf", 4)
              .from_node("Comment")
              .from_relationships(":hasTag")
              .to_node("Tag")
              .property("name", [&](auto &prop) {
                auto gtg = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto etg = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0]));
                return gtg == etg; })
              .from_relationships(":hasCreator", 13)
              .to_node("Person")
              .rship_exists({2, 17})
              .where_qr_tuple([&](const qr_tuple &v) {
                if (v[18].type() == typeid(null_val))
                  return false;
                auto r = boost::get<relationship *>(v[18]);
                return r->rship_label == gdb->get_code(":hasMember") ? true : false; })
              .groupby({1}, {{"count", 0}})
              .union_all({&q1, &q2, &q3})
              .groupby({0}, {{"sum", 1}})
              .project({PExpr_(0, pj::uint64_property(res, "id")),
                        PVar_(1) })
              .orderby([&] (const qr_tuple q1, const qr_tuple q2) {
                return boost::get<uint64_t>(q1[0]) < boost::get<uint64_t>(q2[0]); })
              .collect(rs);

    query::start({&q1, &q2, &q3, &q4});
    rs.wait();
}

void ldbc_bi_query_18(graph_db_ptr &gdb, result_set &rs, params_tuple &params) {

  auto q = query(gdb)
#ifdef RUN_PARALLEL
               .all_nodes()
               .has_label("Person")
               .property( "id",
                           [&](auto &p) { return p.equal(boost::get<uint64_t>(params[0])); })
#else
               .nodes_where("Person", "id",
                            [&](auto &p) { return p.equal(boost::get<uint64_t>(params[0])); })
#endif
              .all_relationships(":knows")
              .all_relationships(":knows")
              .rship_exists({0, 4})
              .rship_exists({4, 0})
              .where_qr_tuple([&] (const qr_tuple v) {
                if (boost::get<node *>(v[0])->id() == boost::get<node *>(v[4])->id())
                  return false;
                relationship *r = v[5].type() == typeid(relationship *) ?
                  boost::get<relationship *>(v[5]) : v[6].type() == typeid(relationship *) ?
                  boost::get<relationship *>(v[6]) : nullptr;
                return !r ? true : r->rship_label == gdb->get_code(":knows") ?
                  false : true; })
              .from_relationships(":hasInterest", 4)
              .to_node("Tag")
              .property("name", [&](auto &prop) {
                auto gtg = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto etg = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[1]));
                return gtg == etg; })
              .groupby({4}, {{"count", 0}})
              .project({PExpr_(0, pj::uint64_property(res, "id")),
                        PVar_(1) })
              .orderby([&] (const qr_tuple q1, const qr_tuple q2) {
                if (boost::get<uint64_t>(q1[1]) == boost::get<uint64_t>(q2[1]))
                  return boost::get<uint64_t>(q1[0]) < boost::get<uint64_t>(q2[0]);
                return boost::get<uint64_t>(q1[1]) > boost::get<uint64_t>(q2[1]); })
              .limit(20)
              .collect(rs);

    q.start();
    rs.wait();
}

void ldbc_bi_query_19(graph_db_ptr &gdb, result_set &rs, params_tuple &params) {

    auto interaction = [&](const node &n1, const node &n2) {
      auto count = 0;
      gdb->foreach_to_relationship_of_node(n1, ":hasCreator", [&](relationship &r1) {
        auto &comment = gdb->node_by_id(r1.from_node_id());
        if (comment.node_label == gdb->get_code("Comment")) {
          gdb->foreach_from_relationship_of_node(comment, ":replyOf", [&](relationship &r2) {
            auto &msg = gdb->node_by_id(r2.to_node_id());
            if (msg.node_label == gdb->get_code("Post") || msg.node_label == gdb->get_code("Comment")) {
              gdb->foreach_from_relationship_of_node(msg, ":hasCreator", [&](relationship &r3) {
                if (r3.to_node_id() == n2.id())
                  count++;
              });
            }
          });
        }
      });
      return count;
    };

    rship_weight rweight = [&](relationship &r) {
        auto &src = gdb->node_by_id(r.from_node_id());
        auto &des = gdb->node_by_id(r.to_node_id());
        auto count = interaction(src, des) + interaction(des, src);
        return count == 0 ? 0 : 1 / (double)count;
    };

    auto q1 = query(gdb)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Place")
              .property("id",
                            [&](auto &p) { return p.equal(boost::get<uint64_t>(params[1])); })
#else
              .nodes_where("Place", "id",
                            [&](auto &p) { return p.equal(boost::get<uint64_t>(params[1])); })
#endif
              .to_relationships(":isLocatedIn")
              .from_node("Person");

    auto q2 = query(gdb)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Place")
              .property("id",
                            [&](auto &p) { return p.equal(boost::get<uint64_t>(params[0])); })
#else
              .nodes_where("Place", "id",
                            [&](auto &p) { return p.equal(boost::get<uint64_t>(params[0])); })
#endif
              .to_relationships(":isLocatedIn")
              .from_node("Person")
              .crossjoin(q1)
              .algo_weighted_shortest_path({2, 5}, [&](relationship &r) {
                return std::string(gdb->get_string(r.rship_label)) == ":knows"; }, rweight, true)
              .project({PExpr_(2, pj::uint64_property(res, "id")),
                        PExpr_(5, pj::uint64_property(res, "id")),
                        PVar_(6) })
              .orderby([&](const qr_tuple &q1, const qr_tuple &q2) {
                if (boost::get<double>(q1[2]) == boost::get<double>(q2[2])) {
                  if (boost::get<uint64_t>(q1[0]) == boost::get<uint64_t>(q2[0]))
                    return boost::get<uint64_t>(q1[1]) < boost::get<uint64_t>(q2[1]);
                  return boost::get<uint64_t>(q1[0]) < boost::get<uint64_t>(q2[0]);
                }
                return boost::get<double>(q1[2]) > boost::get<double>(q2[2]); })
              .limit(20)
              .collect(rs);

    query::start({&q1, &q2});
    rs.wait();
}

void ldbc_bi_query_20(graph_db_ptr &gdb, result_set &rs, params_tuple &params) {

    rship_predicate rpred = [&](relationship &r) {
      auto &src = gdb->node_by_id(r.from_node_id());
      auto &des = gdb->node_by_id(r.to_node_id());
      node::id_t src_uni, des_uni;
      gdb->foreach_from_relationship_of_node(src, ":studyAt", [&](relationship &r) {
        src_uni = r.to_node_id();
      });
      gdb->foreach_from_relationship_of_node(des, ":studyAt", [&](relationship &r) {
        des_uni = r.to_node_id();
      });
      bool a = std::string(gdb->get_string(r.rship_label)) == ":knows";
      bool b = src_uni == des_uni;
      return a && b;
    };

    rship_weight rweight = [&](relationship &r) {
      auto &src = gdb->node_by_id(r.from_node_id());
      auto &des = gdb->node_by_id(r.to_node_id());
      int src_yr, des_yr;
      gdb->foreach_from_relationship_of_node(src, ":studyAt", [&](relationship &r1) {
        auto descr = gdb->get_rship_description(r1.id());
        src_yr = get_property<int>(descr.properties, std::string("classYear")).value();
      });
      gdb->foreach_from_relationship_of_node(des, ":studyAt", [&](relationship &r2) {
        auto descr = gdb->get_rship_description(r2.id());
        des_yr = get_property<int>(descr.properties, std::string("classYear")).value();
      });
      double w = std::abs(src_yr - des_yr) + 1;
      return w;
    };

    auto q1 = query(gdb)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Person")
              .property("id", [&](auto &prop) {
                return prop.equal(boost::get<uint64_t>(params[1])); });
#else
              .nodes_where("Person", "id", [&](auto &prop) {
                return prop.equal(boost::get<uint64_t>(params[1])); });
#endif

    auto q2 = query(gdb)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Organisation")
              .property("name", [&](auto &prop) {
                return *(reinterpret_cast<const dcode_t *>(prop.value_)) ==
                        gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0])); })
#else
              .nodes_where("Organisation", "name", [&](auto &prop) {
                return *(reinterpret_cast<const dcode_t *>(prop.value_)) ==
                        gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0])); })
#endif
              .to_relationships(":workAt")
              .from_node("Person")
              .crossjoin(q1)
              .algo_weighted_shortest_path({2, 3}, rpred, rweight, true)
              .project({PExpr_(2, pj::uint64_property(res, "id")),
                        PVar_(4) })
              .orderby([&](const qr_tuple &q1, const qr_tuple &q2) {
                if (boost::get<double>(q1[1]) == boost::get<double>(q2[1]))
                  return boost::get<uint64_t>(q1[0]) < boost::get<uint64_t>(q2[0]);
                return boost::get<double>(q1[1]) > boost::get<double>(q2[1]); })
              .limit(20)
              .collect(rs);

    query::start({&q1, &q2});
    rs.wait();
}

#ifdef v_3_2

void ldbc_bi_query_2(graph_db_ptr &gdb, result_set &rs, params_tuple &params) {

    auto filter_cdate =
      [&](auto &prop) {
        if ((prop.flags_ & 0xe0) == prop.p_ptime) {
          auto d = *(reinterpret_cast<const ptime *>(prop.value_));
          return d >= boost::get<ptime>(params[0]) && d <= boost::get<ptime>(params[1]);
        }
        throw invalid_typecast(); 
      };

    auto filter_country =
      [&](auto &prop) {
        auto c = *(reinterpret_cast<const dcode_t *>(prop.value_));
        auto c1 = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[2]));
        auto c2 = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[3]));
        return c == c1 || c == c2; 
      };

    auto groupby_age =
      [&](auto res) {
        auto edt = time_from_string(std::string("2013-01-01 00:00:00.000"));
        auto dt = boost::get<std::string>(pj::pr_date(res, "birthday"));
        ptime bdt (boost::gregorian::from_string(dt));
        int age_grp = (edt - bdt).hours() / (8760 * 5);
        return query_result(age_grp); 
      };

    // query pipeline
    auto q = query(gdb)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label(message)
              .property( "creationDate", filter_cdate)
#else
              .nodes_where(message, "creationDate", filter_cdate)
#endif
              .from_relationships(":hasCreator")
              .to_node("Person")
              .from_relationships(":isLocatedIn")
              .to_node("Place")
              .from_relationships(":isPartOf")
              .to_node("Place")
              .property("name", filter_country)
              .from_relationships(":hasTag", 0)
              .to_node("Tag")
              .project({PExpr_(6, pj::string_property(res, "name")),
                        PExpr_(0, pj::pr_month(res, "creationDate")),
                        PExpr_(2, pj::string_property(res, "gender")),
                        projection::expr(2, groupby_age),
                        PExpr_(8, pj::string_property(res, "name")), })
              .groupby(grps, {0, 1, 2, 3, 4}, {{"count", 0}})
              .where_qr_tuple([&](const qr_tuple &v) { return boost::get<uint64_t>(v[5]) > 100; })
              .orderby([&](const qr_tuple &qr1, const qr_tuple &qr2) {
                if (boost::get<uint64_t>(qr1[5]) == boost::get<uint64_t>(qr2[5])) {
                  if (boost::get<std::string>(qr1[4]) == boost::get<std::string>(qr2[4])) {
                    if (boost::get<int>(qr1[3]) == boost::get<int>(qr2[3])) {
                      if (boost::get<std::string>(qr1[2]) == boost::get<std::string>(qr2[2])) {
                        if (boost::get<int>(qr1[1]) == boost::get<int>(qr2[1]))
                          return boost::get<std::string>(qr1[0]) < boost::get<std::string>(qr2[0]);
                        return boost::get<int>(qr1[1]) < boost::get<int>(qr2[1]);
                      }
                      return boost::get<std::string>(qr1[2]) < boost::get<std::string>(qr2[2]);
                    }
                    return boost::get<int>(qr1[3]) < boost::get<int>(qr2[3]);
                  }
                  return boost::get<std::string>(qr1[4]) < boost::get<std::string>(qr2[4]);
                }
                return boost::get<uint64_t>(qr1[5]) > boost::get<uint64_t>(qr2[5]); })
              .limit(100)
              .collect(rs);
    q.start();
    rs.wait();
}

void ldbc_bi_query_9(graph_db_ptr &gdb, result_set &rs, params_tuple &params) {

    auto fltr_tclass1 =
      [&](auto &prop) {
        auto c = *(reinterpret_cast<const dcode_t *>(prop.value_));
        auto c1 = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0]));
        return c == c1;
      };

    auto fltr_tclass2 =
      [&](auto &prop) {
        auto c = *(reinterpret_cast<const dcode_t *>(prop.value_));
        auto c2 = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[1]));
        return c == c2;
      };

    auto fltr_forum =
      [&](const qr_tuple &v) {
        return boost::get<uint64_t>(v[3]) > boost::get<uint64_t>(params[2]);
      };

    auto abs_diff =
      [&](const qr_tuple &v) {
        auto c1 = boost::get<uint64_t>(v[1]);
        auto c2 = boost::get<uint64_t>(v[2]);
        uint64_t diff = c1 > c2 ? c1 - c2 : c2 - c1;
        return query_result(diff);
      };

    // Query pipeline
    auto q2 = query(gdb)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Tagclass")
              .property("name", fltr_tclass2)
#else
              .nodes_where("Tagclass", "name", fltr_tclass2)
#endif
              .to_relationships(":hasType")
              .from_node("Tag")
              .to_relationships(":hasTag")
              .from_node("Post")
              .to_relationships(":containerOf")
              .from_node("Forum")
              .groupby({6}, {{"count", 0}});

    auto q1 = query(gdb)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Tagclass")
              .property("name", fltr_tclass1)
#else
              .nodes_where("Tagclass", "name", fltr_tclass1)
#endif
              .to_relationships(":hasType")
              .from_node("Tag")
              .to_relationships(":hasTag")
              .from_node("Post")
              .to_relationships(":containerOf")
              .from_node("Forum")
              .groupby({6}, {{"count", 0}})
              .hashjoin_on_node({0, 0}, q2)
              .from_relationships(":hasMember", 0)
              .to_node("Person")
              .groupby({0, 1, 3}, {{"count", 0}})
              .where_qr_tuple(fltr_forum)
              .append_to_qr_tuple(abs_diff)
              .project({PExpr_(0, pj::uint64_property(res, "id")),
                        PVar_(1),
                        PVar_(2),
                        PVar_(4) })
              .orderby([&](const qr_tuple &q1, const qr_tuple &q2) {
                if (boost::get<uint64_t>(q1[3]) == boost::get<uint64_t>(q2[3]))
                  return boost::get<uint64_t>(q1[0]) < boost::get<uint64_t>(q2[0]);
                return boost::get<uint64_t>(q1[3]) > boost::get<uint64_t>(q2[3]); })
              .project({PVar_(0),
                        PVar_(1),
                        PVar_(2) })
              .limit(100)
              .collect(rs);

    query::start({&q2, &q1});
    rs.wait();
}

void ldbc_bi_query_11(graph_db_ptr &gdb, result_set &rs, params_tuple &params) {

    auto filter_cntry =
      [&](auto &prop) {
        auto c1 = *(reinterpret_cast<const dcode_t *>(prop.value_));
        auto c2 = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0]));
        return c1 == c2;
      };

    auto get_tags =
      [&](const node &n) {
        std::set<node::id_t> tags;
        auto lcode = gdb->get_code(":hasTag");
        gdb->foreach_from_relationship_of_node(n, lcode, [&](relationship &r) {
          auto tag = r.to_node_id();
          tags.insert(tag);
        });
        return tags;
      };

    auto fltr_reply =
      [&](const qr_tuple &v) {
        auto reply = boost::get<node *>(v[6]);
        auto msg = boost::get<node *>(v[8]);
        auto reply_tags = get_tags(*reply);
        auto msg_tags = get_tags(*msg);
        for (auto rtag : reply_tags) {
          if (msg_tags.find(rtag) != msg_tags.end())
            return false;
        }
        return true;
      };

    auto fltr_blist =
      [&](const qr_tuple &v) {
        auto content = boost::get<std::string>(v[2]);
        auto itr = params.begin();
        while (++itr != params.end()) {
          auto bl_word = boost::get<std::string>(*itr);
          std::regex bl_expr(("(.*)" + bl_word + "(.*)"));
          if (std::regex_match(content, bl_expr))
            return false;
        }
        return true;
      };

    // Query pipelines
    auto q1 = query(gdb)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Place")
              .property("name", filter_cntry)
#else
              .nodes_where("Place", "name", filter_cntry)
#endif
              .to_relationships(":isPartOf")
              .from_node("Place")
              .to_relationships(":isLocatedIn")
              .from_node("Person")
              .to_relationships(":hasCreator")
              .from_node("Comment")
              .from_relationships(":replyOf")
              .to_node(message)
              .where_qr_tuple(fltr_reply)
              .project({PVar_(4),
                        PVar_(6),
                        PExpr_(6, pj::string_property(res, "content")) })
              .where_qr_tuple(fltr_blist)
              .from_relationships(":hasTag", 1)
              .to_node("Tag")
              .groupby({4}, {{"count", 0}});

    auto q2 = query(gdb)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Place")
              .property("name", filter_cntry)
#else
              .nodes_where("Place", "name", filter_cntry)
#endif
              .to_relationships(":isPartOf")
              .from_node("Place")
              .to_relationships(":isLocatedIn")
              .from_node("Person")
              .to_relationships(":hasCreator")
              .from_node("Comment")
              .from_relationships(":replyOf")
              .to_node(message)
              .where_qr_tuple(fltr_reply)
              .project({PVar_(4),
                        PVar_(6),
                        PExpr_(6, pj::string_property(res, "content")) })
              .where_qr_tuple(fltr_blist)
              .from_relationships(":hasTag", 1)
              .to_node("Tag")
              .to_relationships(":likes", 1)
              .from_node("Person")
              .groupby({4}, {{"count", 0}})
              .hashjoin_on_node({0, 0}, q1);

    auto q3 = query(gdb)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Place")
              .property("name", filter_cntry)
#else
              .nodes_where("Place", "name", filter_cntry)
#endif
              .to_relationships(":isPartOf")
              .from_node("Place")
              .to_relationships(":isLocatedIn")
              .from_node("Person")
              .to_relationships(":hasCreator")
              .from_node("Comment")
              .from_relationships(":replyOf")
              .to_node(message)
              .where_qr_tuple(fltr_reply)
              .project({PVar_(4),
                        PVar_(6),
                        PExpr_(6, pj::string_property(res, "content")) })
              .where_qr_tuple(fltr_blist)
              .from_relationships(":hasTag", 1)
              .to_node("Tag")
              .hashjoin_on_node({4, 0}, q2)
              .project({PExpr_(0, pj::uint64_property(res, "id")),
                        PExpr_(4, pj::string_property(res, "name")),
                        PVar_(6),
                        PVar_(8) })
              .orderby([&](const qr_tuple &q1, const qr_tuple &q2) {
                if (boost::get<uint64_t>(q1[2]) == boost::get<uint64_t>(q2[2])) {
                  if (boost::get<uint64_t>(q1[0]) == boost::get<uint64_t>(q2[0]))
                    return boost::get<std::string>(q1[1]) < boost::get<std::string>(q2[1]);
                  return boost::get<uint64_t>(q1[0]) < boost::get<uint64_t>(q2[0]);
                }
                return boost::get<uint64_t>(q1[2]) > boost::get<uint64_t>(q2[2]); })
              .limit(100)
              .collect(rs);

    query::start({&q1, &q2, &q3});
    rs.wait();
}

void ldbc_bi_query_12(graph_db_ptr &gdb, result_set &rs, params_tuple &params) {

    auto q = query(gdb)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label(message)
              .property( "creationDate", [&](auto &prop) {
                  return (*(reinterpret_cast<const ptime *>(prop.value_))) >
                          boost::get<ptime>(params[0]); })
#else
              .nodes_where(message, "creationDate", [&](auto &prop) {
                  return (*(reinterpret_cast<const ptime *>(prop.value_))) >
                          boost::get<ptime>(params[0]); })
#endif
              .to_relationships(":likes")
              .from_node("Person")
              .groupby({0}, {{"count", 0}})
              .where_qr_tuple([&](const auto &v) {
                return boost::get<uint64_t>(v[1]) > boost::get<uint64_t>(params[1]); })
              .from_relationships(":hasCreator", 0)
              .to_node("Person")
              .project({PExpr_(0, pj::uint64_property(res, "id")),
                        PExpr_(0, pj::ptime_property(res, "creationDate")),
                        PExpr_(3, pj::string_property(res, "firstName")),
                        PExpr_(3, pj::string_property(res, "lastName")),
                        PVar_(1) })
              .orderby([&](const qr_tuple &qr1, const qr_tuple &qr2) {
                if (boost::get<uint64_t>(qr1[4]) == boost::get<uint64_t>(qr2[4]))
                  return boost::get<uint64_t>(qr1[0]) < boost::get<uint64_t>(qr2[0]);
                else
                  return boost::get<uint64_t>(qr1[4]) > boost::get<uint64_t>(qr2[4]); })
              .limit(100)
              .collect(rs);
    q.start();
    rs.wait();
}

void ldbc_bi_query_13(graph_db_ptr &gdb, result_set &rs, params_tuple &params) {

    auto filter_cntry =
      [&](auto &prop) {
        auto c1 = *(reinterpret_cast<const dcode_t *>(prop.value_));
        auto c2 = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0]));
        return c1 == c2;
      };

    // Query pipelines
    auto q2 = query(gdb)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Place")
              .property("name", filter_cntry)
#else
              .nodes_where("Place", "name", filter_cntry)
#endif
              .to_relationships(":isLocatedIn")
              .from_node(message)
              .from_relationships(":hasTag")
              .to_node("Tag")
              // .to_relationships(":hasMember")
              // .from_node("Forum")
              // .groupby({6}, {{"count", 0}})
              // .orderby([&](const qr_tuple &q1, const qr_tuple &q2) {
              //   return boost::get<uint64_t>(q1[1]) > boost::get<uint64_t>(q2[1]); })
              // .limit(100);
              .collect(rs);

//     auto q1 = query(gdb)
// #ifdef RUN_PARALLEL
//               .all_nodes()
//               .has_label("Place")
//               .property("name", filter_cntry)
// #else
//               .nodes_where("Place", "name", filter_cntry)
// #endif
//               .to_relationships(":isPartOf")
//               .from_node("Place")
//               .to_relationships(":isLocatedIn")
//               .from_node("Person")
//               .to_relationships(":hasMember")
//               .from_node("Forum")
//               .groupby({6}, {{"count", 0}})
//               .orderby([&](const qr_tuple &q1, const qr_tuple &q2) {
//                 return boost::get<uint64_t>(q1[1]) > boost::get<uint64_t>(q2[1]); })
//               .limit(100)
//               .from_relationships(":hasMember", 0)
//               .to_node("Person")
//               .to_relationships(":hasCreator")
//               .from_node("Post")
//               .to_relationships(":containerOf")
//               .from_node("Forum")
//               .join_on_node({7, 0}, q2)
//               .groupby({3}, {{"count", 0}})
//               .project({PExpr_(0, pj::uint64_property(res, "id")),
//                         PExpr_(0, pj::string_property(res, "firstName")),
//                         PExpr_(0, pj::string_property(res, "lastName")),
//                         PExpr_(0, pj::ptime_property(res, "creationDate")),
//                         PVar_(1)
//                         })
//               .orderby([&](const qr_tuple &q1, const qr_tuple &q2) {
//                 if (boost::get<uint64_t>(q1[4]) == boost::get<uint64_t>(q2[4]))
//                   return boost::get<uint64_t>(q1[0]) < boost::get<uint64_t>(q2[0]);
//                 return boost::get<uint64_t>(q1[4]) > boost::get<uint64_t>(q2[4]); })
//               .collect(rs);

    // query::start({&q2, &q1});
    q2.start();
    rs.wait();
}

#endif