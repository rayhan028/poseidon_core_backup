#include <iostream>
#include <regex>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>

#include "defs.hpp"
#include "gtpc.hpp"
#include "config.h"
#include "graph_pool.hpp"

#include "threadsafe_queue.hpp"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"

using namespace boost::posix_time;

/* ------------------------------------------------------------------------ */

double calc_avg_time(const std::vector<double>& vec) {
    double d = 0.0;
    for (auto v : vec)
      d += v;
    return d / (double)vec.size();
}

double run_query_1(graph_db_ptr gdb) {
    std::vector<params_tuple> params =
        {{time_from_string(std::string("2010-02-14 00:00:00.000")), 90}};

    std::vector<double> runtimes(params.size());

    for (auto i = 0u; i < params.size(); i++) {
        result_set rs;
        auto start_qp = std::chrono::steady_clock::now();

        gdb->run_transaction([&]() {
          gtpch_query_1(gdb, rs, params[i]);
          return true;
        });

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
    std::vector<params_tuple> params = {{"b", "EUROPE"}};

    std::vector<double> runtimes(params.size());

    for (auto i = 0u; i < params.size(); i++) {
        result_set rs;
        auto start_qp = std::chrono::steady_clock::now();

        gdb->run_transaction([&]() {
          gtpch_query_2(gdb, rs, params[i]);
          return true;
        });

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
    std::vector<params_tuple> params =
        {{"A", time_from_string(std::string("2011-10-30 00:00:00.000"))}};

    std::vector<double> runtimes(params.size());

    for (auto i = 0u; i < params.size(); i++) {
        result_set rs;
        auto start_qp = std::chrono::steady_clock::now();

        gdb->run_transaction([&]() {
          gtpch_query_3(gdb, rs, params[i]);
          return true;
        });

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
    std::vector<params_tuple> params =
        {{time_from_string(std::string("2011-08-01 00:00:00.000"))}};

    std::vector<double> runtimes(params.size());

    for (auto i = 0u; i < params.size(); i++) {
        result_set rs;
        auto start_qp = std::chrono::steady_clock::now();

        gdb->run_transaction([&]() {
          gtpch_query_4(gdb, rs, params[i]);
          return true;
        });

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
    std::vector<params_tuple> params =
        {{"ASIA", time_from_string(std::string("2010-01-01 00:00:00.000"))}};

    std::vector<double> runtimes(params.size());

    for (auto i = 0u; i < params.size(); i++) {
        result_set rs;
        auto start_qp = std::chrono::steady_clock::now();

        gdb->run_transaction([&]() {
          gtpch_query_5(gdb, rs, params[i]);
          return true;
        });

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
    std::vector<params_tuple> params =
        {{time_from_string(std::string("2011-04-01 00:00:00.000")), 5}};

    std::vector<double> runtimes(params.size());

    for (auto i = 0u; i < params.size(); i++) {
        result_set rs;
        auto start_qp = std::chrono::steady_clock::now();

        gdb->run_transaction([&]() {
          gtpch_query_6(gdb, rs, params[i]);
          return true;
        });

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
  std::string pool_path, gtpc_home, db_name;

 try {
    options_description desc{"Options"};
    desc.add_options()
        ("help,h", "Help")
        ("verbose,v", bool_switch()->default_value(false), "Verbose - show debug output")
        ("import,i", value<std::string>(&gtpc_home), "Path to directories containing SNB CSV files")
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
      gtpc_home = vm["import"].as<std::string>();

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
    #if defined(RUN_INDEXED) && defined(FPTree)
    gtpc_fptree_recovery(graph);
    #endif
#else
  auto pool = graph_pool::create(pool_path);
  auto graph = pool->create_graph(db_name);

  load_gtpc_data(graph, gtpc_home);
#endif
  graph->print_stats();

  run_benchmark(graph);
}