#include <iostream>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

#include "defs.hpp"
#include "graph_db.hpp"
#include "ldbc.hpp"
#include "config.h"

#include "thread_pool.hpp"

#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"

#define SF_10
#define CREATE_INDEX
#define PARALLEL_LOAD
// #define PARALLEL_RSHIP_LOAD

#ifdef USE_PMDK

const std::string test_path = poseidon::gPmemPath +

#ifdef SF_10
"sf10";
#define POOL_SIZE ((unsigned long long)(1024 * 1024 * 160000ull)) // 16000 MiB
#else
"sf1";
#define POOL_SIZE ((unsigned long long)(1024 * 1024 * 40000ull)) // 4000 MiB
#endif

struct root {
  graph_db_ptr graph;
};

#endif

void load_snb_data(graph_db_ptr &graph, 
                    std::vector<std::string> &node_files,
                    std::vector<std::string> &rship_files, bool strict = true) {
  auto delim = '|';
  graph_db::mapping_t mapping;
  bool nodes_imported = false, rships_imported = false;
  std::mutex imtx;

  if (!node_files.empty()) {
    spdlog::info("--------- Importing nodes...");

#ifdef PARALLEL_LOAD
    std::vector<std::future<std::pair<std::string, std::size_t>>> res;
    res.reserve(node_files.size());
    thread_pool pool;

    for (auto &file : node_files) {
      res.push_back(pool.submit([&](){
        std::vector<std::string> fp;
        boost::split(fp, file, boost::is_any_of("/"));
        assert(fp.back().find(".csv") != std::string::npos);
        auto pos = fp.back().find("_");
        auto label = fp.back().substr(0, pos);
        if (label[0] >= 'a' && label[0] <= 'z')
          label[0] -= 32;

        // spdlog::info("processing file '{}'...", file);
        auto num_nodes = strict 
          ? graph->import_typed_nodes_from_csv(label, file, delim, mapping, &imtx)
          : graph->import_nodes_from_csv(label, file, delim, mapping, &imtx);   
        // spdlog::info("file '{}' finished.", file);
        return std::make_pair(label, num_nodes);     
      }));
    }
    for (auto &f : res) {
      auto resp = f.get();
      spdlog::info("{} '{}' node objects imported", resp.second, resp.first);
    }
#else
    for (auto &file : node_files) {
      std::vector<std::string> fp;
      boost::split(fp, file, boost::is_any_of("/"));
      assert(fp.back().find(".csv") != std::string::npos);
      auto pos = fp.back().find("_");
      auto label = fp.back().substr(0, pos);
      if (label[0] >= 'a' && label[0] <= 'z')
        label[0] -= 32;

      auto num_nodes = strict 
        ? graph->import_typed_nodes_from_csv(label, file, delim, mapping)
        : graph->import_nodes_from_csv(label, file, delim, mapping);
      spdlog::info("{} '{}' node objects imported", num_nodes, label);
    }
#endif
  }

  if (!rship_files.empty()) {
    spdlog::info("--------- Importing relationships ...");
    
#ifdef PARALLEL_RSHIP_LOAD
    std::vector<std::future<std::pair<std::string, std::size_t>>> res;
    res.reserve(rship_files.size());
    thread_pool pool;

    for (auto &file : rship_files) {
      res.push_back(pool.submit([&](){
        std::vector<std::string> fp;
        boost::split(fp, file, boost::is_any_of("/"));
        assert(fp.back().find(".csv") != std::string::npos);
        std::vector<std::string> fn;
        boost::split(fn, fp.back(), boost::is_any_of("_"));
        auto label = ":" + fn[1];

        auto num_rships = strict 
          ? graph->import_typed_relationships_from_csv(file, delim, mapping, &imtx)
          : graph->import_relationships_from_csv(file, delim, mapping, &imtx);
        char buf[100];
        sprintf(buf, "(%s)-[%s]-(%s)", fn[0].c_str(), label.c_str(), fn[2].c_str());
        return std::make_pair(std::string(buf), num_rships);     
      }));
    }
    for (auto &f : res) {
      auto resp = f.get();
      spdlog::info("{} '{}' relationship objects imported", resp.second, resp.first);
    }
#else
    for (auto &file : rship_files) {
      std::vector<std::string> fp;
      boost::split(fp, file, boost::is_any_of("/"));
      assert(fp.back().find(".csv") != std::string::npos);
      std::vector<std::string> fn;
      boost::split(fn, fp.back(), boost::is_any_of("_"));
      auto label = ":" + fn[1];

      auto num_rships = strict 
      ? graph->import_typed_relationships_from_csv(file, delim, mapping)
      : graph->import_relationships_from_csv(file, delim, mapping);
      spdlog::info("{} ({})-[{}]-({}) relationship objects imported", 
        num_rships, fn[0], label, fn[2]);
    }
    #endif
  }
}

using namespace boost::program_options;

int main(int argc, char **argv) {
  bool strict = false;
  std::string db_name, log_file;
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
        ("import,i", value<std::string>(&snb_home), "Path to directories containing SNB CSV files")
        ("log,l", value<std::string>(&log_file), "Write log messages to the given file")
        ("db,d", value<std::string>(&db_name)->required(),"Database name (required)");

    variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);

    if (vm.count("help")) {
      std::cout << "Poseidon Graph Database LDBC Importer Version " << POSEIDON_VERSION
                << "\n"
                << desc << '\n';
      return -1;
    }
    if (vm.count("import"))
      db_name = vm["import"].as<std::string>();

    if (vm.count("log"))
      log_file = vm["log"].as<std::string>();

    if (vm.count("strict"))
      strict = vm["strict"].as<bool>();


    notify(vm);

      } catch (const error &ex) {
    std::cerr << ex.what() << '\n';
    return -1;
  }

  std::shared_ptr<spdlog::logger> file_logger;
  if (!log_file.empty()) {
    file_logger = spdlog::basic_logger_mt("basic_logger", log_file);
    spdlog::set_default_logger(file_logger);  
  }
  if (strict)
    spdlog::info("Using strict mode");
  else
    spdlog::info("Using non-strict mode");
  
  #ifdef USE_PMDK
  namespace nvm = pmem::obj;

  nvm::pool<root> pop;

  if (access(test_path.c_str(), F_OK) != 0) {
    pop = nvm::pool<root>::create(test_path, db_name, POOL_SIZE);
  } else {
    pop = nvm::pool<root>::open(test_path, db_name);
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

  std::string snb_sta = snb_home + "/static/";
  std::string snb_dyn = snb_home + "/dynamic/";

 std::vector<std::string> node_files = {snb_sta + "place_0_0.csv", snb_sta + "place_1_0.csv",
                                         snb_sta + "place_2_0.csv", snb_sta + "place_3_0.csv",
                                         snb_sta + "organisation_0_0.csv", snb_sta + "organisation_1_0.csv",
                                         snb_sta + "organisation_2_0.csv", snb_sta + "organisation_3_0.csv",
                                         snb_sta + "tagclass_0_0.csv", snb_sta + "tagclass_1_0.csv",
                                         snb_sta + "tagclass_2_0.csv", snb_sta + "tagclass_3_0.csv",
                                         snb_sta + "tag_0_0.csv", snb_sta + "tag_1_0.csv",
                                         snb_sta + "tag_2_0.csv", snb_sta + "tag_3_0.csv",
                                         snb_dyn + "comment_0_0.csv", snb_dyn + "comment_1_0.csv",
                                         snb_dyn + "comment_2_0.csv", snb_dyn + "comment_3_0.csv",
                                         snb_dyn + "forum_0_0.csv", snb_dyn + "forum_1_0.csv",
                                         snb_dyn + "forum_2_0.csv", snb_dyn + "forum_3_0.csv",
                                         snb_dyn + "person_0_0.csv", snb_dyn + "person_1_0.csv",
                                         snb_dyn + "person_2_0.csv", snb_dyn + "person_3_0.csv",
                                         snb_dyn + "post_0_0.csv", snb_dyn + "post_1_0.csv",
                                         snb_dyn + "post_2_0.csv", snb_dyn + "post_3_0.csv"};

  std::vector<std::string> rship_files = {snb_dyn + "comment_hasCreator_person_0_0.csv",
                                          snb_dyn + "comment_hasCreator_person_1_0.csv",
                                          snb_dyn + "comment_hasCreator_person_2_0.csv",
                                          snb_dyn + "comment_hasCreator_person_3_0.csv",
                                          snb_dyn + "comment_isLocatedIn_place_0_0.csv",
                                          snb_dyn + "comment_isLocatedIn_place_1_0.csv",
                                          snb_dyn + "comment_isLocatedIn_place_2_0.csv",
                                          snb_dyn + "comment_isLocatedIn_place_3_0.csv",
                                          snb_dyn + "comment_replyOf_comment_0_0.csv",
                                          snb_dyn + "comment_replyOf_comment_1_0.csv",
                                          snb_dyn + "comment_replyOf_comment_2_0.csv",
                                          snb_dyn + "comment_replyOf_comment_3_0.csv",
                                          snb_dyn + "comment_replyOf_post_0_0.csv",
                                          snb_dyn + "comment_replyOf_post_1_0.csv",
                                          snb_dyn + "comment_replyOf_post_2_0.csv",
                                          snb_dyn + "comment_replyOf_post_3_0.csv",
                                          snb_dyn + "forum_containerOf_post_0_0.csv",
                                          snb_dyn + "forum_containerOf_post_1_0.csv",
                                          snb_dyn + "forum_containerOf_post_2_0.csv",
                                          snb_dyn + "forum_containerOf_post_3_0.csv",
                                          snb_dyn + "forum_hasMember_person_0_0.csv",
                                          snb_dyn + "forum_hasMember_person_1_0.csv",
                                          snb_dyn + "forum_hasMember_person_2_0.csv",
                                          snb_dyn + "forum_hasMember_person_3_0.csv",
                                          snb_dyn + "forum_hasModerator_person_0_0.csv",
                                          snb_dyn + "forum_hasModerator_person_1_0.csv",
                                          snb_dyn + "forum_hasModerator_person_2_0.csv",
                                          snb_dyn + "forum_hasModerator_person_3_0.csv",
                                          snb_dyn + "forum_hasTag_tag_0_0.csv",
                                          snb_dyn + "forum_hasTag_tag_1_0.csv",
                                          snb_dyn + "forum_hasTag_tag_2_0.csv",
                                          snb_dyn + "forum_hasTag_tag_3_0.csv",
                                          snb_dyn + "person_hasInterest_tag_0_0.csv",
                                          snb_dyn + "person_hasInterest_tag_1_0.csv",
                                          snb_dyn + "person_hasInterest_tag_2_0.csv",
                                          snb_dyn + "person_hasInterest_tag_3_0.csv",
                                          snb_dyn + "person_isLocatedIn_place_0_0.csv",
                                          snb_dyn + "person_isLocatedIn_place_1_0.csv",
                                          snb_dyn + "person_isLocatedIn_place_2_0.csv",
                                          snb_dyn + "person_isLocatedIn_place_3_0.csv",
                                          snb_dyn + "person_knows_person_0_0.csv",
                                          snb_dyn + "person_knows_person_1_0.csv",
                                          snb_dyn + "person_knows_person_2_0.csv",
                                          snb_dyn + "person_knows_person_3_0.csv",
                                          snb_dyn + "person_likes_comment_0_0.csv",
                                          snb_dyn + "person_likes_comment_1_0.csv",
                                          snb_dyn + "person_likes_comment_2_0.csv",
                                          snb_dyn + "person_likes_comment_3_0.csv",
                                          snb_dyn + "person_likes_post_0_0.csv",
                                          snb_dyn + "person_likes_post_1_0.csv",
                                          snb_dyn + "person_likes_post_2_0.csv",
                                          snb_dyn + "person_likes_post_3_0.csv",
                                          snb_dyn + "post_hasCreator_person_0_0.csv",
                                          snb_dyn + "post_hasCreator_person_1_0.csv",
                                          snb_dyn + "post_hasCreator_person_2_0.csv",
                                          snb_dyn + "post_hasCreator_person_3_0.csv",
                                          snb_dyn + "comment_hasTag_tag_0_0.csv",
                                          snb_dyn + "comment_hasTag_tag_1_0.csv",
                                          snb_dyn + "comment_hasTag_tag_2_0.csv",
                                          snb_dyn + "comment_hasTag_tag_3_0.csv",
                                          snb_dyn + "post_hasTag_tag_0_0.csv",
                                          snb_dyn + "post_hasTag_tag_1_0.csv",
                                          snb_dyn + "post_hasTag_tag_2_0.csv",
                                          snb_dyn + "post_hasTag_tag_3_0.csv",
                                          snb_dyn + "post_isLocatedIn_place_0_0.csv",
                                          snb_dyn + "post_isLocatedIn_place_1_0.csv",
                                          snb_dyn + "post_isLocatedIn_place_2_0.csv",
                                          snb_dyn + "post_isLocatedIn_place_3_0.csv",
                                          snb_dyn + "person_studyAt_organisation_0_0.csv",
                                          snb_dyn + "person_studyAt_organisation_1_0.csv",
                                          snb_dyn + "person_studyAt_organisation_2_0.csv",
                                          snb_dyn + "person_studyAt_organisation_3_0.csv",
                                          snb_dyn + "person_workAt_organisation_0_0.csv",
                                          snb_dyn + "person_workAt_organisation_1_0.csv",
                                          snb_dyn + "person_workAt_organisation_2_0.csv",
                                          snb_dyn + "person_workAt_organisation_3_0.csv"};

  spdlog::info("trying to load data from {} and {}", snb_sta, snb_dyn);
  load_snb_data(graph, node_files, rship_files, strict);

#ifdef CREATE_INDEX
  auto tx = graph->begin_transaction();
  auto idx_1 = graph->create_index("Person", "id");
  auto idx_2 = graph->create_index("Post", "id");
  auto idx_3 = graph->create_index("Comment", "id");
  auto idx_4 = graph->create_index("Place", "id");
  auto idx_5 = graph->create_index("Tag", "id");
  auto idx_6 = graph->create_index("Organisation", "id");
  auto idx_7 = graph->create_index("Forum", "id");
  graph->commit_transaction();
#endif
}
