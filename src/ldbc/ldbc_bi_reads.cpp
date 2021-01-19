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

/* ------------------------------------------------------------------------ */

void ldbc_bi_query_1(graph_db_ptr &gdb, result_set &rs, params_tuple params) {

    std::vector<result_set> grps;

    auto filter_cdate =
      [&](auto &prop) {
          return (*(reinterpret_cast<const ptime *>(prop.value_))) <
                    boost::get<ptime>(params[0]);
      };

    auto group_msg_len =
      [&](auto res) {
        auto len = boost::get<int>(pj::int_property(res, "length"));
        return (len >= 0 && len < 40) ? query_result(std::string("0")) :
                (len >= 40 && len < 80) ? query_result(std::string("1")) :
                (len >= 80 && len < 160) ? query_result(std::string("2")) :
                query_result(std::string("3")); 
      };

    // Query pipeline
    auto q = query(gdb) // TODO add range index scan
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label(message)
              .property( "creationDate", filter_cdate)
#else
              .nodes_where(message, "creationDate", filter_cdate)
#endif
              .project({PExpr_(0, pj::pr_year(res, "creationDate")),
                        PExpr_(0, (pj::has_property(res, "language") || pj::has_property(res, "imageFile")) ?
                                    std::string("False") : std::string("True")),
                        PExpr_(0, pj::int_property(res, "length")),
                        projection::expr(0, group_msg_len) })
              .group(grps, {0, 1, 3})
              .aggregate(grps, {{"count", 0}, {"avg", 2}, {"sum", 2}, {"pcount", 0}})
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

void ldbc_bi_query_2(graph_db_ptr &gdb, result_set &rs, params_tuple params) {

    std::vector<result_set> grps1;
    std::vector<result_set> grps2;

    auto filter_cdate_1 =
      [&](auto &p) {
        auto d = *(reinterpret_cast<const ptime *>(p.value_));
        auto dt = boost::get<ptime>(params[0]);
        time_period duration(dt, hours(24*100));
        return duration.contains(d) ? true : false;
      };

    auto filter_cdate_2 =
      [&](auto &p) {
        auto d = *(reinterpret_cast<const ptime *>(p.value_));
        auto dt1 = boost::get<ptime>(params[0]);
        time_period duration1(dt1, hours(24*100));
        auto dt2 = duration1.last();
        time_period duration2(dt2, hours(24*100));
        return duration2.contains(d) ? true : false; 
      };

    auto compute_diff =
      [&](qr_tuple &v) {
        auto cnt = boost::get<uint64_t>(v[1]);
        auto nxt_cnt = boost::get<uint64_t>(v[2]);
        uint64_t diff = cnt > nxt_cnt ? cnt - nxt_cnt : nxt_cnt - cnt;
        return query_result(diff);
      };

    // Query pipelines
    auto q2 = query(gdb)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label(message)
              .property( "creationDate", filter_cdate_2)
#else
              .nodes_where(message, "creationDate", filter_cdate_2)
#endif
              .from_relationships(":hasTag", 0)
              .to_node("Tag")
              .group(grps2, {2})
              .aggregate(grps2, {{"count", 0}});

    auto q1 = query(gdb)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label(message)
              .property( "creationDate", filter_cdate_1)
#else
              .nodes_where(message, "creationDate", filter_cdate_1)
#endif
              .from_relationships(":hasTag", 0)
              .to_node("Tag")
              .group(grps1, {2})
              .aggregate(grps1, {{"count", 0}})
              .join_on_node({0, 0}, q2) // TODO compare with hashjoin
              .project({PExpr_(0, pj::string_property(res, "name")),
                        PVar_(1),
                        PVar_(3)})
              .append_to_qr_tuple(compute_diff)
              .orderby([&](const qr_tuple &q1, const qr_tuple &q2) {
                if (boost::get<uint64_t>(q1[3]) == boost::get<uint64_t>(q2[3]))
                  return boost::get<std::string>(q1[0]) < boost::get<std::string>(q2[0]);
                return boost::get<uint64_t>(q1[3]) > boost::get<uint64_t>(q2[3]); })
              .limit(100)
              .collect(rs);

    query::start({&q2, &q1});
    rs.wait();
}

void ldbc_bi_query_3(graph_db_ptr &gdb, result_set &rs, params_tuple params) {

    std::vector<result_set> grps;

    auto filter_cntry =
      [&](auto &prop) {
        auto c1 = *(reinterpret_cast<const dcode_t *>(prop.value_));
        auto c2 = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[1]));
        return c1 == c2;
      };
    
    auto filter_tgclass =
      [&](auto &prop) {
        auto c1 = *(reinterpret_cast<const dcode_t *>(prop.value_));
        auto c2 = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0]));
        return c1 == c2;
      };

    // Query pipeline
    auto q = query(gdb)
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
              .to_relationships(":hasModerator")
              .from_node("Forum")
              .from_relationships(":containerOf")
              .to_node(message)
              .from_relationships(":hasTag")
              .to_node("Tag")
              .from_relationships(":hasType")
              .to_node("Tagclass")
              .property("name", filter_tgclass)
              .group(grps, {6, 4})
              .aggregate(grps, {{"count", 0}})
              .project({PExpr_(0, pj::uint64_property(res, "id")),
                        PExpr_(0, pj::string_property(res, "title")),
                        PExpr_(0, pj::ptime_property(res, "creationDate")),
                        PExpr_(1, pj::uint64_property(res, "id")),
                        PVar_(2) })
              .orderby([&](const qr_tuple &q1, const qr_tuple &q2) {
                if (boost::get<uint64_t>(q1[4]) == boost::get<uint64_t>(q2[4]))
                  return boost::get<uint64_t>(q1[0]) < boost::get<uint64_t>(q2[0]);
                return boost::get<uint64_t>(q1[4]) > boost::get<uint64_t>(q2[4]); })
              .limit(20)
              .collect(rs);

    q.start();
    rs.wait();
}

void ldbc_bi_query_4(graph_db_ptr &gdb, result_set &rs, params_tuple params) {

    std::vector<result_set> grps1;
    std::vector<result_set> grps2;
    std::vector<result_set> grps3;

    auto filter_cntry =
      [&](auto &prop) {
        auto c1 = *(reinterpret_cast<const dcode_t *>(prop.value_));
        auto c2 = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0]));
        return c1 == c2;
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
              .to_relationships(":hasMember")
              .from_node("Forum")
              .project({PVar_(6),
                        PExpr_(6, pj::ptime_property(res, "creationDate"))
                        })
              .where_qr_tuple([&](auto &v) {
                return boost::get<ptime>(v[1]) > boost::get<ptime>(params[1]);})
              .group(grps1, {0})
              .aggregate(grps1, {{"count", 0}})
              .orderby([&](const qr_tuple &q1, const qr_tuple &q2) {
                return boost::get<uint64_t>(q1[1]) > boost::get<uint64_t>(q2[1]); })
              .limit(100)
              .collect(rs);

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
              .to_relationships(":hasMember")
              .from_node("Forum")
              .project({PVar_(6),
                        PExpr_(6, pj::ptime_property(res, "creationDate"))
                        })
              .where_qr_tuple([&](auto &v) {
                return boost::get<ptime>(v[1]) > boost::get<ptime>(params[1]);})
              .group(grps2, {0})
              .aggregate(grps2, {{"count", 0}})
              .orderby([&](const qr_tuple &q1, const qr_tuple &q2) {
                return boost::get<uint64_t>(q1[1]) > boost::get<uint64_t>(q2[1]); })
              .limit(100)
              .from_relationships(":hasMember", 0)
              .to_node("Person")
              .to_relationships(":hasCreator")
              .from_node("Post")
              .to_relationships(":containerOf")
              .from_node("Forum")
              .hashjoin_on_node({7, 0}, q1)
              .group(grps3, {3})
              .aggregate(grps3, {{"count", 0}})
              .project({PExpr_(0, pj::uint64_property(res, "id")),
                        PExpr_(0, pj::string_property(res, "firstName")),
                        PExpr_(0, pj::string_property(res, "lastName")),
                        PExpr_(0, pj::ptime_property(res, "creationDate")),
                        PVar_(1)
                        })
              .orderby([&](const qr_tuple &q1, const qr_tuple &q2) {
                if (boost::get<uint64_t>(q1[4]) == boost::get<uint64_t>(q2[4]))
                  return boost::get<uint64_t>(q1[0]) < boost::get<uint64_t>(q2[0]);
                return boost::get<uint64_t>(q1[4]) > boost::get<uint64_t>(q2[4]); })
              .collect(rs);

    query::start({&q1, &q2});
    rs.wait();
}

void ldbc_bi_query_5(graph_db_ptr &gdb, result_set &rs, params_tuple params) {

    std::vector<result_set> grps1;
    std::vector<result_set> grps2;
    std::vector<result_set> grps3;

    auto filter_tag =
      [&](auto &prop) {
        auto c1 = *(reinterpret_cast<const dcode_t *>(prop.value_));
        auto c2 = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0]));
        return c1 == c2;
      };

    auto score_func =
      [&](qr_tuple &v) {
        auto reply_cnt = boost::get<uint64_t>(v[1]);
        auto like_cnt = boost::get<uint64_t>(v[1]);
        auto msg_cnt = boost::get<uint64_t>(v[1]);
        auto score = msg_cnt + 2 * reply_cnt + 10 * like_cnt;
        return query_result(score);
      };

    // Query pipelines
    auto q3 = query(gdb)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Tag")
              .property("name", filter_tag)
#else
              .nodes_where("Tag", "name", filter_tag)
#endif
              .to_relationships(":hasTag")
              .from_node("")
              .from_relationships(":hasCreator")
              .to_node("Person")
              .group(grps3, {4})
              .aggregate(grps3, {{"count", 0}});

    auto q2 = query(gdb)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Tag")
              .property("name", filter_tag)
#else
              .nodes_where("Tag", "name", filter_tag)
#endif
              .to_relationships(":hasTag")
              .from_node("")
              .from_relationships(":hasCreator")
              .to_node("Person")
              .to_relationships(":likes", 2)
              .from_node("Person")
              .group(grps2, {4})
              .aggregate(grps2, {{"count", 0}})
              .join_on_node({0, 0}, q3);

    auto q1 = query(gdb)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Tag")
              .property("name", filter_tag)
#else
              .nodes_where("Tag", "name", filter_tag)
#endif
              .to_relationships(":hasTag")
              .from_node("")
              .from_relationships(":hasCreator")
              .to_node("Person")
              .to_relationships(":replyOf", 2)
              .from_node("Comment")
              .group(grps1, {4})
              .aggregate(grps1, {{"count", 0}})
              .join_on_node({0, 0}, q2)
              .append_to_qr_tuple(score_func)
              .project({PExpr_(0, pj::uint64_property(res, "id")),
                        PVar_(1),
                        PVar_(3),
                        PVar_(5),
                        PVar_(6)
                        })
              .orderby([&](const qr_tuple &q1, const qr_tuple &q2) {
                if (boost::get<uint64_t>(q1[4]) == boost::get<uint64_t>(q2[4]))
                  return boost::get<uint64_t>(q1[0]) < boost::get<uint64_t>(q2[0]);
                return boost::get<uint64_t>(q1[4]) > boost::get<uint64_t>(q2[4]); })
              .limit(100)
              .collect(rs);

    query::start({&q3, &q2, &q1});
    rs.wait();
}

void ldbc_bi_query_6(graph_db_ptr &gdb, result_set &rs, params_tuple params) {

    std::vector<result_set> grps1;
    std::vector<result_set> grps2;

    auto filter_tag =
      [&](auto &prop) {
        auto c1 = *(reinterpret_cast<const dcode_t *>(prop.value_));
        auto c2 = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0]));
        return c1 == c2;
      };

    // Query pipelines
    auto q2 = query(gdb)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Tag")
              .property("name", filter_tag)
#else
              .nodes_where("Tag", "name", filter_tag)
#endif
              .to_relationships(":hasTag")
              .from_node(message)
              .from_relationships(":hasCreator")
              .to_node("Person")
              .to_relationships(":hasCreator")
              .from_node(message)
              .to_relationships(":likes")
              .from_node("Person")
              .project({PVar_(8) })
              .to_relationships(":hasCreator")
              .from_node(message)
              .to_relationships(":likes")
              .from_node("Person")
              .group(grps2, {0})
              .aggregate(grps2, {{"count", 0}});

    auto q1 = query(gdb)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Tag")
              .property("name", filter_tag)
#else
              .nodes_where("Tag", "name", filter_tag)
#endif
              .to_relationships(":hasTag")
              .from_node(message)
              .from_relationships(":hasCreator")
              .to_node("Person")
              .to_relationships(":hasCreator")
              .from_node(message)
              .to_relationships(":likes")
              .from_node("Person")
              .project({PVar_(4),
                        PVar_(8)
                        })
              .hashjoin_on_node({1, 0}, q2)
              // .join_on_node({1, 0}, q2)
              .group(grps1, {0})
              .aggregate(grps1, {{"sum", 3}})
              .project({PExpr_(0, pj::uint64_property(res, "id")),
                        PVar_(1)
                        })
              .orderby([&](const qr_tuple &q1, const qr_tuple &q2) {
                if (boost::get<uint64_t>(q1[1]) == boost::get<uint64_t>(q2[1]))
                  return boost::get<uint64_t>(q1[0]) < boost::get<uint64_t>(q2[0]);
                return boost::get<uint64_t>(q1[1]) > boost::get<uint64_t>(q2[1]); })
              .limit(100)
              .collect(rs);

    query::start({&q2, &q1});
    rs.wait();
}

void ldbc_bi_query_7(graph_db_ptr &gdb, result_set &rs, params_tuple params) {

    std::vector<result_set> grps;

    auto filter_tag =
      [&](auto &prop) {
        auto c1 = *(reinterpret_cast<const dcode_t *>(prop.value_));
        auto c2 = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0]));
        return c1 == c2;
      };

    auto drop_tag =
      [&](auto &prop) {
        auto c1 = *(reinterpret_cast<const dcode_t *>(prop.value_));
        auto c2 = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0]));
        return c1 != c2;
      };

    // Query pipeline
    auto q = query(gdb)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Tag")
              .property("name", filter_tag)
#else
              .nodes_where("Tag", "name", filter_tag)
#endif
              .to_relationships(":hasTag")
              .from_node(message)
              .to_relationships(":replyOf")
              .from_node("Comment")
              .from_relationships(":hasTag")
              .to_node("Tag")
              .property("name", drop_tag)
              .group(grps, {6})
              .aggregate(grps, {{"count", 0}})
              .project({PExpr_(0, pj::string_property(res, "name")),
                        PVar_(1)
                        })
              .orderby([&](const qr_tuple &q1, const qr_tuple &q2) {
                if (boost::get<uint64_t>(q1[1]) == boost::get<uint64_t>(q2[1]))
                  return boost::get<std::string>(q1[0]) < boost::get<std::string>(q2[0]);
                return boost::get<uint64_t>(q1[1]) > boost::get<uint64_t>(q2[1]); })
              .limit(100)
              .collect(rs);

    q.start();
    rs.wait();
}

void ldbc_bi_query_8(graph_db_ptr &gdb, result_set &rs, params_tuple params) {

    std::vector<result_set> grps2;
    std::vector<result_set> grps4;

    auto fltr_tag =
      [&](auto &prop) {
        auto c = *(reinterpret_cast<const dcode_t *>(prop.value_));
        auto c1 = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0]));
        return c == c1;
      };

    auto fltr_cdate =
      [&](auto &prop) {
        auto date = (*(reinterpret_cast<const ptime *>(prop.value_)));
        return date > boost::get<ptime>(params[1]);
      };

    auto check_msg =
      [&](const node &person) {
        bool found = false;
        uint64_t cnt = 0;
        auto lcode1 = gdb->get_code(":hasCreator");
        gdb->foreach_to_relationship_of_node(person, lcode1, [&](relationship &r1) {
          auto &msg = gdb->node_by_id(r1.from_node_id());
          auto lcode2 = gdb->get_code(":hasTag");
          gdb->foreach_from_relationship_of_node(msg, lcode2, [&](relationship &r2) {
            auto &tag = gdb->node_by_id(r2.to_node_id());
            if (gdb->is_node_property(tag, "name", fltr_tag)) {
              found = true;
              ++cnt;
            }
          });
        });
        std::pair<bool, uint64_t> pair(found, cnt);
        return pair;
      };

    auto check_intrst =
      [&](const node &person) {
        bool found = false;
        auto lcode = gdb->get_code(":hasInterest");
        gdb->foreach_from_relationship_of_node(person, lcode, [&](relationship &r) {
          auto &tag = gdb->node_by_id(r.to_node_id());
          if (gdb->is_node_property(tag, "name", fltr_tag))
            found = true;
        });
        return found;
      };

    auto interest_only =
      [&](const qr_tuple &v) {
        const auto person = boost::get<node *>(v[0]);
        return check_msg(*person).first ? false : true;
      };

    auto msg_only =
      [&](const qr_tuple &v) {
        const auto person = boost::get<node *>(v[0]);
        return check_intrst(*person) ? false : true;
      };

    auto get_score =
      [&](node &frnd) {
        auto pair = check_msg(frnd);
        auto has_msg = pair.first;
        auto has_intrst = check_intrst(frnd);
        if (has_intrst && has_msg) {
          return (uint64_t)100 + check_msg(frnd).second;
        } else if (has_intrst && !has_msg) {
          return (uint64_t)100;
        } else if (!has_intrst && has_msg) {
          return check_msg(frnd).second;
        } else {
          return (uint64_t)0;
        }
      };

    auto sum_frnds_score =
      [&](qr_tuple &v) {
        auto person = boost::get<node *>(v[0]);
        auto lcode = gdb->get_code(":knows");
        uint64_t scores = 0;
        gdb->foreach_from_relationship_of_node(*person, lcode, [&](relationship &r) {
          auto &frnd = gdb->node_by_id(r.to_node_id());
          scores += get_score(frnd);
        });
        return query_result(scores);
      };

    auto q4 = query(gdb)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Tag")
              .property("name", fltr_tag)
#else
              .nodes_where("Tag", "name", fltr_tag)
#endif
              .to_relationships(":hasTag")
              .from_node(message)
              .property("creationDate", fltr_cdate)
              .from_relationships(":hasCreator")
              .to_node("Person")
              .group(grps4, {4})
              .aggregate(grps4, {{"count", 0}});

    auto q3 = query(gdb)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Tag")
              .property("name", fltr_tag)
#else
              .nodes_where("Tag", "name", fltr_tag)
#endif
              .to_relationships(":hasInterest")
              .from_node("Person")
              .project({PVar_(2)})
              .hashjoin_on_node({0, 0}, q4)
              .append_to_qr_tuple([&](qr_tuple &v) {
                auto score = boost::get<uint64_t>(v[2]);
                score += 100;
                return query_result(score); })
              .project({PVar_(0),
                        PVar_(3) });

    auto q2 = query(gdb)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Tag")
              .property("name", fltr_tag)
#else
              .nodes_where("Tag", "name", fltr_tag)
#endif
              .to_relationships(":hasTag")
              .from_node(message)
              .property("creationDate", fltr_cdate)
              .from_relationships(":hasCreator")
              .to_node("Person")
              .group(grps2, {4})
              .aggregate(grps2, {{"count", 0}})
              .where_qr_tuple(msg_only);

    auto q1 = query(gdb)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Tag")
              .property("name", fltr_tag)
#else
              .nodes_where("Tag", "name", fltr_tag)
#endif
              .to_relationships(":hasInterest")
              .from_node("Person")
              .project({PVar_(2)})
              .append_to_qr_tuple([&](qr_tuple &v) {
                return query_result((uint64_t)100); })
              .where_qr_tuple(interest_only)
              .union_all({&q2, &q3})
              .append_to_qr_tuple(sum_frnds_score)
              .append_to_qr_tuple([&](qr_tuple &v) {
                auto s = boost::get<uint64_t>(v[1]) + boost::get<uint64_t>(v[2]);
                return query_result(s); })
              .project({PExpr_(0, pj::uint64_property(res, "id")),
                        PVar_(1),
                        PVar_(2),
                        PVar_(3) })
              .orderby([&](const qr_tuple &q1, const qr_tuple &q2) {
                if (boost::get<uint64_t>(q1[3]) == boost::get<uint64_t>(q2[3]))
                  return boost::get<uint64_t>(q1[0]) < boost::get<uint64_t>(q2[0]);
                return boost::get<uint64_t>(q1[3]) > boost::get<uint64_t>(q2[3]); })
              .project({PVar_(0),
                        PVar_(1),
                        PVar_(2) })
              .limit(100)
              .collect(rs);

    query::start({&q4, &q3, &q2, &q1});
    rs.wait();
}

void ldbc_bi_query_9(graph_db_ptr &gdb, result_set &rs, params_tuple params) {
  std::vector<result_set> grps1;
  std::vector<result_set> grps2;

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
                        PVar_(1),
                        PVar_(2),
                        PExpr_(2, pj::ptime_property(res, "creationDate")) })
              .where_qr_tuple([&](const qr_tuple &v) {
                return boost::get<ptime>(v[3]) >= boost::get<ptime>(params[0]) &&
                        boost::get<ptime>(v[3]) <= boost::get<ptime>(params[1]); })
              .to_relationships({1, 100}, ":replyOf", 2)
              .from_node("Comment")
              .project({PVar_(0),
                        PVar_(1),
                        PVar_(2),
                        PVar_(4),
                        PVar_(5),
                        PExpr_(5, pj::ptime_property(res, "creationDate")) })
              .where_qr_tuple([&](const qr_tuple &v) {
                return boost::get<ptime>(v[5]) >= boost::get<ptime>(params[0]) &&
                        boost::get<ptime>(v[5]) <= boost::get<ptime>(params[1]); })
              .group(grps1, {0, 2})
              .aggregate(grps1, {{"count", 0}})
              .group(grps2, {0})
              .aggregate(grps2, {{"count", 0}, {"sum", 2}})
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

void ldbc_bi_query_10(graph_db_ptr &gdb, result_set &rs, params_tuple params) {
  auto min_hop = boost::get<int>(params[3]);
  auto max_hop = boost::get<int>(params[4]);
  std::vector<result_set> grps;

  auto q = query(gdb)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Person")
              .property("name", "id", [&](auto &p) {
                return p.equal(boost::get<uint64_t>(params[0])); })
#else
              .nodes_where("Person", "id", [&](auto &p) {
                return p.equal(boost::get<uint64_t>(params[0])); })
#endif
              .from_relationships({/*min_hop*/1, max_hop}, ":knows")
              .to_node("Person")
              .from_relationships(":isLocatedIn")
              .to_node("Place")
              .from_relationships(":isPartOf")
              .to_node("Place")
              .property("name", [&](auto &prop) {
                return *(reinterpret_cast<const dcode_t *>(prop.value_)) ==
                        gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[1])); })
              .project({PVar_(2)})
              .to_relationships(":hasCreator")
              .from_node(message)
              .from_relationships(":hasTag")
              .to_node("Tag")
              .from_relationships(":hasType")
              .to_node("Tagclass")
              .property("name", [&](auto &prop) {
                return *(reinterpret_cast<const dcode_t *>(prop.value_)) ==
                        gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[2])); })
              .project({PVar_(0),
                        PVar_(2)} )
              .from_relationships(":hasTag")
              .to_node("Tag")
              .group(grps, {0, 3})
              .aggregate(grps, {{"count", 0}})
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

void ldbc_bi_query_11(graph_db_ptr &gdb, result_set &rs, params_tuple params) {
  std::vector<result_set> grps;

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
              .from_node("Person");

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
              .join_on_rship({4, 4}, q1);

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
              .join_on_rship({4, 4}, q2)
              .rship_exists({4, 14})
              .project({PVar_(0)})
              .group(grps, {0})
              .aggregate(grps, {{"count", 0}})
              .project({PVar_(1)})
              .collect(rs);

  query::start({&q1, &q2, &q3});
  rs.wait();
}

void ldbc_bi_query_12(graph_db_ptr &gdb, result_set &rs, params_tuple params) {
  std::vector<result_set> grps1;
  std::vector<result_set> grps2;
  std::vector<result_set> grps3;

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
                        PVar_(1),
                        PVar_(2),
                        PExpr_(2, pj::has_property(res, "content") ? 1 : 0 ),
                        PExpr_(2, pj::int_property(res, "length")),
                        PExpr_(2, pj::ptime_property(res, "creationDate")),
                        PExpr_(2, pj::has_property(res, "content") ?
                                    pj::string_property(res, "language") : std::string("n/a")) })
              .where_qr_tuple([&](const qr_tuple &v) {
                return boost::get<int>(v[3]) == 0 ? false :
                        boost::get<int>(v[4]) >= boost::get<int>(params[1]) ? false :
                        boost::get<ptime>(v[5]) <= boost::get<ptime>(params[0]) ? false :
                        boost::get<std::string>(v[6]).compare(boost::get<std::string>(params[2])) != 0 ? false : true; })
              .group(grps1, {0})
              .aggregate(grps1, {{"count", 0}});

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
                        PVar_(1),
                        PVar_(2),
                        PExpr_(2, pj::has_property(res, "content") ? 1 : 0 ),
                        PExpr_(2, pj::int_property(res, "length")),
                        PExpr_(2, pj::ptime_property(res, "creationDate")) })
              .where_qr_tuple([&](const qr_tuple &v) {
                return boost::get<int>(v[3]) == 0 ? false :
                        boost::get<int>(v[4]) >= boost::get<int>(params[1]) ? false :
                        boost::get<ptime>(v[5]) <= boost::get<ptime>(params[0]) ? false : true; })
              .from_relationships({1, 100}, ":replyOf", 2)
              .to_node("Post")
              .project({PVar_(0),
                        PVar_(1),
                        PVar_(2),
                        PVar_(7),
                        PExpr_(7, pj::string_property(res, "language")) })
              .where_qr_tuple([&](const qr_tuple &v) {
                return boost::get<std::string>(v[4]).compare(boost::get<std::string>(params[2])) != 0 ? false : true; })
              .group(grps2, {0})
              .aggregate(grps2, {{"count", 0}})
              .hashjoin_on_node({0, 0}, q1)
              .append_to_qr_tuple([&](qr_tuple &v) {
                return boost::get<uint64_t>(v[1]) + boost::get<uint64_t>(v[3]); })
              .group(grps3, {4})
              .aggregate(grps3, {{"count", 0}})
              .orderby([&](const qr_tuple &q1, const qr_tuple &q2) {
                if (boost::get<uint64_t>(q1[1]) == boost::get<uint64_t>(q2[1]))
                  return boost::get<uint64_t>(q1[0]) > boost::get<uint64_t>(q2[0]);
                return boost::get<uint64_t>(q1[1]) > boost::get<uint64_t>(q2[1]); })
              .collect(rs);

  query::start({&q1, &q2});
  rs.wait();
}

void ldbc_bi_query_13(graph_db_ptr &gdb, result_set &rs, params_tuple params) {
    std::vector<result_set> grps1;
    std::vector<result_set> grps2;
    std::vector<result_set> grps3;
    std::vector<result_set> grps4;

    // Query pipeline
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
              .group(grps1, {4})
              .aggregate(grps1, {{"count", 0}})
              .project({PVar_(0),
                        PVar_(1),
                        PExpr_(0, pj::ptime_property(res, "creationDate")) })
              .where_qr_tuple([&](const qr_tuple &v) {
                time_period duration(boost::get<ptime>(v[2]), boost::get<ptime>(params[1])); // TODO
                auto mpm = (duration.length().hours() / 713) / boost::get<uint64_t>(v[1]);
                return mpm < 1; });

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
              .to_relationships(":hasCreator")
              .from_node(message)
              .group(grps2, {4})
              .aggregate(grps2, {{"count", 0}})
              .project({PVar_(0),
                        PVar_(1),
                        PExpr_(0, pj::ptime_property(res, "creationDate")) })
              .where_qr_tuple([&](const qr_tuple &v) {
                time_period duration(boost::get<ptime>(v[2]), boost::get<ptime>(params[1]));
                auto mpm = (duration.length().hours() / 713) / boost::get<uint64_t>(v[1]);
                return mpm < 1; })
              .to_relationships(":hasCreator", 0)
              .from_node(message)
              .to_relationships(":likes")
              .from_node("Person")
              .hashjoin_on_node({6, 0}, q1)
              .group(grps3, {0})
              .aggregate(grps3, {{"count", 0}})
              .to_relationships(":hasCreator", 0)
              .from_node(message)
              .to_relationships(":likes")
              .from_node("Person")
              .group(grps4, {0, 1})
              .aggregate(grps4, {{"count", 0}})
              .append_to_qr_tuple([&](qr_tuple &v) {
                auto score = boost::get<uint64_t>(v[2]) == 0 ?
                      0 : boost::get<uint64_t>(v[1]) / (double)boost::get<uint64_t>(v[2]);
                return query_result(score); })
              .project({PExpr_(0, pj::uint64_property(res, "id")),
                        PVar_(1),
                        PVar_(2),
                        PVar_(3) })
              .orderby([&](const qr_tuple &q1, const qr_tuple &q2) {
                if (boost::get<double>(q1[3]) == boost::get<double>(q2[3]))
                  return boost::get<uint64_t>(q1[0]) < boost::get<uint64_t>(q2[0]);
                return boost::get<double>(q1[3]) > boost::get<double>(q2[3]); })
              .limit(100)
              .collect(rs);

    query::start({&q1, &q2});
    rs.wait();
}

void ldbc_bi_query_14(graph_db_ptr &gdb, result_set &rs, params_tuple params) {

    auto c1 =
      [&](const node &p1, const node &p2, bool flag) {
        bool found = false;
        auto creator_code = gdb->get_code(":hasCreator");
        auto reply_code = gdb->get_code(":replyOf");
        auto cmt_code = gdb->get_code("Comment");
        auto post_code = gdb->get_code("Post");
        gdb->foreach_to_relationship_of_node(p1, creator_code, [&](relationship &r1) {
          auto &n1 = gdb->node_by_id(r1.from_node_id());
          if (n1.node_label == cmt_code) {
            gdb->foreach_from_relationship_of_node(n1, reply_code, [&](relationship &r2) {
              auto &n2 = gdb->node_by_id(r2.to_node_id());
              if (n2.node_label == cmt_code || n2.node_label == post_code) {
                gdb->foreach_from_relationship_of_node(n2, creator_code, [&](relationship &r3) {
                  if (r3.to_node_id() == p2.id())
                    found = true;
                  });
              }
              });
          }
          });
        return !found ? query_result(0) :
                flag ? query_result(4) : query_result(1);
      };

    auto c4 =
      [&](const node &p1, const node &p2, bool flag) {
        bool found = false;
        uint64_t cnt = 0;
        auto creator_code = gdb->get_code(":hasCreator");
        auto like_code = gdb->get_code(":likes");
        auto cmt_code = gdb->get_code("Comment");
        auto post_code = gdb->get_code("Post");
        gdb->foreach_from_relationship_of_node(p1, like_code, [&](relationship &r1) {
          auto &n1 = gdb->node_by_id(r1.to_node_id());
          if (n1.node_label == cmt_code || n1.node_label == post_code) {
            gdb->foreach_from_relationship_of_node(n1, creator_code, [&](relationship &r2) {
              if (r2.to_node_id() == p2.id())
                found = true;
              });
          }
          });
        return !found ? query_result(0) :
                flag ? query_result(10) : query_result(1);
      };

    // Query pipeline
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
              .project({PVar_(2),
                        PVar_(4) });

    auto q2 = query(gdb)
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
                        PVar_(4) })
              .crossjoin(q1)
              .append_to_qr_tuple([&](qr_tuple &v) {
                auto p1 = boost::get<node *>(v[1]);
                auto p2 = boost::get<node *>(v[3]);
                return c1(*p1, *p2, true); })
              .append_to_qr_tuple([&](qr_tuple &v) {
                auto p1 = boost::get<node *>(v[1]);
                auto p2 = boost::get<node *>(v[3]);
                return c1(*p2, *p1, false); })
              .rship_exists({1, 3}, true)
              .rship_exists({3, 1}, true)
              .append_to_qr_tuple([&](qr_tuple &v) {
                if (v[6].type() == typeid(null_t) && v[7].type() == typeid(null_t))
                  return query_result(0);
                else
                  return query_result(15); })
              .append_to_qr_tuple([&](qr_tuple &v) {
                auto p1 = boost::get<node *>(v[1]);
                auto p2 = boost::get<node *>(v[3]);
                return c4(*p1, *p2, true); })
              .append_to_qr_tuple([&](qr_tuple &v) {
                auto p1 = boost::get<node *>(v[1]);
                auto p2 = boost::get<node *>(v[3]);
                return c4(*p2, *p1, false); })
              .append_to_qr_tuple([&](qr_tuple &v) {
                auto s1 = boost::get<int>(v[4]);
                auto s2 = boost::get<int>(v[5]);
                auto s3 = boost::get<int>(v[8]);
                auto s4 = boost::get<int>(v[9]);
                auto s5 = boost::get<int>(v[10]);
                auto s = s1 + s2 + s3 + s4 + s5;
                return query_result(s); })
              .project({PExpr_(1, pj::uint64_property(res, "id")),
                        PExpr_(3, pj::uint64_property(res, "id")),
                        PExpr_(0, pj::string_property(res, "name")),
                        PVar_(11) })
              .orderby([&](const qr_tuple &q1, const qr_tuple &q2) {
                if (boost::get<int>(q1[3]) == boost::get<int>(q2[3])) {
                  if (boost::get<uint64_t>(q1[0]) == boost::get<uint64_t>(q2[0]))
                    return boost::get<uint64_t>(q1[1]) < boost::get<uint64_t>(q2[1]);
                  return boost::get<uint64_t>(q1[0]) > boost::get<uint64_t>(q2[0]);
                }
                return boost::get<int>(q1[3]) > boost::get<int>(q2[3]); })
              .collect(rs);

    query::start({&q1, &q2});
    rs.wait();
}

void ldbc_bi_query_15(graph_db_ptr &gdb, result_set &rs, params_tuple params) {

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

    // Query pipeline
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
              .algo_shortest_path({0, 2}, [&](relationship &r) {
                return std::string(gdb->get_string(r.rship_label)) == ":knows"; }, true)
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
              .append_to_qr_tuple([&](qr_tuple v) {
                auto nids = boost::get<array_t>(v[3]).elems;
                std::sort(nids.begin(), nids.end());
                array_t res(nids);
                return query_result(res); })
              .orderby([&](const qr_tuple &q1, const qr_tuple &q2) {
                return boost::get<double>(q1[5]) < boost::get<double>(q2[5]); })
              .project({PVar_(6),
                        PVar_(5) })
              .collect(rs);

    q.start();
    rs.wait();
}

void ldbc_bi_query_16(graph_db_ptr &gdb, result_set &rs, params_tuple params) {
    std::vector<result_set> grps1;
    std::vector<result_set> grps2;
    std::vector<result_set> grps3;
    std::vector<result_set> grps4;
    std::vector<result_set> grps5;
    std::vector<result_set> grps6;
    std::vector<result_set> grps7;
    std::vector<result_set> grps8;

    // Query pipelines
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
              .from_node(message)
              .property("creationDate", [&](auto &prop) {
                auto gdt = (*(reinterpret_cast<const ptime *>(prop.value_)));
                auto edt = boost::get<ptime>(params[1]);
                return gdt.date() == edt.date(); })
              .from_relationships(":hasCreator")
              .to_node("Person")
              .from_relationships(":knows") // first of pair
              .to_node("Person")
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
              .group(grps1, {2, 4})
              .aggregate(grps1, {{"count", 0}})
              .where_qr_tuple([&](const qr_tuple v) {
                return boost::get<uint64_t>(v[2]) <= (uint64_t)boost::get<int>(params[4]); })
              .group(grps2, {1})
              .aggregate(grps2, {{"count", 0}})
              .from_relationships(":knows", 0)
              .to_node("Person")
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
              .from_relationships(":knows", 3) // Second of pair
              .to_node("Person")
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
              .group(grps3, {0, 1, 3, 5})
              .aggregate(grps3, {{"count", 0}})
              .where_qr_tuple([&](const qr_tuple v) {
                return boost::get<uint64_t>(v[4]) <= (uint64_t)boost::get<int>(params[4]); })
              .group(grps4, {0, 1, 2})
              .aggregate(grps4, {{"count", 0}});

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
              .from_relationships(":knows") // first of pair
              .to_node("Person")
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
              .group(grps5, {2, 4})
              .aggregate(grps5, {{"count", 0}})
              .where_qr_tuple([&](const qr_tuple v) {
                return boost::get<uint64_t>(v[2]) <= (uint64_t)boost::get<int>(params[4]); })
              .group(grps6, {1})
              .aggregate(grps6, {{"count", 0}})
              .from_relationships(":knows", 0)
              .to_node("Person")
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
              .from_relationships(":knows", 3) // Second of pair
              .to_node("Person")
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
              .group(grps7, {0, 1, 3, 5})
              .aggregate(grps7, {{"count", 0}})
              .where_qr_tuple([&](const qr_tuple v) {
                return boost::get<uint64_t>(v[4]) <= (uint64_t)boost::get<int>(params[4]); })
              .group(grps8, {0, 1, 2})
              .aggregate(grps8, {{"count", 0}})
              .hashjoin_on_node({0, 0}, q1)
              .append_to_qr_tuple([&](qr_tuple v) {
                uint64_t cnt = boost::get<uint64_t>(v[1]) + boost::get<uint64_t>(v[4]);
                return query_result(cnt); })
              .project({PVar_(8),
                        PExpr_(0, pj::uint64_property(res, "id")) })
              .orderby([&](const qr_tuple &qr1, const qr_tuple &qr2) {
                if (boost::get<uint64_t>(qr1[0]) == boost::get<uint64_t>(qr2[0]))
                  return boost::get<uint64_t>(qr1[1]) < boost::get<uint64_t>(qr2[1]);
                return boost::get<uint64_t>(qr1[0]) > boost::get<uint64_t>(qr2[0]); })
              .limit(20)
              .collect(rs);

    query::start({&q1, &q2});
    rs.wait();
}

void ldbc_bi_query_17(graph_db_ptr &gdb, result_set &rs, params_tuple params) {
    std::vector<result_set> grps1;
    std::vector<result_set> grps2;
    std::vector<result_set> grps3;
    std::vector<result_set> grps4;

    // Query pipelines
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
              .from_node("Post") // message1 (Post)
              .to_relationships(":containerOf")
              .from_node("Forum")
              .from_relationships(":hasMember")
              .to_node("Person"); // person2


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
              .from_node("Post") // message1 (Post)
              .from_relationships(":hasCreator")
              .to_node("Person")
              .to_relationships(":containerOf", 2)
              .from_node("Forum")
              .from_relationships(":hasMember")
              .to_node("Person") // person3
              .hashjoin_on_node({6, 4}, q1)
              .to_relationships(":hasCreator", 8)
              .from_node("Post") // message2 (Post)
              .project({PVar_(0), // tag
                        PVar_(2), // message1
                        PExpr_(2, pj::ptime_property(res, "creationDate")),
                        PVar_(4), // person1
                        PVar_(6), // forum1
                        PVar_(8), // person3
                        PVar_(15), // person2
                        PVar_(17), // message2 (Post)
                        PExpr_(17, pj::ptime_property(res, "creationDate")) })
              .where_qr_tuple([&] (const qr_tuple v) {
                auto msg1_dt = boost::get<ptime>(v[2]);
                auto msg2_dt = boost::get<ptime>(v[8]);
                auto hrs = boost::get<int>(params[1]);
                return (msg1_dt + hours(hrs)) < msg2_dt; })
              .from_relationships(":hasTag", 7)
              .to_node("Tag")
              .property("name", [&](auto &prop) {
                auto gtg = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto etg = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0]));
                return gtg == etg; })
              .to_relationships(":containerOf", 7)
              .from_node("Forum")
              .where_qr_tuple([&] (const qr_tuple v) {
                auto f1 = boost::get<node *>(v[4]);
                auto f2 = boost::get<node *>(v[12]);
                return f1->id() != f2->id(); })
              .rship_exists({12, 3}, true)
              .where_qr_tuple([&] (const qr_tuple v) {
                return v[13].type() != typeid(null_t); })
              .to_relationships(":replyOf", 7)
              .from_node("Comment")
              .from_relationships(":hasCreator")
              .to_node("Person")
              .where_qr_tuple([&] (const qr_tuple v) {
                auto p1 = boost::get<node *>(v[6]);
                auto p2 = boost::get<node *>(v[17]);
                return p1->id() != p2->id(); })
              .group(grps1, {3})
              .aggregate(grps1, {{"count", 0}});

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
              .from_node("Post") // message1 (Post)
              .to_relationships(":containerOf")
              .from_node("Forum")
              .from_relationships(":hasMember")
              .to_node("Person"); // person2


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
              .from_node("Post") // message1 (Post)
              .from_relationships(":hasCreator")
              .to_node("Person")
              .to_relationships(":containerOf", 2)
              .from_node("Forum")
              .from_relationships(":hasMember")
              .to_node("Person") // person3
              .hashjoin_on_node({6, 4}, q3)
              .to_relationships(":hasCreator", 8)
              .from_node("Comment") // message2 (Comment)
              .project({PVar_(0), // tag
                        PVar_(2), // message1
                        PExpr_(2, pj::ptime_property(res, "creationDate")),
                        PVar_(4), // person1
                        PVar_(6), // forum1
                        PVar_(8), // person3
                        PVar_(15), // person2
                        PVar_(17), // message2 (Comment)
                        PExpr_(17, pj::ptime_property(res, "creationDate")) })
              .where_qr_tuple([&] (const qr_tuple v) {
                auto msg1_dt = boost::get<ptime>(v[2]);
                auto msg2_dt = boost::get<ptime>(v[8]);
                auto hrs = boost::get<int>(params[1]);
                return (msg1_dt + hours(hrs)) < msg2_dt; })
              .from_relationships(":hasTag", 7)
              .to_node("Tag")
              .property("name", [&](auto &prop) {
                auto gtg = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto etg = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0]));
                return gtg == etg; })
              .from_relationships({1, 100}, ":replyOf", 7)
              .to_node("Post")
              .to_relationships(":containerOf")
              .from_node("Forum")
              .where_qr_tuple([&] (const qr_tuple v) {
                auto f1 = boost::get<node *>(v[4]);
                auto f2 = boost::get<node *>(v[14]);
                return f1->id() != f2->id(); })
              .rship_exists({14, 3}, true)
              .where_qr_tuple([&] (const qr_tuple v) {
                return v[15].type() != typeid(null_t); })
              .to_relationships(":replyOf", 7)
              .from_node("Comment")
              .from_relationships(":hasCreator")
              .to_node("Person")
              .where_qr_tuple([&] (const qr_tuple v) {
                auto p1 = boost::get<node *>(v[6]);
                auto p2 = boost::get<node *>(v[19]);
                return p1->id() != p2->id(); })
              .group(grps2, {3})
              .aggregate(grps2, {{"count", 0}});

    auto q5 = query(gdb)
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
              .from_node("Comment") // message1 (Comment)
              .from_relationships({1, 100}, ":replyOf")
              .to_node("Post")
              .to_relationships(":containerOf")
              .from_node("Forum")
              .from_relationships(":hasMember")
              .to_node("Person"); // person2


    auto q6 = query(gdb)
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
              .from_node("Comment") // message1 (Comment)
              .from_relationships(":hasCreator")
              .to_node("Person")
              .from_relationships({1, 100}, ":replyOf", 2)
              .to_node("Post")
              .to_relationships(":containerOf")
              .from_node("Forum")
              .from_relationships(":hasMember")
              .to_node("Person") // person3
              .hashjoin_on_node({8, 6}, q5)
              .to_relationships(":hasCreator", 10)
              .from_node("Post") // message2 (Post)
              .project({PVar_(0), // tag
                        PVar_(2), // message1 (Comment)
                        PExpr_(2, pj::ptime_property(res, "creationDate")),
                        PVar_(4), // person1
                        PVar_(8), // forum1
                        PVar_(10), // person3
                        PVar_(19), // person2
                        PVar_(21), // message2 (Post)
                        PExpr_(21, pj::ptime_property(res, "creationDate")) })
              .where_qr_tuple([&] (const qr_tuple v) {
                auto msg1_dt = boost::get<ptime>(v[2]);
                auto msg2_dt = boost::get<ptime>(v[8]);
                auto hrs = boost::get<int>(params[1]);
                return (msg1_dt + hours(hrs)) < msg2_dt; })
              .from_relationships(":hasTag", 7)
              .to_node("Tag")
              .property("name", [&](auto &prop) {
                auto gtg = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto etg = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0]));
                return gtg == etg; })
              .to_relationships(":containerOf", 7)
              .from_node("Forum")
              .where_qr_tuple([&] (const qr_tuple v) {
                auto f1 = boost::get<node *>(v[4]);
                auto f2 = boost::get<node *>(v[12]);
                return f1->id() != f2->id(); })
              .rship_exists({12, 3}, true)
              .where_qr_tuple([&] (const qr_tuple v) {
                return v[13].type() != typeid(null_t); })
              .to_relationships(":replyOf", 7)
              .from_node("Comment")
              .from_relationships(":hasCreator")
              .to_node("Person")
              .where_qr_tuple([&] (const qr_tuple v) {
                auto p1 = boost::get<node *>(v[6]);
                auto p2 = boost::get<node *>(v[17]);
                return p1->id() != p2->id(); })
              .group(grps3, {3})
              .aggregate(grps3, {{"count", 0}});

    auto q7 = query(gdb)
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
              .from_node("Comment") // message1 (Comment)
              .from_relationships({1, 100}, ":replyOf")
              .to_node("Post")
              .to_relationships(":containerOf")
              .from_node("Forum")
              .from_relationships(":hasMember")
              .to_node("Person"); // person2


    auto q8 = query(gdb)
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
              .from_node("Comment") // message1 (Comment)
              .from_relationships(":hasCreator")
              .to_node("Person")
              .from_relationships({1, 100}, ":replyOf", 2)
              .to_node("Post")
              .to_relationships(":containerOf")
              .from_node("Forum")
              .from_relationships(":hasMember")
              .to_node("Person") // person3
              .hashjoin_on_node({8, 6}, q7)
              .to_relationships(":hasCreator", 10)
              .from_node("Comment") // message2 (Comment)
              .project({PVar_(0), // tag
                        PVar_(2), // message1 (Comment)
                        PExpr_(2, pj::ptime_property(res, "creationDate")),
                        PVar_(4), // person1
                        PVar_(8), // forum1
                        PVar_(10), // person3
                        PVar_(19), // person2
                        PVar_(21), // message2 (Comment)
                        PExpr_(21, pj::ptime_property(res, "creationDate")) })
              .where_qr_tuple([&] (const qr_tuple v) {
                auto msg1_dt = boost::get<ptime>(v[2]);
                auto msg2_dt = boost::get<ptime>(v[8]);
                auto hrs = boost::get<int>(params[1]);
                return (msg1_dt + hours(hrs)) < msg2_dt; })
              .from_relationships(":hasTag", 7)
              .to_node("Tag")
              .property("name", [&](auto &prop) {
                auto gtg = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto etg = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0]));
                return gtg == etg; })
              .from_relationships({1, 100}, ":replyOf", 7)
              .to_node("Post")
              .to_relationships(":containerOf")
              .from_node("Forum")
              .where_qr_tuple([&] (const qr_tuple v) {
                auto f1 = boost::get<node *>(v[4]);
                auto f2 = boost::get<node *>(v[12]);
                return f1->id() != f2->id(); })
              .rship_exists({14, 3})
              .where_qr_tuple([&] (const qr_tuple v) {
                return v[15].type() != typeid(null_t); })
              .to_relationships(":replyOf", 7)
              .from_node("Comment")
              .from_relationships(":hasCreator")
              .to_node("Person")
              .where_qr_tuple([&] (const qr_tuple v) {
                auto p1 = boost::get<node *>(v[6]);
                auto p2 = boost::get<node *>(v[19]);
                return p1->id() != p2->id(); })
              .group(grps4, {3})
              .aggregate(grps4, {{"count", 0}})
              .union_all({&q2, &q4, &q6}) // TODO
              .project({PExpr_(0, pj::uint64_property(res, "id")),
                        PVar_(1) })
              .orderby([&] (const qr_tuple q1, const qr_tuple q2) {
                return boost::get<uint64_t>(q1[0]) < boost::get<uint64_t>(q2[0]); })
              .collect(rs);

    query::start({&q1, &q2, &q3, &q4, &q5, &q6, &q7, &q8});
    rs.wait();
}

void ldbc_bi_query_18(graph_db_ptr &gdb, result_set &rs, params_tuple params) {
    std::vector<result_set> grps;

    // Query pipeline
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
              .from_relationships(":knows")
              .to_node("Person")
              .from_relationships(":knows")
              .to_node("Person")
              .rship_exists({0, 4})
              .where_qr_tuple([&] (const qr_tuple v) {
                return v[5].type() != typeid(null_t); })
              .from_relationships(":hasInterest", 4)
              .to_node("Tag")
              .property("name", [&](auto &prop) {
                auto gtg = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto etg = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[1]));
                return gtg == etg; })
              .group(grps, {4})
              .aggregate(grps, {{"count", 0}})
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

void ldbc_bi_query_19(graph_db_ptr &gdb, result_set &rs, params_tuple params) {

    auto cases = [&](const node &n1, const node &n2) {
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

    rship_predicate rpred = [&](relationship &r) {
        return std::string(gdb->get_string(r.rship_label)) == ":knows"; };

    rship_weight rweight = [&](relationship &r) {
        double w = 0.0;
        auto &src = gdb->node_by_id(r.from_node_id());
        auto &des = gdb->node_by_id(r.to_node_id());
        auto count = cases(src, des) + cases(des, src);
        return count == 0 ? 0 : 1 / (double)count; };

    // Query pipelines
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
              .to_relationships(":isLocatedIn")
              .from_node("Person");

    auto q2 = query(gdb)
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
              .to_relationships(":isLocatedIn")
              .from_node("Person")
              .crossjoin(q1)
              .limit(10) // TODO
              .algo_weighted_shortest_path({2, 5}, rpred, rweight, true)
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
              .collect(rs);

    query::start({&q1, &q2});
    rs.wait();
}

void ldbc_bi_query_20(graph_db_ptr &gdb, result_set &rs, params_tuple params) {

    rship_predicate rpred = [&](relationship &r) {
        return std::string(gdb->get_string(r.rship_label)) == ":knows"; };

    rship_weight rweight = [&](relationship &r) {
        double w = 0.0;
        node::id_t src_uni;
        node::id_t des_uni;
        int src_yr;
        int des_yr;
        auto &src = gdb->node_by_id(r.from_node_id());
        auto &des = gdb->node_by_id(r.to_node_id());
        gdb->foreach_from_relationship_of_node(src, ":studyAt", [&](relationship &r1) {
          src_uni = r1.to_node_id();
          auto descr = gdb->get_rship_description(r1.id());
          src_yr = get_property<int>(descr.properties, std::string("classYear")).value();
        });
        gdb->foreach_from_relationship_of_node(des, ":studyAt", [&](relationship &r2) {
          des_uni = r2.to_node_id();
          auto descr = gdb->get_rship_description(r2.id());
          des_yr = get_property<int>(descr.properties, std::string("classYear")).value();
        });
        if (src_uni == des_uni)
          w = std::abs(src_yr - des_yr) + 1;
        return w;   };

    // Query pipelines
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
              .limit(5) // TODO
              .algo_k_weighted_shortest_path({2, 3}, 20, rpred, rweight, true)
              // .algo_weighted_shortest_path({2, 3}, rpred, rweight, true)
              .project({PExpr_(2, pj::uint64_property(res, "id")),
                        PVar_(4) })
              .orderby([&](const qr_tuple &q1, const qr_tuple &q2) {
                return boost::get<uint64_t>(q1[0]) < boost::get<uint64_t>(q2[0]); })
              .collect(rs);

    query::start({&q1, &q2});
    rs.wait();
}

/* ------------------------------------------------------------------------ */

double calc_avg_time(const std::vector<double>& vec) {
    double d = 0.0;
    for (auto v : vec)
      d += v;
    return d / (double)vec.size();
}

double run_query_1(graph_db_ptr gdb) {
    std::vector<params_tuple> params =
        {{time_from_string(std::string("2017-04-14 01:51:21.746"))}};

    std::vector<double> runtimes(params.size());

    for (auto i = 0u; i < params.size(); i++) {
        result_set rs;
        auto start_qp = std::chrono::steady_clock::now();

        auto tx = gdb->begin_transaction();
        ldbc_bi_query_1(gdb, rs, params[i]);
        gdb->commit_transaction();

        auto end_qp = std::chrono::steady_clock::now();
        runtimes[i] = std::chrono::duration_cast<std::chrono::milliseconds>(end_qp -
                                                                       start_qp).count();
#ifdef PRINT_RESULT
        std::cout << rs << "\n";
#endif
    }
    return calc_avg_time(runtimes);
}

double run_query_2(graph_db_ptr gdb) {
    std::vector<params_tuple> params = {{time_from_string(std::string("2011-04-14 01:51:21.746"))}};

    std::vector<double> runtimes(params.size());

    for (auto i = 0u; i < params.size(); i++) {
        result_set rs;
        auto start_qp = std::chrono::steady_clock::now();

        auto tx = gdb->begin_transaction();
        ldbc_bi_query_2(gdb, rs, params[i]);
        gdb->commit_transaction();

        auto end_qp = std::chrono::steady_clock::now();
        runtimes[i] = std::chrono::duration_cast<std::chrono::milliseconds>(end_qp -
                                                                       start_qp).count();
#ifdef PRINT_RESULT
        std::cout << rs << "\n";
#endif
    }
    return calc_avg_time(runtimes);
}

double run_query_3(graph_db_ptr gdb) {
    std::vector<params_tuple> params = {{"TennisPlayer", "United_States"}};

    std::vector<double> runtimes(params.size());

    for (auto i = 0u; i < params.size(); i++) {
        result_set rs;
        auto start_qp = std::chrono::steady_clock::now();

        auto tx = gdb->begin_transaction();
        ldbc_bi_query_3(gdb, rs, params[i]);
        gdb->commit_transaction();

        auto end_qp = std::chrono::steady_clock::now();
        runtimes[i] = std::chrono::duration_cast<std::chrono::milliseconds>(end_qp -
                                                                       start_qp).count();
#ifdef PRINT_RESULT
        std::cout << rs << "\n";
#endif
    }
    return calc_avg_time(runtimes);
}

double run_query_4(graph_db_ptr gdb) {
    std::vector<params_tuple> params = {{"Turkey", time_from_string(std::string("2011-04-14 01:51:21.746"))}};

    std::vector<double> runtimes(params.size());

    for (auto i = 0u; i < params.size(); i++) {
        result_set rs;
        auto start_qp = std::chrono::steady_clock::now();

        auto tx = gdb->begin_transaction();
        ldbc_bi_query_4(gdb, rs, params[i]);
        gdb->commit_transaction();

        auto end_qp = std::chrono::steady_clock::now();
        runtimes[i] = std::chrono::duration_cast<std::chrono::milliseconds>(end_qp -
                                                                       start_qp).count();
#ifdef PRINT_RESULT
        std::cout << rs << "\n";
#endif
    }
    return calc_avg_time(runtimes);
}

double run_query_5(graph_db_ptr gdb) {
    std::vector<params_tuple> params = {{"Arthur_Conan_Doyle"}};

    std::vector<double> runtimes(params.size());

    for (auto i = 0u; i < params.size(); i++) {
        result_set rs;
        auto start_qp = std::chrono::steady_clock::now();

        auto tx = gdb->begin_transaction();
        ldbc_bi_query_5(gdb, rs, params[i]);
        gdb->commit_transaction();

        auto end_qp = std::chrono::steady_clock::now();
        runtimes[i] = std::chrono::duration_cast<std::chrono::milliseconds>(end_qp -
                                                                       start_qp).count();
#ifdef PRINT_RESULT
        std::cout << rs << "\n";
#endif
    }
    return calc_avg_time(runtimes);
}

double run_query_6(graph_db_ptr gdb) {
    std::vector<params_tuple> params = {{"Deep_Sea_Skiving"}};

    std::vector<double> runtimes(params.size());

    for (auto i = 0u; i < params.size(); i++) {
        result_set rs;
        auto start_qp = std::chrono::steady_clock::now();

        auto tx = gdb->begin_transaction();
        ldbc_bi_query_6(gdb, rs, params[i]);
        gdb->commit_transaction();

        auto end_qp = std::chrono::steady_clock::now();
        runtimes[i] = std::chrono::duration_cast<std::chrono::milliseconds>(end_qp -
                                                                       start_qp).count();
#ifdef PRINT_RESULT
        std::cout << rs << "\n";
#endif
    }
    return calc_avg_time(runtimes);
}

double run_query_7(graph_db_ptr gdb) {
    std::vector<params_tuple> params = {{"Zulu_Kingdom"}};

    std::vector<double> runtimes(params.size());

    for (auto i = 0u; i < params.size(); i++) {
        result_set rs;
        auto start_qp = std::chrono::steady_clock::now();

        auto tx = gdb->begin_transaction();
        ldbc_bi_query_7(gdb, rs, params[i]);
        gdb->commit_transaction();

        auto end_qp = std::chrono::steady_clock::now();
        runtimes[i] = std::chrono::duration_cast<std::chrono::milliseconds>(end_qp -
                                                                       start_qp).count();
#ifdef PRINT_RESULT
        std::cout << rs << "\n";
#endif
    }
    return calc_avg_time(runtimes);
}

double run_query_8(graph_db_ptr gdb) {
    std::vector<params_tuple> params = {{"Garry_Kasparov",
        time_from_string(std::string("2011-04-14 01:51:21.746"))
                                      /*(uint64_t)1311285600000*/}};

    std::vector<double> runtimes(params.size());

    for (auto i = 0u; i < params.size(); i++) {
        result_set rs;
        auto start_qp = std::chrono::steady_clock::now();

        auto tx = gdb->begin_transaction();
        ldbc_bi_query_8(gdb, rs, params[i]);
        gdb->commit_transaction();

        auto end_qp = std::chrono::steady_clock::now();
        runtimes[i] = std::chrono::duration_cast<std::chrono::milliseconds>(end_qp -
                                                                       start_qp).count();
#ifdef PRINT_RESULT
        std::cout << rs << "\n";
#endif
    }
    return calc_avg_time(runtimes);
}

double run_query_9(graph_db_ptr gdb) {
    std::vector<params_tuple> params =
        {{time_from_string(std::string("2010-10-30 01:51:21.746")),
        time_from_string(std::string("2014-04-14 01:51:21.746"))}};

    std::vector<double> runtimes(params.size());

    for (auto i = 0u; i < params.size(); i++) {
        result_set rs;
        auto start_qp = std::chrono::steady_clock::now();

        auto tx = gdb->begin_transaction();
        ldbc_bi_query_9(gdb, rs, params[i]);
        gdb->commit_transaction();

        auto end_qp = std::chrono::steady_clock::now();
        runtimes[i] = std::chrono::duration_cast<std::chrono::milliseconds>(end_qp -
                                                                       start_qp).count();
#ifdef PRINT_RESULT
        std::cout << rs << "\n";
#endif
    }
    return calc_avg_time(runtimes);
}

double run_query_10(graph_db_ptr gdb) {
    std::vector<params_tuple> params =
        {{(uint64_t)13194139538042, "India", "MusicalArtist", 3, 5}};

    std::vector<double> runtimes(params.size());

    for (auto i = 0u; i < params.size(); i++) {
        result_set rs;
        auto start_qp = std::chrono::steady_clock::now();

        auto tx = gdb->begin_transaction();
        ldbc_bi_query_10(gdb, rs, params[i]);
        gdb->commit_transaction();

        auto end_qp = std::chrono::steady_clock::now();
        runtimes[i] = std::chrono::duration_cast<std::chrono::milliseconds>(end_qp -
                                                                       start_qp).count();
#ifdef PRINT_RESULT
        std::cout << rs << "\n";
#endif
    }
    return calc_avg_time(runtimes);
}

double run_query_11(graph_db_ptr gdb) {
    std::vector<params_tuple> params = {{"England"}};

    std::vector<double> runtimes(params.size());

    for (auto i = 0u; i < params.size(); i++) {
        result_set rs;
        auto start_qp = std::chrono::steady_clock::now();

        auto tx = gdb->begin_transaction();
        ldbc_bi_query_11(gdb, rs, params[i]);
        gdb->commit_transaction();

        auto end_qp = std::chrono::steady_clock::now();
        runtimes[i] = std::chrono::duration_cast<std::chrono::milliseconds>(end_qp -
                                                                       start_qp).count();
#ifdef PRINT_RESULT
        std::cout << rs << "\n";
#endif
    }
    return calc_avg_time(runtimes);
}

double run_query_12(graph_db_ptr gdb) {
    std::vector<params_tuple> params = {{time_from_string(std::string("2010-10-30 01:51:21.746")), 100, "ar"}};

    std::vector<double> runtimes(params.size());

    for (auto i = 0u; i < params.size(); i++) {
        result_set rs;
        auto start_qp = std::chrono::steady_clock::now();

        auto tx = gdb->begin_transaction();
        ldbc_bi_query_12(gdb, rs, params[i]);
        gdb->commit_transaction();

        auto end_qp = std::chrono::steady_clock::now();
        runtimes[i] = std::chrono::duration_cast<std::chrono::milliseconds>(end_qp -
                                                                       start_qp).count();
#ifdef PRINT_RESULT
        std::cout << rs << "\n";
#endif
    }
    return calc_avg_time(runtimes);
}

double run_query_13(graph_db_ptr gdb) {
    std::vector<params_tuple> params = {{"Spain", time_from_string(std::string("2010-10-30 01:51:21.746"))}};

    std::vector<double> runtimes(params.size());

    for (auto i = 0u; i < params.size(); i++) {
        result_set rs;
        auto start_qp = std::chrono::steady_clock::now();

        auto tx = gdb->begin_transaction();
        ldbc_bi_query_13(gdb, rs, params[i]);
        gdb->commit_transaction();

        auto end_qp = std::chrono::steady_clock::now();
        runtimes[i] = std::chrono::duration_cast<std::chrono::milliseconds>(end_qp -
                                                                       start_qp).count();
#ifdef PRINT_RESULT
        std::cout << rs << "\n";
#endif
    }
    return calc_avg_time(runtimes);
}

double run_query_14(graph_db_ptr gdb) {
    std::vector<params_tuple> params = {{"Mexico", "Indonesia"}};

    std::vector<double> runtimes(params.size());

    for (auto i = 0u; i < params.size(); i++) {
        result_set rs;
        auto start_qp = std::chrono::steady_clock::now();

        auto tx = gdb->begin_transaction();
        ldbc_bi_query_14(gdb, rs, params[i]);
        gdb->commit_transaction();

        auto end_qp = std::chrono::steady_clock::now();
        runtimes[i] = std::chrono::duration_cast<std::chrono::milliseconds>(end_qp -
                                                                       start_qp).count();
#ifdef PRINT_RESULT
        std::cout << rs << "\n";
#endif
    }
    return calc_avg_time(runtimes);
}

double run_query_15(graph_db_ptr gdb) {
    std::vector<params_tuple> params = {{(uint64_t)933, (uint64_t)4139, 
      time_from_string(std::string("2010-01-22 21:45:42.621")),
      time_from_string(std::string("2011-10-12 03:05:14.333"))}};

    std::vector<double> runtimes(params.size());

    for (auto i = 0u; i < params.size(); i++) {
        result_set rs;
        auto start_qp = std::chrono::steady_clock::now();

        auto tx = gdb->begin_transaction();
        ldbc_bi_query_15(gdb, rs, params[i]);
        gdb->commit_transaction();

        auto end_qp = std::chrono::steady_clock::now();
        runtimes[i] = std::chrono::duration_cast<std::chrono::milliseconds>(end_qp -
                                                                       start_qp).count();
#ifdef PRINT_RESULT
        std::cout << rs << "\n";
#endif
    }
    return calc_avg_time(runtimes);
}

double run_query_16(graph_db_ptr gdb) {
    std::vector<params_tuple> params = {{"George_Frideric_Handel",
      time_from_string(std::string("2011-10-22 21:45:42.621")), "Napoleon",
      time_from_string(std::string("2011-06-02 08:39:49.912")), 3}};

    std::vector<double> runtimes(params.size());

    for (auto i = 0u; i < params.size(); i++) {
        result_set rs;
        auto start_qp = std::chrono::steady_clock::now();

        auto tx = gdb->begin_transaction();
        ldbc_bi_query_16(gdb, rs, params[i]);
        gdb->commit_transaction();

        auto end_qp = std::chrono::steady_clock::now();
        runtimes[i] = std::chrono::duration_cast<std::chrono::milliseconds>(end_qp -
                                                                       start_qp).count();
#ifdef PRINT_RESULT
        std::cout << rs << "\n";
#endif
    }
    return calc_avg_time(runtimes);
}

double run_query_17(graph_db_ptr gdb) {
    std::vector<params_tuple> params = {{"Napoleon", 3}};

    std::vector<double> runtimes(params.size());

    for (auto i = 0u; i < params.size(); i++) {
        result_set rs;
        auto start_qp = std::chrono::steady_clock::now();

        auto tx = gdb->begin_transaction();
        ldbc_bi_query_17(gdb, rs, params[i]);
        gdb->commit_transaction();

        auto end_qp = std::chrono::steady_clock::now();
        runtimes[i] = std::chrono::duration_cast<std::chrono::milliseconds>(end_qp -
                                                                       start_qp).count();
#ifdef PRINT_RESULT
        std::cout << rs << "\n";
#endif
    }
    return calc_avg_time(runtimes);
}

double run_query_18(graph_db_ptr gdb) {
    std::vector<params_tuple> params = {{(uint64_t)1129, "Ferdinand_Marcos"}};

    std::vector<double> runtimes(params.size());

    for (auto i = 0u; i < params.size(); i++) {
        result_set rs;
        auto start_qp = std::chrono::steady_clock::now();

        auto tx = gdb->begin_transaction();
        ldbc_bi_query_18(gdb, rs, params[i]);
        gdb->commit_transaction();

        auto end_qp = std::chrono::steady_clock::now();
        runtimes[i] = std::chrono::duration_cast<std::chrono::milliseconds>(end_qp -
                                                                       start_qp).count();
#ifdef PRINT_RESULT
        std::cout << rs << "\n";
#endif
    }
    return calc_avg_time(runtimes);
}

double run_query_19(graph_db_ptr gdb) {
    std::vector<params_tuple> params = {{"London", "Glasgow"}};

    std::vector<double> runtimes(params.size());

    for (auto i = 0u; i < params.size(); i++) {
        result_set rs;
        auto start_qp = std::chrono::steady_clock::now();

        auto tx = gdb->begin_transaction();
        ldbc_bi_query_19(gdb, rs, params[i]);
        gdb->commit_transaction();

        auto end_qp = std::chrono::steady_clock::now();
        runtimes[i] = std::chrono::duration_cast<std::chrono::milliseconds>(end_qp -
                                                                       start_qp).count();
#ifdef PRINT_RESULT
        std::cout << rs << "\n";
#endif
    }
    return calc_avg_time(runtimes);
}

double run_query_20(graph_db_ptr gdb) {
    std::vector<params_tuple> params = {{"Kam_Air", (uint64_t)2199023256684}};

    std::vector<double> runtimes(params.size());

    for (auto i = 0u; i < params.size(); i++) {
        result_set rs;
        auto start_qp = std::chrono::steady_clock::now();

        auto tx = gdb->begin_transaction();
        ldbc_bi_query_20(gdb, rs, params[i]);
        gdb->commit_transaction();

        auto end_qp = std::chrono::steady_clock::now();
        runtimes[i] = std::chrono::duration_cast<std::chrono::milliseconds>(end_qp -
                                                                       start_qp).count();
#ifdef PRINT_RESULT
        std::cout << rs << "\n";
#endif
    }
    return calc_avg_time(runtimes);
}

void run_benchmark(graph_db_ptr gdb) {
    double t = 0.0;
    t = run_query_1(gdb);
    spdlog::info("Query #1: {} msecs", t);
    t = run_query_2(gdb);
    spdlog::info("Query #2: {} msecs", t);
    t = run_query_3(gdb);
    spdlog::info("Query #3: {} msecs", t);
    t = run_query_4(gdb);
    spdlog::info("Query #4: {} msecs", t);
    t = run_query_5(gdb);
    spdlog::info("Query #5: {} msecs", t);
    t = run_query_6(gdb);
    spdlog::info("Query #6: {} msecs", t);
    t = run_query_7(gdb);
    spdlog::info("Query #7: {} msecs", t);
    t = run_query_8(gdb);
    spdlog::info("Query #8: {} msecs", t);
    t = run_query_9(gdb);
    spdlog::info("Query #9: {} msecs", t);
    t = run_query_10(gdb);
    spdlog::info("Query #10: {} msecs", t);
    t = run_query_11(gdb);
    spdlog::info("Query #11: {} msecs", t);
    t = run_query_12(gdb);
    spdlog::info("Query #12: {} msecs", t);
    t = run_query_13(gdb);
    spdlog::info("Query #13: {} msecs", t);
    t = run_query_14(gdb);
    spdlog::info("Query #14: {} msecs", t);
    t = run_query_15(gdb);
    spdlog::info("Query #15: {} msecs", t);
    t = run_query_16(gdb);
    spdlog::info("Query #16: {} msecs", t);
    t = run_query_17(gdb);
    spdlog::info("Query #17: {} msecs", t);
    t = run_query_18(gdb);
    spdlog::info("Query #18: {} msecs", t);
    t = run_query_19(gdb);
    spdlog::info("Query #19: {} msecs", t);
    t = run_query_20(gdb);
    spdlog::info("Query #20: {} msecs", t);
}

/* ---------------------------------------------------------------------------- */

using namespace boost::program_options;

int main(int argc, char **argv) {
  bool strict = false;
  std::string pool_path, db_name;
  std::string snb_home =
#ifdef SF_10
    "/home/data/SNB_SF_10/";
#else
    "/home/data/SNB_SF_1/";
#endif

 try {
    options_description desc{"Options"};
    desc.add_options()
        ("help,h", "Help")
        ("verbose,v", bool_switch()->default_value(false), "Verbose - show debug output")
        ("import,i", value<std::string>(&snb_home), "Path to directories containing SNB CSV files")
        ("strict,s", bool_switch()->default_value(false), "Strict mode - assumes that all columns contain values of the same type")
        ("pool,p", value<std::string>(&pool_path)->required(), "Path to the PMem pool")
        ("db,d", value<std::string>(&db_name)->required(),"Database name (required)");

    variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);

    if (vm.count("help")) {
      std::cout << "Poseidon Graph Database LDBC Benchmark Version " << POSEIDON_VERSION
                << "\n"
                << desc << '\n';
      return -1;
    }
    if (vm.count("import"))
      snb_home = vm["import"].as<std::string>();

    if (vm.count("strict"))
      strict = vm["strict"].as<bool>();

    if (vm.count("db_name"))
      db_name = vm["db_name"].as<std::string>();

    if (vm.count("pool"))
      pool_path = vm["pool"].as<std::string>();

    notify(vm);

      } catch (const error &ex) {
    std::cerr << ex.what() << '\n';
    return -1;
  }

#ifdef USE_PMDK
    auto pool = graph_pool::open(pool_path);
    auto graph = pool->open_graph(db_name);
    #ifdef FPTree
    fptree_recovery(graph);
    #endif
#else
  auto pool = graph_pool::create(pool_path);
  auto graph = pool->create_graph(db_name);

  load_snb_data(graph, snb_home);
#endif
  graph->print_stats();

  run_benchmark(graph);
}

#ifdef v_3_2

void ldbc_bi_query_2(graph_db_ptr &gdb, result_set &rs, params_tuple params) {

    std::vector<result_set> grps;
    std::vector<std::string> message = {"Post", "Comment"};

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

    auto group_age =
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
                        projection::expr(2, group_age),
                        PExpr_(8, pj::string_property(res, "name")), })
              .group(grps, {0, 1, 2, 3, 4})
              .aggregate(grps, {{"count", 0}})
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

void ldbc_bi_query_9(graph_db_ptr &gdb, result_set &rs, params_tuple params) {

    std::vector<result_set> grps1;
    std::vector<result_set> grps2;
    std::vector<result_set> grps3;

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
      [&](qr_tuple &v) {
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
              .group(grps2, {6})
              .aggregate(grps2, {{"count", 0}});

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
              .group(grps1, {6})
              .aggregate(grps1, {{"count", 0}})
              .hashjoin_on_node({0, 0}, q2)
              .from_relationships(":hasMember", 0)
              .to_node("Person")
              .group(grps3, {0, 1, 3})
              .aggregate(grps3, {{"count", 0}})
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

void ldbc_bi_query_11(graph_db_ptr &gdb, result_set &rs, params_tuple params) {

    std::vector<result_set> grps1;
    std::vector<result_set> grps2;

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
              .group(grps1, {4})
              .aggregate(grps1, {{"count", 0}});

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
              .group(grps2, {4})
              .aggregate(grps2, {{"count", 0}})
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

void ldbc_bi_query_12(graph_db_ptr &gdb, result_set &rs, params_tuple params) {

    std::vector<result_set> grps;

    // Query pipeline
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
              .group(grps, {0})
              .aggregate(grps, {{"count", 0}})
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

void ldbc_bi_query_13(graph_db_ptr &gdb, result_set &rs, params_tuple params) {

    std::vector<result_set> grps1;
    std::vector<result_set> grps2;
    std::vector<result_set> grps3;

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
              // .group(grps2, {6})
              // .aggregate(grps2, {{"count", 0}})
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
//               .group(grps1, {6})
//               .aggregate(grps1, {{"count", 0}})
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
//               .group(grps3, {3})
//               .aggregate(grps3, {{"count", 0}})
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