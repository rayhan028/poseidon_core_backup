
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

queryc qlc;

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
 * Execute the query given as string by interpreting the plan.
 */
void interpret_query(graph_db_ptr &gdb, const std::string &qstr) {
  queryc qlc;
  spdlog::debug("create AOT query code");
  auto qset = qlc.generate_qex_plan(gdb, qstr);  
  qset.start(); 
}

/**
 * Execute the query given as string by generating code via LLVM.
 */
void compile_query(graph_db_ptr &gdb, const std::string &qstr) {

#ifdef USE_LLVM
  spdlog::debug("compile to plan via LLVM");     

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

	result_set rs;

  auto start_qp = std::chrono::steady_clock::now();
  spdlog::debug("generate query code");     
  queryEngine.generate(plan, false);
  auto end_qc = std::chrono::steady_clock::now();
  
  spdlog::debug("execute query code");     
	queryEngine.run(&rs);

  auto end_qp = std::chrono::steady_clock::now();
  
  std::cout << "Query compiled in "
            << std::chrono::duration_cast<std::chrono::milliseconds>(end_qc -
                                                                     start_qp)
                   .count()
            << " ms and executed in " 
            << std::chrono::duration_cast<std::chrono::milliseconds>(end_qp -
                                                                     end_qc)
                   .count()
            << " ms" << std::endl;

  std::cout << rs << std::endl;
#else
spdlog::debug("query compiler is disabled, create AOT query code");
interpret_query(gdb, qstr);
#endif
}

/**
 * Execute the query given as string. If qex_cc is set to true then the
 * query is compiled using LLVM, otherwise the query interpreter is used.
 */
void exec_query(graph_db_ptr &gdb, const std::string &qstr, bool qex_cc) {
  if (qex_cc)
    compile_query(gdb, qstr);
  else 
    interpret_query(gdb, qstr);
}


std::string read_from_file(const std::string& qfile) {
  std::string qstr, line;

  std::ifstream myfile(qfile);
  if (myfile.is_open()) {
    while (getline(myfile, line))
      qstr.append(line);
    myfile.close();
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
void run_shell(graph_db_ptr &gdb, bool qex_cc) {
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
      exec_query(gdb, query_string, qex_cc);
    } else if(line.rfind("set", 0) == 0) { // save sub-query: > q1:End(NodeScan("Person"))
      spdlog::info("Save query: {} as {}", line.substr(line.find(":") + 1), line.substr(0, line.find(":")).substr(4));
      qlc.parse_and_save_plan(line.substr(0, line.find(":")).substr(4), line.substr(line.find(":") + 1));
    } else if(line.rfind("run", 0) == 0) { // run saved query plan -> run:q1
      spdlog::info("Execute query: {} ", line.substr(line.find(":") + 1));
      qlc.exec_plan(line.substr(line.find(":") + 1), gdb);
    }
    else
      exec_query(gdb, line, qex_cc);

    // Add line to history
    linenoise::AddHistory(line.c_str());

    // Save history
    linenoise::SaveHistory(path);
  }

}

int main(int argc, char* argv[]) {
  std::string db_name, query_file, dot_file, qmode;
  std::vector<std::string> import_files;
  bool start_shell = false;
  bool n4j_mode = false;
  bool qex_cc = true; 
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
        ("qmode", value<std::string>(&qmode), "Query compile mode: llvm (default) | aot");

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

    if (vm.count("qmode")) {
      std::cout << "qmode = " << qmode << std::endl;
      if (qmode != "llvm" && qmode != "aot") {
        std::cout << "ERROR: unknown qmode value: 'llvm' or 'aot' expected.\n";
        return -1;
      }
      qex_cc = (qmode == "llvm");
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

  if (start_shell) {
    run_shell(graph, qex_cc);
  }

  //exec_query(graph, "Create(($1)-[r:Label { name1: 'Val1', name2: 42 }]->($2)), NodeScan('Person'))");

  // exec_query(graph, "Create(($1)-[r:Label { name1: 'Val1', name2: 42 }]->($2)), NodeScan('Person'))");

  if (!query_file.empty()) {
    // load the query from the file
    auto query_string = read_from_file(query_file);
    exec_query(graph, query_string, qex_cc);
  }
}
 