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

static const auto NUM_OLAP = 22;
static const auto NUM_OLTP = 5;

/* ------------------------------------------------------------------------ */

void run_olap_query_stream(graph_db_ptr gdb, std::size_t stream) {
  const std::vector<func> olap_queries =
    {gtpc_olap_1, gtpc_olap_2, gtpc_olap_3, gtpc_olap_4,
     gtpc_olap_5, gtpc_olap_6, gtpc_olap_7, gtpc_olap_8,
     gtpc_olap_9, gtpc_olap_10, gtpc_olap_11, gtpc_olap_12,
     gtpc_olap_13, gtpc_olap_14, gtpc_olap_15, gtpc_olap_16,
     gtpc_olap_17, gtpc_olap_18, gtpc_olap_19, gtpc_olap_20,
     gtpc_olap_21, gtpc_olap_22};

  auto start_query = gen_random_uniform_int(1, NUM_OLAP);
  for (auto i = 0; i < NUM_OLAP; i++) {
    auto idx = (start_query + i) % NUM_OLAP;
    result_set rs;
    auto start_qp = std::chrono::steady_clock::now();

    gdb->run_transaction([&]() {
      try {
        olap_queries[idx](gdb, rs);
        return true;
      }
      catch (const std::exception &ex) {
        spdlog::error("Query #{} in OLAP Stream #{} failed.", (idx + 1), stream);
        std::cerr << ex.what() << "\n";
        return false;
      }
    });

    auto end_qp = std::chrono::steady_clock::now();
    double t = std::chrono::duration_cast<std::chrono::milliseconds>(end_qp -
                                                                    start_qp).count();
#ifdef PRINT_RESULT
    std::cout << rs << "\n";
#endif
    spdlog::info("Query #{} in OLAP Stream #{}: {} msecs", (idx + 1), stream, t);
  }
}

void run_oltp_query_stream(graph_db_ptr gdb, std::size_t stream) {
  const std::vector<func> transactions =
    {gtpc_oltp_1, gtpc_oltp_2, gtpc_oltp_3,
     gtpc_oltp_4, gtpc_oltp_5};

  std::random_device rd;
  std::mt19937 gen(rd());
  std::vector<std::size_t> idxs(NUM_OLTP);
  std::iota(idxs.begin(), idxs.end(), 0);
  std::shuffle(idxs.begin(), idxs.end(), gen);

  for (auto idx : idxs) {
    result_set rs;
    auto start_qp = std::chrono::steady_clock::now();

    gdb->run_transaction([&]() {
      try {
        transactions[idx](gdb, rs);
        return true;
      }
      catch (const std::exception &ex) {
        spdlog::error("Query #{} in OLTP Stream #{} failed", (idx + 1), stream);
        std::cerr << ex.what() << "\n";
        return false;
      }
    });

    auto end_qp = std::chrono::steady_clock::now();
    double t = std::chrono::duration_cast<std::chrono::milliseconds>(end_qp -
                                                                    start_qp).count();
#ifdef PRINT_RESULT
    std::cout << rs << "\n";
#endif
    spdlog::info("Query #{} in OLTP Stream #{}: {} msecs", (idx + 1), stream, t);
  }
}

void run_benchmark(graph_db_ptr gdb, std::size_t qr_streams, std::size_t tx_streams) {
  spdlog::info("--------- Dispatching {} OLAP Streams and "
               "{} OLTP Streams...", qr_streams, tx_streams);
  std::vector<std::future<void>> res;
  res.reserve((qr_streams + tx_streams));
  thread_pool pool;

  for (std::size_t i = 0; i < qr_streams; i++) {
    auto olap_s = i + 1;
    spdlog::info("Dispatching OLAP Stream #{}", olap_s);
    res.push_back(pool.submit([&]() {
      run_olap_query_stream(gdb, olap_s);
    }));
  }
  for (std::size_t j = 0; j < tx_streams; j++) {
    auto oltp_s = j + 1;
    spdlog::info("Dispatching OLTP Stream #{}", oltp_s);
    res.push_back(pool.submit([&]() {
      run_oltp_query_stream(gdb, oltp_s);
    }));
  }
  for (auto &f : res)
    f.get();
}

/* ---------------------------------------------------------------------------- */

using namespace boost::program_options;

int main(int argc, char **argv) {
  bool strict = false;
  std::string pool_path, gtpc_home, db_name;
  std::size_t olap_streams, oltp_streams;

 try {
    options_description desc{"Options"};
    desc.add_options()
        ("help,h", "Help")
        ("verbose,v", bool_switch()->default_value(false), "Verbose - show debug output")
        ("import,i", value<std::string>(&gtpc_home), "Path to directories containing GTPC CSV files")
        ("strict,s", bool_switch()->default_value(false), "Strict mode - assumes that all columns contain values of the same type")
        ("pool,p", value<std::string>(&pool_path)->required(), "Path to the PMem pool")
        ("db,d", value<std::string>(&db_name)->required(),"Database name (required)")
        ("olap,a", value<std::size_t>(&olap_streams)->required(),"Query streams (required)")
        ("oltp,t", value<std::size_t>(&oltp_streams)->required(),"Transaction streams (required)");

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
  bool exists = access(pool_path.c_str(), F_OK) == 0;
  auto pool = exists
            ? graph_pool::open(pool_path)
            : graph_pool::create(pool_path);
  auto graph = exists
             ? pool->open_graph(db_name)
             : pool->create_graph(db_name);
  if (!exists) {
    load_gtpc_data(graph, gtpc_home);
  }
  #if defined(RUN_INDEXED) && defined(FPTree)
  gtpc_fptree_recovery(graph);
  #endif
#else
  auto pool = graph_pool::create(pool_path);
  auto graph = pool->create_graph(db_name);

  load_gtpc_data(graph, gtpc_home);
#endif
  graph->print_stats();

  // run_olap_query_stream(graph, olap_streams);
  // run_oltp_query_stream(graph, oltp_streams);
  run_benchmark(graph, olap_streams, oltp_streams);
}