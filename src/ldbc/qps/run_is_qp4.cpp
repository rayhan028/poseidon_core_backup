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
        ldbc_is_qp4_query_2_c(gdb, rs, personIds[i]);
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

double run_query_7_p(graph_db_ptr gdb) {
    std::vector<uint64_t> postIds =
#ifdef SF_100
        {39582418599936, 17592187092992, 65971148923702, 43980918701267, 65971018451354,
        26388730323952, 8796096167946, 65971150117789};
#elif defined(SF_10)
        {4398074288624, 3298546507517, 5497587033299, 4398052366789, 6047338065986,
        8246359029970, 7146840417476, 6597099197111, 6047334688191, 4947831710060,
        2748817142166, 1649296572103, 5497575564527, 2199062360690, 6047335741465,
        2199044209868, 4398061553596, 7146844362356, 4947825543184, 6047344045696,
        8246348974651, 2199034832840, 6597082193070, 8246348170051, 6597107012577,
        6597101836499, 6047346170037, 4398081795065, 3298564597702, 8246368178974,
        2748818954854, 6047342805070, 5497562418044, 5497590305765, 5497590128285,
        4398086919921, 3848319343421, 8246361188746, 17215163, 7696621510202,
        8246341795356, 1099533817307, 4398082691758, 5497589393851, 7146828897760,
        5497570220329, 7696596013117, 1099514518363, 6047316070076, 7146847131440,
        5497559818606, 4398078795093};
#else
        {1374392023251, 549760018385, 137439936821, 1786707720205, 4359157,
        1924145803111, 1099514497795, 2199029560793, 824638049981, 1099514248244,
        1099517927842, 1374394239279, 2061590620755, 2061585333094, 1649273998043,
        1374391802860, 137440027420, 1099513852571, 1649272411554, 1786707657668};
#endif

    std::vector<double> runtimes(postIds.size());

    for (auto i = 0u; i < postIds.size(); i++) {
      result_set rs;
      auto start_qp = std::chrono::steady_clock::now();

      auto tx = gdb->begin_transaction();
      ldbc_is_qp4_query_7_p(gdb, rs, postIds[i]);
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

double run_query_7_c(graph_db_ptr gdb) {
    std::vector<uint64_t> commentIds =
#ifdef SF_100
        {39582418599937, 17592187092993, 52776560230402, 43980918701269, 70369064968225,
        26388730323960, 8796096167948, 8796545473432};
#elif defined(SF_10)
        {7146846240480, 5497578410380, 5497572128778, 3298557786474, 3298564869730,
        6597085241772, 5497562407626, 7146865899861, 2199063215958, 3298543129796,
        7696585937397, 3848303485145, 7696584725469, 8246372882931, 4624321,
        7696598517932, 6597097715505, 5497574002790, 2748783314629, 4947806953871,
        2199027045720, 8246351169549, 2199045162022, 4398062238469, 3298555317270,
        1649290760770, 3848327767348, 8246366635007, 8246346947901, 1099532289463,
        3848300985206, 5497581299875, 5497579358934, 2199063595159, 7696607737987,
        6047353121515, 7696590312905, 5497593006914, 6597089610781, 7146832026578,
        7696595067279, 1099545270694, 5497564138318, 1099537597905, 6047325524027,
        3298545496742, 5497572456680, 3848302106259, 2748794743559, 7146846003719,
        8246346437108, 7696607465071};
#else
        {549755814584, 962074006383, 1374390866272, 1511833539098, 687196097161,
        1786710956803, 1924148155034, 824636527214, 2061587107320, 274880712071,
        962075482675, 1786708701848, 2061588922925, 4784850, 4784913,
        412321645469, 1374394320184, 1374390902281, 1511834991008, 824634964783};
#endif

    std::vector<double> runtimes(commentIds.size());

    for (auto i = 0u; i < commentIds.size(); i++) {
      result_set rs;
      auto start_qp = std::chrono::steady_clock::now();

      auto tx = gdb->begin_transaction();
      ldbc_is_qp4_query_7_c(gdb, rs, commentIds[i]);
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
    t = run_query_7_p(gdb);
    spdlog::info("Query #7p: {} msecs", t);
    t = run_query_7_c(gdb);
    spdlog::info("Query #7c: {} msecs", t);
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