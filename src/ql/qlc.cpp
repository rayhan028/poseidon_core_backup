
#include <chrono>
#include <iostream>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/program_options.hpp>

#include "linenoise.hpp"
#include "queryc.hpp"
#include "graph_db.hpp"

#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"

#ifdef USE_PMDK

#define POOL_SIZE ((unsigned long long)(1024 * 1024 * 40000ull)) // 4000 MiB

#define PMEM_PATH "/mnt/pmem0/poseidon/"

struct root {
  graph_db_ptr graph;
};

#endif

using namespace boost::program_options;


/**
 * Import data from the given list of CSV files. The list contains
 * not only the files names but also nodes/relationships as well as
 * the labels.
 */
bool import_csv_files(graph_db_ptr &gdb, const std::vector<std::string> &files,
                      char delimiter, bool n4j_mode = false) {
  graph_db::mapping_t id_mapping;

  for (auto s : files) {
    if (s.find("nodes:") != std::string::npos) {
      std::vector<std::string> result;
      boost::split(result, s, boost::is_any_of(":"));

      if (result.size() != 3) {
        std::cerr << "ERROR: unknown import option for nodes." << std::endl;
        return false;
      }

      auto start = std::chrono::steady_clock::now();
      auto num = n4j_mode 
                  ? gdb->import_typed_n4j_nodes_from_csv(result[1], result[2], delimiter,
                                            id_mapping)
                  : gdb->import_nodes_from_csv(result[1], result[2], delimiter,
                                            id_mapping);
      auto end = std::chrono::steady_clock::now();

      std::cout << num << " nodes of type '" << result[1] << "' imported in "
                << std::chrono::duration_cast<std::chrono::milliseconds>(end -
                                                                         start)
                       .count()
                << " msecs." << std::endl;
    } else if (s.find("relationships:") != std::string::npos) {
      std::vector<std::string> result;
      boost::split(result, s, boost::is_any_of(":"));

      if (n4j_mode) {
        if (result.size() < 2 || result.size() > 3) {
          std::cerr << "ERROR: unknown import option for relationships."
                    << std::endl;
          return false;
        }
      }
      else if (result.size() != 2 ) {
          std::cerr << "ERROR: unknown import option for relationships."
                    << std::endl;
          return false;
      }

      std::size_t num = 0;
      auto start = std::chrono::steady_clock::now();
      if (n4j_mode) {
        auto rship_type = result.size() == 3 ? result[1] : "";
        num = gdb->import_typed_n4j_relationships_from_csv(result.back(), delimiter, id_mapping, rship_type);
      }
      else {
        num = gdb->import_relationships_from_csv(result[1], delimiter, id_mapping);
      }
      auto end = std::chrono::steady_clock::now();

      std::cout << num << " relationships";
      if (result.size() == 3) 
        std::cout << " of type '" << result[1] << "'";
      std::cout << " imported in "
                << std::chrono::duration_cast<std::chrono::milliseconds>(end -
                                                                         start)
                       .count()
                << " msecs." << std::endl;
    } else {
      std::cerr << "ERROR: unknown import (nodes or relationships expected)."
                << std::endl;
      return false;
    }
  }
  return true;
}


/**
 * Remove leading and trailing whitespaces from the given string.
 */
static void trim(std::string &s) {
  s.erase(s.begin(), std::find_if_not(s.begin(), s.end(),
                                      [](char c) { return std::isspace(c); }));
  s.erase(std::find_if_not(s.rbegin(), s.rend(),
                           [](char c) { return std::isspace(c); })
              .base(),
          s.end());
}

/**
 * 
 */
void exec_query(graph_db_ptr &gdb, const std::string &qstr) {
  queryc qlc;

  spdlog::debug("compile_to_plan");     
  auto plan = qlc.compile_to_plan(qstr);

/*
  std::ostringstream os;
  os << "Execution plan: '";
  // qplan.dump(os);
  os << "'";
  spdlog::debug(os.str());
*/

  spdlog::debug("create query_engine");     
	query_engine queryEngine(gdb, 1, gdb->get_nodes()->num_chunks());
  arg_builder args;
  args.arg(1, "Product"); // TODO: 

	result_set rs;

  auto start_qp = std::chrono::steady_clock::now();
  spdlog::debug("generate query code");     
  queryEngine.generate(plan, false);
 
  spdlog::debug("execute query code");     
	queryEngine.run(&rs, args.args);

  auto end_qp = std::chrono::steady_clock::now();

  std::cout << "Query executed in "
            << std::chrono::duration_cast<std::chrono::milliseconds>(end_qp -
                                                                     start_qp)
                   .count()
            << " ms" << std::endl;
}


/**
 * Run an interactive shell for entering and executing queries.
 */
void run_shell(graph_db_ptr &gdb) {
  const auto path = "history.txt";
  // Enable the multi-line mode
  linenoise::SetMultiLine(true);

  // Set max length of the history
  linenoise::SetHistoryMaxLen(4);
  // Load history
  linenoise::LoadHistory(path);

  while (true) {
    std::string line;
    auto quit = linenoise::Readline("poseidon> ", line);

    if (quit) {
      std::cout << "Bye!" << std::endl;
      break;
    }

    trim(line);
    if (line.length() == 0)
      continue;

    exec_query(gdb, line);

    // Add line to history
    linenoise::AddHistory(line.c_str());

    // Save history
    linenoise::SaveHistory(path);
  }

}

int main(int argc, char* argv[]) {
  std::string db_name, query_string, dot_file;
  std::vector<std::string> import_files;
  bool start_shell = false;
  bool n4j_mode = false;
  char delim_character = ',';


  spdlog::info("Starting poseidon_cli, Version {}", POSEIDON_VERSION);

  try {
    options_description desc{"Options"};
    desc.add_options()
      ("help,h", "Help")
        ("verbose,v", bool_switch()->default_value(false), "Verbose - show debug output")
        ("db,d", value<std::string>(&db_name)->required(), "Database name (required)")
        ("output,o", value<std::string>(&dot_file), "Dump the graph to the given file (in DOT format)")
        ("import", value<std::vector<std::string>>()->composing(),
        "Import files in CSV format (either nodes:<node type>:<filename> or "
        "relationships:<rship type>:<filename>")
        ("n4j", bool_switch()->default_value(false), "Import CSV data in Neo4j format")
        ("query,q", value<std::string>(&query_string), "Execute the given query")
        ("shell,s", bool_switch()->default_value(false), "Start the interactive shell");

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

    if (vm.count("delimiter"))
      delim_character = vm["delimiter"].as<char>();

    if (vm.count("n4j"))
      n4j_mode = vm["n4j"].as<bool>();

    if (vm.count("verbose"))
      if (vm["verbose"].as<bool>())
        spdlog::set_level(spdlog::level::debug);

    if (vm.count("shell"))
      start_shell = vm["shell"].as<bool>();

    if (start_shell && !query_string.empty()) {
      std::cout
          << "ERROR: options --shell and --query cannot be used together.\n";
      return -1;
    }
  } catch (const error &ex) {
    std::cerr << ex.what() << '\n';
    return -1;
  }

#ifdef USE_PMDK
  namespace nvm = pmem::obj;

  nvm::pool<root> pop;
  const auto path = PMEM_PATH + db_name;

  if (access(path.c_str(), F_OK) != 0) {
    pop = nvm::pool<root>::create(path, db_name, POOL_SIZE);
  } else {
    pop = nvm::pool<root>::open(path, db_name);
  }

  auto q = pop.root();
  if (!q->graph) {
    // create a new persistent graph_db object
    nvm::transaction::run(pop, [&] { q->graph = p_make_ptr<graph_db>(); });
  }
  auto &graph = q->graph;
  graph->runtime_initialize();
#else
  auto graph = p_make_ptr<graph_db>(db_name);
#endif

  if (!import_files.empty()) {
    std::cout << "import files..." << std::endl;
    import_csv_files(graph, import_files, delim_character, n4j_mode);
    graph->print_stats();
  }

  if (!dot_file.empty())
    graph->dump_dot(dot_file);

  if (start_shell) {
    run_shell(graph);
  }

  exec_query(graph, "NodeScan('Product')");

  if (!query_string.empty()) {
    exec_query(graph, query_string);
  }
}
 