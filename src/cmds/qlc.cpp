
#include <chrono>
#include <iostream>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/program_options.hpp>

#include "linenoise.hpp"
#include "fmt/chrono.h"
#include "qproc.hpp"
#include "graph_db.hpp"
#include "graph_pool.hpp"

#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"


graph_pool_ptr pool;

#ifdef USE_PMDK

#define POOL_SIZE ((unsigned long long)(1024 * 1024 * 40000ull)) // 4000 MiB

#define PMEM_PATH "/mnt/pmem0/poseidon/"

#endif
graph_db_ptr graph;

using namespace boost::program_options;

std::unique_ptr<qproc> qproc_ptr;
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
  auto res = qproc_ptr->execute_and_output_query(qmode, qstr, true);
  auto end_qp = std::chrono::steady_clock::now();

  std::chrono::duration<double> diff = end_qp - start_qp;
  fmt::print("Query executed in {}\n", diff); 
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

void print_stats(graph_db_ptr &gdb) {
  gdb->print_stats();
}

void sync_db(graph_db_ptr &gdb) {
  gdb->flush();
}

void show_help() {
  std::cout << "Available commands:\n"
            << "\thelp          " << "show this help" << "\n"
            << "\tstring s      " << "display the dictionary code of the string s" << "\n"
            << "\tcode c        " << "display the string of the dictionary code c" << "\n"
            << "\tstats         " << "print database statistics" << "\n"
            << "\tsync          " << "ensure that all pages are written to disk" << "\n"
            << "\t@file         " << "execute the query stored in the given file" << "\n"
            << "\t<query-expr>  " << "execute the given query" << std::endl;
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
    else if (line.rfind("help", 0) == 0) {
      show_help();
    }
    else if (line.rfind("stats", 0) == 0) {
      print_stats(gdb);
    }
    else if (line.rfind("sync", 0) == 0) {
      sync_db(gdb);
    }    
    else if (line.rfind("string", 0) == 0) {
      // lookup_string
      if (line.length() > 6) {
        auto s = line.substr(6);
        trim(s);
        std::cout << "dict code for '" << s << "': " << gdb->get_dictionary()->lookup_string(s) << std::endl;
      }
    }
    else if (line.rfind("code", 0) == 0) {
      // lookup_code
      if (line.length() > 4) {
        auto s = line.substr(4);
        trim(s);
        std::cout << "dict string for '" << s << "': " << gdb->get_dictionary()->lookup_code(std::stoi(s)) << std::endl;
      }
    }
    else
      exec_query(line, qmode);

    // Add line to history
    linenoise::AddHistory(line.c_str());

    // Save history
    linenoise::SaveHistory(path);
  }

}

int main(int argc, char* argv[]) {
  std::string db_name, pool_path, query_file, dot_file, qmode_str, format = "ldbc";
  std::vector<std::string> import_files;
  bool start_shell = false;
  bool n4j_mode = false;
  qproc::mode qmode = qproc::Compile; 
  char delim_character = ',';
  bool strict = false;

  spdlog::info("Starting poseidon_cli, Version {}", POSEIDON_VERSION);

  try {
    options_description desc{"Options"};
    desc.add_options()
      ("help,h", "Help")
        ("verbose,v", bool_switch()->default_value(false), "Verbose - show debug output")
        ("db,d", value<std::string>(&db_name)->required(), "Database name (required)")
        ("pool,p", value<std::string>(&pool_path)->required(), "Path to the PMem/file pool")
        ("output,o", value<std::string>(&dot_file), "Dump the graph to the given file (in DOT format)")
        ("strict", bool_switch()->default_value(true), "Strict mode - assumes that all columns contain values of the same type")
        ("delimiter", value<char>(&delim_character)->default_value('|'), "Character delimiter")
        ("format,f", value<std::string>(&format), "CSV format: n4j | gtpc | ldbc")
        ("import", value<std::vector<std::string>>()->composing(),
        "Import files in CSV format (either nodes:<node type>:<filename> or "
        "relationships:<rship type>:<filename>")
        ("query,q", value<std::string>(&query_file), "Execute the query from the given file")
        ("shell,s", bool_switch()->default_value(false), "Start the interactive shell")
        ("qmode", value<std::string>(&qmode_str), "Query compile mode: llvm (default) | interp | adapt");

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

    if (vm.count("shell"))
      start_shell = vm["shell"].as<bool>();

    if (vm.count("qmode")) {
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

  if (access(pool_path.c_str(), F_OK) != 0) {
    spdlog::info("create poolset {}", pool_path);
    pool = graph_pool::create(pool_path);
    graph = pool->create_graph(db_name);
  } else {
    spdlog::info("open poolset {}", pool_path);
    pool = graph_pool::open(pool_path, true);
    graph = pool->open_graph(db_name);
  }

  if (!import_files.empty()) {
    spdlog::info("--------- Importing files ...");
    import_csv_files(graph, import_files, delim_character, format, strict);
    graph->print_stats();
  }

  if (!dot_file.empty())
    graph->dump_dot(dot_file);

  // graph->dump();

    /*
    {
        auto& nodes = graph->get_nodes();
        node n;
        std::cout << "chunk_size for nodes: " << nodes->as_vec().real_chunk_size() << " bytes\n"
                  << "size of a node      : " << sizeof(node) << " bytes\n"
                  << "offset of id_       : " << n._offset() << " bytes\n"
                  << "offset of node_label: " << ((uint64_t)((uint8_t *)&n.node_label) - (uint64_t)((uint8_t *)&n)) << " bytes"<< std::endl; 
    }
    */

  query_ctx ctx(graph);
  qproc_ptr = std::make_unique<qproc>(ctx);

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
 
