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
#include "prepare_plan_visitor.hpp"
#include "expression.hpp"
#include "binary_expression.hpp"
#include "func_call_expr.hpp"
#include "qop.hpp"
#include "qop_joins.hpp"

void* prepare_expr_visitor::visit(std::shared_ptr<func_call> op) {
    // std::cout << "prepare func_call: " << op->func_name_ << " : " << op->param_list_.size() << std::endl;     
    if (op->param_list_.size() == 1) {          
        op->func1_ptr_ = udf_lib_->get<query_result(query_ctx&, query_result&)>(op->func_name_);
    } 
    else if (op->param_list_.size() == 2) {
        op->func2_ptr_ = udf_lib_->get<query_result(query_ctx&, query_result&, query_result&)>(op->func_name_);
    }
}

void prepare_plan_visitor::visit(std::shared_ptr<filter_op> op) {
    auto ex = op->get_expression();
    if (ex)
        ex->accept(expr_visitor_);
}

void prepare_plan_visitor::visit(std::shared_ptr<left_outer_join_op> op) { 
    auto ex = op->get_expression();
    if (ex)
        ex->accept(expr_visitor_);
}