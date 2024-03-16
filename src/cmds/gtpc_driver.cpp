/*
 * Copyright (C) 2019-2024 DBIS Group - TU Ilmenau, All Rights Reserved.
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
#include <chrono>
#include <iostream>
#include <filesystem>
#include <random>
#include <atomic>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/program_options.hpp>

#include "query_proc.hpp"
#include "graph_db.hpp"
#include "graph_pool.hpp"

#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

using namespace boost::program_options;

const int WAREHOUSE_COUNT = 5;
const int DISTRICT_PER_WAREHOUSE = 10;
const int CUSTOMER_PER_DISTRICT = 3000;
const int ITEM_COUNT = 100000;

graph_pool_ptr pool;
graph_db_ptr graph;
std::unique_ptr<query_proc> qproc_ptr;

std::random_device rd;  // a seed source for the random number engine
std::mt19937 gen(rd()); // mersenne_twister_engine seeded with rd()

struct sequence_generator {
    sequence_generator(uint64_t start = 0) : current_(start) {}
    ~sequence_generator() = default;

    uint64_t current() { return current_; }
    uint64_t next() { return ++current_; }

    std::atomic_ullong current_;
};

int uniform_random_int(int l, int h) {
    std::uniform_int_distribution<> distrib(l, h); 
    return distrib(gen);
}

int non_uniform_random_int(int a, int x, int y, int c) {
    return ((((uniform_random_int(0, a) | uniform_random_int(x, y)) + c) % (y - x + 1)) + x);
}

void build_indexes(query_ctx& ctx) {
    std::vector<std::pair<std::string, std::string>> indexes { 
        std::make_pair("District", "id"),
        std::make_pair("Customer", "id"),
        std::make_pair("Item", "id"),
        std::make_pair("Stock", "id"),
        std::make_pair("Order", "id")
    };
 
    ctx.run_transaction([&]() {
        for (auto& idx : indexes) {
            if (!ctx.gdb_->has_index(idx.first, idx.second))
                ctx.gdb_->create_index(idx.first, idx.second);
        }
        return true;
    });
    ctx.gdb_->flush();
}

/*
 * https://github.com/AgilData/tpcc/blob/master/src/main/java/com/codefutures/tpcc/NewOrder.java
 */
bool oltp_new_order(bool check_result = false) {
    // 1. compute random values
    auto warehouse_id = uniform_random_int(1, 5);
    sequence_generator seq(WAREHOUSE_COUNT * DISTRICT_PER_WAREHOUSE * CUSTOMER_PER_DISTRICT * 100 + 1000);

    auto district_id = (warehouse_id - 1) * DISTRICT_PER_WAREHOUSE + uniform_random_int(1, DISTRICT_PER_WAREHOUSE);
    auto cust_min = (district_id - 1) * CUSTOMER_PER_DISTRICT + 1;
    auto cust_max = district_id * CUSTOMER_PER_DISTRICT;
    auto customer_id = non_uniform_random_int(1023, cust_min, cust_max, uniform_random_int(0, 1023));
    auto ol_cnt = uniform_random_int(5, 15);

    double w_tax = 0.0;
    double d_tax = 0.0;
    int next_o_id = 0;
    double c_discount = 0.0;
    std::string last_name;
    std::string credit_status;
  
    spdlog::info("New-Order transaction for w_id={}, d_id={}, c_id={}", warehouse_id, district_id, customer_id);
    // 2. start a new transaction
    graph->begin_transaction();

    // 3. retrieve the data (warehouse tax rate) for the given warehouse
    // 4. retrieve the data (district tax rate, next available order number) for the corresponding district
    // 5. retrieve the customer data (customer's discount rate, the customer's last name, the customer's credit status)
    auto iter1 = qproc_ptr->exec_query( 
        "Project([$4.discount:double, $4.last:string, $4.credit:string, $0.tax:double, $2.next_o_id:int, $2.tax:double], "
        "Match((w:Warehouse {{id: {0} }})-[:covers]->(d:District {{id: {1} }})-[:serves]->(c:Customer {{id: {2} }})))", 
        warehouse_id, district_id, customer_id);
    if (iter1.is_valid()) {
        c_discount = iter1.get<double>(0);
        last_name = iter1.get<std::string>(1);
        credit_status = iter1.get<std::string>(2);
        w_tax = iter1.get<double>(3);
        next_o_id = iter1.get<int>(4);
        d_tax = iter1.get<double>(5);
    }
    iter1.close();
    next_o_id++;

    auto iter2 = qproc_ptr->exec_query( 
        "Update($0:node {{ next_o_id: {0} }}, IndexScan('District', 'id', {1}))", next_o_id, district_id);
    if (!iter2.is_valid()) {
        graph->abort_transaction();
        return false;
    }
    iter2.close();

    // 7. insert into both the NEW-ORDER table and the ORDER table: carrier_Id is set to a null value.
    auto o_carrier_id = 0;
    // 8. If the order includes only home order-lines, then allLocal is set to 1, otherwise allLocal is set to 0.
    auto all_local = 1;
    auto o_new_order = 1;
    uint64_t o_id = WAREHOUSE_COUNT * DISTRICT_PER_WAREHOUSE * CUSTOMER_PER_DISTRICT + uniform_random_int(1, 10);
    node* new_order = nullptr;
    auto iter3 = qproc_ptr->exec_query(
        "Create((o:Order {{id: {0}, entry_d: pb::now(), carrier_id: {1}, ol_cnt: {2}, all_local: {3}, new_order: {4} }}))",
        o_id, o_carrier_id, ol_cnt, all_local, o_new_order);
    if (iter3.is_valid()) {
        new_order = iter3.get<node *>(0);
        spdlog::info("New order {} created: #{}", o_id, new_order->id());
    } 
    else {
        graph->abort_transaction();
        return false;
    }
    iter3.close();
 
    // 9. for each item in the order
    for (auto ol_number = 1; ol_number <= ol_cnt; ol_number++) {
        // 9.1 retrieve the data (price, name, data) for the item and the stock; if not found then abort the transaction
        auto item_id = non_uniform_random_int(8191, 1, 100000, uniform_random_int(1, 100000));
        auto stock_id = 0ul;

        auto iter4 = qproc_ptr->exec_query( 
            "Project([$0.price:double, $0.name:string, $0.data:string], IndexScan('Item', 'id', {}))", item_id);
        if (!iter4.is_valid()) {
            graph->abort_transaction();
            return false;
        }
        auto i_price = iter4.get<double>(0);
        auto i_name = iter4.get<std::string>(1);
        auto i_data = iter4.get<std::string>(2);
        iter4.close();
        
        // 9.2 update the stock data
        int s_quantity = 0.0;
        int s_ytd = 0, s_remote_cnt = 0;
        std::string s_data;
        std::vector<std::string> s_dist(10);

        auto iter5 = qproc_ptr->exec_query( 
        "Project([$2.quantity:int, $2.data:string, $2.ytd:int, $2.remote_cnt:int, $2.id:uint64, "
        "$2.dist_01:string, $2.dist_02:string, $2.dist_03:string, $2.dist_04:string, $2.dist_05:string, "
        "$2.dist_06:string, $2.dist_07:string, $2.dist_08:string, $2.dist_09:string, $2.dist_10:string],"
        "Match((i:Item {{ id: {0} }})-[:hasStock]->(s:Stock)))", item_id);
        if (iter5.is_valid()) {
            s_quantity = iter5.get<int>(0);
            s_data = iter5.get<std::string>(1);
            s_ytd = iter5.get<int>(2);
            s_remote_cnt = iter5.get<int>(3);
            stock_id = iter5.get<uint64_t>(4);
            for (auto i = 0u; i < 10; i++) {
                s_dist[i] = iter5.get<std::string>(5 + i);
            }
        }
        iter5.close();

        auto iter6 = qproc_ptr->exec_query(
        "Update($0:node {{ quantity: {0} }}, IndexScan('Stock', 'id', {1}))", s_quantity, stock_id);
        if (!iter6.is_valid()) {
            graph->abort_transaction();
            return false;
        }
        iter6.close();
        // 9.3 compute the amount for the item in the order
        // 9.4 if both I_DATA and S_DATA include the string ORIGINAL then set brand-generic field to "B" otherwise to "G"
        // 9.5 compute the total amount for the complete order
        // 9.6 insert a row into OrderLine: OL_DELIVERY_D is set to a null value, OL_NUMBER is set to a unique value within
  
        uint64_t ol_id = seq.next();
        auto ol_quantity = 0;
        auto ol_amount = ol_quantity * i_price * (1 + w_tax + d_tax) * (1 - c_discount);
        auto ol_dist_info = s_dist[district_id % warehouse_id];    

        // ol_o_id, ol_d_id, ol_w_id, ol_number, ol_i_id, ol_supply_w_id, ol_quantity, ol_amount, ol_dist_info
        // id|number|delivery_d|quantity|amount|dist_info
        // $0=Order, $1=Stock, $2=OrderLine
        auto iter7 = qproc_ptr->exec_query(
        "Create(($2)-[:hasStock]->($1), "
        "Create(($0)-[:contains]->($2), "
        "Create((o:OrderLine {{ id: {0}, number: {1}, quantity: {2}, amount: {3}, dist_info: '{4}' }}), "
        "CrossJoin(NodeById({5}), IndexScan('Stock', 'id', {6})))))",
        ol_id, ol_number, ol_quantity, ol_amount, ol_dist_info, new_order->id(), stock_id);
        if (!iter7.is_valid()) {
            graph->abort_transaction();
            return false;
        }
        spdlog::info("New order_line created: #{}", ol_id);
       iter7.close();
    }

    // 10. commit the transaction
    graph->commit_transaction();

    if (check_result) {
        graph->run_transaction([&]() {
            auto iter8 = qproc_ptr->exec_query(
                "Project([$0.id:int, $0.ol_cnt:int, $2.id:int, $2.number:int], "
                "Expand(OUT, 'OrderLine', ForeachRelationship(FROM, 'contains', IndexScan('Order', 'id', {0}))))",
                o_id);
            while (iter8.is_valid()) {
                auto order_id = iter8.get<int>(0);
                auto ol_cnt = iter8.get<int>(1);
                auto orderline_id = iter8.get<int>(2);
                auto number = iter8.get<int>(3);
                spdlog::info("---> {}, {}", order_id, orderline_id);
                ++iter8;
            }
            iter8.close();
            return true;
        });
    }
    return true;
}

void oltp_query_2() {
    
}

void oltp_query_3() {
    
}

void oltp_query_4() {
    
}

std::string check_config_files(const std::string& fname) {
  std::filesystem::path cwd_config_file(fname);
  if (std::filesystem::exists(cwd_config_file))
    return cwd_config_file.string();
  
  std::string full_name = getenv("HOME") + std::string("/") + fname;
  std::filesystem::path home_config_file(full_name);
  if (std::filesystem::exists(home_config_file))
    return home_config_file.string();

  return "";
}

int main(int argc, char *argv[]) {
    std::string db_name, pool_path, query_file, qmode_str;
    std::size_t bp_size = 0;
    query_proc::mode qmode = query_proc::Interpret;

    auto console = spdlog::stdout_color_mt("poseidon");
    spdlog::set_default_logger(console);
    spdlog::info("Starting poseidon cli, Version {}", POSEIDON_VERSION);

    try {
        options_description desc{"Options"};
        desc.add_options()("help,h", "Help")
        ("verbose,v", bool_switch()->default_value(false), "Verbose - show debug output")
        ("db,d", value<std::string>(&db_name)->required(), "Database name (required)")
        ("pool,p", value<std::string>(&pool_path)->required(), "Path to the PMem/file pool")
        ("buffersize,b", value<std::size_t>(&bp_size), "Size of the bufferpool (in MB)")
        ("llvm", bool_switch()->default_value(false), "Use query compile mode");

        variables_map vm;
        store(parse_command_line(argc, argv, desc), vm);
        auto config_name = check_config_files("poseidon.ini");
        if (!config_name.empty())
        {
            spdlog::info("loading config from '{}'", config_name);
            std::ifstream ifs(config_name);
            store(parse_config_file(ifs, desc), vm);
        }

        if (vm.count("help"))
        {
            std::cout << "Poseidon Graph Database Version " << POSEIDON_VERSION << " ("
#ifdef USE_PMDK
                      << "persistent memory"
#elif defined(USE_IN_MEMORY)
                      << "in-memory"
#else
                      << "paged files"
#endif
                      << ")\n"
                      << desc << '\n';
            return -1;
        }

        notify(vm);

        if (vm.count("pool"))
            pool_path = vm["pool"].as<std::string>();

        if (vm.count("buffersize"))
            bp_size = vm["buffersize"].as<std::size_t>();

        if (vm.count("verbose"))
            if (vm["verbose"].as<bool>())
                spdlog::set_level(spdlog::level::debug);

        if (vm.count("llvm"))
            qmode = vm["llvm"].as<bool>() ? query_proc::Compile : query_proc::Interpret;
    }
    catch (const error &ex)
    {
        std::cerr << ex.what() << '\n';
        return -1;
    }

    auto num_buf_pages = (bp_size * 1024 * 1024) / PF_PAGE_SIZE;

    if (access(pool_path.c_str(), F_OK) != 0) {
        exit(1);
    }

    spdlog::info("opening poolset {}", pool_path);
    pool = graph_pool::open(pool_path, true);
    graph = pool->open_graph(db_name, num_buf_pages );

    query_ctx ctx(graph);
    qproc_ptr = std::make_unique<query_proc>(ctx);
    qproc_ptr->set_execution_mode(qmode);

    build_indexes(ctx);

    ctx.set_auto_commit(false);

    // TODO
    auto res = oltp_new_order(true);
    if (res)
        spdlog::info("New-Order transaction finished successfully.");
    else 
        spdlog::info("New-Order transaction aborted.");

    graph->flush();
    graph->close_files();
}
