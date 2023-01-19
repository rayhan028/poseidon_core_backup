/*
 * Copyright (C) 2019-2022 DBIS Group - TU Ilmenau, All Rights Reserved.
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

#include "qproc.hpp"
#include "qop.hpp"
#include "join.hpp"
#include "func_call_expr.hpp"

qresult_iterator qproc::execute_query(qproc::mode m, const std::string& qstr, bool print_plan) {
    auto qplan = prepare_query(qstr, print_plan);
    result_set result;
    qplan.append_collect(result);
    prepare_plan(qplan);

    if (m == Interpret) {
        //if (print_plan)
        //    qplan.print_plan();
        interp_query(qplan);
        if (print_plan)
            qplan.print_plan();
    }
    else {
        // TODO: compile & execute query
        compile_query(qplan);
        if (print_plan)
            qplan.print_plan();
    }
    return qresult_iterator(std::move(result));
}

std::size_t qproc::execute_and_output_query(mode m, const std::string& qstr, bool print_plan) {
  auto qplan = prepare_query(qstr, print_plan);
    qplan.append_printer(); // TODO: if we print then we should return at least the number of tuples via result
    prepare_plan(qplan);

    if (m == Interpret) {
        interp_query(qplan);
        if (print_plan)
            qplan.print_plan();
    }
    else {
        // TODO: compile & execute query
        compile_query(qplan);
        if (print_plan)
            qplan.print_plan();
    }
    return 0; // qplan.result_size();    
}

query_set qproc::prepare_query(const std::string& qstr, bool print_plan) {
    auto op_tree = parser_.parse(qstr);
    if (print_plan)
        print_ast(op_tree);
    return planner_.transform(ctx_, op_tree);
}
    
void qproc::interp_query(query_set& plan) {
    interp_.execute(ctx_, plan);
}

void qproc::compile_query(query_set& plan) {
    compiler_.execute(plan);    
}

bool qproc::load_library(const std::string& lib_path) {
    udf_lib_ = std::make_shared<boost::dll::shared_library>(lib_path);
    if (udf_lib_->is_loaded()) {
        planner_.add_udf_library(udf_lib_);
        return true;
    }
    return false;
}
 
class prepare_expr_visitor : public expression_visitor {
public:
    prepare_expr_visitor(query_ctx& ctx, std::shared_ptr<boost::dll::shared_library> udf_lib) : 
        ctx_(ctx), udf_lib_(udf_lib) {}
    ~prepare_expr_visitor() = default;

    void visit(int rank, std::shared_ptr<func_call> op) override {
        auto func_name = op->func_name_.substr(5);
        // std::cout << "prepare func_call: " << func_name << " : " << op->param_list_.size() << std::endl;     
        if (op->param_list_.size() == 1) {          
            op->func1_ptr_ = udf_lib_->get<query_result(query_ctx&, query_result&)>(func_name);
        } 
        else if (op->param_list_.size() == 2) {
            op->func2_ptr_ = udf_lib_->get<query_result(query_ctx&, query_result&, query_result&)>(func_name);
        }
    }
private:
    query_ctx& ctx_;
    std::shared_ptr<boost::dll::shared_library> udf_lib_;
};

class prepare_plan_visitor : public qop_visitor {
public:
    prepare_plan_visitor(query_ctx& ctx, std::shared_ptr<boost::dll::shared_library> udf_lib) : 
        expr_visitor_(ctx, udf_lib) {}

    ~prepare_plan_visitor() = default;

    void visit(std::shared_ptr<filter_tuple> op) override {
        auto ex = op->get_expression();
        if (ex) {
            ex->accept(0, expr_visitor_);
        }
    }

    void visit(std::shared_ptr<left_outerjoin> op) override { 
        auto ex = op->get_expression();
        if (ex) {
            ex->accept(0, expr_visitor_);
        }
    }

   private:
        prepare_expr_visitor expr_visitor_;
};

void qproc::prepare_plan(query_set& qplan) {
    prepare_plan_visitor visitor(ctx_, udf_lib_);
    qplan.accept(visitor);
}
