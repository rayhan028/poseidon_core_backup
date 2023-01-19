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

#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do
                          // this in one cpp file

#include <catch2/catch_test_macros.hpp>
#include "config.h"
#include "graph_db.hpp"
#include "graph_pool.hpp"
#include "qop.hpp"
#include "query.hpp"

#include <set>
#include <iostream>
#include <boost/variant.hpp>

const std::string test_path = poseidon::gPmemPath + "query_recovery_test";

void init_graph(graph_db_ptr &gdb) {
    gdb->begin_transaction();
    for(int i = 0; i < 10; i++) {
        auto p1 = gdb->add_node("Person", {});
        auto p2 = gdb->add_node("Person", {});
        gdb->add_relationship(p1, p2, ":knows", {}, false);
    }
    gdb->commit_transaction();
}

TEST_CASE("Store checkpoint test") {
#ifdef QOP_RECOVERY
    auto pool = graph_pool::create(test_path);
    auto graph = pool->create_graph("my_graph");

    init_graph(graph);

    SECTION("Test storing checkpoints") {
        auto iter = graph->get_nodes()->range(0,0);
        for(int i = 0; i < 6; i++) {

            REQUIRE(iter.get_cur_chunk() == 0);
            REQUIRE(iter.get_cur_pos() == i);

            graph->store_iter({iter.get_cur_chunk(), iter.get_cur_pos()});
            auto checkpoints = graph->get_query_checkpoints();

            int j = 0;
            for(auto & cp : *checkpoints) {
                REQUIRE(cp.first == 0);
                REQUIRE(cp.second == i);
            }

            ++iter;
            }
    }

    SECTION("Test restoring checkpoints") {
        for(int i = 0; i < 10; i++) {
            graph->store_iter({i, i*42});
        }

        auto checkpoints = graph->restore_positions();

        int chunk = 0;
        for(auto & cp : checkpoints) {
            auto pos = chunk * 42;
            REQUIRE(cp.first == chunk);
            REQUIRE(cp.second == pos);
            chunk++;
        }
        REQUIRE(chunk == 10);
    }

    SECTION("Test storing tuple result") {
        qr_tuple qrt;
        qrt.push_back(int(21));
        qrt.push_back(int(42));

        for(int i = 0; i < 10; i++) {
            graph->store_query_result(qrt, 0);
        }

        auto recs = graph->get_recovery_results();

        REQUIRE(recs->size() == 10);

        std::list<qr_tuple> restored_list;
        graph->restore_results(restored_list);

        REQUIRE(restored_list.size() == 10);

        for(auto & rt : restored_list) {
            REQUIRE(rt.size() == 2);
            int i = 21;
            for(auto & tp : rt) {
                REQUIRE(boost::get<int>(tp) == i);
                i *= 2;
            }
        }
    }

    SECTION("Test storing nodes/rships") {
        graph->begin_transaction();
        auto & n1 = graph->node_by_id(7);
        auto & r = graph->rship_by_id(3);
        auto & n2 = graph->node_by_id(4);
        graph->commit_transaction();

        qr_tuple qrt;
        qrt.push_back(&n1);
        qrt.push_back(&r);
        qrt.push_back(&n2);

        graph->store_query_result(qrt, 0);

        auto recs = graph->get_recovery_results();

        REQUIRE(recs->size() == 1);

        std::vector<offset_t> expected = {7, 3, 4};
        int i = 0;
        for(auto & res : recs->as_vec()) {
            res.res_ = expected[i++];    
        }
    }

    SECTION("Test storing strings") {
        std::string to_insert1 = "test_42_str1";
        std::string to_insert2 = "test_42_str2";
        auto dicti = graph->get_dictionary();
        auto cd1 = dicti->lookup_string(to_insert1);
        auto cd2 = dicti->lookup_string(to_insert2);

        REQUIRE(cd1 == 0);
        REQUIRE(cd2 == 0);

        qr_tuple qrt;
        qrt.push_back(to_insert1);
        qrt.push_back(to_insert2);

        graph->store_query_result(qrt, 0);

        auto recs = graph->get_recovery_results();

        REQUIRE(recs->size() == 1);

        cd1 = dicti->lookup_string(to_insert1);
        cd2 = dicti->lookup_string(to_insert2);

        REQUIRE(cd1 == 3);
        REQUIRE(cd2 == 4);
    }

    SECTION("Test storing tuple result with different type") {
        qr_tuple qrt;
        qrt.push_back(int(21));
        qrt.push_back(std::string("test_42"));
        qrt.push_back(double(5.4f));
        graph->begin_transaction();
        qrt.push_back(&graph->node_by_id(7));
        graph->commit_transaction();
        graph->begin_transaction();
        qrt.push_back(&graph->rship_by_id(3));
        graph->commit_transaction();

        graph->store_query_result(qrt, 0);
        auto recs = graph->get_recovery_results();

        REQUIRE(recs->size() == 1);
        
        std::list<qr_tuple> restored_list;
        
        graph->begin_transaction();
        graph->restore_results(restored_list);
        graph->commit_transaction();
        
        auto tp = restored_list.front();
        REQUIRE(tp.size() == 5);

        auto i_res = tp.begin();
        REQUIRE(boost::get<int>(*i_res) == 21);

        auto str_res = ++i_res;
        REQUIRE(boost::get<std::string>(*str_res) == "test_42");

        auto d_res = ++str_res;
        REQUIRE(boost::get<double>(*d_res) == 5.4f);

        auto n_res = ++d_res;
        REQUIRE(boost::get<node*>(*n_res)->id() == 7);

        auto r_res = ++n_res;
        REQUIRE(boost::get<relationship*>(*r_res)->id() == 3);

        std::vector<offset_t> exp_type = {2, 3, 4, 0, 1};

        offset_t dbl_val = 0.0;
        double src = 5.4f;
        std::memcpy(&dbl_val, &src, sizeof(double));

        std::vector<offset_t> exp_val = {21, 3, dbl_val, 7, 3};

        int i = 0;
        for(auto & res : recs->as_vec()) {
            REQUIRE(res.type_ == exp_type[i]);
            REQUIRE(res.res_ == exp_val[i++]);
            REQUIRE(res.tuple_id_ == 0);
            REQUIRE(res.chunk_ == 0);
        }
    }

    SECTION("Test parallel insert") {
        std::vector<std::thread> threads;
        qr_tuple qrt;
        qrt.push_back(int(21));
        qrt.push_back(std::string("test_42"));
        qrt.push_back(double(5.4f));

        for(int i = 0; i < 4; i++) {
            threads.push_back(std::thread([&]{
                for(int j = 0; j < 10; j++) {
                    graph->store_query_result(qrt, i);
                }
            }));
        }

        for(auto & t : threads) {
            t.join();
        }

        auto recs = graph->get_recovery_results();

        REQUIRE(recs->size() == 40);

        std::list<qr_tuple> restored_list;
        graph->restore_results(restored_list);

        REQUIRE(restored_list.size() == 40);

        offset_t dbl_val = 0.0;
        double src = 5.4f;
        std::memcpy(&dbl_val, &src, sizeof(double));

        for(auto & tp : restored_list) {
            REQUIRE(tp.size() == 3);
            int i = 0;
            for(auto & res : tp) {
                if(i++ == 0) {
                    REQUIRE(boost::get<int>(res) == 21);
                } else if (i++ == 1) {
                    REQUIRE(boost::get<std::string>(res) == "test_42");
                } else if (i++ == 2) {
                    REQUIRE(boost::get<double>(res) == 5.4f);
                }
            }
        }
    }

    SECTION("Test persist results in query pipeline") {
        result_set rs;
        auto q = query(graph).all_nodes("Person")
            .from_relationships(":knows")
                .to_node("Person")
                    .persist()
                        .collect(rs);

        graph->begin_transaction();
        query::start({&q});
        graph->commit_transaction();

        REQUIRE(rs.data.size() == 10);

        auto recs = graph->get_recovery_results();

        REQUIRE(recs->size() == 10);

        std::list<qr_tuple> restored_list;

        graph->begin_transaction();
        graph->restore_results(restored_list);
        graph->commit_transaction(); 
        
        REQUIRE(restored_list.size() == 10);

        for(auto & res : restored_list) {
            REQUIRE(res.size() == 3);
            
            auto res_it = res.begin();
            auto n1 = boost::get<node*>(*res_it++);
            REQUIRE(n1);

            auto r = boost::get<relationship*>(*res_it++);
            REQUIRE(r);

            auto n2 = boost::get<node*>(*res_it);
            REQUIRE(n2);
        }
    }

    SECTION("Test checkpoints in query pipeline") {
        result_set rs;
        auto q = query(graph).all_nodes()
            .has_label("Person")
            .from_relationships(":knows")
                .to_node("Person")
                    .persist()
                        .collect(rs);

        graph->begin_transaction();
        query::start({&q});
        graph->commit_transaction();
        rs.wait();
        REQUIRE(rs.data.size() == 10);

        auto checkpoints = graph->get_query_checkpoints();

        int chunk = 0;
        for(auto & cp : *checkpoints) {
            REQUIRE(cp.second == 19);
            chunk++;
        }
        REQUIRE(chunk == 1);

        graph->clear_result_storage();

        checkpoints = graph->get_query_checkpoints();

        chunk = 0;
        for(auto & cp : *checkpoints) {
            chunk++;
        }
        REQUIRE(chunk == 0);

        rs.data.clear();

        q = query(graph).all_nodes()
            .has_label("Person")
            .from_relationships(":knows")
                .to_node("Person")
                        .persist()
                            .crash(7)
                            .collect(rs);

        graph->begin_transaction();
        try {
            query::start({&q});
        } catch(std::exception& e) {
            graph->commit_transaction();
        }
        
        REQUIRE(rs.data.size() == 7);
    }

    SECTION("Test reset of intermediate result storage") {
        qr_tuple qrt;
        qrt.push_back(int(21));
        qrt.push_back(std::string("test_42"));
        qrt.push_back(double(5.4f));

        for(int i = 0; i < 42; i++) {
            graph->store_query_result(qrt, 0);
        }

        auto recs = graph->get_recovery_results();

        REQUIRE(recs->size() == 42);

        recs->clear();

        REQUIRE(recs->size() == 0);

        int i = 0;
        for(auto & res : recs->as_vec()) {
            i++;
        }
        REQUIRE(i==0);
    }

    SECTION("Test checkpoints and results after crash") {
        result_set rs; 

        auto q = query(graph).all_nodes()
                .has_label("Person")
                    .crash(7)
                        .persist()
                            .collect(rs);

        graph->begin_transaction();
        try {
            query::start({&q});
        } catch(std::exception& e) {
            graph->commit_transaction();
        }
        
        REQUIRE(rs.data.size() == 7);
        auto checkpoints = graph->get_query_checkpoints();

        int chunk = 0;
        for(auto & cp : *checkpoints) {
            REQUIRE(cp.first == chunk);
            REQUIRE(cp.second == 6);
            chunk++;
        }
        REQUIRE(chunk == 1);

        graph->begin_transaction();
        std::list<qr_tuple> restored_list;
        graph->restore_results(restored_list);
        graph->commit_transaction();
        
        REQUIRE(restored_list.size() == 7);

        int i = 0;
        for(auto & res : restored_list) {
            REQUIRE(res.size() == 1);
            REQUIRE(res.front().type() == typeid(node*));
            auto n = boost::get<node*>(res.front());
            REQUIRE(n->id() == i++);
        }
    }

    SECTION("Test results restoration after crash with multiple elements") {
        result_set rs; 
        auto q = query(graph).all_nodes()
                    .has_label("Person")
                        .from_relationships(":knows")
                            .to_node("Person")
                            .crash(6)
                                    .persist()
                                        .collect(rs);

        graph->begin_transaction();
        try {
            query::start({&q});
        } catch(std::exception& e) {
            graph->commit_transaction();
        }

        REQUIRE(rs.data.size() == 6);

        graph->begin_transaction();
        std::list<qr_tuple> restored_list;
        graph->restore_results(restored_list);
        graph->commit_transaction();
        
        REQUIRE(restored_list.size() == 6);

        for(auto & res : restored_list) {
            REQUIRE(res.size() == 3);

            auto rit = res.begin();

            auto n1 = boost::get<node*>(*rit++);
            REQUIRE(n1->id() >= 0);
        
            auto r = boost::get<relationship*>(*rit++);
            REQUIRE(r->id() >= 0);
        
            auto n2 = boost::get<node*>(*rit);
            REQUIRE(n2->id() >= 0);
        }
    }



    graph_pool::destroy(pool);

#else
    REQUIRE(true);
#endif
}

TEST_CASE("Recovery and continue failed query") {
#ifdef QOP_RECOVERY
    auto pool = graph_pool::create(test_path);
    auto graph = pool->create_graph("my_graph");

    init_graph(graph);

    SECTION("Continue query after failure") {
        result_set rs; 
        auto q = query(graph).all_nodes()
                    .has_label("Person")
                        .from_relationships(":knows")
                            .to_node("Person")
                            .crash(3)
                                    .persist()
                                        .collect(rs);

        graph->begin_transaction();
        try {
            query::start({&q});
        } catch(std::exception& e) {
            graph->commit_transaction();
        }

        REQUIRE(rs.data.size() == 3);

        auto checkpoints = graph->restore_positions();

        auto c = query(graph)
                    .continue_scan(checkpoints)
                        .has_label("Person")
                            .from_relationships(":knows")
                                .to_node("Person")
                                    .persist()
                                        .collect(rs);
        graph->begin_transaction();
        query::start({&c});
        graph->commit_transaction();

        REQUIRE(rs.data.size() == 10);

        auto new_cp = graph->restore_positions();

        REQUIRE(new_cp[0] == 19);

        graph->clear_result_storage();
    }

    SECTION("Restore scan after failure") {
        result_set rs; 
        auto q = query(graph).all_nodes()
                    .has_label("Person")
                        .from_relationships(":knows")
                            .to_node("Person")
                            .crash(3)
                                    .persist()
                                        .collect(rs);

        graph->begin_transaction();
        try {
            query::start({&q});
        } catch(std::exception& e) {
            graph->commit_transaction();
        }

        result_set recovered;
        auto rq = query(graph).recover_results().collect(recovered);

        graph->begin_transaction();
        query::start({&rq});
        graph->commit_transaction();

        REQUIRE(recovered.data.size() == 3);

        graph->clear_result_storage();
    }

    SECTION("Restore query pipeline and continue operator") {
        result_set rs; 
        auto q = query(graph).all_nodes()
                    .has_label("Person")
                            .crash(3)
                                    .persist()
                                        .collect(rs);

        graph->begin_transaction();
        try {
            query::start({&q});
        } catch(std::exception& e) {
            graph->commit_transaction();
        }

        result_set recovered;
        auto rq = query(graph)
                .recover_results()
                    .from_relationships(":knows")
                            .to_node("Person")
                                .collect(recovered);

        graph->begin_transaction();
        query::start({&rq});
        graph->commit_transaction();

        REQUIRE(recovered.data.size() == 2);

        for(auto & res : recovered.data) {
            REQUIRE(res.size() == 3);
        }                        

    }

    graph_pool::destroy(pool);
#else
    REQUIRE(true);
#endif
}