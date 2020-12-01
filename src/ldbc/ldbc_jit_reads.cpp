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

#include "ldbc_jit_reads.hpp"
#include "ldbc.hpp"

using namespace boost::program_options;

void ldbc_jit_is_query_1_a(graph_db_ptr &gdb, query_engine &qeng, result_set &rs, bool adaptive, uint64_t personId) {
  auto q = Scan("Person",
              Filter(EQ(Key(0, "id"), Int(personId)),
                ForeachRship(RSHIP_DIR::FROM, {}, ":isLocatedIn", 
                  Expand(EXPAND::OUT, "Place",
                    Project({{0, "firstName", FTYPE::STRING}, {0, "lastName", FTYPE::STRING},
                            {0, "birthday", FTYPE::DATE}, {0, "locationIP", FTYPE::STRING},
                            {0, "browserUsed", FTYPE::STRING}, {2, "id", FTYPE::STRING},
                            {0, "gender", FTYPE::STRING}, {0, "creationDate", FTYPE::TIME}}, 
                      Collect())))));
  arg_builder ab;
  ab.arg(1, "Person");
  ab.arg(2, personId);
  ab.arg(3, ":isLocatedIn");
  ab.arg(4, "Place");

  qeng.generate(q, adaptive);
  qeng.run(&rs, ab.args);
}

void ldbc_jit_is_query_1_b(graph_db_ptr &gdb, query_engine &qeng, result_set &rs, bool adaptive, uint64_t personId) {
  auto q = Scan("Place",
            ForeachRship(RSHIP_DIR::TO, {}, ":isLocatedIn",
              Expand(EXPAND::IN, "Person",
                Filter(EQ(Key(2, "id"), Int(personId)),
                  Project({{0, "firstName", FTYPE::STRING}, {0, "lastName", FTYPE::STRING},
                            {0, "birthday", FTYPE::DATE}, {0, "locationIP", FTYPE::STRING},
                            {0, "browserUsed", FTYPE::STRING}, {2, "id", FTYPE::STRING},
                            {0, "gender", FTYPE::STRING}, {0, "creationDate", FTYPE::TIME}}, 
                    Collect())))));

  arg_builder ab;
  ab.arg(1, "Place");
  ab.arg(2, ":isLocatedIn");
  ab.arg(3, ":Person");
  ab.arg(4, personId);

  qeng.generate(q, adaptive);
  qeng.run(&rs, ab.args);
}

void ldbc_jit_is_query_2_p(graph_db_ptr &gdb, query_engine &qeng, result_set &rs, bool adaptive, uint64_t personId) {
  auto q = Scan("Person", 
            Filter(EQ(Key(0, "id"), Int(personId)), 
              ForeachRship(RSHIP_DIR::TO, {}, ":hasCreator",
                Expand(EXPAND::IN, "Post",
                  Project({{2, "id", FTYPE::UINT64}, {2, "imageFile", FTYPE::STRING},
                           {2, "creationDate", FTYPE::TIME}, {2, "id", FTYPE::UINT64},
                           {0, "id", FTYPE::UINT64}, {0, "firstName", FTYPE::STRING},
                           {0, "lastName", FTYPE::STRING}}, Collect())))));
  
  arg_builder ab;
  ab.arg(1, "Person");
  ab.arg(2, personId);
  ab.arg(3, ":hasCreator");
  ab.arg(4, "Post");

  qeng.generate(q, adaptive);
  qeng.run(&rs, ab.args);
}

void ldbc_jit_is_query_2_c(graph_db_ptr &gdb, query_engine &qeng, result_set &rs, bool adaptive, uint64_t personId) {
  auto maxHops = 100;
  
  auto q  = Scan("Person",
              Filter(EQ(Key(0, "id"), Int(personId)), 
                ForeachRship(RSHIP_DIR::TO, {}, ":hasCreator",
                  Expand(EXPAND::IN, "Comment", 
                    ForeachRship(RSHIP_DIR::FROM, {1, maxHops}, ":replyOf",
                      Expand(EXPAND::OUT, "Post",
                        ForeachRship(RSHIP_DIR::FROM, {}, ":hasCreator",
                          Expand(EXPAND::OUT, "Person",
                            Project({{2, "id", FTYPE::UINT64}, {2, "content", FTYPE::STRING},
                                     {2, "creationDate", FTYPE::TIME}, {4, "id", FTYPE::UINT64},
                                     {6, "id", FTYPE::UINT64}, {6, "firstName", FTYPE::STRING},
                                     {6, "lastName", FTYPE::STRING}}, 
                              Sort([&](const qr_tuple &qr1, const qr_tuple &qr2) {
                                if (boost::get<boost::posix_time::ptime>(qr1[2]) == boost::get<boost::posix_time::ptime>(qr2[2]))
                                  return boost::get<uint64_t>(qr1[0]) > boost::get<uint64_t>(qr2[0]);
                                return boost::get<boost::posix_time::ptime>(qr1[2]) > boost::get<boost::posix_time::ptime>(qr2[2]); }, 
                                  Limit(10, Collect())))))))))));

  arg_builder ab;
  ab.arg(1, "Person");
  ab.arg(2, personId);
  ab.arg(3, ":hasCreator");
  ab.arg(4, "Comment");
  ab.arg(5, ":replyOf");
  ab.arg(6, "Post");
  ab.arg(7, ":hasCreator");
  ab.arg(8, "Person");

  qeng.generate(q, adaptive);
  qeng.run(&rs, ab.args);
}

void ldbc_jit_is_query_3(graph_db_ptr &gdb, query_engine &qeng, result_set &rs, bool adaptive, uint64_t personId) {
  auto q = Scan("Person",
            Filter(EQ(Key(0, "id"), Int(personId)), 
              ForeachRship(RSHIP_DIR::FROM, {}, ":knows",
                Expand(EXPAND::OUT, "Person",
                  Project({}, 
                    Sort([&](const qr_tuple &qr1, const qr_tuple &qr2) {
                          if (boost::get<boost::posix_time::ptime>(qr1[3]) == boost::get<boost::posix_time::ptime>(qr2[3]))
                            return boost::get<uint64_t>(qr1[0]) < boost::get<uint64_t>(qr2[0]);
                          return boost::get<boost::posix_time::ptime>(qr1[3]) > boost::get<boost::posix_time::ptime>(qr2[3]); },
                      Collect()))))));

}

void run_benchmark(graph_db_ptr gdb) {

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

  for (auto i = 0; i < 10; i++) {
          std::cout << "Run: " << i << "\n";
    run_benchmark(graph);
  }
}
