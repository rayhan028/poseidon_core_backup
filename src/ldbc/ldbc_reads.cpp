#include <iostream>
#include <boost/program_options.hpp>

#include "defs.hpp"
#include "graph_db.hpp"
#include "ldbc.hpp"
#include "config.h"

#include "threadsafe_queue.hpp"

#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"

#ifdef USE_PMDK

#define POOL_SIZE ((unsigned long long)(1024 * 1024 * 40000ull)) // 4000 MiB

const std::string test_path = poseidon::gPmemPath + "ldbc";

struct root {
  graph_db_ptr graph;
};

#endif

double calc_avg_time(const std::vector<double>& vec) {
    double d = 0.0;
    for (auto v : vec) {
        d += v;
    }
    return d / (double)vec.size();
}

double run_query_1(graph_db_ptr gdb) {
     std::vector<uint64_t> personIds = {933, 24189255812290, 6597069773744, 2199023266220, 13194139544176,
                                      17592186050570, 24189255815734, 28587302330379, 32985348842922, 3601,
                                      4398046511870, 32985348834284, 17592186045096, 17592186053245, 4398046520495,
                                      4233, 344, 10995116286457, 10976, 24189255813927};
    std::vector<double> runtimes(personIds.size());

    for (auto i = 0u; i < personIds.size(); i++) {
        result_set rs;
        auto start_qp = std::chrono::steady_clock::now();

        auto tx = gdb->begin_transaction();
        ldbc_is_query_1(gdb, rs, personIds[i]);
        gdb->commit_transaction();

        auto end_qp = std::chrono::steady_clock::now();
        runtimes[i] = std::chrono::duration_cast<std::chrono::milliseconds>(end_qp -
                                                                       start_qp).count();
    }
    return calc_avg_time(runtimes);
}

double run_query_2(graph_db_ptr gdb) {
 std::vector<uint64_t> personIds = {65, 28587302330379, 3601, 24189255817217, 4398046511870,
                                      8698, 6597069773744, 13194139544176, 17592186050570, 6597069766993,
                                      24189255815734, 26388279077330, 15393162799262, 32985348843825, 32985348843760,
                                      32985348842653, 13194139540894, 13194139540856, 8796093028361, 6597069766998};  
    std::vector<double> runtimes(personIds.size());

    for (auto i = 0u; i < personIds.size(); i++) {
        result_set rs;
        auto start_qp = std::chrono::steady_clock::now();

        auto tx = gdb->begin_transaction();
        ldbc_is_query_2_p(gdb, rs, personIds[i]);
        gdb->commit_transaction();

        auto end_qp = std::chrono::steady_clock::now();
        runtimes[i] = std::chrono::duration_cast<std::chrono::milliseconds>(end_qp -
                                                                       start_qp).count();
    }
    return calc_avg_time(runtimes);
}

void run_benchmark(graph_db_ptr gdb) {
    double t = 0.0;
    t = run_query_1(gdb);
    spdlog::info("Query #1: {} msecs", t);
    t = run_query_2(gdb);
    spdlog::info("Query #1: {} msecs", t);
}

/* ---------------------------------------------------------------------------- */

using namespace boost::program_options;

int main(int argc, char **argv) {
  std::string db_name;

 try {
    options_description desc{"Options"};
    desc.add_options()
        ("help,h", "Help")
        ("verbose,v", bool_switch()->default_value(false), "Verbose - show debug output")
        ("db,d", value<std::string>(&db_name)->required(),"Database name (required)");

    variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);

    if (vm.count("help")) {
      std::cout << "Poseidon Graph Database LDBC Benchmark Version " << POSEIDON_VERSION
                << "\n"
                << desc << '\n';
      return -1;
    }

    notify(vm);

      } catch (const error &ex) {
    std::cerr << ex.what() << '\n';
    return -1;
  }

  #ifdef USE_PMDK
  namespace nvm = pmem::obj;

  nvm::pool<root> pop;

  if (access(test_path.c_str(), F_OK) != 0) {
      std::cerr << "Cannot find pmem path '" << test_path << "'" << std::endl;
      return -1;
  } else {
    pop = nvm::pool<root>::open(test_path, db_name);
  }

  auto q = pop.root();
  if (!q->graph) {
      std::cerr << "Cannot open database '" << db_name << "'" << std::endl;
      return -1;
  }
  auto &graph = q->graph;
  graph->runtime_initialize();
#else
  auto graph = p_make_ptr<graph_db>(db_name);
#endif

    run_benchmark(graph);
}