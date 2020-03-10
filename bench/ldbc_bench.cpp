/*
 * Copyright (C) 2019-2020 DBIS Group - TU Ilmenau, All Rights Reserved.
 *
 * This file is part of the Poseidon package.
 *
 * Poseidon is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Poseidon is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Poseidon. If not, see <http://www.gnu.org/licenses/>.
 */

#include "benchmark/benchmark.h"
#include <iostream>
#include <boost/algorithm/string.hpp>

#include "config.h"
#include "defs.hpp"
#include "ldbc.hpp"
#include "qop.hpp"
#include "query.hpp"

namespace pj = builtin;

#ifdef USE_PMDK

#define PMEMOBJ_POOL_SIZE                                                      \
  ((unsigned long long)(1024 * 1024 * 40000ull)) // 4000 MiB

namespace nvm = pmem::obj;
const std::string bench_path = poseidon::gPmemPath + "bench";
#endif

class MyFixture : public benchmark::Fixture {
public:
  graph_db_ptr graph;
#ifdef USE_PMDK
  nvm::pool_base pop;
#endif
  void SetUp(const ::benchmark::State &state) {
#ifdef USE_PMDK
    pop = nvm::pool_base::create(bench_path, "", PMEMOBJ_POOL_SIZE);
    nvm::transaction::run(pop, [&] { graph = p_make_ptr<graph_db>(); });
#else
    graph = p_make_ptr<graph_db>();
#endif
  }

  void TearDown(const ::benchmark::State &state) {
#ifdef USE_PMDK
    nvm::transaction::run(pop, [&] { nvm::delete_persistent<graph_db>(graph); });
    pop.close();
    remove(bench_path.c_str());
#else
    graph.reset();
#endif
  }
};

const std::string snb_sta("/home/data/SNB_SF_10/static/");
const std::string snb_dyn("/home/data/SNB_SF_10/dynamic/");

void load_snb_data(graph_db_ptr &graph, 
                    std::vector<std::string> &node_files,
                    std::vector<std::string> &rship_files){
  auto delim = '|';
  graph_db::mapping_t mapping;
  bool nodes_imported = false, rships_imported = false;
  
  if (!node_files.empty()){
    //std::cout << "\n######## \n# NODES \n######## \n";

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
      //std::cout << num_nodes[i] << " \"" << label << "\" node objects imported \n";
      if (num_nodes[i] > 0)
        nodes_imported = true;
      i++;
    }
  }

  if (!rship_files.empty()){
    //std::cout << "\n \n################ \n# RELATIONSHIPS \n################ \n";
    
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
      //std::cout << num_rships[i] << " (" << fn[0] << ")-[\"" << label << "\"]->(" 
        //                          << fn[2] << ") relationship objects imported \n";
      if (num_rships[i] > 0)
        rships_imported = true;
      i++;
    }
  }
  assert(nodes_imported || rships_imported); // data imported to run benchmark 
}

/* ------------------------------------------------------------- */

BENCHMARK_DEFINE_F(MyFixture, BM_InteractiveShort_1)(benchmark::State &state) {
  std::vector<std::string> node_files = {snb_dyn + "person_0_0.csv", snb_dyn + "person_1_0.csv",
                                          snb_dyn + "person_2_0.csv", snb_dyn + "person_3_0.csv",
                                          snb_sta + "place_0_0.csv", snb_sta + "place_1_0.csv",
                                          snb_sta + "place_2_0.csv", snb_sta + "place_3_0.csv"};

  std::vector<std::string> rship_files = {snb_dyn + "person_isLocatedIn_place_0_0.csv",
                                          snb_dyn + "person_isLocatedIn_place_1_0.csv",
                                          snb_dyn + "person_isLocatedIn_place_2_0.csv",
                                          snb_dyn + "person_isLocatedIn_place_3_0.csv"};

#ifdef USE_TX
    auto tx = graph->begin_transaction();
#endif

  load_snb_data(graph, node_files, rship_files);
  result_set rs;

#ifdef USE_TX
    graph->commit_transaction();
#endif

  for (auto _ : state) {
    for (int i = 0u, i_end = state.range(0); i < i_end; i++) {
#ifdef USE_TX
    auto tx = graph->begin_transaction();
#endif
    ldbc_is_query_1(graph, rs);
#ifdef USE_TX
      graph->commit_transaction();
#endif
    }
  }
}

BENCHMARK_REGISTER_F(MyFixture, BM_InteractiveShort_1)->Range(8, 8 << 4);

/* ------------------------------------------------------------- */

BENCHMARK_DEFINE_F(MyFixture, BM_InteractiveShort_2)(benchmark::State &state) {
  std::vector<std::string> node_files = {snb_dyn + "person_0_0.csv", snb_dyn + "person_1_0.csv",
                                          snb_dyn + "person_2_0.csv", snb_dyn + "person_3_0.csv",
                                          snb_dyn + "post_0_0.csv", snb_dyn + "post_1_0.csv",
                                          snb_dyn + "post_2_0.csv", snb_dyn + "post_3_0.csv",
                                          snb_dyn + "comment_0_0.csv", snb_dyn + "comment_1_0.csv",
                                          snb_dyn + "comment_2_0.csv", snb_dyn + "comment_3_0.csv"};

  std::vector<std::string> rship_files = {snb_dyn + "post_hasCreator_person_0_0.csv",
                                          snb_dyn + "post_hasCreator_person_1_0.csv",
                                          snb_dyn + "post_hasCreator_person_2_0.csv",
                                          snb_dyn + "post_hasCreator_person_3_0.csv",
                                          snb_dyn + "comment_hasCreator_person_0_0.csv",
                                          snb_dyn + "comment_hasCreator_person_1_0.csv",
                                          snb_dyn + "comment_hasCreator_person_2_0.csv",
                                          snb_dyn + "comment_hasCreator_person_3_0.csv",
                                          snb_dyn + "comment_replyOf_post_0_0.csv",
                                          snb_dyn + "comment_replyOf_post_1_0.csv",
                                          snb_dyn + "comment_replyOf_post_2_0.csv",
                                          snb_dyn + "comment_replyOf_post_3_0.csv",
                                          snb_dyn + "comment_replyOf_comment_0_0.csv",
                                          snb_dyn + "comment_replyOf_comment_1_0.csv",
                                          snb_dyn + "comment_replyOf_comment_2_0.csv",
                                          snb_dyn + "comment_replyOf_comment_3_0.csv"};

#ifdef USE_TX
    auto tx = graph->begin_transaction();
#endif
  load_snb_data(graph, node_files, rship_files);
  result_set rs;
#ifdef USE_TX
    graph->commit_transaction();
#endif

  for (auto _ : state) {
    for (int i = 0u, i_end = state.range(0); i < i_end; i++) {
#ifdef USE_TX
    auto tx = graph->begin_transaction();
#endif
    ldbc_is_query_2(graph, rs);
#ifdef USE_TX
      graph->commit_transaction();
#endif
    }
  }
}

BENCHMARK_REGISTER_F(MyFixture, BM_InteractiveShort_2)->Range(8, 8 << 4);

/* ------------------------------------------------------------- */

BENCHMARK_DEFINE_F(MyFixture, BM_InteractiveShort_3)(benchmark::State &state) {
  std::vector<std::string> node_files = {snb_dyn + "person_0_0.csv", snb_dyn + "person_1_0.csv",
                                          snb_dyn + "person_2_0.csv", snb_dyn + "person_3_0.csv"};

  std::vector<std::string> rship_files = {snb_dyn + "person_knows_person_0_0.csv",
                                          snb_dyn + "person_knows_person_1_0.csv",
                                          snb_dyn + "person_knows_person_2_0.csv",
                                          snb_dyn + "person_knows_person_3_0.csv"};
#ifdef USE_TX
    auto tx = graph->begin_transaction();
#endif
  load_snb_data(graph, node_files, rship_files);
  result_set rs;
#ifdef USE_TX
    graph->commit_transaction();
#endif

  for (auto _ : state) {
    for (int i = 0u, i_end = state.range(0); i < i_end; i++) {
#ifdef USE_TX
    auto tx = graph->begin_transaction();
#endif
    ldbc_is_query_3(graph, rs);
#ifdef USE_TX
      graph->commit_transaction();
#endif
    }
  }
}

BENCHMARK_REGISTER_F(MyFixture, BM_InteractiveShort_3)->Range(8, 8 << 4);

/* ------------------------------------------------------------- */

BENCHMARK_DEFINE_F(MyFixture, BM_InteractiveShort_4)(benchmark::State &state) {
  std::vector<std::string> node_files = {snb_dyn + "post_0_0.csv", snb_dyn + "post_1_0.csv",
                                          snb_dyn + "post_2_0.csv", snb_dyn + "post_3_0.csv"};

  std::vector<std::string> rship_files = {};
#ifdef USE_TX
    auto tx = graph->begin_transaction();
#endif
  load_snb_data(graph, node_files, rship_files);
  result_set rs;
#ifdef USE_TX
    graph->commit_transaction();
#endif

  for (auto _ : state) {
    for (int i = 0u, i_end = state.range(0); i < i_end; i++) {
#ifdef USE_TX
    auto tx = graph->begin_transaction();
#endif
    ldbc_is_query_4(graph, rs);
#ifdef USE_TX
      graph->commit_transaction();
#endif
    }
  }
}

BENCHMARK_REGISTER_F(MyFixture, BM_InteractiveShort_4)->Range(8, 8 << 4);

/* ------------------------------------------------------------- */

BENCHMARK_DEFINE_F(MyFixture, BM_InteractiveShort_5)(benchmark::State &state) {
  std::vector<std::string> node_files = {snb_dyn + "person_0_0.csv", snb_dyn + "person_1_0.csv",
                                          snb_dyn + "person_2_0.csv", snb_dyn + "person_3_0.csv",
                                          snb_dyn + "comment_0_0.csv", snb_dyn + "comment_1_0.csv",
                                          snb_dyn + "comment_2_0.csv", snb_dyn + "comment_3_0.csv"};

  std::vector<std::string> rship_files = {snb_dyn + "comment_hasCreator_person_0_0.csv",
                                          snb_dyn + "comment_hasCreator_person_1_0.csv",
                                          snb_dyn + "comment_hasCreator_person_2_0.csv",
                                          snb_dyn + "comment_hasCreator_person_3_0.csv"};
#ifdef USE_TX
    auto tx = graph->begin_transaction();
#endif
  load_snb_data(graph, node_files, rship_files);
  result_set rs;
#ifdef USE_TX
    graph->commit_transaction();
#endif

  for (auto _ : state) {
    for (int i = 0u, i_end = state.range(0); i < i_end; i++) {
#ifdef USE_TX
    auto tx = graph->begin_transaction();
#endif
    ldbc_is_query_5(graph, rs);
#ifdef USE_TX
      graph->commit_transaction();
#endif
    }
  }
}

BENCHMARK_REGISTER_F(MyFixture, BM_InteractiveShort_5)->Range(8, 8 << 4);

/* ------------------------------------------------------------- */

BENCHMARK_DEFINE_F(MyFixture, BM_InteractiveShort_6)(benchmark::State &state) {
  std::vector<std::string> node_files = {snb_dyn + "person_0_0.csv", snb_dyn + "person_1_0.csv",
                                          snb_dyn + "person_2_0.csv", snb_dyn + "person_3_0.csv",
                                          snb_dyn + "post_0_0.csv", snb_dyn + "post_1_0.csv",
                                          snb_dyn + "post_2_0.csv", snb_dyn + "post_3_0.csv",
                                          snb_dyn + "comment_0_0.csv", snb_dyn + "comment_1_0.csv",
                                          snb_dyn + "comment_2_0.csv", snb_dyn + "comment_3_0.csv",
                                          snb_dyn + "forum_0_0.csv", snb_dyn + "forum_1_0.csv",
                                          snb_dyn + "forum_2_0.csv", snb_dyn + "forum_3_0.csv"};

  std::vector<std::string> rship_files = {snb_dyn + "comment_replyOf_post_0_0.csv",
                                          snb_dyn + "comment_replyOf_post_1_0.csv",
                                          snb_dyn + "comment_replyOf_post_2_0.csv",
                                          snb_dyn + "comment_replyOf_post_3_0.csv",
                                          snb_dyn + "comment_replyOf_comment_0_0.csv",
                                          snb_dyn + "comment_replyOf_comment_1_0.csv",
                                          snb_dyn + "comment_replyOf_comment_2_0.csv",
                                          snb_dyn + "comment_replyOf_comment_3_0.csv",
                                          snb_dyn + "forum_containerOf_post_0_0.csv",
                                          snb_dyn + "forum_containerOf_post_1_0.csv",
                                          snb_dyn + "forum_containerOf_post_2_0.csv",
                                          snb_dyn + "forum_containerOf_post_3_0.csv",
                                          snb_dyn + "forum_hasModerator_person_0_0.csv",
                                          snb_dyn + "forum_hasModerator_person_1_0.csv",
                                          snb_dyn + "forum_hasModerator_person_2_0.csv",
                                          snb_dyn + "forum_hasModerator_person_3_0.csv"};
#ifdef USE_TX
    auto tx = graph->begin_transaction();
#endif
  load_snb_data(graph, node_files, rship_files);
  result_set rs;
#ifdef USE_TX
    graph->commit_transaction();
#endif

  for (auto _ : state) {
    for (int i = 0u, i_end = state.range(0); i < i_end; i++) {
#ifdef USE_TX
    auto tx = graph->begin_transaction();
#endif
    ldbc_is_query_6(graph, rs);
#ifdef USE_TX
      graph->commit_transaction();
#endif
    }
  }
}

BENCHMARK_REGISTER_F(MyFixture, BM_InteractiveShort_6)->Range(8, 8 << 4);

/* ------------------------------------------------------------- */

BENCHMARK_DEFINE_F(MyFixture, BM_InteractiveShort_7)(benchmark::State &state) {
  std::vector<std::string> node_files = {snb_dyn + "person_0_0.csv", snb_dyn + "person_1_0.csv",
                                          snb_dyn + "person_2_0.csv", snb_dyn + "person_3_0.csv",
                                          snb_dyn + "comment_0_0.csv", snb_dyn + "comment_1_0.csv",
                                          snb_dyn + "comment_2_0.csv", snb_dyn + "comment_3_0.csv"};

  std::vector<std::string> rship_files = {snb_dyn + "comment_hasCreator_person_0_0.csv",
                                          snb_dyn + "comment_hasCreator_person_1_0.csv",
                                          snb_dyn + "comment_hasCreator_person_2_0.csv",
                                          snb_dyn + "comment_hasCreator_person_3_0.csv",
                                          snb_dyn + "comment_replyOf_comment_0_0.csv",
                                          snb_dyn + "comment_replyOf_comment_1_0.csv",
                                          snb_dyn + "comment_replyOf_comment_2_0.csv",
                                          snb_dyn + "comment_replyOf_comment_3_0.csv"};
#ifdef USE_TX
    auto tx = graph->begin_transaction();
#endif
  load_snb_data(graph, node_files, rship_files);
  result_set rs;
#ifdef USE_TX
    graph->commit_transaction();
#endif

  for (auto _ : state) {
    for (int i = 0u, i_end = state.range(0); i < i_end; i++) {
#ifdef USE_TX
    auto tx = graph->begin_transaction();
#endif
    ldbc_is_query_7(graph, rs);
#ifdef USE_TX
      graph->commit_transaction();
#endif
    }
  }
}

BENCHMARK_REGISTER_F(MyFixture, BM_InteractiveShort_7)->Range(8, 8 << 4);

/* ------------------------------------------------------------- */

BENCHMARK_DEFINE_F(MyFixture, BM_InteractiveInsert_1)(benchmark::State &state) {
  std::vector<std::string> node_files = {snb_dyn + "person_0_0.csv", snb_dyn + "person_1_0.csv",
                                          snb_dyn + "person_2_0.csv", snb_dyn + "person_3_0.csv",
                                          snb_sta + "place_0_0.csv", snb_sta + "place_1_0.csv",
                                          snb_sta + "place_2_0.csv", snb_sta + "place_3_0.csv",
                                          snb_sta + "tag_0_0.csv", snb_sta + "tag_1_0.csv",
                                          snb_sta + "tag_2_0.csv", snb_sta + "tag_3_0.csv",
                                          snb_sta + "organisation_0_0.csv", snb_sta + "organisation_1_0.csv",
                                          snb_sta + "organisation_2_0.csv", snb_sta + "organisation_3_0.csv"};

  std::vector<std::string> rship_files = {snb_dyn + "person_isLocatedIn_place_0_0.csv",
                                          snb_dyn + "person_isLocatedIn_place_1_0.csv",
                                          snb_dyn + "person_isLocatedIn_place_2_0.csv",
                                          snb_dyn + "person_isLocatedIn_place_3_0.csv",
                                          snb_dyn + "person_hasInterest_tag_0_0.csv",
                                          snb_dyn + "person_hasInterest_tag_1_0.csv",
                                          snb_dyn + "person_hasInterest_tag_2_0.csv",
                                          snb_dyn + "person_hasInterest_tag_3_0.csv",
                                          snb_dyn + "person_studyAt_organisation_0_0.csv",
                                          snb_dyn + "person_studyAt_organisation_1_0.csv",
                                          snb_dyn + "person_studyAt_organisation_2_0.csv",
                                          snb_dyn + "person_studyAt_organisation_3_0.csv",
                                          snb_dyn + "person_workAt_organisation_0_0.csv",
                                          snb_dyn + "person_workAt_organisation_1_0.csv",
                                          snb_dyn + "person_workAt_organisation_2_0.csv",
                                          snb_dyn + "person_workAt_organisation_3_0.csv"};
#ifdef USE_TX
    auto tx = graph->begin_transaction();
#endif
  load_snb_data(graph, node_files, rship_files);
  result_set rs;
#ifdef USE_TX
    graph->commit_transaction();
#endif

  for (auto _ : state) {
    for (int i = 0u, i_end = state.range(0); i < i_end; i++) {
#ifdef USE_TX
    auto tx = graph->begin_transaction();
#endif
    ldbc_iu_query_1(graph, rs);
#ifdef USE_TX
      graph->commit_transaction();
#endif
    }
  }
}

BENCHMARK_REGISTER_F(MyFixture, BM_InteractiveInsert_1)->Range(8, 8 << 4);

/* ------------------------------------------------------------- */

BENCHMARK_DEFINE_F(MyFixture, BM_InteractiveInsert_2)(benchmark::State &state) {
  std::vector<std::string> node_files = {snb_dyn + "person_0_0.csv", snb_dyn + "person_1_0.csv",
                                          snb_dyn + "person_2_0.csv", snb_dyn + "person_3_0.csv",
                                          snb_dyn + "post_0_0.csv", snb_dyn + "post_1_0.csv",
                                          snb_dyn + "post_2_0.csv", snb_dyn + "post_3_0.csv"};

  std::vector<std::string> rship_files = {snb_dyn + "person_likes_post_0_0.csv",
                                          snb_dyn + "person_likes_post_1_0.csv",
                                          snb_dyn + "person_likes_post_2_0.csv",
                                          snb_dyn + "person_likes_post_3_0.csv"};
#ifdef USE_TX
    auto tx = graph->begin_transaction();
#endif
  load_snb_data(graph, node_files, rship_files);
  result_set rs;
#ifdef USE_TX
    graph->commit_transaction();
#endif

  for (auto _ : state) {
    for (int i = 0u, i_end = state.range(0); i < i_end; i++) {
#ifdef USE_TX
    auto tx = graph->begin_transaction();
#endif
    ldbc_iu_query_2(graph, rs);
#ifdef USE_TX
      graph->commit_transaction();
#endif
    }
  }
}

BENCHMARK_REGISTER_F(MyFixture, BM_InteractiveInsert_2)->Range(8, 8 << 4);

/* ------------------------------------------------------------- */

BENCHMARK_DEFINE_F(MyFixture, BM_InteractiveInsert_3)(benchmark::State &state) {
  std::vector<std::string> node_files = {snb_dyn + "person_0_0.csv", snb_dyn + "person_1_0.csv",
                                          snb_dyn + "person_2_0.csv", snb_dyn + "person_3_0.csv",
                                          snb_dyn + "comment_0_0.csv", snb_dyn + "comment_1_0.csv",
                                          snb_dyn + "comment_2_0.csv", snb_dyn + "comment_3_0.csv"};

  std::vector<std::string> rship_files = {snb_dyn + "person_likes_comment_0_0.csv",
                                          snb_dyn + "person_likes_comment_1_0.csv",
                                          snb_dyn + "person_likes_comment_2_0.csv",
                                          snb_dyn + "person_likes_comment_3_0.csv"};
#ifdef USE_TX
    auto tx = graph->begin_transaction();
#endif
  load_snb_data(graph, node_files, rship_files);
  result_set rs;
#ifdef USE_TX
    graph->commit_transaction();
#endif

  for (auto _ : state) {
    for (int i = 0u, i_end = state.range(0); i < i_end; i++) {
#ifdef USE_TX
    auto tx = graph->begin_transaction();
#endif
    ldbc_iu_query_3(graph, rs);
#ifdef USE_TX
      graph->commit_transaction();
#endif
    }
  }
}

BENCHMARK_REGISTER_F(MyFixture, BM_InteractiveInsert_3)->Range(8, 8 << 4);

/* ------------------------------------------------------------- */

BENCHMARK_DEFINE_F(MyFixture, BM_InteractiveInsert_4)(benchmark::State &state) {
  std::vector<std::string> node_files = {snb_dyn + "forum_0_0.csv", snb_dyn + "forum_1_0.csv",
                                          snb_dyn + "forum_2_0.csv", snb_dyn + "forum_3_0.csv",
                                          snb_dyn + "person_0_0.csv", snb_dyn + "person_1_0.csv",
                                          snb_dyn + "person_2_0.csv", snb_dyn + "person_3_0.csv", 
                                          snb_sta + "tag_0_0.csv", snb_sta + "tag_1_0.csv",
                                          snb_sta + "tag_2_0.csv", snb_sta + "tag_3_0.csv"};

  std::vector<std::string> rship_files = {snb_dyn + "forum_hasModerator_person_0_0.csv",
                                            snb_dyn + "forum_hasModerator_person_1_0.csv",
                                            snb_dyn + "forum_hasModerator_person_2_0.csv",
                                            snb_dyn + "forum_hasModerator_person_3_0.csv",
                                            snb_dyn + "forum_hasTag_tag_0_0.csv",
                                            snb_dyn + "forum_hasTag_tag_1_0.csv",
                                            snb_dyn + "forum_hasTag_tag_2_0.csv",
                                            snb_dyn + "forum_hasTag_tag_3_0.csv"};
#ifdef USE_TX
    auto tx = graph->begin_transaction();
#endif
  load_snb_data(graph, node_files, rship_files);
  result_set rs;
#ifdef USE_TX
    graph->commit_transaction();
#endif

  for (auto _ : state) {
    for (int i = 0u, i_end = state.range(0); i < i_end; i++) {
#ifdef USE_TX
    auto tx = graph->begin_transaction();
#endif
    ldbc_iu_query_4(graph, rs);
#ifdef USE_TX
      graph->commit_transaction();
#endif
    }
  }
}

BENCHMARK_REGISTER_F(MyFixture, BM_InteractiveInsert_4)->Range(8, 8 << 4);

/* ------------------------------------------------------------- */

BENCHMARK_DEFINE_F(MyFixture, BM_InteractiveInsert_5)(benchmark::State &state) {
  std::vector<std::string> node_files = {snb_dyn + "forum_0_0.csv", snb_dyn + "forum_1_0.csv",
                                          snb_dyn + "forum_2_0.csv", snb_dyn + "forum_3_0.csv",
                                          snb_dyn + "person_0_0.csv", snb_dyn + "person_1_0.csv",
                                          snb_dyn + "person_2_0.csv", snb_dyn + "person_3_0.csv"};

  std::vector<std::string> rship_files = {snb_dyn + "forum_hasMember_person_0_0.csv",
                                          snb_dyn + "forum_hasMember_person_1_0.csv",
                                          snb_dyn + "forum_hasMember_person_2_0.csv",
                                          snb_dyn + "forum_hasMember_person_3_0.csv"};
#ifdef USE_TX
    auto tx = graph->begin_transaction();
#endif
  load_snb_data(graph, node_files, rship_files);
  result_set rs;
#ifdef USE_TX
    graph->commit_transaction();
#endif

  for (auto _ : state) {
    for (int i = 0u, i_end = state.range(0); i < i_end; i++) {
#ifdef USE_TX
    auto tx = graph->begin_transaction();
#endif
    ldbc_iu_query_5(graph, rs);
#ifdef USE_TX
      graph->commit_transaction();
#endif
    }
  }
}

BENCHMARK_REGISTER_F(MyFixture, BM_InteractiveInsert_5)->Range(8, 8 << 4);

/* ------------------------------------------------------------- */

BENCHMARK_DEFINE_F(MyFixture, BM_InteractiveInsert_6)(benchmark::State &state) {
  std::vector<std::string> node_files = {snb_dyn + "post_0_0.csv", snb_dyn + "post_1_0.csv",
                                          snb_dyn + "post_2_0.csv", snb_dyn + "post_3_0.csv",
                                          snb_dyn + "person_0_0.csv", snb_dyn + "person_1_0.csv",
                                          snb_dyn + "person_2_0.csv", snb_dyn + "person_3_0.csv",
                                          snb_dyn + "forum_0_0.csv", snb_dyn + "forum_1_0.csv",
                                          snb_dyn + "forum_2_0.csv", snb_dyn + "forum_3_0.csv",
                                          snb_sta + "place_0_0.csv", snb_sta + "place_1_0.csv",
                                          snb_sta + "place_2_0.csv", snb_sta + "place_3_0.csv",
                                          snb_sta + "tag_0_0.csv", snb_sta + "tag_1_0.csv",
                                          snb_sta + "tag_2_0.csv", snb_sta + "tag_3_0.csv"};

  std::vector<std::string> rship_files = {snb_dyn + "post_hasCreator_person_0_0.csv",
                                            snb_dyn + "post_hasCreator_person_1_0.csv",
                                            snb_dyn + "post_hasCreator_person_2_0.csv",
                                            snb_dyn + "post_hasCreator_person_3_0.csv",
                                            snb_dyn + "forum_containerOf_post_0_0.csv",
                                            snb_dyn + "forum_containerOf_post_1_0.csv",
                                            snb_dyn + "forum_containerOf_post_2_0.csv",
                                            snb_dyn + "forum_containerOf_post_3_0.csv",
                                            snb_dyn + "post_isLocatedIn_place_0_0.csv",
                                            snb_dyn + "post_isLocatedIn_place_1_0.csv",
                                            snb_dyn + "post_isLocatedIn_place_2_0.csv",
                                            snb_dyn + "post_isLocatedIn_place_3_0.csv",
                                            snb_dyn + "post_hasTag_tag_0_0.csv",
                                            snb_dyn + "post_hasTag_tag_1_0.csv",
                                            snb_dyn + "post_hasTag_tag_2_0.csv",
                                            snb_dyn + "post_hasTag_tag_3_0.csv"};
#ifdef USE_TX
    auto tx = graph->begin_transaction();
#endif
  load_snb_data(graph, node_files, rship_files);
  result_set rs;
#ifdef USE_TX
    graph->commit_transaction();
#endif

  for (auto _ : state) {
    for (int i = 0u, i_end = state.range(0); i < i_end; i++) {
#ifdef USE_TX
    auto tx = graph->begin_transaction();
#endif
    ldbc_iu_query_6(graph, rs);
#ifdef USE_TX
      graph->commit_transaction();
#endif
    }
  }
}

BENCHMARK_REGISTER_F(MyFixture, BM_InteractiveInsert_6)->Range(8, 8 << 4);

/* ------------------------------------------------------------- */

BENCHMARK_DEFINE_F(MyFixture, BM_InteractiveInsert_7)(benchmark::State &state) {
  std::vector<std::string> node_files = {snb_dyn + "comment_0_0.csv", snb_dyn + "comment_1_0.csv",
                                          snb_dyn + "comment_2_0.csv", snb_dyn + "comment_3_0.csv",
                                          snb_dyn + "post_0_0.csv", snb_dyn + "post_1_0.csv",
                                          snb_dyn + "post_2_0.csv", snb_dyn + "post_3_0.csv",
                                          snb_dyn + "person_0_0.csv", snb_dyn + "person_1_0.csv",
                                          snb_dyn + "person_2_0.csv", snb_dyn + "person_3_0.csv",
                                          snb_sta + "place_0_0.csv", snb_sta + "place_1_0.csv",
                                          snb_sta + "place_2_0.csv", snb_sta + "place_3_0.csv",
                                          snb_sta + "tag_0_0.csv", snb_sta + "tag_1_0.csv",
                                          snb_sta + "tag_2_0.csv", snb_sta + "tag_3_0.csv"};

  std::vector<std::string> rship_files = {snb_dyn + "comment_hasCreator_person_0_0.csv",
                                            snb_dyn + "comment_hasCreator_person_1_0.csv",
                                            snb_dyn + "comment_hasCreator_person_2_0.csv",
                                            snb_dyn + "comment_hasCreator_person_3_0.csv",
                                            snb_dyn + "comment_replyOf_post_0_0.csv",
                                            snb_dyn + "comment_replyOf_post_1_0.csv",
                                            snb_dyn + "comment_replyOf_post_2_0.csv",
                                            snb_dyn + "comment_replyOf_post_3_0.csv",
                                            snb_dyn + "comment_isLocatedIn_place_0_0.csv",
                                            snb_dyn + "comment_isLocatedIn_place_1_0.csv",
                                            snb_dyn + "comment_isLocatedIn_place_2_0.csv",
                                            snb_dyn + "comment_isLocatedIn_place_3_0.csv",
                                            snb_dyn + "comment_hasTag_tag_0_0.csv",
                                            snb_dyn + "comment_hasTag_tag_1_0.csv",
                                            snb_dyn + "comment_hasTag_tag_2_0.csv",
                                            snb_dyn + "comment_hasTag_tag_3_0.csv"};
#ifdef USE_TX
    auto tx = graph->begin_transaction();
#endif
  load_snb_data(graph, node_files, rship_files);
  result_set rs;
#ifdef USE_TX
    graph->commit_transaction();
#endif

  for (auto _ : state) {
    for (int i = 0u, i_end = state.range(0); i < i_end; i++) {
#ifdef USE_TX
    auto tx = graph->begin_transaction();
#endif
    ldbc_iu_query_7(graph, rs);
#ifdef USE_TX
      graph->commit_transaction();
#endif
    }
  }
}

BENCHMARK_REGISTER_F(MyFixture, BM_InteractiveInsert_7)->Range(8, 8 << 4);

/* ------------------------------------------------------------- */

BENCHMARK_DEFINE_F(MyFixture, BM_InteractiveInsert_8)(benchmark::State &state) {
  std::vector<std::string> node_files = {snb_dyn + "person_0_0.csv", snb_dyn + "person_1_0.csv",
                                          snb_dyn + "person_2_0.csv", snb_dyn + "person_3_0.csv"};

  std::vector<std::string> rship_files = {snb_dyn + "person_knows_person_0_0.csv",
                                          snb_dyn + "person_knows_person_1_0.csv",
                                          snb_dyn + "person_knows_person_2_0.csv",
                                          snb_dyn + "person_knows_person_3_0.csv"};
#ifdef USE_TX
    auto tx = graph->begin_transaction();
#endif
  load_snb_data(graph, node_files, rship_files);
  result_set rs;
#ifdef USE_TX
    graph->commit_transaction();
#endif

  for (auto _ : state) {
    for (int i = 0u, i_end = state.range(0); i < i_end; i++) {
#ifdef USE_TX
    auto tx = graph->begin_transaction();
#endif
    ldbc_iu_query_8(graph, rs);
#ifdef USE_TX
      graph->commit_transaction();
#endif
    }
  }
}

BENCHMARK_REGISTER_F(MyFixture, BM_InteractiveInsert_8)->Range(8, 8 << 4);

/* ------------------------------------------------------------- */

BENCHMARK_MAIN();
