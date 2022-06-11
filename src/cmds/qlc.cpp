
#include <chrono>
#include <iostream>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/program_options.hpp>

#include "linenoise.hpp"
#include "qproc.hpp"
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

std::unique_ptr<qproc> qproc_ptr;

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
 * Print the query result to standard output.
 */
void print_result(qresult_iterator& qres) {
  std::cout << "Result: \n" << qres.result() << std::dec << qres.result_size() << " tuple(s)" << std::endl;
}

/**
 * Execute the query given as string. If qex_cc is set to true then the
 * query is compiled using LLVM, otherwise the query interpreter is used.
 */
void exec_query(const std::string &qstr, qproc::mode qmode) {
  try {
  auto start_qp = std::chrono::steady_clock::now();
  auto res = qproc_ptr->execute_query(qmode, qstr);
  auto end_qp = std::chrono::steady_clock::now();

  print_result(res);

  std::cout << "Query executed in " 
            << std::chrono::duration_cast<std::chrono::microseconds>(end_qp -
                                                                     start_qp)
                   .count()
            << " µs" << std::endl;
  } catch (std::exception& exc) {
    std::cerr << "Error in query execution: " << exc.what() << std::endl;
  }
}


std::string read_from_file(const std::string& qfile) {
  std::string qstr, line;

  std::ifstream myfile(qfile);
  if (myfile.is_open()) {
    while (getline(myfile, line))
      qstr.append(line);
    myfile.close();
  } else {
    std::cout << "File not found" << std::endl;
  }
  
  spdlog::info("execute query {}", qstr);
  return qstr;
}

/**
 * Linenoise - autocompletion
 */
void query_completion(const char* buf, std::vector<std::string>& completions) {
    if (buf[0] == 'n' || buf[0] == 'N') {
        completions.push_back("NodeScan(");
    } else if (buf[0] == 'l' || buf[0] == 'L') {
        completions.push_back("Limit(");
    } else if (buf[0] == 's' || buf[0] == 'S') {
        completions.push_back("set");
    }
}


/**
 * Run an interactive shell for entering and executing queries.
 */
void run_shell(graph_db_ptr &gdb, qproc::mode qmode) {
  const auto path = "history.txt";
  // Enable the multi-line mode
  linenoise::SetMultiLine(true);

  // Set max length of the history
  linenoise::SetHistoryMaxLen(4);
  // Load history
  linenoise::LoadHistory(path);
  linenoise::SetCompletionCallback(query_completion);

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

    if (line.rfind("@", 0) == 0) {
      auto query_string = read_from_file(line.substr(1));
      exec_query(query_string, qmode);
    } 
#if USE_LLVM    
    else if(line.rfind("set", 0) == 0) { // save sub-query: > q1:End(NodeScan("Person"))
      spdlog::info("Save query: {} as {}", line.substr(line.find(":") + 1), line.substr(0, line.find(":")).substr(4));
      // TODO: qlc.parse_and_save_plan(line.substr(0, line.find(":")).substr(4), line.substr(line.find(":") + 1));
    } else if(line.rfind("run", 0) == 0) { // run saved query plan -> run:q1
      spdlog::info("Execute query: {} ", line.substr(line.find(":") + 1));
      //qlc.exec_plan(line.substr(line.find(":") + 1), gdb);
      exec_query(line.substr(line.find(":") + 1), qmode);
    }
#endif
    else
      exec_query(line, qmode);

    // Add line to history
    linenoise::AddHistory(line.c_str());

    // Save history
    linenoise::SaveHistory(path);
  }

}

int main(int argc, char* argv[]) {
  std::string db_name, query_file, dot_file, qmode_str;
  std::vector<std::string> import_files;
  bool start_shell = false;
  bool n4j_mode = false;
  qproc::mode qmode = qproc::Compile; 
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
        ("query,q", value<std::string>(&query_file), "Execute the query from the given file")
        ("shell,s", bool_switch()->default_value(false), "Start the interactive shell")
        ("qmode", value<std::string>(&qmode_str), "Query compile mode: llvm (default) | interp | adapt");

    variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);

    if (vm.count("help")) {
      std::cout << "Poseidon Graph Database Version " << POSEIDON_VERSION << " ("
#ifdef USE_PMDK
                << "persistent memory"
#elif USE_PFILE
                << "paged files"
#else
                << "in-memory"
#endif
                << ")\n" << desc << '\n';
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

    if (vm.count("qmode")) {
      std::cout << "qmode = " << qmode_str << std::endl;
      if (qmode_str != "llvm" && qmode_str != "interp" && qmode_str != "adapt") {
        std::cout << "ERROR: unknown query mode value: 'llvm' or 'interp' or 'adapt' expected.\n";
        return -1;
      }
      if (qmode_str == "llvm")
        qmode = qproc::Compile;
      else if (qmode_str == "interp")
        qmode = qproc::Interpret;
      else
        qmode = qproc::Adaptive;
    }

    if (start_shell && !query_file.empty()) {
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

  graph->dump();
  
  qproc_ptr = std::make_unique<qproc>(graph);

  if (start_shell) {
    run_shell(graph, qmode);
  }

  //exec_query(graph, "Filter($0.customerId == 42, NodeScan())", false);
  //exec_query(graph, "Create(($1)-[r:Label { name1: 'Val1', name2: 42 }]->($2)), NodeScan('Person'))");

  if (!query_file.empty()) {
    // load the query from the file
    auto query_string = read_from_file(query_file);
    exec_query(query_string, qmode);
  }
}
 
