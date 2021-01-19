#include <iostream>
#include <numeric>
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

#include "ldbc_jit_reads.hpp"
#include "ldbc.hpp"

using namespace boost::program_options;
bool compiled = false;
std::vector<int64_t> runtimes;

//using param_val = boost::variant<uint64_t, std::string, int, boost::posix_time::ptime>;
//using params_tuple = std::vector<param_val>;

double calc_avg() {
  auto avg = std::accumulate(runtimes.begin(), runtimes.end(), 0) / runtimes.size();
  runtimes.clear();
  compiled = false;
  scan_task::callee_ = &scan_task::scan;
  return avg;
}

bool dummy(int *) {
    return true;
}

void ldbc_jit_is_query_1(graph_db_ptr &gdb, query_engine &qeng, result_set &rs, bool adaptive, uint64_t personId) {
    auto q = Scan("Post", 
                Filter(Fct(dummy),
                    Project({},
                        GroupBy({},
                            Aggr({},
                                Collect())))));

}

void run_is_1(graph_db_ptr gdb, query_engine &qeng) {

}

void run_benchmark(graph_db_ptr gdb, query_engine &qeng) {

}

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
        ("strict,s", bool_switch()->default_value(false), "Strict mode - assumes that all columns contain values of the same type")
        ("pool,p", value<std::string>(&pool_path)->required(), "Path to the PMem pool")
        ("import,i", value<std::string>(&snb_home), "Path to directories containing SNB CSV files")
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
	auto THREAD_NUM = 4;
	auto chunks = graph->get_nodes()->num_chunks();
	auto cv_range = chunks / THREAD_NUM;

	query_engine queryEngine(graph, THREAD_NUM, cv_range);

  for (auto i = 0; i < 1; i++) {
    std::cout << "Run: " << i << "\n";
    //run_benchmark(graph, queryEngine);
  }
}
