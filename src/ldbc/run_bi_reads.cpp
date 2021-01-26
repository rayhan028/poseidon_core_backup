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