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
#include "prepare_expr_visitor.hpp"
#include "expression.hpp"
#include "binary_expression.hpp"
#include "qop.hpp"
#include "qop_joins.hpp"
#include "qop_builtins.hpp"

void* prepare_expr_visitor::visit(std::shared_ptr<func_call> op) {
    // std::cout << "prepare func_call: " << op->func_prefix_ << ":" << op->func_name_ << " : " << op->param_list_.size() << std::endl;     
    if (op->func_prefix_ == "pb") {
        if (op->func_name_ == "label") {
            op->func1_ptr_ = [](query_ctx& ctx, const query_result& v) { return builtin::get_label(ctx, v); };
        }
        else if (op->func_name_ == "to_datetime") {
           op->func1_ptr_ = [](query_ctx& ctx, const query_result& v) { return builtin::dtimestring_to_ptime(qv_get_string(v)); };
        }
        else if (op->func_name_ == "ptime_to_dtimestring") {
           op->func1_ptr_ = [](query_ctx& ctx, const query_result& v) { return builtin::ptime_to_dtimestring(qv_get_ptime(v)); };
        }
        else
            throw udf_not_found();
        return nullptr;
    }
    
    if (op->param_list_.size() == 1) {          
        op->func1_ptr_ = udf_lib_->get<query_result(query_ctx&, query_result&)>(op->func_name_);
        
    } 
    else if (op->param_list_.size() == 2) {
        op->func2_ptr_ = udf_lib_->get<query_result(query_ctx&, query_result&, query_result&)>(op->func_name_);
    }
    return nullptr;
}
