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

    SECTION("Testing compiling and executing expression") {
        query_ctx ctx(graph);    
        jit_engine jit(graph);
        ir_generator codegen(jit.get_context());

        auto ex = EQ(Variable(0, "id", 25), UInt64(42));
        auto module = codegen.generate(ex, "simple_filter_2");
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
    graph_pool::destroy(pool);
}