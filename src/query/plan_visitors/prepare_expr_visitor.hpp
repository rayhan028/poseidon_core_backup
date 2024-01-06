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

#ifndef prepare_expr_visitor_hpp_
#define prepare_expr_visitor_hpp_

#include <boost/dll/import.hpp> 

#include "query_ctx.hpp"
#include "expression.hpp"
#include "qop.hpp"

class prepare_expr_visitor : public expression_visitor {
public:
    prepare_expr_visitor(query_ctx& ctx, std::shared_ptr<boost::dll::shared_library> udf_lib) : 
        udf_lib_(udf_lib) {}
    ~prepare_expr_visitor() = default;

    void* visit(std::shared_ptr<func_call> op) override; 

private:
    std::shared_ptr<boost::dll::shared_library> udf_lib_;
};

#endif