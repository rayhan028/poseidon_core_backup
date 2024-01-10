
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
#ifndef binary_expression_hpp_
#define binary_expression_hpp_

#include <regex>
#include "expression.hpp"

struct binary_expression : public expression {
    expr_op fop_;
    expr left_;
    expr right_;

    binary_expression(expr_op fop, expr lhs, expr rhs) : fop_(fop), left_(lhs), right_(rhs) {}
};

struct math_expression : public binary_expression, std::enable_shared_from_this<math_expression> {
    math_expression(expr_op fp, expr const left, expr const right) : binary_expression(fp, left, right) {}
    void* accept(expression_visitor &fep) override;
    std::string dump() const override;
};

inline bin_expr PLUS(expr lhs, expr rhs) { return std::make_shared<math_expression>(expr_op::PLUS, lhs, rhs); }
inline bin_expr MINUS(expr lhs, expr rhs) { return std::make_shared<math_expression>(expr_op::MINUS, lhs, rhs); }
inline bin_expr MULT(expr lhs, expr rhs) { return std::make_shared<math_expression>(expr_op::MULT, lhs, rhs); }
inline bin_expr DIV(expr lhs, expr rhs) { return std::make_shared<math_expression>(expr_op::DIV, lhs, rhs); }
inline bin_expr MOD(expr lhs, expr rhs) { return std::make_shared<math_expression>(expr_op::MOD, lhs, rhs); }

struct binary_predicate : public binary_expression {
    binary_predicate(expr_op fop, expr const left = nullptr, expr const right = nullptr);

    std::string dump() const override;
};

struct eq_predicate : public binary_predicate, std::enable_shared_from_this<eq_predicate> {
    eq_predicate(expr const left, expr const right);
    void* accept(expression_visitor &fep) override;
};

struct neq_predicate : public binary_predicate, std::enable_shared_from_this<neq_predicate> {
    neq_predicate(expr const left, expr const right);
    void* accept(expression_visitor &fep) override;
};

struct le_predicate : public binary_predicate, std::enable_shared_from_this<le_predicate> {
    le_predicate(expr const left, expr const mach_port_right_t);
    void* accept(expression_visitor &fep) override;
};

struct lt_predicate : public binary_predicate, std::enable_shared_from_this<lt_predicate> {
    lt_predicate(expr const left, expr const right);
    void* accept(expression_visitor &fep) override;
};

struct ge_predicate : public binary_predicate, std::enable_shared_from_this<ge_predicate> {
    ge_predicate(expr const left, expr const right);

    void* accept(expression_visitor &fep) override;
};

struct gt_predicate : public binary_predicate, std::enable_shared_from_this<gt_predicate> {
    gt_predicate(expr const left, expr const right);

    void* accept(expression_visitor &fep) override;
};

inline bin_expr EQ(expr lhs, expr rhs) { return std::make_shared<eq_predicate>(lhs, rhs); }

inline bin_expr NEQ(expr lhs, expr rhs) { return std::make_shared<neq_predicate>(lhs, rhs); }

inline bin_expr LE(expr lhs, expr rhs) { return std::make_shared<le_predicate>(lhs, rhs); }

inline bin_expr LT(expr lhs, expr rhs) { return std::make_shared<lt_predicate>(lhs, rhs); }

inline bin_expr GE(expr lhs, expr rhs) { return std::make_shared<ge_predicate>(lhs, rhs); }

inline bin_expr GT(expr lhs, expr rhs) { return std::make_shared<gt_predicate>(lhs, rhs); }

struct and_predicate : public binary_predicate, std::enable_shared_from_this<and_predicate> {
    and_predicate(expr const left, expr const right);

    void* accept(expression_visitor &fep) override;
};

inline bin_expr AND(expr lhs, expr rhs) {
    return std::make_shared<and_predicate>(lhs, rhs);
}


struct or_predicate : public binary_predicate, std::enable_shared_from_this<or_predicate> {
    or_predicate(expr const left, expr const right);

    void* accept(expression_visitor &fep) override;
};

inline bin_expr OR(expr lhs, expr rhs) { return std::make_shared<or_predicate>(lhs, rhs); }

struct regex_predicate : public binary_predicate, std::enable_shared_from_this<regex_predicate> {
    regex_predicate(expr const left, expr const right);

    void* accept(expression_visitor &fep) override;

    std::regex re_;
};

inline bin_expr RE(expr lhs, expr rhs) { return std::make_shared<regex_predicate>(lhs, rhs); }


#endif