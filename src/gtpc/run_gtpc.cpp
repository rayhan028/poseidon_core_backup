#include <iostream>
#include <regex>
#include <random>
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
using func = std::function<void (graph_db_ptr &, result_set &)>;

static const auto NUM_QUERIES = 22;
static const auto NUM_TRANSACTIONS = 5;

/* ------------------------------------------------------------------------ */

double calc_avg_time(const std::vector<double>& vec) {
    double d = 0.0;
    for (auto v : vec)
      d += v;
    return d / (double)vec.size();
}

double run(graph_db_ptr gdb, const func &f) {

    auto iterations = 1u;
    std::vector<double> runtimes(iterations);

    for (auto i = 0u; i < iterations; i++) {
        result_set rs;
        auto start_qp = std::chrono::steady_clock::now();

        gdb->run_transaction([&]() {
          f(gdb, rs);
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

void run_query_stream(graph_db_ptr gdb) {
    const std::vector<func> queries =
        {gtpc_query_1, gtpc_query_2, gtpc_query_3, gtpc_query_4,
         gtpc_query_5, gtpc_query_6, gtpc_query_7, gtpc_query_8,
         gtpc_query_9, gtpc_query_10, gtpc_query_11, gtpc_query_12,
         gtpc_query_13, gtpc_query_14, gtpc_query_15, gtpc_query_16,
         gtpc_query_17, gtpc_query_18, gtpc_query_19, gtpc_query_20,
         gtpc_query_21, gtpc_query_22};

    auto start_query = gen_random_uniform_int(1, NUM_QUERIES);
    for (auto i = 0; i < NUM_QUERIES; i++) {
        auto idx = (start_query + i) % NUM_QUERIES;
        double t = run(gdb, queries[idx]);
        std::string msg = "Query #" + std::to_string(idx + 1) + ": {} msecs";
        spdlog::info(msg, t);
    }
}

void run_transaction_stream(graph_db_ptr gdb) {
    const std::vector<func> transactions =
        {gtpc_transaction_1, gtpc_transaction_2, gtpc_transaction_3,
         gtpc_transaction_4, gtpc_transaction_5};

    std::random_device rd;
    std::mt19937 gen(rd());
    std::vector<int> idxs(NUM_TRANSACTIONS);
    std::iota(idxs.begin(), idxs.end(), 0);
    std::shuffle(idxs.begin(), idxs.end(), gen);
    for (auto idx : idxs) {
        double t = run(gdb, transactions[idx]);
        std::string msg = "Transaction #" + 
                           std::to_string(idx + 1) + 
                           ": {} msecs";
        spdlog::info(msg, t);
    }
}

void run_benchmark(graph_db_ptr gdb) {
    auto query_streams = 1;
    auto transaction_streams = 1;
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

  run_query_stream(graph);
  run_transaction_stream(graph);
//   run_benchmark(graph);
}