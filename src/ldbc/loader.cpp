#include <iostream>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>

#include "defs.hpp"
#include "graph_db.hpp"
#include "ldbc.hpp"
#include "config.h"

#include "threadsafe_queue.hpp"

#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"

#ifdef USE_PMDK

#define POOL_SIZE ((unsigned long long)(1024 * 1024 * 40000ull)) // 4000 MiB

const std::string test_path = poseidon::gPmemPath + "sf1";

struct root {
  graph_db_ptr graph;
};

#endif

void load_snb_data(graph_db_ptr &graph, 
                    std::vector<std::string> &node_files,
                    std::vector<std::string> &rship_files){
  auto delim = '|';
  graph_db::mapping_t mapping;
  bool nodes_imported = false, rships_imported = false;
  
  if (!node_files.empty()){
    spdlog::info("######## NODES ########");

    std::vector<std::size_t> num_nodes(node_files.size());
    auto i = 0;
    for (auto &file : node_files){
      std::vector<std::string> fp;
      boost::split(fp, file, boost::is_any_of("/"));
      assert(fp.back().find(".csv") != std::string::npos);
      auto pos = fp.back().find("_");
      auto label = fp.back().substr(0, pos);
      if (label[0] >= 'a' && label[0] <= 'z')
        label[0] -= 32;

      num_nodes[i] = graph->import_nodes_from_csv(label, file, delim, mapping);
      spdlog::info("{} '{}' node objects imported", num_nodes[i], label);
      if (num_nodes[i] > 0)
        nodes_imported = true;
      i++;
    }
  }

  if (!rship_files.empty()){
    spdlog::info("################ RELATIONSHIPS ################");
    
    std::vector<std::size_t> num_rships(rship_files.size());
    auto i = 0;
    for (auto &file : rship_files){
      std::vector<std::string> fp;
      boost::split(fp, file, boost::is_any_of("/"));
      assert(fp.back().find(".csv") != std::string::npos);
      std::vector<std::string> fn;
      boost::split(fn, fp.back(), boost::is_any_of("_"));
      auto label = ":" + fn[1];

      num_rships[i] = graph->import_relationships_from_csv(file, delim, mapping);
      spdlog::info("{} ({})-[{}]-({}) relationship objects imported", 
        num_rships[i], fn[0], label, fn[2]);
      if (num_rships[i] > 0)
        rships_imported = true;
      i++;
    }
  }
}

using namespace boost::program_options;

int main(int argc, char **argv) {
  std::string db_name;
  std::string snb_home = "/home/data/SNB_SF_10/";

 try {
    options_description desc{"Options"};
    desc.add_options()
        ("help,h", "Help")
        ("verbose,v", bool_switch()->default_value(false), "Verbose - show debug output")
        ("import,i", value<std::string>(&snb_home), "Path to directories containing SNB CSV files")
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


    notify(vm);

      } catch (const error &ex) {
    std::cerr << ex.what() << '\n';
    return -1;
  }

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
  load_snb_data(graph, node_files, rship_files);
}