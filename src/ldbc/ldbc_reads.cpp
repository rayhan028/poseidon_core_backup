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

double run_query_2_p(graph_db_ptr gdb) {
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

double run_query_2_c(graph_db_ptr gdb) {
    std::vector<uint64_t> personIds = {65, 28587302330379, 3601, 24189255817217, 4398046511870,
                                      8698, 6597069773744, 13194139544176, 17592186050570, 6597069766993,
                                      24189255815734, 26388279077330, 15393162799262, 32985348843825, 32985348843760,
                                      32985348842653, 13194139540894, 13194139540856, 8796093028361, 6597069766998};  
    std::vector<double> runtimes(personIds.size());

    for (auto i = 0u; i < personIds.size(); i++) {
        result_set rs;
        auto start_qp = std::chrono::steady_clock::now();

        auto tx = gdb->begin_transaction();
        ldbc_is_query_2_c(gdb, rs, personIds[i]);
        gdb->commit_transaction();

        auto end_qp = std::chrono::steady_clock::now();
        runtimes[i] = std::chrono::duration_cast<std::chrono::milliseconds>(end_qp -
                                                                       start_qp).count();
    }
    return calc_avg_time(runtimes);
}

double run_query_3(graph_db_ptr gdb) {
    std::vector<uint64_t> personIds = {19791209304051, 28587302326940, 2199023262021, 8796093027111, 2199023262994,
                                    6597069773744, 13194139544176, 17592186050570, 30786325588658, 24189255815734,
                                    6597069774931, 13194139544258, 15393162791382, 21990232558836, 28587302322686,
                                    24189255820923, 32985348833548, 30786325581208, 26388279074032, 32985348834375};
    std::vector<double> runtimes(personIds.size());

    for (auto i = 0u; i < personIds.size(); i++) {
      result_set rs;
      auto start_qp = std::chrono::steady_clock::now();

      auto tx = gdb->begin_transaction();
      ldbc_is_query_3(gdb, rs, personIds[i]);
      gdb->commit_transaction();

      auto end_qp = std::chrono::steady_clock::now();
      runtimes[i] = std::chrono::duration_cast<std::chrono::milliseconds>(end_qp -
                                                                       start_qp).count();
    }
    return calc_avg_time(runtimes);
}

double run_query_4_p(graph_db_ptr gdb) {
    std::vector<uint64_t> postIds = {1374389534801, 687194926510, 1236950581577, 824633724379, 687194903818,
                                      549755930326, 1649267546616, 1649267453265, 1924145376549, 1099511719169};
    std::vector<double> runtimes(postIds.size());

    for (auto i = 0u; i < postIds.size(); i++) {
      result_set rs;
      auto start_qp = std::chrono::steady_clock::now();

      auto tx = gdb->begin_transaction();
      ldbc_is_query_4_p(gdb, rs, postIds[i]);
      gdb->commit_transaction();

      auto end_qp = std::chrono::steady_clock::now();
      runtimes[i] = std::chrono::duration_cast<std::chrono::milliseconds>(end_qp -
                                                                       start_qp).count();
    }
    return calc_avg_time(runtimes);
}

double run_query_4_c(graph_db_ptr gdb) {
    std::vector<uint64_t> commentIds = {1236950581249, 1374389535139, 687194767797, 962072674365, 274877974096,
                                      1374389620660, 1374389535186, 2061584302604, 1099511678319, 1099511755889};
    std::vector<double> runtimes(commentIds.size());

    for (auto i = 0u; i < commentIds.size(); i++) {
      result_set rs;
      auto start_qp = std::chrono::steady_clock::now();

      auto tx = gdb->begin_transaction();
      ldbc_is_query_4_c(gdb, rs, commentIds[i]);
      gdb->commit_transaction();

      auto end_qp = std::chrono::steady_clock::now();
      runtimes[i] = std::chrono::duration_cast<std::chrono::milliseconds>(end_qp -
                                                                       start_qp).count();
    }
    return calc_avg_time(runtimes);
}

double run_query_5_p(graph_db_ptr gdb) {
    std::vector<uint64_t> postIds = {1649267611029, 1649267641500, 1649267717129, 549756117312, 962073027971,
                                      1924145709571, 1786706759766, 137439322338, 962073047211, 1786706792809};
    std::vector<double> runtimes(postIds.size());

    for (auto i = 0u; i < postIds.size(); i++) {
      result_set rs;
      auto start_qp = std::chrono::steady_clock::now();

      auto tx = gdb->begin_transaction();
      ldbc_is_query_5_p(gdb, rs, postIds[i]);
      gdb->commit_transaction();

      auto end_qp = std::chrono::steady_clock::now();
      runtimes[i] = std::chrono::duration_cast<std::chrono::milliseconds>(end_qp -
                                                                       start_qp).count();
    }
    return calc_avg_time(runtimes);
}

double run_query_5_c(graph_db_ptr gdb) {
    std::vector<uint64_t> commentIds = {2061584429975, 1099511764068, 1511828638961, 1099511794459, 1924145529653,
                                      137439153914, 1374389758562, 687194998602, 1099511869402, 1649267722310};
    std::vector<double> runtimes(commentIds.size());

    for (auto i = 0u; i < commentIds.size(); i++) {
      result_set rs;
      auto start_qp = std::chrono::steady_clock::now();

      auto tx = gdb->begin_transaction();
      ldbc_is_query_5_c(gdb, rs, commentIds[i]);
      gdb->commit_transaction();

      auto end_qp = std::chrono::steady_clock::now();
      runtimes[i] = std::chrono::duration_cast<std::chrono::milliseconds>(end_qp -
                                                                       start_qp).count();
    }
    return calc_avg_time(runtimes);
}

double run_query_6_p(graph_db_ptr gdb) {
    std::vector<uint64_t> postIds = {1374389534795, 3, 246, 1786710746552, 1786710746860,
                                      962077492609, 4818574, 137443772206, 4818783, 1649273779906,
                                      1099512706784, 1924145709571, 274879100510, 2061585683162, 2061585683383,
                                      824638318943, 962073868902, 962076990540, 1236955780271, 1924151699930};
    std::vector<double> runtimes(postIds.size());

    for (auto i = 0u; i < postIds.size(); i++) {
      result_set rs;
      auto start_qp = std::chrono::steady_clock::now();

      auto tx = gdb->begin_transaction();
      ldbc_is_query_6_p(gdb, rs, postIds[i]);
      gdb->commit_transaction();

      auto end_qp = std::chrono::steady_clock::now();
      runtimes[i] = std::chrono::duration_cast<std::chrono::milliseconds>(end_qp -
                                                                       start_qp).count();
    }
    return calc_avg_time(runtimes);
}

double run_query_6_c(graph_db_ptr gdb) {
    std::vector<uint64_t> commentIds = {549756150652, 2061587049723, 1786710610862, 1924150141935, 1649271672251,
                                      1099518023455, 1511835112930, 962079298952, 549762439424, 1786707596571,
                                      824635086444, 2199024637100, 549762296256, 412319368884, 1924148311956,
                                      687196868319, 1786710956334, 2882812, 274878321446, 687194840176};
    std::vector<double> runtimes(commentIds.size());

    for (auto i = 0u; i < commentIds.size(); i++) {
      result_set rs;
      auto start_qp = std::chrono::steady_clock::now();

      auto tx = gdb->begin_transaction();
      ldbc_is_query_6_c(gdb, rs, commentIds[i]);
      gdb->commit_transaction();

      auto end_qp = std::chrono::steady_clock::now();
      runtimes[i] = std::chrono::duration_cast<std::chrono::milliseconds>(end_qp -
                                                                       start_qp).count();
    }
    return calc_avg_time(runtimes);
}

double run_query_7(graph_db_ptr gdb) {
    std::vector<uint64_t> commentIds = {549755814584, 962074006383, 1374390866272, 1511833539098, 687196097161,
                                      1786710956803, 1924148155034, 824636527214, 2061587107320, 274880712071,
                                      962075482675, 1786708701848, 2061588922925, 4784850, 4784913,
                                      412321645469, 1374394320184, 1374390902281, 1511834991008, 824634964783};
    std::vector<double> runtimes(commentIds.size());

    for (auto i = 0u; i < commentIds.size(); i++) {
      result_set rs;
      auto start_qp = std::chrono::steady_clock::now();

      auto tx = gdb->begin_transaction();
      ldbc_is_query_7(gdb, rs, commentIds[i]);
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
    t = run_query_2_p(gdb);
    spdlog::info("Query #2p: {} msecs", t);
    t = run_query_2_c(gdb);
    spdlog::info("Query #2c: {} msecs", t);
    t = run_query_3(gdb);
    spdlog::info("Query #3: {} msecs", t);
    t = run_query_4_p(gdb);
    spdlog::info("Query #4p: {} msecs", t);
    t = run_query_4_c(gdb);
    spdlog::info("Query #4c: {} msecs", t);
    t = run_query_5_p(gdb);
    spdlog::info("Query #5p: {} msecs", t);
    t = run_query_5_c(gdb);
    spdlog::info("Query #5c: {} msecs", t);
    t = run_query_6_p(gdb);
    spdlog::info("Query #6p: {} msecs", t);
    t = run_query_6_c(gdb);
    spdlog::info("Query #6c: {} msecs", t);
    t = run_query_7(gdb);
    spdlog::info("Query #7: {} msecs", t);
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