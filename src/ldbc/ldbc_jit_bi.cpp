#include "ldbc_jit_bi.hpp"


bool compiled = false;
std::vector<int64_t> runtimes;

double calc_avg() {
  auto avg = std::accumulate(runtimes.begin(), runtimes.end(), 0) / runtimes.size();
  runtimes.clear();
  compiled = false;
  scan_task::callee_ = &scan_task::scan;
  return avg;
}

bool q1_filter_cdate(int *prop_ptr) {
    auto prop = (p_item*)prop_ptr;
    return (*reinterpret_cast<const ptime *>(prop->value_)) < time_from_string(std::string("2017-04-14 01:51:21.746"));
    
}

bool q2_filter_cdate_1(int *prop_ptr) {
    auto prop = (p_item*)prop_ptr;
    auto d = *reinterpret_cast<const ptime *>(prop->value_);
    auto dt = time_from_string(std::string("2011-04-14 01:51:21.746"));
    time_period duration(dt, hours(24*100));
    return duration.contains(d) ? true : false;
}

bool q2_filter_cdate_2(int *prop_ptr) {
    auto prop = (p_item*)prop_ptr;
    auto d = *reinterpret_cast<const ptime *>(prop->value_);
    auto dt1 = time_from_string(std::string("2011-04-14 01:51:21.746"));
    time_period duration1(dt1, hours(24*100));
    auto dt2 = duration1.last();
    time_period duration2(dt2, hours(24*100));
    return duration2.contains(d) ? true : false;
}

bool q2_compute_diff(qr_tuple *q) {
    
}

std::vector<std::string> message = {"Post", "Comment"};

int dummy(node *n) {

}

void ldbc_jit_bi_query_1(graph_db_ptr &gdb, query_engine &qeng, result_set &rs, bool adaptive) {
    auto q = Scan(message, 
                Filter(Fct(q1_filter_cdate),
                    Project({{0, "creationDate", FTYPE::DATE}, 
                             {0, {"language", "imageFile"}, {"true", "false"}},
                             {0, "length", FTYPE::INT},
                             {0, dummy}},
                        GroupBy({0, 1, 3},
                            Aggr({{"count", 0}, {"avg", 2}, {"sum", 2}, {"pcount", 0}},
                                //Sort()
                                Collect())))));

    arg_builder ab;
    ab.arg(1, message[0]);
    ab.arg(2, message[1]);

    if(!compiled) {
        auto c_s = std::chrono::steady_clock::now();
        qeng.generate(q, adaptive);
        compiled = true;
        auto c_e = std::chrono::steady_clock::now();
        std::cout << "Compilation: " 
            << std::chrono::duration_cast<std::chrono::milliseconds>(c_e-c_s).count() 
            << " ms" << std::endl;
    
    }

    auto e_s = std::chrono::steady_clock::now();
    qeng.run(&rs, ab.args, false);
    auto e_e = std::chrono::steady_clock::now();
    
    std::cout << "Execution: " 
        << std::chrono::duration_cast<std::chrono::milliseconds>(e_e-e_s).count()
        << " ms" << std::endl;
}

void ldbc_jit_bi_query_2(graph_db_ptr &gdb, query_engine &qeng, result_set &rs, bool adaptive) {

}

void run_is_1(graph_db_ptr gdb, query_engine &qeng) {
    
    
    for(auto i = 0u; i < 1; i++) {
        result_set rs;
        ldbc_jit_bi_query_1(gdb, qeng, rs, false);
        std::cout << rs << std::endl;
    }

}

void run_benchmark(graph_db_ptr gdb, query_engine &qeng) {

}

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
	auto THREAD_NUM = 4;
	auto chunks = graph->get_nodes()->num_chunks();
	auto cv_range = chunks / THREAD_NUM;

	query_engine queryEngine(graph, THREAD_NUM, cv_range);

  for (auto i = 0; i < 1; i++) {
    std::cout << "Run: " << i << "\n";
    //run_benchmark(graph, queryEngine);
  }
}
