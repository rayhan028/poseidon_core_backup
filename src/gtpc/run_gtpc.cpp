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
using transaction_func = std::function<void (graph_db_ptr &, result_set &)>;
using query_func = std::function<void (graph_db_ptr &, result_set &, params_tuple &)>;

/* ------------------------------------------------------------------------ */

double calc_avg_time(const std::vector<double>& vec) {
    double d = 0.0;
    for (auto v : vec)
      d += v;
    return d / (double)vec.size();
}

double run_query(graph_db_ptr gdb, const query_func &qr_func, std::vector<params_tuple> &params) {

    std::vector<double> runtimes(params.size());

    for (auto i = 0u; i < params.size(); i++) {
        result_set rs;
        auto start_qp = std::chrono::steady_clock::now();

        gdb->run_transaction([&]() {
          qr_func(gdb, rs, params[i]);
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

double run_transaction(graph_db_ptr gdb, const transaction_func &txn_func) {

    auto streams = 1u;
    std::vector<double> runtimes(streams);

    for (auto i = 0u; i < streams; i++) {
        result_set rs;
        auto start_qp = std::chrono::steady_clock::now();

        gdb->run_transaction([&]() {
          txn_func(gdb, rs);
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

void run_queries(graph_db_ptr gdb) {
    std::vector<params_tuple> gtpc_query_1_params =
        {{time_from_string(std::string("2010-02-14 00:00:00.000")), 90}};

    std::vector<params_tuple> gtpc_query_2_params = {{"b", "EUROPE"}};

    std::vector<params_tuple> gtpc_query_3_params =
        {{"A", time_from_string(std::string("2011-10-30 00:00:00.000"))}};

    std::vector<params_tuple> gtpc_query_4_params =
        {{time_from_string(std::string("2011-08-01 00:00:00.000"))}};

    std::vector<params_tuple> gtpc_query_5_params =
        {{"ASIA", time_from_string(std::string("2010-01-01 00:00:00.000"))}};

    std::vector<params_tuple> gtpc_query_6_params =
        {{time_from_string(std::string("2011-04-01 00:00:00.000")), 5}};

    std::vector<params_tuple> gtpc_query_7_params =
        {{"FRANCE", "GERMANY", time_from_string(std::string("2011-01-01 00:00:00.000")),
        time_from_string(std::string("2012-01-01 00:00:00.000"))}};

    std::vector<params_tuple> gtpc_query_8_params =
        {{"EUROPE", "BRAZIL", "b", time_from_string(std::string("2011-01-01 00:00:00.000")),
        time_from_string(std::string("2012-01-01 00:00:00.000"))}};

    std::vector<params_tuple> gtpc_query_9_params = {{"BB"}};

    std::vector<params_tuple> gtpc_query_10_params =
        {{time_from_string(std::string("2011-08-01 00:00:00.000"))}};

    std::vector<params_tuple> gtpc_query_11_params =
        {{"GERMANY", 0.0001}};

    std::vector<params_tuple> gtpc_query_12_params =
        {{time_from_string(std::string("2011-04-01 00:00:00.000"))}};

    std::vector<params_tuple> gtpc_query_13_params = {{8}};

    std::vector<params_tuple> gtpc_query_14_params =
        {{time_from_string(std::string("2011-04-01 00:00:00.000")), "PR"}};

    std::vector<params_tuple> gtpc_query_15_params =
        {{time_from_string(std::string("2011-04-01 00:00:00.000")), "PR"}};

    std::vector<params_tuple> gtpc_query_16_params = {{"zz", "bad"}};

    std::vector<params_tuple> gtpc_query_17_params = {{"b"}};

    std::vector<params_tuple> gtpc_query_18_params = {{300}};

    std::vector<params_tuple> gtpc_query_19_params = {{"a", 1, 400000, 1, 10, "b", "c"}};

    std::vector<params_tuple> gtpc_query_20_params =
        {{"co", time_from_string(std::string("2011-10-30 00:00:00.000")), "CHINA"}};

    std::vector<params_tuple> gtpc_query_21_params =
        {{"SAUDI ARABIA"}};

    std::vector<params_tuple> gtpc_query_22_params =
        {{"1", "2", "3", "4", "5", "6", "7"}};

    double t = 0.0;
    t = run_query(gdb, gtpc_query_1, gtpc_query_1_params);
    spdlog::info("Query #1: {} msecs", t);
    t = run_query(gdb, gtpc_query_2, gtpc_query_2_params);
    spdlog::info("Query #2: {} msecs", t);
    t = run_query(gdb, gtpc_query_3, gtpc_query_3_params);
    spdlog::info("Query #3: {} msecs", t);
    t = run_query(gdb, gtpc_query_4, gtpc_query_4_params);
    spdlog::info("Query #4: {} msecs", t);
    t = run_query(gdb, gtpc_query_5, gtpc_query_5_params);
    spdlog::info("Query #5: {} msecs", t);
    t = run_query(gdb, gtpc_query_6, gtpc_query_6_params);
    spdlog::info("Query #6: {} msecs", t);
    t = run_query(gdb, gtpc_query_7, gtpc_query_7_params);
    spdlog::info("Query #7: {} msecs", t);
    t = run_query(gdb, gtpc_query_8, gtpc_query_8_params);
    spdlog::info("Query #8: {} msecs", t);
    t = run_query(gdb, gtpc_query_9, gtpc_query_9_params);
    spdlog::info("Query #9: {} msecs", t);
    t = run_query(gdb, gtpc_query_10, gtpc_query_10_params);
    spdlog::info("Query #10: {} msecs", t);
    t = run_query(gdb, gtpc_query_11, gtpc_query_11_params);
    spdlog::info("Query #11: {} msecs", t);
    t = run_query(gdb, gtpc_query_12, gtpc_query_12_params);
    spdlog::info("Query #12: {} msecs", t);
    t = run_query(gdb, gtpc_query_13, gtpc_query_13_params);
    spdlog::info("Query #13: {} msecs", t);
    t = run_query(gdb, gtpc_query_14, gtpc_query_14_params);
    spdlog::info("Query #14: {} msecs", t);
    t = run_query(gdb, gtpc_query_15, gtpc_query_15_params);
    spdlog::info("Query #15: {} msecs", t);
    t = run_query(gdb, gtpc_query_16, gtpc_query_16_params);
    spdlog::info("Query #16: {} msecs", t);
    t = run_query(gdb, gtpc_query_17, gtpc_query_17_params);
    spdlog::info("Query #17: {} msecs", t);
    t = run_query(gdb, gtpc_query_18, gtpc_query_18_params);
    spdlog::info("Query #18: {} msecs", t);
    t = run_query(gdb, gtpc_query_19, gtpc_query_19_params);
    spdlog::info("Query #19: {} msecs", t);
    t = run_query(gdb, gtpc_query_20, gtpc_query_20_params);
    spdlog::info("Query #20: {} msecs", t);
    t = run_query(gdb, gtpc_query_21, gtpc_query_21_params);
    spdlog::info("Query #21: {} msecs", t);
    t = run_query(gdb, gtpc_query_22, gtpc_query_22_params);
    spdlog::info("Query #22: {} msecs", t);
}

void run_transactions(graph_db_ptr gdb) {
    double t = 0.0;
    t = run_transaction(gdb, gtpc_transaction_1);
    spdlog::info("Transaction #1: {} msecs", t);
    t = run_transaction(gdb, gtpc_transaction_2);
    spdlog::info("Transaction #2: {} msecs", t);
    t = run_transaction(gdb, gtpc_transaction_3);
    spdlog::info("Transaction #3: {} msecs", t);
    t = run_transaction(gdb, gtpc_transaction_4);
    spdlog::info("Transaction #4: {} msecs", t);
    t = run_transaction(gdb, gtpc_transaction_5);
    spdlog::info("Transaction #5: {} msecs", t);
}

void run_benchmark(graph_db_ptr gdb) {}

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
      std::cout << "Poseidon Graph Database GTPC Benchmark Version " << POSEIDON_VERSION
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

  run_queries(graph);
  run_transactions(graph);
//   run_benchmark(graph);
}