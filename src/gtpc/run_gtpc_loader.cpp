#include <iostream>
#include <chrono>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

#include "defs.hpp"
#include "graph_db.hpp"
#include "graph_pool.hpp"
#include "gtpc.hpp"
#include "config.h"

#include "thread_pool.hpp"

#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"

using namespace boost::program_options;

int main(int argc, char **argv) {
  bool strict = false;
  std::string pool_path, gtpc_home, db_name, log_file;

 try {
    options_description desc{"Options"};
    desc.add_options()
        ("help,h", "Help")
        ("verbose,v", bool_switch()->default_value(false), "Verbose - show debug output")
        ("strict,s", bool_switch()->default_value(false), "Strict mode - assumes that all columns contain values of the same type")
        ("pool,p", value<std::string>(&pool_path)->required(), "Path to the PMem pool")
        ("import,i", value<std::string>(&gtpc_home)->required(), "Path to directory containing the graph GTPC CSV files")
        ("log,l", value<std::string>(&log_file), "Write log messages to the given file")
        ("db,d", value<std::string>(&db_name)->required(),"Database name (required)");

    variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);

    if (vm.count("help")) {
      std::cout << "Poseidon Graph Database GTPC Importer Version " << POSEIDON_VERSION
                << "\n"
                << desc << '\n';
      return -1;
    }
    if (vm.count("import"))
      gtpc_home = vm["import"].as<std::string>();

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
  if (access(pool_path.c_str(), F_OK) != 0) {
    pool = graph_pool::create(pool_path);
  } else {
  	  //remove(pool_path.c_str());
	  // pool = graph_pool::create(pool_path);
    pool = graph_pool::open(pool_path, true);
  }
  spdlog::info("open poolset {}", pool_path);
 #else
  pool = graph_pool::create(pool_path);
#endif
  auto graph = pool->create_graph(db_name);
 
  graph->print_stats();
  
  load_gtpc_data(graph, gtpc_home);
  #ifdef CREATE_INDEX
  create_gtpc_index(graph);
  #endif

  graph->print_stats();
}