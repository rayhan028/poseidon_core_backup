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



std::vector<std::string> message = {"Post", "Comment"};

void ldbc_jit_bi_query_1(graph_db_ptr &gdb, query_engine &qeng, result_set &rs, bool adaptive) {
    auto q = Scan(message, 
                Filter(Call(Key(0, "creationDate"), Fct(q1_filter_cdate)),
                    Project({{0, "creationDate", FTYPE::TIME}, 
                             {0, {"language", "imageFile"}, {"true", "false"}},
                             {0, "length", FTYPE::INT},
                             {0, q1_group_msg_len}},
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

    std::cout << "Run query" << std::endl;
    auto e_s = std::chrono::steady_clock::now();
    qeng.run(&rs, ab, false);
    auto e_e = std::chrono::steady_clock::now();
    std::cout << "Complete" << std::endl;
    std::cout << "Execution: " 
        << std::chrono::duration_cast<std::chrono::milliseconds>(e_e-e_s).count()
        << " ms" << std::endl;
}



void ldbc_jit_bi_query_2(graph_db_ptr &gdb, query_engine &qeng, result_set &rs, bool adaptive) {
  std::cout << "Q2" << std::endl;
    auto q2 = Scan(message,
                Filter(Call(Key(0, "creationDate"), Fct(q2_filter_cdate_2)),
                  ForeachRship(RSHIP_DIR::FROM, {}, ":hasTag", 
                    Expand(EXPAND::OUT, "Tag",
                      GroupBy({2},
                        Aggr({{"count", 0}},
                          End(JOIN_OP::NESTED_LOOP, 0)))))));
    auto q1 = Scan(message,
                Filter(Call(Key(0, "creationDate"), Fct(q2_filter_cdate_1)),
                  ForeachRship(RSHIP_DIR::FROM, {}, ":hasTag",
                    Expand(EXPAND::OUT, "Tag",
                      GroupBy({2},
                        Aggr({{"count", 0}},
                          Join(JOIN_OP::NESTED_LOOP, {0,0}, 
                            Project({{0, "name", FTYPE::STRING}, {1}, {3}}, 
                              Append(q2_compute_diff, FTYPE::INT,
                              //Sort(),
                                Collect())), q2)))))));
    
    arg_builder ab;
    ab.arg(1, message[0]);
    ab.arg(2, message[1]);
    ab.arg(3, "creationDate");
    ab.arg(4, ":hasTag");
    ab.arg(5, "Tag");
    ab.arg(9, message[0]);
    ab.arg(10, message[1]);
    ab.arg(12, ":hasTag");
    ab.arg(13, "Tag");

    if(!compiled) {
        auto c_s = std::chrono::steady_clock::now();
        qeng.generate(q1, adaptive);
        compiled = true;
        auto c_e = std::chrono::steady_clock::now();
        std::cout << "Compilation: " 
            << std::chrono::duration_cast<std::chrono::milliseconds>(c_e-c_s).count() 
            << " ms" << std::endl;
    
    }

    auto e_s = std::chrono::steady_clock::now();
    qeng.run(&rs, ab, false);
    auto e_e = std::chrono::steady_clock::now();
    
    std::cout << "Execution: " 
        << std::chrono::duration_cast<std::chrono::milliseconds>(e_e-e_s).count()
        << " ms" << std::endl; 
}

void run_is_1(graph_db_ptr gdb, query_engine &qeng) {
    for(auto i = 0u; i < 1; i++) {
        result_set rs;
        ldbc_jit_bi_query_1(gdb, qeng, rs, false);
        std::cout << rs.data.size() << std::endl;
    }
}


void run_is_2(graph_db_ptr gdb, query_engine &qeng) {
    for(auto i = 0u; i < 1; i++) {
        result_set rs;
        ldbc_jit_bi_query_2(gdb, qeng, rs, false);
        std::cout << rs.data.size() << std::endl;
    }
}

void ldbc_jit_bi_query_3(graph_db_ptr &gdb, query_engine &qeng, result_set &rs, bool adaptive) {
    auto q = Scan("Place", 
              //Filter(Call(Key(0, "name"), Fct(q3_filter_cntry)),
                ForeachRship(RSHIP_DIR::TO, {}, ":isPartOf",
                  /*Expand(EXPAND::IN, "Place",
                    ForeachRship(RSHIP_DIR::TO, {}, ":isLocatedIn",
                      Expand(EXPAND::IN, "Person",
                        ForeachRship(RSHIP_DIR::TO, {}, ":hasModerator",
                          Expand(EXPAND::IN, "Forum", 
                            ForeachRship(RSHIP_DIR::FROM, {}, ":containerOf",
                              Expand(EXPAND::OUT, message,
                                ForeachRship(RSHIP_DIR::FROM, {}, ":hasTag",
                                  Expand(EXPAND::OUT, "Tag",
                                    ForeachRship(RSHIP_DIR::FROM, {}, ":hasType", 
                                      Expand(EXPAND::OUT, "Tagclass",
                                        Filter(Call(Key(12, "name"), Fct(q3_filter_tgclass)),
                                            GroupBy({6, 4},
                                              Aggr({{"count", 0}},*/
                                                //Project({/*{0, "id", FTYPE::UINT64},*/ {0, "title", FTYPE::STRING}/*, {0, "creationDate", FTYPE::TIME}, {1, "id", FTYPE::UINT64}, {2}*/},
                                                  //Sort
                                                      Collect()));

    arg_builder ab;
    ab.arg(1, "Place");
    ab.arg(2, "name");
    ab.arg(3, ":isPartOf");
    ab.arg(4, "Place");
    ab.arg(5, ":isLocatedIn");
    ab.arg(6, "Person");
    ab.arg(7, ":hasModerator");
    ab.arg(8, "Forum");
    ab.arg(9, ":containerOf");
    ab.arg(10, message[0]);
    ab.arg(11, message[1]);
    ab.arg(12, ":hasTag");
    ab.arg(13, "Tag");
    ab.arg(14, ":hasType");
    ab.arg(15, "Tagclass");
    ab.arg(16, "name");

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
    qeng.run(&rs, ab, false);
    auto e_e = std::chrono::steady_clock::now();
    
    std::cout << "Execution: " 
        << std::chrono::duration_cast<std::chrono::milliseconds>(e_e-e_s).count()
        << " ms" << std::endl; 
}

void run_is_3(graph_db_ptr gdb, query_engine &qeng) {
    for(auto i = 0u; i < 1; i++) {
        result_set rs;
        ldbc_jit_bi_query_3(gdb, qeng, rs, false);
        std::cout << rs << std::endl;
    }
}



void ldbc_jit_bi_query_4(graph_db_ptr &gdb, query_engine &qeng, result_set &rs, bool adaptive) {
  //std::vector<params_tuple> q4_params = {{"Turkey", time_from_string(std::string("2011-04-14 01:51:21.746"))}};

    auto sort_fct1 = [&](const qr_tuple &q1, const qr_tuple &q2) {
                return boost::get<uint64_t>(q1[1]) > boost::get<uint64_t>(q2[1]); };

    auto sort_fct2 = [&](const qr_tuple &q1, const qr_tuple &q2) {
                if (boost::get<uint64_t>(q1[4]) == boost::get<uint64_t>(q2[4]))
                  return boost::get<uint64_t>(q1[0]) < boost::get<uint64_t>(q2[0]);
                return boost::get<uint64_t>(q1[4]) > boost::get<uint64_t>(q2[4]); };



    auto q1 = Scan("Place",
                Filter(Fct(q3_filter_cntry),
                  ForeachRship(RSHIP_DIR::TO, {}, ":isPartOf",
                    Expand(EXPAND::IN, "Place",
                      ForeachRship(RSHIP_DIR::TO, {}, ":isLocatedIn",
                        Expand(EXPAND::IN, "Person",
                          ForeachRship(RSHIP_DIR::TO, {}, ":hasMember",
                            Expand(EXPAND::IN, "Forum",
                              Project({{6}, {6, "creationDate", FTYPE::TIME}},
                                //Filter(GT(Tuple(1), Time(q4_params[1])),
                                  GroupBy({0},
                                    Aggr({{"count", 0}},
                                      //Sort()
                                        Limit(100,
                                          End(JOIN_OP::HASH_JOIN, 0)))))))))))));

    auto q2 = Scan("Place",
                Filter(Fct(q3_filter_cntry),
                  ForeachRship(RSHIP_DIR::TO, {}, ":isPartOf",
                    Expand(EXPAND::IN, "Place",
                      ForeachRship(RSHIP_DIR::TO, {}, ":isLocatedIn",
                        Expand(EXPAND::IN, "Person",
                          ForeachRship(RSHIP_DIR::TO, {}, ":hasMember",
                            Expand(EXPAND::IN, "Forum",
                              Project({{6}, {6, "creationDate", FTYPE::TIME}},
                                //Filter(GT(Tuple(1), Time(q4_params[1])),
                                  GroupBy({0},
                                    Aggr({{"count", 0}},
                                      Sort(sort_fct1,
                                        Limit(100,
                                          ForeachRship(RSHIP_DIR::FROM, ":hasMember", 0,
                                            Expand(EXPAND::OUT, "Person",
                                              ForeachRship(RSHIP_DIR::TO, {}, ":hasCreator",
                                                Expand(EXPAND::IN, "Post",
                                                  ForeachRship(RSHIP_DIR::TO, {}, ":containerOf",
                                                    Expand(EXPAND::IN, "Forum",
                                                      Join(JOIN_OP::HASH_JOIN, {7,0}, 
                                                        GroupBy({3},
                                                          Aggr({{"count", 0}},
                                                            Project({{0, "id", FTYPE::UINT64}, {0, "firstName", FTYPE::STRING}, {0, "lastName", FTYPE::STRING}, {0, "creationDate", FTYPE::TIME}, {1}},
                                                              Sort(sort_fct2,
                                                                Collect())))), q1))))))))))))))))))));

    arg_builder ab;
    ab.arg(1, "Place");
    ab.arg(3, ":isPartOf");
    ab.arg(4, "Place");
    ab.arg(5, ":isLocatedIn");
    ab.arg(6, "Person");
    ab.arg(7, ":hasMember");
    ab.arg(8, "Forum");
    ab.arg(9, ":hasMember");
    ab.arg(10, "Person");
    ab.arg(11, ":hasCreator");
    ab.arg(12, "Post");
    ab.arg(13, ":containerOf");
    ab.arg(14, "Forum");

    if(!compiled) {
        auto c_s = std::chrono::steady_clock::now();
        qeng.generate(q2, adaptive);
        compiled = true;
        auto c_e = std::chrono::steady_clock::now();
        std::cout << "Compilation: " 
            << std::chrono::duration_cast<std::chrono::milliseconds>(c_e-c_s).count() 
            << " ms" << std::endl;
    
    }

    auto e_s = std::chrono::steady_clock::now();
    qeng.run(&rs, ab, false);
    auto e_e = std::chrono::steady_clock::now();
    
    std::cout << "Execution: " 
        << std::chrono::duration_cast<std::chrono::milliseconds>(e_e-e_s).count()
        << " ms" << std::endl; 
}

void run_is_4(graph_db_ptr gdb, query_engine &qeng) {
    for(auto i = 0u; i < 1; i++) {
        result_set rs;
        ldbc_jit_bi_query_4(gdb, qeng, rs, false);
        std::cout << rs.data.size() << std::endl;
    }
}

/*
void ldbc_bi_query_5(graph_db_ptr &gdb, result_set &rs, params_tuple params) {

    auto q3 = Scan("Tag",
                Filter(Fct(q5_filter_tag), 
                  ForeachRship(RSHIP_DIR::TO, {}, ":hasTag",
                    Expand(EXPAND::IN, "",
                      ForeachRship(RSHIP_DIR::FROM, {}, ":hasCreator",
                        Expand(EXPAND::OUT, "Person",
                          GroupBy({4},
                            Aggr({{"count", 0}},
                              End(JOIN_OP::NESTED_LOOP, 0)))))))));

    auto q2 = Scan("Tag",
                Filter(Fct(q5_filter_tag),
                  ForeachRship(RSHIP_DIR::TO, {}, ":hasTag",
                    Expand(EXPAND::IN, "", 
                      ForeachRship(RSHIP_DIR::FROM, {}, ":hasCreator",
                        Expand(EXPAND::OUT, "Person",
                          ForeachRship(RSHIP_DIR::TO, ":likes", 2,
                            Expand(EXPAND::IN, "Person",
                              GroupBy({4},
                                Aggr({{"count", 0}},
                                  Join(JOIN_OP::NESTED_LOOP, {0,0}, 
                                    End(JOIN_OP::NESTED_LOOP, 0),
                                  q3)))))))))));
    auto q1 = Scan("Tag",
                Filter(Fct(q5_filter_tag),
                  ForeachRship(RSHIP_DIR::TO, {}, ":hasTag",
                    Expand(EXPAND::IN, "",
                      ForeachRship(RSHIP_DIR::FROM, {}, ":hasCreator",
                        Expand(EXPAND::OUT, "Person",
                          ForeachRship(RSHIP_DIR::TO, ":replyOf", 2,
                            Expand(EXPAND::IN, "Comment",
                              GroupBy({4},
                                Aggr({{"count", 0}},
                                  Join(JOIN_OP::NESTED_LOOP, {0,0},
                                    Append(q5_score_func, FTYPE::INT,
                                      Limit(100,
                                        Project({{0, "id", FTYPE::STRING},
                                                {1},
                                                {3},
                                                {5},./run_ldbc_jit_bi --db sf1 --pool .
                                            Collect())))), 
                                  q2)))))))))));
}

void ldbc_bi_query_6(graph_db_ptr &gdb, result_set &rs, params_tuple params) {
    auto q2 = Scan("Tag",
                Filter(Fct(q6_filter_tag),
                  ForeachRship(RSHIP_DIR::TO, {}, ":hasTag",
                    Expand(EXPAND::IN, message,
                      ForeachRship(RSHIP_DIR::FROM, {}, ":hasCreator",
                        Expand(EXPAND::OUT, "Person",
                          ForeachRship(RSHIP_DIR::TO, {}, ":hasCreator",
                            Expand(EXPAND::IN, message,
                              ForeachRship(RSHIP_DIR::TO, {}, ":likes",
                                Expand(EXPAND::IN, "Person",
                                  Project({8},
                                    ForeachRship(RSHIP_DIR::TO, {}, ":hasCreator",
                                      Expand(EXPAND::IN, message,
                                        ForeachRship(RSHIP_DIR::TO, {}, ":likes",
                                          Expand(EXPAND::IN, "Person",
                                            GroupBy({0},
                                              Aggr({{"count", 0}},
                                                End(JOIN_OP::NESTED_LOOP, 0))))))))))))))))));
    auto q1 = Scan("Tag",
                Filter(Fct(q6_filter_tag),
                  ForeachRship(RSHIP_DIR::TO, {}, ":hasTag",
                    Expand(EXPAND::IN, message,
                      ForeachRship(RSHIP_DIR::FROM, {}, ":hasCreator",
                        Expand(EXPAND::OUT, "Person",
                          ForeachRship(RSHIP_DIR::TO, {}, ":hasCreator",
                            Expand(EXPAND::IN, message,
                              ForeachRship(RSHIP_DIR::TO, {}, ":likes",
                                Expand(EXPAND::IN, "Person",
                                  Project({{4}, {8}},
                                    Join(JOIN_OP::HASH_JOIN, {1, 0}, 
                                      GroupBy({0}, 
                                        Aggr({{"sum", 3}},
                                          Project({{0, "id", FTYPE::UINT64}, {1}},
                                            Limit(100,
                                              Sort(q6_sort_fct,
                                                Collect()))))),
                                    q2))))))))))));
}


void ldbc_bi_query_6(graph_db_ptr &gdb, result_set &rs, params_tuple params) {
      auto filter_tag =
      [&](auto &prop) {
        auto c1 = *(reinterpret_cast<const dcode_t *>(prop.value_));
        auto c2 = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0]));
        return c1 == c2;
      };

  auto q2 = Scan("Tag",
              Filter(Fct(filter_tag),
                FromRships(":hasTag",
                  ExpandIn(message,
                    FromRships(":hasCreator",
                      ExpandOut("Person",
                        ToRships(":hasCreator",
                          ExpandIn(message,
                            ToRships(":likes",
                              ExpandIn("Person",
                                Project({{8}},
                                  ToRships(":likes",
                                    ExpandIn(message,
                                      ToRships(":likes",
                                        ExpandIn("Person",
                                          GroupBy({0},
                                            Aggr({{"count", 0}},
                                              End()))))))))))))))));

  auto q1 = Scan("Tag",
              Filter(Fct(filter_tag),
                ToRships(":hasTag",
                  ExpandIn(message,
                    FromRships(":hasCreator",
                      ExpandOut("Person",
                        ToRships(":hasCreator",
                          ExpandIn(message,
                            ToRships(":likes",
                              ExpandIn("Person",
                                Project({{4}, {8}},
                                  Join(JOIN_OP::HASH_JOIN, {1, 0}, GroupBy({0}, Aggr({{"sum", 3}}, Project({{0, "id", FTYPE::UINT}, {1}}, Limit(100, Collect())))), q2))))))))))));
}

void ldbc_bi_query_6(graph_db_ptr &gdb, result_set &rs, params_tuple params) {
    auto filter_tag =
      [&](auto &prop) {
        auto c1 = *(reinterpret_cast<const dcode_t *>(prop.value_));
        auto c2 = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0]));
        return c1 == c2;
      };

    auto drop_tag =
      [&](auto &prop) {
        auto c1 = *(reinterpret_cast<const dcode_t *>(prop.value_));
        auto c2 = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0]));
        return c1 != c2;
      };


}
*/

void run_benchmark(graph_db_ptr gdb, query_engine &qeng) {
  g = gdb;
  run_is_1(gdb, qeng);
  compiled = false;
  //qeng.cleanup();
  //run_is_4(gdb, qeng);
  //compiled = false;
  qeng.cleanup();
  run_is_3(gdb, qeng);
}

int main(int argc, char **argv) {
  bool strict = false;
  std::string pool_path, db_name;
  std::string snb_home =
#ifdef SF_10
    "/home/data/SNB_SF_1/";
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
    #if defined(RUN_INDEXED) && defined(FPTree)
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
    run_benchmark(graph, queryEngine);
  }
}
