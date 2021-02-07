#include <iostream>
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

// -------------------------------------------------------------------------------------------------------------------------

double calc_avg_time(const std::vector<double>& vec) {
    double d = 0.0;
    for (auto v : vec) {
        d += v;
        std::cout << v << " ";
    }
    std::cout << "\n";
    return d / (double)(vec.size() * 1000);
}

double run_query_2_c(graph_db_ptr gdb) {
    std::vector<uint64_t> personIds =
#ifdef SF_100
        {316996};
#elif defined(SF_10)
        {6597069786683, 4398046519893, 36226, 36675, 8796093036632, 13194139577652,
        10995116308345, 8796093050459, 4398046578984, 4398046530211, 4398046584079,
        10995116294248, 15393162815075, 8796093047284, 17592186050313, 26736,
        28587302371191, 72220, 8599, 10995116301838, 4398046574671, 21990232622930,
        21990232610624, 6597069796889, 26388279075075, 6597069819367, 4398046583349,
        21990232580825, 15393162794693, 13194139540856, 63829, 6597069789305, 21990232556528,
        4398046558107, 2199023273750, 8796093090229, 26388279098001, 15393162805575,
        4398046537118, 19791209361803, 10995116324570, 2199023283682, 10995116322754,
        2199023271643, 4398046562506, 17592186090419, 17592186113138, 2199023298757,
        72607, 8796093052442, 10995116318624, 8796093067457};
        /*{36226, 36675, 8796093036632, 13194139577652, 8796093050459, 10995116294248,
        2199023273750, 15393162805575, 10995116322754, 17592186090419};*/
#else
        {65, 28587302330379, 3601, 24189255817217, 4398046511870,
        8698, 6597069773744, 13194139544176, 17592186050570, 6597069766993,
        24189255815734, 26388279077330, 15393162799262, 32985348843825, 32985348843760,
        32985348842653, 13194139540894, 13194139540856, 8796093028361, 6597069766998};
        //{28587302330379};
#endif

    std::vector<double> runtimes(personIds.size());

    for (auto i = 0u; i < personIds.size(); i++) {
        result_set rs;
        auto start_qp = std::chrono::steady_clock::now();

        auto tx = gdb->begin_transaction();
        ldbc_is_qp5_query_2_c(gdb, rs, personIds[i]);
        gdb->commit_transaction();

        auto end_qp = std::chrono::steady_clock::now();
        runtimes[i] = std::chrono::duration_cast<std::chrono::microseconds>(end_qp -
                                                                       start_qp).count();
#ifdef PRINT_RESULT
        std::cout << rs << "\n";
#endif
    }
    return calc_avg_time(runtimes);
}

void run_benchmark(graph_db_ptr gdb) {
    double t = 0.0;
    t = run_query_2_c(gdb);
    spdlog::info("Query #2c: {} msecs", t);
}

/* ---------------------------------------------------------------------------- */

using namespace boost::program_options;

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
  
  run_benchmark(graph);
}