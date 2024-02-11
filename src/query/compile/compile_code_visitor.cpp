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

#include "compile_code_visitor.hpp"

void compile_code_visitor::visit(std::shared_ptr<filter_op> op) {
    // TODO: generate unique name by including query id
    // std::cout << op->ex_->dump() << std::endl;
    auto fname = fmt::format("filter_{}", f_cnt_++);
    auto module = gen_->generate(op->ex_, fname);
    // gen_->dump(module);
    jit_->add_module(std::move(module));
    op->pred_func_ = jit_->get_predicate_function(fname);  
}

void compile_code_visitor::visit(std::shared_ptr<aggregate> op) {
    auto fname = fmt::format("aggr_{}", f_cnt_++);
    auto module = gen_->generate(op, fname);
    // gen_->dump(module);
    jit_->add_module(std::move(module));
    auto [fun1, fun2, fun3] = jit_->get_aggregate_functions(fname);
    op->init_func_ = fun1;
    op->iterate_func_ = fun2;
    op->finish_func_ = fun3;
    op->init_buffer();
}
