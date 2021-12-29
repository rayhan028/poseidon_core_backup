#include <iostream>
#include <chrono>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

#include "defs.hpp"
#include "graph_db.hpp"
#include "graph_pool.hpp"
#include "ldbc.hpp"
#include "config.h"

#include "thread_pool.hpp"

#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"

// ------------------------------------------------------------------------------------------------------------------------

using namespace boost::program_options;

void load_snb_data(graph_db_ptr &graph, const std::string& path, bool strict) {
  std::string snb_sta = path + "/static/";
  std::string snb_dyn = path + "/dynamic/";

  std::vector<std::string> node_files =
    {snb_dyn + "comment_0_0.csv", snb_dyn + "comment_1_0.csv",
    snb_dyn + "comment_2_0.csv", snb_dyn + "comment_3_0.csv",
    snb_dyn + "forum_0_0.csv", snb_dyn + "forum_1_0.csv",
    snb_dyn + "forum_2_0.csv", snb_dyn + "forum_3_0.csv",
    snb_sta + "organisation_0_0.csv", snb_sta + "organisation_1_0.csv",
    snb_sta + "organisation_2_0.csv", snb_sta + "organisation_3_0.csv",
    snb_dyn + "person_0_0.csv", snb_dyn + "person_1_0.csv",
    snb_dyn + "person_2_0.csv", snb_dyn + "person_3_0.csv",
    snb_sta + "place_0_0.csv", snb_sta + "place_1_0.csv",
    snb_sta + "place_2_0.csv", snb_sta + "place_3_0.csv",
    snb_dyn + "post_0_0.csv", snb_dyn + "post_1_0.csv",
    snb_dyn + "post_2_0.csv", snb_dyn + "post_3_0.csv",
    snb_sta + "tag_0_0.csv", snb_sta + "tag_1_0.csv",
    snb_sta + "tag_2_0.csv", snb_sta + "tag_3_0.csv",
    snb_sta + "tagclass_0_0.csv", snb_sta + "tagclass_1_0.csv",
    snb_sta + "tagclass_2_0.csv", snb_sta + "tagclass_3_0.csv"};

  std::vector<std::string> rship_files =
    {snb_dyn + "comment_hasCreator_person_0_0.csv",
    snb_dyn + "comment_hasCreator_person_1_0.csv",
    snb_dyn + "comment_hasCreator_person_2_0.csv",
    snb_dyn + "comment_hasCreator_person_3_0.csv",
    snb_dyn + "comment_hasTag_tag_0_0.csv",
    snb_dyn + "comment_hasTag_tag_1_0.csv",
    snb_dyn + "comment_hasTag_tag_2_0.csv",
    snb_dyn + "comment_hasTag_tag_3_0.csv",
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
    snb_sta + "organisation_isLocatedIn_place_0_0.csv",
    snb_sta + "organisation_isLocatedIn_place_1_0.csv",
    snb_sta + "organisation_isLocatedIn_place_2_0.csv",
    snb_sta + "organisation_isLocatedIn_place_3_0.csv",
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
    snb_dyn + "person_studyAt_organisation_0_0.csv",
    snb_dyn + "person_studyAt_organisation_1_0.csv",
    snb_dyn + "person_studyAt_organisation_2_0.csv",
    snb_dyn + "person_studyAt_organisation_3_0.csv",
    snb_dyn + "person_workAt_organisation_0_0.csv",
    snb_dyn + "person_workAt_organisation_1_0.csv",
    snb_dyn + "person_workAt_organisation_2_0.csv",
    snb_dyn + "person_workAt_organisation_3_0.csv",
    snb_sta + "place_isPartOf_place_0_0.csv",
    snb_sta + "place_isPartOf_place_1_0.csv",
    snb_sta + "place_isPartOf_place_2_0.csv",
    snb_sta + "place_isPartOf_place_3_0.csv",
    snb_dyn + "post_hasCreator_person_0_0.csv",
    snb_dyn + "post_hasCreator_person_1_0.csv",
    snb_dyn + "post_hasCreator_person_2_0.csv",
    snb_dyn + "post_hasCreator_person_3_0.csv",
    snb_dyn + "post_hasTag_tag_0_0.csv",
    snb_dyn + "post_hasTag_tag_1_0.csv",
    snb_dyn + "post_hasTag_tag_2_0.csv",
    snb_dyn + "post_hasTag_tag_3_0.csv",
    snb_dyn + "post_isLocatedIn_place_0_0.csv",
    snb_dyn + "post_isLocatedIn_place_1_0.csv",
    snb_dyn + "post_isLocatedIn_place_2_0.csv",
    snb_dyn + "post_isLocatedIn_place_3_0.csv",
    snb_sta + "tag_hasType_tagclass_0_0.csv",
    snb_sta + "tag_hasType_tagclass_1_0.csv",
    snb_sta + "tag_hasType_tagclass_2_0.csv",
    snb_sta + "tag_hasType_tagclass_3_0.csv",
    snb_sta + "tagclass_isSubclassOf_tagclass_0_0.csv",
    snb_sta + "tagclass_isSubclassOf_tagclass_1_0.csv",
    snb_sta + "tagclass_isSubclassOf_tagclass_2_0.csv",
    snb_sta + "tagclass_isSubclassOf_tagclass_3_0.csv"};

  spdlog::info("trying to load data from {} and {}", snb_sta, snb_dyn);
  load_snb_data(graph, node_files, rship_files, strict);
}


void load_snb_data(graph_db_ptr &graph,
                    std::vector<std::string> &node_files,
                    std::vector<std::string> &rship_files, bool strict) {
  if (strict)
    spdlog::info("using strict mode for loading");

  auto delim = '|';
  graph_db::mapping_t mapping;
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
         assert(fp.back().find(".csv",fp.size()-4) != std::string::npos);
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
      if (resp.second > 0)
        spdlog::info("{} '{}' node objects imported", resp.second, resp.first);
    }
#else
    for (auto &file : node_files) {
      auto start_tm = std::chrono::steady_clock::now();

      std::vector<std::string> fp;
      boost::split(fp, file, boost::is_any_of("/"));
       assert(fp.back().find(".csv",fp.size()-4) != std::string::npos);
      auto pos = fp.back().find("_");
      auto label = fp.back().substr(0, pos);
      if (label[0] >= 'a' && label[0] <= 'z')
        label[0] -= 32;

      auto num_nodes = strict
        ? graph->import_typed_nodes_from_csv(label, file, delim, mapping)
        : graph->import_nodes_from_csv(label, file, delim, mapping);
      auto end_tm = std::chrono::steady_clock::now();
      auto runtime = std::chrono::duration_cast<std::chrono::milliseconds>(end_tm -
                                                                       start_tm).count();

      if (num_nodes > 0) {
        spdlog::info("{} '{}' nodes imported in {} secs", num_nodes, label, runtime / 1000.0);
        // graph->print_stats();
      }
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
        assert(fp.back().find(".csv",fp.size()-4) != std::string::npos);
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
      if (resp.second > 0)
        spdlog::info("{} '{}' relationships imported", resp.second, resp.first);
    }
#else
    for (auto &file : rship_files) {
      auto start_tm = std::chrono::steady_clock::now();

      std::vector<std::string> fp;
      boost::split(fp, file, boost::is_any_of("/"));
      assert(fp.back().find(".csv",fp.size()-4) != std::string::npos);
      std::vector<std::string> fn;
      boost::split(fn, fp.back(), boost::is_any_of("_"));
      auto label = ":" + fn[1];

      auto num_rships = strict
      ? graph->import_typed_relationships_from_csv(file, delim, mapping)
      : graph->import_relationships_from_csv(file, delim, mapping);
      auto end_tm = std::chrono::steady_clock::now();
      auto runtime = std::chrono::duration_cast<std::chrono::milliseconds>(end_tm -
                                                                       start_tm).count();
      if (num_rships > 0) {
        spdlog::info("{} ({})-[{}]-({}) relationships imported in {} secs",
          num_rships, fn[0], label, fn[2], runtime / 1000.0);
        // graph->print_stats();
      }
    }
    #endif
  }

#ifdef CREATE_INDEX
  graph->begin_transaction();
  graph->create_index("Person", "id");
  graph->create_index("Post", "id");
  graph->create_index("Comment", "id");
  graph->create_index("Place", "id");
  graph->create_index("Tag", "id");
  graph->create_index("Tagclass", "id");
  graph->create_index("Organisation", "id");
  graph->create_index("Forum", "id");
  graph->commit_transaction();
#endif
}

#if defined USE_PMDK && defined FPTree
void fptree_recovery(graph_db_ptr &graph){
  std::vector<index_id> indexes;

  graph->begin_transaction();
  indexes.push_back(graph->get_index("Person", "id"));
  indexes.push_back(graph->get_index("Post", "id"));
  indexes.push_back(graph->get_index("Comment", "id"));
  indexes.push_back(graph->get_index("Place", "id"));
  indexes.push_back(graph->get_index("Tag", "id"));
  indexes.push_back(graph->get_index("Tagclass", "id"));
  indexes.push_back(graph->get_index("Organisation", "id"));
  indexes.push_back(graph->get_index("Forum", "id"));
  graph->commit_transaction();

  for (auto idx_ptr : indexes)
    idx_ptr->recover();
}
#endif