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

#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do
                          // this in one cpp file

#include <filesystem>
#include "spdlog/spdlog.h"
#include "fmt/format.h"
#include <catch2/catch_test_macros.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "defs.hpp"
#include "expression.hpp"
#include "binary_expression.hpp"
#include "expr_interpreter.hpp"
#include "graph_pool.hpp"
#include "jit_engine.hpp"
#include "ir_generator.hpp"
#include "qresult_iterator.hpp"
#include "query_proc.hpp"
#include "plan_visitors/prepare_expr_visitor.hpp"

const std::string test_path = PMDK_PATH("expr_tst");

TEST_CASE("Creating and interpreting expressions", "[expression]") {
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
    query_ctx qctx(graph);

    SECTION("plain expressions with int") {
        qr_tuple tup;
        auto ex1 = EQ(Int(42), Int(42));
        REQUIRE(interpret_expression(qctx, ex1, tup) == true);

        auto ex2 = EQ(Int(42), Int(15));
        REQUIRE(interpret_expression(qctx, ex2, tup) == false);

        auto ex3 = LT(Int(42), Int(15));
        REQUIRE(interpret_expression(qctx, ex3, tup) == false);

        auto ex4 = LT(Int(15), Int(16));
        REQUIRE(interpret_expression(qctx, ex4, tup) == true);

        auto ex5 = LE(Int(15), Int(16));
        REQUIRE(interpret_expression(qctx, ex5, tup) == true);

        auto ex6 = LE(Int(15), Int(15));
        REQUIRE(interpret_expression(qctx, ex6, tup) == true);

        auto ex7 = LE(Int(15), Int(14));
        REQUIRE(interpret_expression(qctx, ex7, tup) == false);

        auto ex8 = GT(Int(42), Int(15));
        REQUIRE(interpret_expression(qctx, ex8, tup) == true);

        auto ex9 = GT(Int(15), Int(16));
        REQUIRE(interpret_expression(qctx, ex9, tup) == false);

        auto ex10 = GE(Int(16), Int(1));
        REQUIRE(interpret_expression(qctx, ex10, tup) == true);

        auto ex11 = GE(Int(15), Int(15));
        REQUIRE(interpret_expression(qctx, ex11, tup) == true);

        auto ex12 = GE(Int(12), Int(14));
        REQUIRE(interpret_expression(qctx, ex12, tup) == false);

        auto ex13 = NEQ(Int(42), Int(42));
        REQUIRE(interpret_expression(qctx, ex13, tup) == false);

        auto ex14 = NEQ(Int(43), Int(42));
        REQUIRE(interpret_expression(qctx, ex14, tup) == true);
    }

    SECTION("plain expressions with double") {
        qr_tuple tup;
        auto ex1 = EQ(Float(42.0), Float(42.0));
        REQUIRE(interpret_expression(qctx, ex1, tup) == true);

        auto ex2 = EQ(Float(42.0), Float(15.0));
        REQUIRE(interpret_expression(qctx, ex2, tup) == false);

        auto ex3 = LT(Float(42.0), Float(15.0));
        REQUIRE(interpret_expression(qctx, ex3, tup) == false);

        auto ex4 = LT(Float(15.0), Float(16.0));
        REQUIRE(interpret_expression(qctx, ex4, tup) == true);

        auto ex5 = LE(Float(15.0), Float(16.0));
        REQUIRE(interpret_expression(qctx, ex5, tup) == true);

        auto ex6 = LE(Float(15.0), Float(15.0));
        REQUIRE(interpret_expression(qctx, ex6, tup) == true);

        auto ex7 = LE(Float(15.0), Float(14.0));
        REQUIRE(interpret_expression(qctx, ex7, tup) == false);

        auto ex8 = GT(Float(42.0), Float(15.0));
        REQUIRE(interpret_expression(qctx, ex8, tup) == true);

        auto ex9 = GT(Float(15.0), Float(16.0));
        REQUIRE(interpret_expression(qctx, ex9, tup) == false);

        auto ex10 = GE(Float(16.0), Float(1.0));
        REQUIRE(interpret_expression(qctx, ex10, tup) == true);

        auto ex11 = GE(Float(15.0), Float(15.0));
        REQUIRE(interpret_expression(qctx, ex11, tup) == true);

        auto ex12 = GE(Float(12.0), Float(14.0));
        REQUIRE(interpret_expression(qctx, ex12, tup) == false);

        auto ex13 = NEQ(Float(42.0), Float(42.0));
        REQUIRE(interpret_expression(qctx, ex13, tup) == false);

        auto ex14 = NEQ(Float(43.0), Float(42.0));
        REQUIRE(interpret_expression(qctx, ex14, tup) == true);
    }

    SECTION("plain expressions with strings") {
        qr_tuple tup;
        auto ex1 = EQ(Str("Hallo"), Str("Hallo"));
        REQUIRE(interpret_expression(qctx, ex1, tup) == true);

        auto ex2 = NEQ(Str("Hallo"), Str("Hallo"));
        REQUIRE(interpret_expression(qctx, ex2, tup) == false);
 
        auto ex3 = EQ(Str("Hallo"), Str("Hallo2"));
        REQUIRE(interpret_expression(qctx, ex3, tup) == false);

        auto ex4 = NEQ(Str("Hallo"), Str("Hallo2"));
        REQUIRE(interpret_expression(qctx, ex4, tup) == true);
    }

    SECTION("expressions with functions") {
        qr_tuple tup;
        std::string ts("2002-01-20 23:59:59.000");
        boost::posix_time::ptime t(boost::posix_time::time_from_string(ts));
        prepare_expr_visitor vis(qctx, nullptr);

        auto ex1 = EQ(Fct("pb", "iso_dtimestring_to_ptime", std::vector<expr>{ Str(ts)}), Time(t));
        ex1->accept(vis);
        REQUIRE(interpret_expression(qctx, ex1, tup) == true);

        auto ts2 = boost::posix_time::to_iso_extended_string(t);
        auto ex2 = EQ(Fct("pb", "ptime_to_dtimestring", std::vector<expr>{ Time(t)}), Str(ts2));
        ex2->accept(vis);
        REQUIRE(interpret_expression(qctx, ex2, tup) == true);
    }
    
    SECTION("expressions with variables") {
        qr_tuple tup(1);
        prepare_expr_visitor vis(qctx, nullptr);

        graph->run_transaction([&]() {
            auto& n = graph->node_by_id(n1);
            tup[0] = &n;
   
            auto ex1 = EQ(Fct("pb", "label", std::vector<expr>{ Variable(0)}), Str("Person"));
            ex1->accept(vis);
            REQUIRE(interpret_expression(qctx, ex1, tup) == true);

            return true;
        });
    }

    graph_pool::destroy(pool);
}
