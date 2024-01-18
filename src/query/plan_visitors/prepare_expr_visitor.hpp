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
        udf_lib_(udf_lib), ctx_(ctx) {}
    ~prepare_expr_visitor() = default;

    void* visit(std::shared_ptr<func_call> op) override; 
    void* visit(std::shared_ptr<eq_predicate> op) override;     
    void* visit(std::shared_ptr<neq_predicate> op) override;  
    void* visit(std::shared_ptr<regex_predicate> op) override;  
    void* visit(std::shared_ptr<le_predicate> op) override;
    void* visit(std::shared_ptr<lt_predicate> op) override;
    void* visit(std::shared_ptr<ge_predicate> op) override;
    void* visit(std::shared_ptr<gt_predicate> op) override;
    void* visit(std::shared_ptr<and_predicate> op) override;
    void* visit(std::shared_ptr<or_predicate> op) override;
    void* visit(std::shared_ptr<math_expression> op) override;
    
private:
    bool is_func_call(expr op) const;
    void* handle_binary_expression(std::shared_ptr<binary_expression> op);

    std::shared_ptr<boost::dll::shared_library> udf_lib_;
    query_ctx& ctx_;
};

#endif