#include <chrono>
#include <iostream>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/program_options.hpp>

#include "linenoise.hpp"
#include "queryc.hpp"
#include "graph_db.hpp"
#include "graph_pool.hpp"

#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"

using namespace boost::program_options;


/**
 * Import data from the given list of CSV files. The list contains
 * not only the files names but also nodes/relationships as well as
 * the labels.
 */
bool import_csv_files(graph_db_ptr &gdb, const std::vector<std::string> &files,
                      char delimiter, std::string format, bool strict) {
  graph_db::mapping_t id_mapping;

  for (auto s : files) {
    if (s.find("nodes:") != std::string::npos) {
      std::vector<std::string> result;
      boost::split(result, s, boost::is_any_of(":"));

      if (result.size() != 3) {
        std::cerr << "ERROR: unknown import option for nodes." << std::endl;
        return false;
      }

      std::size_t num = 0;
      auto start = std::chrono::steady_clock::now();
      if (format == "n4j") {
        num = gdb->import_typed_n4j_nodes_from_csv(result[1], result[2],
                                                   delimiter, id_mapping);
      }
      else {
        num = strict
          ? gdb->import_typed_nodes_from_csv(result[1], result[2], delimiter, id_mapping)
          : gdb->import_nodes_from_csv(result[1], result[2], delimiter, id_mapping);
      }
      auto end = std::chrono::steady_clock::now();

      auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
      spdlog::info("{} '{}' nodes imported in {} msecs", num, result[1], time);
    }
    else if (s.find("relationships:") != std::string::npos) {
      std::vector<std::string> result;
      boost::split(result, s, boost::is_any_of(":"));

      if (format == "n4j") {
        if (result.size() < 2 || result.size() > 3) {
          std::cerr << "ERROR: unknown import option for relationships."
                    << std::endl;
          return false;
        }
      }
      else if (result.size() != 3) {
          std::cerr << "ERROR: unknown import option for relationships."
                    << std::endl;
          return false;
      }

      std::size_t num = 0;
      auto start = std::chrono::steady_clock::now();
      if (format == "n4j") {
        auto rship_type = result.size() == 3 ? result[1] : "";
        num = gdb->import_typed_n4j_relationships_from_csv(result.back(), delimiter, id_mapping, rship_type);
      }
      else {
        num = strict
         ? gdb->import_typed_relationships_from_csv(result[2], delimiter, id_mapping)
         : gdb->import_relationships_from_csv(result[2], delimiter, id_mapping);
      }
      auto end = std::chrono::steady_clock::now();

      auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
      if (result.size() == 3) 
        spdlog::info("{} '{}' relationships imported in {} msecs", num, result[1], time);
      else
        spdlog::info("{} relationships imported in {} msecs", num, time);
    }
    else {
      std::cerr << "ERROR: unknown import (nodes or relationships expected)."
                << std::endl;
      return false;
    }
  }
  return true;
}

int main(int argc, char* argv[]) {
  std::string db_name, pool_path, log_file, dot_file, format;
  std::vector<std::string> import_files;
  char delim_character = '|';
  bool strict = false;


  spdlog::info("Starting Poseidon Generic CSV Loader, Version {}", POSEIDON_VERSION);

  try {
    options_description desc{"Options"};
    desc.add_options()
      ("help,h", "Help")
        ("verbose,v", bool_switch()->default_value(false), "Verbose - show debug output")
        ("strict,s", bool_switch()->default_value(true), "Strict mode - assumes that all columns contain values of the same type")
        ("delimiter", value<char>(&delim_character)->default_value('|'), "Character delimiter")
        ("db,d", value<std::string>(&db_name)->required(), "Database name (required)")
        ("pool,p", value<std::string>(&pool_path)->required(), "Path to the PMem pool")
        ("log,l", value<std::string>(&log_file), "Write log messages to the given file")
        ("output,o", value<std::string>(&dot_file), "Dump the graph to the given file (in DOT format)")
        ("import,i", value<std::vector<std::string>>()->composing()->required(),
        "Import files in CSV format (either nodes:<node type>:<filename> or "
        "relationships:<rship type>:<filename>")
        ("format,f", value<std::string>(&format)->required(), "CSV format: n4j | gtpc | ldbc (required)");

    variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);

    if (vm.count("help")) {
      std::cout << "Poseidon Graph Database Version " << POSEIDON_VERSION
                << "\n"
                << desc << '\n';
      return -1;
    }

    notify(vm);

    if (vm.count("import"))
      import_files = vm["import"].as<std::vector<std::string>>();

    if (vm.count("log"))
      log_file = vm["log"].as<std::string>();

    if (vm.count("strict"))
      strict = vm["strict"].as<bool>();

    if (vm.count("pool"))
      pool_path = vm["pool"].as<std::string>();

    if (vm.count("delimiter"))
      delim_character = vm["delimiter"].as<char>();

    if (vm.count("format"))
      format = vm["format"].as<std::string>();

    if (vm.count("verbose"))
      if (vm["verbose"].as<bool>())
        spdlog::set_level(spdlog::level::debug);

    if (format != "n4j" && format != "gtpc" && format != "ldbc") {
      std::cout
          << "ERROR: choose format --n4j or --gtpc or --ldbc.\n";
      return -1;
    }
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
    pool = graph_pool::open(pool_path, true);
  }
  spdlog::info("open poolset {}", pool_path);
#else
  pool = graph_pool::create(pool_path);
#endif
  auto graph = pool->create_graph(db_name);

  
  spdlog::info("--------- Importing files ...");
  import_csv_files(graph, import_files, delim_character, format, strict);
  graph->print_stats();

  if (!dot_file.empty())
    graph->dump_dot(dot_file);
}