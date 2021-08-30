#include <iostream>
#include <chrono>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

#include "defs.hpp"
#include "graph_db.hpp"
#include "graph_pool.hpp"
#include "gtpc.hpp"
#include "config.h"

#include "thread_pool.hpp"

#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"

// ------------------------------------------------------------------------------------------------------------------------

using namespace boost::program_options;

void load_gtpc_data(graph_db_ptr &graph, const std::string& path, bool strict) {
  std::string post_fix = "_0_0.csv";

  std::vector<std::string> node_files =
    {path + "customer" + post_fix,
    path + "district" + post_fix,
    path + "item" + post_fix,
    path + "nation" + post_fix,
    path + "order" + post_fix,
    path + "orderLine" + post_fix,
    path + "region" + post_fix,
    path + "stock" + post_fix,
    path + "supplier" + post_fix,
    path + "warehouse" + post_fix};

  std::vector<std::string> rship_files =
    {path + "customer_hasPlaced_order" + post_fix,
    path + "customer_isLocatedIn_nation" + post_fix,
    path + "district_serves_customer" + post_fix,
    path + "item_hasStock_stock" + post_fix,
    path + "nation_isPartOf_region" + post_fix,
    path + "order_contains_orderLine" + post_fix,
    path + "orderLine_hasStock_stock" + post_fix,
    path + "stock_hasSupplier_supplier" + post_fix,
    path + "supplier_isLocatedIn_nation" + post_fix,
    path + "warehouse_covers_district" + post_fix,
    path + "warehouse_hasStock_stock" + post_fix};

  spdlog::info("trying to load data from {}", path);
  load_gtpc_data(graph, node_files, rship_files, strict);
}

void load_gtpc_data(graph_db_ptr &graph,
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
}

void create_gtpc_index(graph_db_ptr &graph) {
  graph->run_transaction([&]() {
    // graph->create_index("Customer", "id");
    return true;
  });
}

#ifdef FPTree
void gtpc_fptree_recovery(graph_db_ptr &graph) {
  std::vector<index_id> indexes;
  graph->run_transaction([&]() {
    // indexes.push_back(graph->get_index("Customer", "id"));
    return true;
  });
  for (auto idx_ptr : indexes)
    idx_ptr->recover();
}
#endif