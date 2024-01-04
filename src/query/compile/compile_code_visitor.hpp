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

#ifndef compile_code_visitor_hpp_
#define compile_code_visitor_hpp_

#include "query_ctx.hpp"
#include "expression.hpp"
#include "qop.hpp"

#include "jit_engine.hpp"
#include "ir_generator.hpp"

class compile_code_visitor : public qop_visitor {
public:
    compile_code_visitor(query_ctx& ctx, std::unique_ptr<jit_engine>& jit, std::unique_ptr<ir_generator>& gen) : 
        f_cnt_(1), jit_(jit), gen_(gen) {}

    ~compile_code_visitor() = default;

    void visit(std::shared_ptr<filter_op> op) override;
    void visit(std::shared_ptr<aggregate> op) override;

private:
    uint32_t f_cnt_;
    std::unique_ptr<jit_engine>& jit_;
    std::unique_ptr<ir_generator>& gen_;
};

#endif 