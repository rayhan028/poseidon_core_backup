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

#include "binary_expression.hpp"

void* math_expression::accept(expression_visitor &fep) {
    return fep.visit(shared_from_this());
}

std::string math_expression::dump() const {
    auto lhs = (*left_).dump();
    auto rhs = (*right_).dump();
    auto op = op_as_string(fop_);
    return lhs + op + rhs ;
}

binary_predicate::binary_predicate(expr_op fop, const expr left, const expr right) : 
    binary_expression(fop, left, right) {}

std::string binary_predicate::dump() const {
    auto lhs = (*left_).dump();
    auto rhs = (*right_).dump();
    auto op = op_as_string(fop_);

    return lhs + op + rhs ;
}

eq_predicate::eq_predicate(const expr left, const expr right)
        : binary_predicate(expr_op::EQ, left, right) {
    ftype_ = expr_type::OP;
    rtype_ = expr_type::BOOLEAN;
}

void* eq_predicate::accept(expression_visitor &fep) {
    return fep.visit(shared_from_this());
}

neq_predicate::neq_predicate(expr const left, expr const right) 
        : binary_predicate(expr_op::EQ, left, right) {
    ftype_ = expr_type::OP;
    rtype_ = expr_type::BOOLEAN;
}

void* neq_predicate::accept(expression_visitor &fep) {
    return fep.visit(shared_from_this());
}

gt_predicate::gt_predicate(const expr left, const expr right)
        : binary_predicate(expr_op::GT, left, right) {
    ftype_ = expr_type::OP;
    rtype_ = expr_type::BOOLEAN;
}

void* gt_predicate::accept(expression_visitor &fep) {
    return fep.visit(shared_from_this());
}

ge_predicate::ge_predicate(const expr left, const expr right)
        : binary_predicate(expr_op::GE, left, right) {
    ftype_ = expr_type::OP;
    rtype_ = expr_type::BOOLEAN;
}

void* ge_predicate::accept(expression_visitor &fep) {
    return fep.visit(shared_from_this());
}

lt_predicate::lt_predicate(const expr left, const expr right)
        : binary_predicate(expr_op::LT, left, right) {
    ftype_ = expr_type::OP;
    rtype_ = expr_type::BOOLEAN;
}

void* lt_predicate::accept(expression_visitor &fep) {
    return fep.visit(shared_from_this());
}

le_predicate::le_predicate(const expr left, const expr right)
        : binary_predicate(expr_op::LE, left, right) {
    ftype_ = expr_type::OP;
    rtype_ = expr_type::BOOLEAN;
}

void* le_predicate::accept(expression_visitor &fep) {
    return fep.visit(shared_from_this());
}

and_predicate::and_predicate(const expr left, const expr right)
        : binary_predicate(expr_op::AND, left, right) {
    ftype_ = expr_type::OP;
    rtype_ = expr_type::BOOLEAN;
}

void* and_predicate::accept(expression_visitor &fep) {
    return fep.visit(shared_from_this());
}

or_predicate::or_predicate(const expr left, const expr right)
        : binary_predicate(expr_op::OR, left, right) {
    ftype_ = expr_type::OP;
    rtype_ = expr_type::BOOLEAN;
}

void* or_predicate::accept(expression_visitor &fep) {
    return fep.visit(shared_from_this());
}

regex_predicate::regex_predicate(const expr left, const expr right)
        : binary_predicate(expr_op::REGEX, left, right) {
    ftype_ = expr_type::OP;
    rtype_ = expr_type::BOOLEAN;
    auto s = dynamic_cast<string_literal *>(right.get());
    re_ = std::regex(s->str_);
}

void* regex_predicate::accept(expression_visitor &fep) {
    return fep.visit(shared_from_this());
}