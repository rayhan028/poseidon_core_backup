#include <iostream>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>

#include "defs.hpp"
#include "graph_db.hpp"
#include "graph_pool.hpp"
#include "ldbc.hpp"
#include "config.h"
#include "qop.hpp"
#include "query.hpp"

#include "threadsafe_queue.hpp"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"

namespace pj = builtin;
using namespace boost::posix_time;

/* ------------------------------------------------------------------------ */

void ldbc_bi_query_1(graph_db_ptr &gdb, result_set &rs, ptime dt) {

    std::vector<result_set> grps;
    std::vector<std::string> message = {"Post", "Comment"};

    auto filter_cdate =
      [&](auto &prop) {
        if ((prop.flags_ & 0xe0) == prop.p_ptime)
          return (*(reinterpret_cast<const ptime *>(prop.value_))) < dt;
        throw invalid_typecast();
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

void ldbc_bi_query_3(graph_db_ptr &gdb, result_set &rs, params_tuple params) {

    std::vector<result_set> grps1;
    std::vector<result_set> grps2;
    std::vector<std::string> message = {"Post", "Comment"};

    auto filter_cdate_1 =
      [&](auto &p) {
        if ((p.flags_ & 0xe0) == p.p_ptime) {
          auto yr = boost::get<int>(params[0]);
          auto mo = boost::get<int>(params[1]);
          auto dt = *(reinterpret_cast<const ptime *>(p.value_));
          auto dts = to_iso_extended_string(dt);
          auto year = std::stoi(dts.substr(0, dts.find("-")));
          auto month = std::stoi(dts.substr(5, 2));
          return year == yr && month == mo;
        }
        throw invalid_typecast(); 
      };

    auto filter_cdate_2 =
      [&](auto &p) {
        if ((p.flags_ & 0xe0) == p.p_ptime) {
          auto nxt_yr = boost::get<int>(params[0]);
          auto nxt_mo = boost::get<int>(params[1]) + 1;
          if (nxt_mo > 12) {
            nxt_mo %= 12;
            ++nxt_yr;
          }
          auto dt = *(reinterpret_cast<const ptime *>(p.value_));
          auto dts = to_iso_extended_string(dt);
          auto year = std::stoi(dts.substr(0, dts.find("-")));
          auto month = std::stoi(dts.substr(5, 2));
          return year == nxt_yr && month == nxt_mo;
        }
        throw invalid_typecast(); 
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

void ldbc_bi_query_4(graph_db_ptr &gdb, result_set &rs, params_tuple params) {

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
              .to_node("Post")
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

void ldbc_bi_query_5(graph_db_ptr &gdb, result_set &rs, params_tuple params) {

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
              .to_relationships(":isPartOf")
              .from_node("Place")
              .to_relationships(":isLocatedIn")
              .from_node("Person")
              .to_relationships(":hasMember")
              .from_node("Forum")
              .group(grps2, {6})
              .aggregate(grps2, {{"count", 0}})
              .orderby([&](const qr_tuple &q1, const qr_tuple &q2) {
                return boost::get<uint64_t>(q1[1]) > boost::get<uint64_t>(q2[1]); })
              .limit(100);

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
              .group(grps1, {6})
              .aggregate(grps1, {{"count", 0}})
              .orderby([&](const qr_tuple &q1, const qr_tuple &q2) {
                return boost::get<uint64_t>(q1[1]) > boost::get<uint64_t>(q2[1]); })
              .limit(100)
              .from_relationships(":hasMember", 0)
              .to_node("Person")
              .to_relationships(":hasCreator")
              .from_node("Post")
              .to_relationships(":containerOf")
              .from_node("Forum")
              .join_on_node({7, 0}, q2)
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

    query::start({&q2, &q1});
    rs.wait();
}

void ldbc_bi_query_6(graph_db_ptr &gdb, result_set &rs, params_tuple params) {

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

/* ------------------------------------------------------------------------ */

double calc_avg_time(const std::vector<double>& vec) {
    double d = 0.0;
    for (auto v : vec)
      d += v;
    return d / (double)vec.size();
}

double run_query_1(graph_db_ptr gdb) {
    std::vector<ptime> dates =
        {time_from_string(std::string("2017-04-14 01:51:21.746"))};

    std::vector<double> runtimes(dates.size());

    for (auto i = 0u; i < dates.size(); i++) {
        result_set rs;
        auto start_qp = std::chrono::steady_clock::now();

        auto tx = gdb->begin_transaction();
        ldbc_bi_query_1(gdb, rs, dates[i]);
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
    std::vector<params_tuple> params =
        {{time_from_string(std::string("2011-04-14 01:51:21.746")),
        time_from_string(std::string("2012-04-14 01:51:21.746")),
        "Germany", "India"}};

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
    std::vector<params_tuple> params = {{2012, 4}};

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
    std::vector<params_tuple> params = {{"TennisPlayer", "United_States"}};

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
    std::vector<params_tuple> params = {{"Turkey"}};

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
    std::vector<params_tuple> params = {{"Arthur_Conan_Doyle"}};

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