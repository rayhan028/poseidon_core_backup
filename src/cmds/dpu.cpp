
#include <chrono>
#include <iostream>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/program_options.hpp>

#include "graph_db.hpp"
#include "graph_pool.hpp"

#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"

using namespace boost::program_options;

graph_pool_ptr pool;
graph_db_ptr graph;

void dpu_scan(graph_db_ptr &gdb);


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
      spdlog::info("{} '{}' nodes imported in {} msecs ({} items/s)", num, result[1], time, (int)((double)num/time * 1000.0));
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
        spdlog::info("{} '{}' relationships imported in {} msecs ({} items/s)", num, result[1], time, (int)((double)num/time * 1000.0));
      else
        spdlog::info("{} relationships imported in {} msecs ({} items/s)", num, time, (int)((double)num/time * 1000.0));
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
  spdlog::set_level(spdlog::level::warn);
  std::string db_name, pool_path, format = "ldbc";
  std::vector<std::string> import_files;
  bool n4j_mode = false;
  char delim_character = ',';
  bool strict = false;
  
  spdlog::info("Starting dpu_test, Poseidon Version {}", POSEIDON_VERSION);

  try {
    options_description desc{"Options"};
    desc.add_options()
      ("help,h", "Help")
        ("verbose,v", bool_switch()->default_value(false), "Verbose - show debug output")
        ("db,d", value<std::string>(&db_name)->required(), "Database name (required)")
        ("pool,p", value<std::string>(&pool_path)->required(), "Path to the PMem/file pool")
        ("strict", bool_switch()->default_value(true), "Strict mode - assumes that all columns contain values of the same type")
        ("delimiter", value<char>(&delim_character)->default_value('|'), "Character delimiter")
        ("format,f", value<std::string>(&format), "CSV format: n4j | gtpc | ldbc")
        ("import", value<std::vector<std::string>>()->composing(),
        "Import files in CSV format (either nodes:<node type>:<filename> or "
        "relationships:<rship type>:<filename>");

    variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);

    if (vm.count("help")) {
      std::cout << "Poseidon Graph Database Version " << POSEIDON_VERSION << " ("
#ifdef USE_PMDK
                << "persistent memory"
#elif defined(USE_IN_MEMORY)
                << "in-memory"
#else
                << "paged files"
#endif
                << ")\n" << desc << '\n';
      return -1;
    }

    notify(vm);

    if (vm.count("import"))
      import_files = vm["import"].as<std::vector<std::string>>();

   if (vm.count("pool"))
      pool_path = vm["pool"].as<std::string>();

    if (vm.count("delimiter"))
      delim_character = vm["delimiter"].as<char>();

   if (vm.count("strict"))
      strict = vm["strict"].as<bool>();

    if (vm.count("format"))
      format = vm["format"].as<std::string>();

    if (format != "n4j" && format != "gtpc" && format != "ldbc") {
      std::cout
          << "ERROR: choose format --n4j or --gtpc or --ldbc.\n";
      return -1;
    }

    if (vm.count("verbose"))
      if (vm["verbose"].as<bool>())
        spdlog::set_level(spdlog::level::debug);
  } catch (const error &ex) {
    std::cerr << ex.what() << '\n';
    return -1;
  }

  if (access(pool_path.c_str(), F_OK) != 0) {
    spdlog::info("create poolset {}", pool_path);
    pool = graph_pool::create(pool_path);
    graph = pool->create_graph(db_name);
  } 

  if (!import_files.empty()) {
    spdlog::info("--------- Importing files ...");
    for(int i = 0; i < 100000; i++)
      import_csv_files(graph, import_files, delim_character, format, strict);
    graph->print_stats();
  }

  dpu_scan(graph);
}
 
