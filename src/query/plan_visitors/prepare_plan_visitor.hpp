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

#ifndef prepare_plan_visitor_hpp_
#define prepare_plan_visitor_hpp_

#include <boost/dll/import.hpp> 

#include "query_ctx.hpp"
#include "expression.hpp"
#include "qop.hpp"
#include "qop_updates.hpp"
#include "prepare_expr_visitor.hpp"

class prepare_plan_visitor : public qop_visitor {
public:
    prepare_plan_visitor(query_ctx& ctx, std::shared_ptr<boost::dll::shared_library> udf_lib) : 
        expr_visitor_(ctx, udf_lib) {}

    ~prepare_plan_visitor() = default;

    void visit(std::shared_ptr<filter_op> op) override;
    void visit(std::shared_ptr<left_outer_join_op> op) override;
    void visit(std::shared_ptr<create_node> op) override;
    void visit(std::shared_ptr<create_relationship> op) override;

private:
    prepare_expr_visitor expr_visitor_;
};

#endif 