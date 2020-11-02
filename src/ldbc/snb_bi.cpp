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

    auto q = query(gdb) // TODO add range index scan
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Post")
              .property( "creationDate",
                           [&](auto &p) {
                              if ((p.flags_ & 0xe0) == p.p_ptime)
                                return (*(reinterpret_cast<const ptime *>(p.value_))) < dt;
                              throw invalid_typecast(); })
#else
              .nodes_where("Post", "creationDate",
                            [&](auto &p) {
                              if ((p.flags_ & 0xe0) == p.p_ptime)
                                return (*(reinterpret_cast<const ptime *>(p.value_))) < dt;
                              throw invalid_typecast(); })
#endif
              .project({PExpr_(0, pj::pr_year(res, "creationDate")),
                        PExpr_(0, (pj::has_property(res, "language")) ?
                            std::string("False") : std::string("True")),
                        PExpr_(0, pj::int_property(res, "length")),
                        projection::expr(0, [&](auto res) {
                          auto len = boost::get<int>(pj::int_property(res, "length"));
                          return (len >= 0 && len < 40) ? query_result(std::string("0")) :
                          (len >= 40 && len < 80) ? query_result(std::string("1")) :
                          (len >= 80 && len < 160) ? query_result(std::string("2")) : query_result(std::string("3")); })
                        })
              .groupby({0, 1, 3}, {{1, 1}, {3, 2}, {2, 2}} )
              .orderby([&](const qr_tuple &qr1, const qr_tuple &qr2) {
                        if (boost::get<int>(qr1[0]) == boost::get<int>(qr2[0])) {
                          if (boost::get<std::string>(qr1[1]) == boost::get<std::string>(qr2[1]))
                            return boost::get<std::string>(qr1[2]) < boost::get<std::string>(qr2[2]);
                          else
                            return boost::get<std::string>(qr1[1]) < boost::get<std::string>(qr2[1]);
                        }
                        else
                          return boost::get<int>(qr1[0]) > boost::get<int>(qr2[0]); })
              .collect(rs);
    q.start();
    rs.wait();
}

/* ------------------------------------------------------------------------ */

double calc_avg_time(const std::vector<double>& vec) {
    double d = 0.0;
    for (auto v : vec) {
        d += v;
        std::cout << v << " ";
    }
    // std::cout << "\n";
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

void run_benchmark(graph_db_ptr gdb) {
    double t = 0.0;
    t = run_query_1(gdb);
    spdlog::info("Query #1: {} msecs", t);
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