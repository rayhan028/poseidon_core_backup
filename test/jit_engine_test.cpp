/*
 * Copyright (C) 2019-2023 DBIS Group - TU Ilmenau, All Rights Reserved.
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
#include "defs.hpp"
#include "graph_pool.hpp"
#include "jit_engine.hpp"
#include "ir_generator.hpp"
#include "qresult_iterator.hpp"
#include "expression.hpp"
#include "binary_expression.hpp"
#include "query_proc.hpp"

const std::string test_path = PMDK_PATH("jit_tst");

TEST_CASE("Testing JIT code generation and compilation", "[jit_engine]") {
    auto pool = graph_pool::create(test_path);
    auto graph = pool->create_graph("my_jit_graph");
    
    node::id_t n1, n2;

    graph->run_transaction([&]() {
        n1 = graph->add_node("Person", {{"id", std::any((int)42)},
                        {"firstName", std::any(std::string("Dom"))},
                        {"lastName", std::any(std::string("Cobb"))}});
        n2 = graph->add_node("Person", {{"id", std::any((int)44)},
                        {"firstName", std::any(std::string("Andrei"))},
                        {"lastName", std::any(std::string("Sator"))}});
        return true;
    });

    SECTION("Testing loading and executing an IR predicate function") {
        query_ctx ctx(graph);    
        jit_engine jit(graph);

        std::string home(".");
        auto h = getenv("TEST_HOME");
        if (h != nullptr)
            home = h;

        REQUIRE(jit.load_code(home + "/test/simple_filter.ll") == true);

        auto fun = jit.get_predicate_function("simple_filter_1");
        REQUIRE(fun != nullptr);

        graph->run_transaction([&]() {
            qr_tuple v1(1);
            auto& node1 = graph->node_by_id(n1);
            v1[0] = &node1;

            bool res1 = fun(&ctx, &v1);
            REQUIRE(res1 == true);
            return true;
        });


        graph->run_transaction([&]() {
            qr_tuple v2(1);
            auto& node2 = graph->node_by_id(n2);
            v2[0] = &node2;

            bool res2 = fun(&ctx, &v2);
            REQUIRE(res2 == false);
            return true;
        });
    }

    SECTION("Testing loading and executing IR aggregate functions") {
        query_ctx ctx(graph);    
        jit_engine jit(graph);

        std::string home(".");
        auto h = getenv("TEST_HOME");
        if (h != nullptr)
            home = h;

        REQUIRE(jit.load_code(home + "/test/simple_aggregate.ll") == true);

        auto [fun1, fun2, fun3] = jit.get_aggregate_functions("aggr_1");
        REQUIRE(fun1 != nullptr);
        REQUIRE(fun2 != nullptr);
        REQUIRE(fun3 != nullptr);

        auto fun_iterate = fun2; // workaround for a bug in clang before C++20
        uint8_t* buf = (uint8_t *)malloc(100);
        // aggr[0]: sum(id), aggr[1] = count(id), aggr2[0] = avg(id)
        uint64_t *aggr = reinterpret_cast<uint64_t *>(buf);
        double *aggr2 = reinterpret_cast<double *>(buf + 32);

        fun1(buf, 100);
        REQUIRE(aggr[0] == 0);
        REQUIRE(aggr[1] == 0);
        REQUIRE(aggr[2] == 0);
        REQUIRE(aggr[3] == 0);
        REQUIRE(aggr2[0] == 0.0);

        graph->run_transaction([&]() {
            qr_tuple v(1);
            auto& node1 = graph->node_by_id(n1);
            v[0] = &node1;
            fun_iterate(&ctx, buf, 100, &v);
            REQUIRE(aggr[0] == 42);
            REQUIRE(aggr[1] == 1);
            REQUIRE(aggr[2] == 42);
            REQUIRE(aggr[3] == 1);

            auto& node2 = graph->node_by_id(n2);
            v[0] = &node2;
            fun_iterate(&ctx, buf, 100, &v);
            REQUIRE(aggr[0] == 86);
            REQUIRE(aggr[1] == 2);
            REQUIRE(aggr[2] == 86);
            REQUIRE(aggr[3] == 2);

            return true;
        });

        fun3(&ctx, buf, 100);
        REQUIRE(aggr[0] == 86);
        REQUIRE(aggr[1] == 2);
        REQUIRE(aggr2[0] == 43.0);

        free(buf);
    }

    SECTION("Testing compiling and executing expression") {
        query_ctx ctx(graph);    
        jit_engine jit(graph);
        ir_generator codegen(jit.get_context());

        auto ex = EQ(Variable(0, "id", 25), UInt64(42));
        auto fop = std::make_shared<filter_op>(ex);
        auto module = codegen.generate(fop->ex_, "simple_filter_2");
        // codegen.dump(module);

        auto res = jit.add_module(std::move(module));
        REQUIRE(res == true);

        auto fun = jit.get_predicate_function("simple_filter_2");
        REQUIRE(fun != nullptr);  

        graph->run_transaction([&]() {
            qr_tuple v(1);
            auto& node1 = graph->node_by_id(n1);
            v[0] = &node1;

            bool res = fun(&ctx, &v);
            REQUIRE(res == true);
            return true;
        });     
    }

    SECTION("Testing compiling eq expression as part of filter_op") {
        query_ctx ctx(graph);    
        query_proc qp(ctx);    
        auto res = qp.execute_query(query_proc::Compile, "Filter($0.id == 42, NodeScan())");
    
        result_set expected;
        expected.append({qv_("Person[0]{firstName: \"Dom\", id: 42, lastName: \"Cobb\"}")});
        REQUIRE(res.result() == expected);
    }

    SECTION("Testing compiling lt expression as part of filter_op") {
        query_ctx ctx(graph);    
        query_proc qp(ctx);    
        auto res = qp.execute_query(query_proc::Compile, "Filter($0.id < 100, NodeScan())");
    
        result_set expected;
        expected.append({qv_("Person[0]{firstName: \"Dom\", id: 42, lastName: \"Cobb\"}")});
        expected.append({qv_("Person[1]{firstName: \"Andrei\", id: 44, lastName: \"Sator\"}")});
        REQUIRE(res.result() == expected);
    }

    SECTION("Testing compiling aggregation functions") {
        query_ctx ctx(graph);    
        jit_engine jit(graph);
        ir_generator codegen(jit.get_context());

        std::vector<aggregate::expr> ex { { aggregate::expr::f_sum, 0u, "id", 25, int_type }, 
                                          { aggregate::expr::f_count, 0u, "id", 25, int_type }, 
                                          { aggregate::expr::f_avg, 0u, "id", 25, int_type } };      
        auto agg = std::make_shared<aggregate>(ex, ctx.get_dictionary());
        auto module = codegen.generate(agg, "aggr_1");
        // codegen.dump(module);

        auto res = jit.add_module(std::move(module));
        REQUIRE(res == true);

        auto [fun1, fun2, fun3] = jit.get_aggregate_functions("aggr_1");
        REQUIRE(fun1 != nullptr);
        REQUIRE(fun2 != nullptr);
        REQUIRE(fun3 != nullptr);
        auto fun_iterate = fun2;

        uint8_t *buf = new uint8_t[1024];
        fun1(buf, 1024);

        graph->run_transaction([&]() {
            qr_tuple v(1);
            auto& node1 = graph->node_by_id(n1);
            v[0] = &node1;
            fun_iterate(&ctx, buf, 1024, &v);

            auto& node2 = graph->node_by_id(n2);
            v[0] = &node2;
            fun_iterate(&ctx, buf, 1024, &v);

            return true;
        });
        
        fun3(&ctx, buf, 1023);
        // aggr[0]: sum(id), aggr[1] = count(id), aggr2[0] = avg(id)
        uint64_t *aggr1 = reinterpret_cast<uint64_t *>(buf);
        double *aggr2 = reinterpret_cast<double *>(buf + 32);
        
        REQUIRE(aggr1[0] == 86);
        REQUIRE(aggr1[1] == 2);
        REQUIRE(aggr2[0] == 43.0);
        
        delete [] buf;
    }

    SECTION("Testing compiling and executing aggregation") {
        query_ctx ctx(graph);    
        query_proc qp(ctx);    
        auto res = qp.execute_query(query_proc::Compile, "Aggregate([sum($0.id:int), count($0.id:int), avg($0.id:int)], NodeScan())");
    
        result_set expected;
        expected.append({qv_("86"), qv_("2"), qv_("43.000000")});
        REQUIRE(res.result() == expected);
    }

    graph_pool::destroy(pool);
}