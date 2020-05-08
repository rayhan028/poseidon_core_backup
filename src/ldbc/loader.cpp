#include <iostream>
#include <chrono>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

#include "defs.hpp"
#include "graph_db.hpp"
#include "graph_pool.hpp"
#include "ldbc.hpp"
#include "config.h"

#include "thread_pool.hpp"

#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"

#define SF_100
// #define CREATE_INDEX
// #define PARALLEL_LOAD
// #define PARALLEL_RSHIP_LOAD

#ifdef SF_100
#define POOL_SIZE ((unsigned long long)(1024 * 1024 * 1024 * 650ull)) // 600 GiB
#elif defined(SF_10)
#define POOL_SIZE ((unsigned long long)(1024 * 1024 * 160000ull)) // 16000 MiB
#else
#define POOL_SIZE ((unsigned long long)(1024 * 1024 * 40000ull)) // 4000 MiB
#endif

using namespace boost::program_options;

int main(int argc, char **argv) {
  bool strict = false;
  std::string pool_path, db_name, log_file;
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
        ("log,l", value<std::string>(&log_file), "Write log messages to the given file")
        ("db,d", value<std::string>(&db_name)->required(),"Database name (required)");

    variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);

    if (vm.count("help")) {
      std::cout << "Poseidon Graph Database LDBC Importer Version " << POSEIDON_VERSION
                << "\n"
                << desc << '\n';
      return -1;
    }
    if (vm.count("import"))
      snb_home = vm["import"].as<std::string>();

    if (vm.count("log"))
      log_file = vm["log"].as<std::string>();

    if (vm.count("strict"))
      strict = vm["strict"].as<bool>();

    if (vm.count("pool"))
      pool_path = vm["pool"].as<std::string>();

    notify(vm);

      } catch (const error &ex) {
    std::cerr << ex.what() << '\n';
    return -1;
  }
  
  std::shared_ptr<spdlog::logger> file_logger;
  if (!log_file.empty()) {
    file_logger = spdlog::basic_logger_mt("basic_logger", log_file);
    spdlog::set_default_logger(file_logger);  
  }
  if (strict)
    spdlog::info("Using strict mode");
  else
    spdlog::info("Using non-strict mode");
  
  graph_pool_ptr pool;

  #ifdef USE_PMDK
  if (access(test_path.c_str(), F_OK) != 0) {
    pool = graph_pool::create(pool_path);
  } else {
    pool = graph_pool::open(pool_path);
  }
  spdlog::info("open poolset {}", pool_path);
 #else
  pool = graph_pool::create(pool_path);
#endif
  auto graph = pool->create_graph(db_name);
 
  graph->print_stats();
  
  load_snb_data(graph, snb_home);

  graph->print_stats();


}
