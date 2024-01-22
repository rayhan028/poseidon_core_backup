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

std::unique_ptr<ir_generator> gen;
std::unique_ptr<jit_engine> jit;

static int f_cnt = 1;

bool compile_expression(query_ctx& ctx, const expr& ex, const qr_tuple& tup, bool print = false) {
    auto fname = fmt::format("filter_{}", f_cnt++);
    auto module = gen->generate(ex, fname);
    if (print)
        gen->dump(module);
    jit->add_module(std::move(module));
    auto func = jit->get_predicate_function(fname);  
    return func(&ctx, &tup);
}

bool load_compiled_expression(query_ctx& ctx, const expr& ex, const qr_tuple& tup, int fn) {
    auto fname = fmt::format("filter_{}", fn);
    
    std::string home(".");
    auto h = getenv("TEST_HOME");
    if (h != nullptr)
        home = h;

    auto path = fmt::format("{}/test/{}.ll", home, fname);
    spdlog::info("IR file: {}", path);
    if (!jit->load_code(path))
        spdlog::info("IR code not loaded");
    auto func = jit->get_predicate_function(fname);  
    if (!func)
        spdlog::info("IR function {} not found", fname);
    return func(&ctx, &tup);
}

TEST_CASE("Creating and interpreting expressions", "[expression]") {
    auto pool = graph_pool::create(test_path);
    auto graph = pool->create_graph("my_jit_graph");
    
    query_ctx ctx(graph);    
    jit = std::make_unique<jit_engine>(graph);
    gen = std::make_unique<ir_generator>(jit->get_context());

    node::id_t n1, n2;

    graph->run_transaction([&]() {
        n1 = graph->add_node("Person", {{"id", std::any((int)42)},
                        {"salary", std::any(3456.7)},
                        {"firstName", std::any(std::string("Dom"))},
                        {"lastName", std::any(std::string("Cobb"))}});
        n2 = graph->add_node("Person", {{"id", std::any((int)44)},
                        {"salary", std::any(4567.8)},
                        {"firstName", std::any(std::string("Andrei"))},
                        {"lastName", std::any(std::string("Sator"))}});
        return true;
    });
    graph->flush();
    
    auto id_code = graph->get_code("id");
    auto salary_code = graph->get_code("salary");
    auto lastName_code = graph->get_code("lastName");

    query_ctx qctx(graph);

    SECTION("plain expressions with int") {
        qr_tuple tup;
        auto ex1 = EQ(Int(42), Int(42));
        REQUIRE(interpret_bool_expression(qctx, ex1, tup) == true);
        REQUIRE(compile_expression(qctx, ex1, tup) == true);

        auto ex2 = EQ(Int(42), Int(15));
        REQUIRE(interpret_bool_expression(qctx, ex2, tup) == false);
        REQUIRE(compile_expression(qctx, ex2, tup) == false);

        auto ex3 = LT(Int(42), Int(15));
        REQUIRE(interpret_bool_expression(qctx, ex3, tup) == false);
        REQUIRE(compile_expression(qctx, ex3, tup) == false);

        auto ex4 = LT(Int(15), Int(16));
        REQUIRE(interpret_bool_expression(qctx, ex4, tup) == true);
        REQUIRE(compile_expression(qctx, ex4, tup) == true);

        auto ex5 = LE(Int(15), Int(16));
        REQUIRE(interpret_bool_expression(qctx, ex5, tup) == true);
        REQUIRE(compile_expression(qctx, ex5, tup) == true);

        auto ex6 = LE(Int(15), Int(15));
        REQUIRE(interpret_bool_expression(qctx, ex6, tup) == true);
        REQUIRE(compile_expression(qctx, ex6, tup) == true);

        auto ex7 = LE(Int(15), Int(14));
        REQUIRE(interpret_bool_expression(qctx, ex7, tup) == false);
        REQUIRE(compile_expression(qctx, ex7, tup) == false);

        auto ex8 = GT(Int(42), Int(15));
        REQUIRE(interpret_bool_expression(qctx, ex8, tup) == true);
        REQUIRE(compile_expression(qctx, ex8, tup) == true);

        auto ex9 = GT(Int(15), Int(16));
        REQUIRE(interpret_bool_expression(qctx, ex9, tup) == false);
        REQUIRE(compile_expression(qctx, ex9, tup) == false);

        auto ex10 = GE(Int(16), Int(1));
        REQUIRE(interpret_bool_expression(qctx, ex10, tup) == true);
        REQUIRE(compile_expression(qctx, ex10, tup) == true);

        auto ex11 = GE(Int(15), Int(15));
        REQUIRE(interpret_bool_expression(qctx, ex11, tup) == true);
        REQUIRE(compile_expression(qctx, ex11, tup) == true);

        auto ex12 = GE(Int(12), Int(14));
        REQUIRE(interpret_bool_expression(qctx, ex12, tup) == false);
        REQUIRE(compile_expression(qctx, ex12, tup) == false);

        auto ex13 = NEQ(Int(42), Int(42));
        REQUIRE(interpret_bool_expression(qctx, ex13, tup) == false);
        REQUIRE(compile_expression(qctx, ex13, tup) == false);

        auto ex14 = NEQ(Int(43), Int(42));
        REQUIRE(interpret_bool_expression(qctx, ex14, tup) == true);
        REQUIRE(compile_expression(qctx, ex14, tup) == true);
    }

    SECTION("plain expressions with double") {
        qr_tuple tup;
        auto ex1 = EQ(Float(42.0), Float(42.0));
        REQUIRE(interpret_bool_expression(qctx, ex1, tup) == true);
        REQUIRE(compile_expression(qctx, ex1, tup) == true);

        auto ex2 = EQ(Float(42.0), Float(15.0));
        REQUIRE(interpret_bool_expression(qctx, ex2, tup) == false);
        //REQUIRE(load_compiled_expression(qctx, ex2, tup, 22) == false);
        REQUIRE(compile_expression(qctx, ex2, tup) == false);

        auto ex3 = LT(Float(42.0), Float(15.0));
        REQUIRE(interpret_bool_expression(qctx, ex3, tup) == false);
        REQUIRE(compile_expression(qctx, ex3, tup) == false);

        auto ex4 = LT(Float(15.0), Float(16.0));
        REQUIRE(interpret_bool_expression(qctx, ex4, tup) == true);
        REQUIRE(compile_expression(qctx, ex4, tup) == true);

        auto ex5 = LE(Float(15.0), Float(16.0));
        REQUIRE(interpret_bool_expression(qctx, ex5, tup) == true);
        REQUIRE(compile_expression(qctx, ex5, tup) == true);

        auto ex6 = LE(Float(15.0), Float(15.0));
        REQUIRE(interpret_bool_expression(qctx, ex6, tup) == true);
        REQUIRE(compile_expression(qctx, ex6, tup) == true);

        auto ex7 = LE(Float(15.0), Float(14.0));
        REQUIRE(interpret_bool_expression(qctx, ex7, tup) == false);
        REQUIRE(compile_expression(qctx, ex7, tup) == false);

        auto ex8 = GT(Float(42.0), Float(15.0));
        REQUIRE(interpret_bool_expression(qctx, ex8, tup) == true);
        REQUIRE(compile_expression(qctx, ex8, tup) == true);

        auto ex9 = GT(Float(15.0), Float(16.0));
        REQUIRE(interpret_bool_expression(qctx, ex9, tup) == false);
        REQUIRE(compile_expression(qctx, ex9, tup) == false);

        auto ex10 = GE(Float(16.0), Float(1.0));
        REQUIRE(interpret_bool_expression(qctx, ex10, tup) == true);
        REQUIRE(compile_expression(qctx, ex10, tup) == true);

        auto ex11 = GE(Float(15.0), Float(15.0));
        REQUIRE(interpret_bool_expression(qctx, ex11, tup) == true);
        REQUIRE(compile_expression(qctx, ex11, tup) == true);

        auto ex12 = GE(Float(12.0), Float(14.0));
        REQUIRE(interpret_bool_expression(qctx, ex12, tup) == false);
        REQUIRE(compile_expression(qctx, ex12, tup) == false);

        auto ex13 = NEQ(Float(42.0), Float(42.0));
        REQUIRE(interpret_bool_expression(qctx, ex13, tup) == false);
        REQUIRE(compile_expression(qctx, ex13, tup) == false);

        auto ex14 = NEQ(Float(43.0), Float(42.0));
        REQUIRE(interpret_bool_expression(qctx, ex14, tup) == true);
        REQUIRE(compile_expression(qctx, ex14, tup) == true);
    }

    SECTION("plain expressions with strings") {
        qr_tuple tup;
        auto ex1 = EQ(Str("Hallo"), Str("Hallo"));
        REQUIRE(interpret_bool_expression(qctx, ex1, tup) == true);
        REQUIRE(compile_expression(qctx, ex1, tup) == true);

        auto ex2 = NEQ(Str("Hallo"), Str("Hallo"));
        REQUIRE(interpret_bool_expression(qctx, ex2, tup) == false);
        REQUIRE(compile_expression(qctx, ex2, tup) == false);

        auto ex3 = EQ(Str("Hallo"), Str("Hallo2"));
        REQUIRE(interpret_bool_expression(qctx, ex3, tup) == false);
        REQUIRE(compile_expression(qctx, ex3, tup) == false);

        auto ex4 = NEQ(Str("Hallo"), Str("Hallo2"));
        REQUIRE(interpret_bool_expression(qctx, ex4, tup) == true);
        REQUIRE(compile_expression(qctx, ex4, tup) == true);

        auto ex5 = RE(Str("Hallo"), Str("H.*"));
        REQUIRE(interpret_bool_expression(qctx, ex5, tup) == true);

        auto ex6 = RE(Str("Halto"), Str("Hal[lt]o"));
        REQUIRE(interpret_bool_expression(qctx, ex6, tup) == true);

        auto ex7 = RE(Str("None"), Str("H.*"));
        REQUIRE(interpret_bool_expression(qctx, ex7, tup) == false);

        auto ex8 = LT(Str("Hallo"), Str("Hallo2"));
        REQUIRE(interpret_bool_expression(qctx, ex8, tup) == true);
        REQUIRE(compile_expression(qctx, ex8, tup) == true);

        auto ex9 = GT(Str("Hallo"), Str("Hallo2"));
        REQUIRE(interpret_bool_expression(qctx, ex9, tup) == false);
        REQUIRE(compile_expression(qctx, ex9, tup) == false);
 
        auto ex10 = LT(Str("XYZ"), Str("Hallo"));
        REQUIRE(interpret_bool_expression(qctx, ex10, tup) == false);
        REQUIRE(compile_expression(qctx, ex10, tup) == false);
 
        auto ex11 = LT(Str("XYZ"), Str("Hallo"));
        REQUIRE(interpret_bool_expression(qctx, ex11, tup) == false);
        REQUIRE(compile_expression(qctx, ex11, tup) == false);

        auto ex12 = LE(Str("Hallo"), Str("Hallo2"));
        REQUIRE(interpret_bool_expression(qctx, ex12, tup) == true);
        REQUIRE(compile_expression(qctx, ex12, tup) == true);

        auto ex13 = GE(Str("Hallo2"), Str("Hallo"));
        REQUIRE(interpret_bool_expression(qctx, ex13, tup) == true);
        REQUIRE(compile_expression(qctx, ex13, tup) == true);

        auto ex14 = LE(Str("Hallo"), Str("Hallo"));
        REQUIRE(interpret_bool_expression(qctx, ex14, tup) == true);
        REQUIRE(compile_expression(qctx, ex14, tup) == true);

        auto ex15 = GE(Str("Hallo"), Str("Hallo"));
        REQUIRE(interpret_bool_expression(qctx, ex15, tup) == true);
        REQUIRE(compile_expression(qctx, ex15, tup) == true);

        auto ex16 = GE(Str("Hallo"), Str("Hallo2"));
        REQUIRE(interpret_bool_expression(qctx, ex16, tup) == false);
        REQUIRE(compile_expression(qctx, ex16, tup) == false);
 
        auto ex17 = LE(Str("XYZ"), Str("Hallo"));
        REQUIRE(interpret_bool_expression(qctx, ex17, tup) == false);
        REQUIRE(compile_expression(qctx, ex17, tup) == false);

       // TODO: RE
    }

   SECTION("plain expressions with ptime") {
        qr_tuple tup;
        boost::posix_time::ptime t1(boost::posix_time::time_from_string("2023-01-20 23:59:59.000"));
        boost::posix_time::ptime t2(boost::posix_time::time_from_string("2023-01-20 23:59:59.000"));
        boost::posix_time::ptime t3(boost::posix_time::time_from_string("2023-09-20 23:59:59.000"));

        auto ex1 = EQ(Time(t1), Time(t2));
        REQUIRE(interpret_bool_expression(qctx, ex1, tup) == true);
        REQUIRE(compile_expression(qctx, ex1, tup) == true);

        auto ex2 = EQ(Time(t1), Time(t3));
        REQUIRE(interpret_bool_expression(qctx, ex2, tup) == false);
        REQUIRE(compile_expression(qctx, ex2, tup) == false);

        auto ex3 = LT(Time(t1), Time(t2));
        REQUIRE(interpret_bool_expression(qctx, ex3, tup) == false);
        REQUIRE(compile_expression(qctx, ex3, tup) == false);

        auto ex4 = LT(Time(t1), Time(t3));
        REQUIRE(interpret_bool_expression(qctx, ex4, tup) == true);
        REQUIRE(compile_expression(qctx, ex4, tup) == true);

        auto ex5 = LE(Time(t1), Time(t3));
        REQUIRE(interpret_bool_expression(qctx, ex5, tup) == true);
        REQUIRE(compile_expression(qctx, ex5, tup) == true);

        auto ex6 = LE(Time(t1), Time(t3));
        REQUIRE(interpret_bool_expression(qctx, ex6, tup) == true);
        REQUIRE(compile_expression(qctx, ex6, tup) == true);

        auto ex7 = LE(Time(t3), Time(t1));
        REQUIRE(interpret_bool_expression(qctx, ex7, tup) == false);
        REQUIRE(compile_expression(qctx, ex7, tup) == false);

        auto ex8 = GT(Time(t3), Time(t2));
        REQUIRE(interpret_bool_expression(qctx, ex8, tup) == true);
        REQUIRE(compile_expression(qctx, ex8, tup) == true);

        auto ex9 = GT(Time(t1), Time(t2));
        REQUIRE(interpret_bool_expression(qctx, ex9, tup) == false);
        REQUIRE(compile_expression(qctx, ex9, tup) == false);

        auto ex10 = GE(Time(t1), Time(t2));
        REQUIRE(interpret_bool_expression(qctx, ex10, tup) == true);
        REQUIRE(compile_expression(qctx, ex10, tup) == true);

        auto ex11 = GE(Time(t1), Time(t2));
        REQUIRE(interpret_bool_expression(qctx, ex11, tup) == true);
        REQUIRE(compile_expression(qctx, ex11, tup) == true);

        auto ex12 = GE(Time(t1), Time(t3));
        REQUIRE(interpret_bool_expression(qctx, ex12, tup) == false);
        REQUIRE(compile_expression(qctx, ex12, tup) == false);

        auto ex13 = NEQ(Time(t1), Time(t2));
        REQUIRE(interpret_bool_expression(qctx, ex13, tup) == false);
        REQUIRE(compile_expression(qctx, ex13, tup) == false);

        auto ex14 = NEQ(Time(t1), Time(t3));
        REQUIRE(interpret_bool_expression(qctx, ex14, tup) == true);
        REQUIRE(compile_expression(qctx, ex14, tup) == true);
    }

    SECTION("expressions with functions") {
        qr_tuple tup;
        std::string ts("2002-01-20 23:59:59.000");
        boost::posix_time::ptime t(boost::posix_time::time_from_string(ts));
        prepare_expr_visitor vis(qctx, nullptr);

        auto ex1 = EQ(Fct("pb", "to_datetime", std::vector<expr>{ Str(ts)}), Time(t));
        ex1->accept(vis);
        REQUIRE(interpret_bool_expression(qctx, ex1, tup) == true);

        auto ts2 = boost::posix_time::to_iso_extended_string(t);
        auto ex2 = EQ(Fct("pb", "ptime_to_dtimestring", std::vector<expr>{ Time(t)}), Str(ts2));
        ex2->accept(vis);
        REQUIRE(interpret_bool_expression(qctx, ex2, tup) == true);
    }
    
    SECTION("expressions with variables") {
        qr_tuple tup(1);
        prepare_expr_visitor vis(qctx, nullptr);

        graph->run_transaction([&]() {
            auto& n = graph->node_by_id(n1);
            tup[0] = &n;
   
            auto ex1 = EQ(Fct("pb", "label", std::vector<expr>{ Variable(0, expr_type::NODE) }), Str("Person"));
            ex1->accept(vis);
            REQUIRE(interpret_bool_expression(qctx, ex1, tup) == true);

            auto ex2 = EQ(Variable(0, "id", id_code, expr_type::INT), Int(42));
            ex2->accept(vis);
            REQUIRE(interpret_bool_expression(qctx, ex2, tup) == true);
            REQUIRE(compile_expression(qctx, ex2, tup) == true);

            auto ex3 = EQ(Variable(0, "id", id_code, expr_type::INT), Int(4));
            ex3->accept(vis);
            REQUIRE(interpret_bool_expression(qctx, ex3, tup) == false);
            REQUIRE(compile_expression(qctx, ex3, tup) == false);

            auto ex4 = LT(Variable(0, "id", id_code, expr_type::INT), Int(100));
            ex4->accept(vis);
            REQUIRE(interpret_bool_expression(qctx, ex4, tup) == true);
            REQUIRE(compile_expression(qctx, ex4, tup) == true);

            auto ex5 = GT(Variable(0, "salary", salary_code, expr_type::DOUBLE), Float(2500.0));
            ex5->accept(vis);
            REQUIRE(interpret_bool_expression(qctx, ex5, tup) == true);
            REQUIRE(compile_expression(qctx, ex5, tup) == true);

            auto ex6 = GT(Variable(0, "salary", salary_code, expr_type::DOUBLE), Float(10000.0));
            ex6->accept(vis);
            REQUIRE(interpret_bool_expression(qctx, ex6, tup) == false);
            REQUIRE(compile_expression(qctx, ex6, tup) == false);

            auto ex7 = EQ(Variable(0, "lastName", lastName_code, expr_type::STRING), Str("Cobb"));
            ex7->accept(vis);
            REQUIRE(interpret_bool_expression(qctx, ex7, tup) == true);
            REQUIRE(compile_expression(qctx, ex7, tup) == true);

            tup[0] = 42;
            auto ex8 = EQ(Variable(0, expr_type::INT), Int(42));
            ex8->accept(vis);
            REQUIRE(interpret_bool_expression(qctx, ex8, tup) == true);
            REQUIRE(compile_expression(qctx, ex8, tup) == true);

            return true;
        });
    }

    SECTION("plain expressions with logical ops") {
        qr_tuple tup;
        auto ex1 = AND(EQ(Int(42), Int(42)), EQ(Float(33.0), Float(33.0)));
        REQUIRE(interpret_bool_expression(qctx, ex1, tup) == true);
        REQUIRE(compile_expression(qctx, ex1, tup) == true);

        auto ex2 = AND(EQ(Int(42), Int(42)), EQ(Float(33.0), Float(88.0)));
        REQUIRE(interpret_bool_expression(qctx, ex2, tup) == false);
        REQUIRE(compile_expression(qctx, ex2, tup) == false);

        auto ex3 = OR(EQ(Int(42), Int(42)), EQ(Float(33.0), Float(88.0)));
        REQUIRE(interpret_bool_expression(qctx, ex3, tup) == true);
        REQUIRE(compile_expression(qctx, ex3, tup) == true);

        auto ex4 = OR(EQ(Float(33.0), Float(88.0)), EQ(Int(42), Int(42)));
        REQUIRE(interpret_bool_expression(qctx, ex4, tup) == true);
        REQUIRE(compile_expression(qctx, ex4, tup) == true);

        auto ex5 = OR(EQ(Float(33.0), Float(88.0)), EQ(Int(40), Int(42)));
        REQUIRE(interpret_bool_expression(qctx, ex5, tup) == false);
        REQUIRE(compile_expression(qctx, ex5, tup) == false);
    }

    SECTION("arithmetic expressions") {
        qr_tuple tup;
        auto ex1 = PLUS(Int(2), Int(5));
        auto res1 = interpret_expression(qctx, ex1, tup);
        REQUIRE(qv_get_int(res1) == 7);

        auto ex2 = MINUS(Int(20), Int(5));
        auto res2 = interpret_expression(qctx, ex2, tup);
        REQUIRE(qv_get_int(res2) == 15);
 
        auto ex3 = MULT(Int(3), Int(5));
        auto res3 = interpret_expression(qctx, ex3, tup);
        REQUIRE(qv_get_int(res3) == 15);
 
        auto ex4 = DIV(Int(12), Int(6));
        auto res4 = interpret_expression(qctx, ex4, tup);
        REQUIRE(qv_get_int(res4) == 2);
 
        auto ex5 = MOD(Int(13), Int(6));
        auto res5 = interpret_expression(qctx, ex5, tup);
        REQUIRE(qv_get_int(res5) == 1);
 
    }

    gen.reset();
    jit.reset();
    graph_pool::destroy(pool);
}
