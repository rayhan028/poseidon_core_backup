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
    // TODO   
    return nullptr; 
}

std::string math_expression::dump() const {
    auto lhs = (*left_).dump();
    auto rhs = (*right_).dump();
    auto op = op_as_string(fop_);
    return lhs + op + rhs ;
}

binary_predicate::binary_predicate(expr_op fop, const expr left, const expr right, bool prec, bool is_bool) : 
    binary_expression(fop, left, right), is_bool_(is_bool), prec_(prec) {}

std::string binary_predicate::dump() const {
    auto lhs = (*left_).dump();
    auto rhs = (*right_).dump();
    auto op = op_as_string(fop_);
    auto pro = prec_ ? "(" : "";
    auto epi = prec_ ? ")" : "";
    return pro + lhs + op + rhs + epi;
}

eq_predicate::eq_predicate(const expr left, const expr right, bool prec, bool not_)
        : binary_predicate(expr_op::EQ, left, right, prec) {
    name_ = "EQ";
    ftype_ = expr_type::OP;
    rtype_ = expr_type::BOOL_OP;
    if (not_) fop_ = expr_op::NEQ;
}

void* eq_predicate::accept(expression_visitor &fep) {
    //left_->accept(rank+1, fep);
    //right_->accept(rank+1, fep);
    return fep.visit(shared_from_this());
    // TODO: do binary stuff here
}

gt_predicate::gt_predicate(const expr left, const expr right, bool prec, bool not_)
        : binary_predicate(expr_op::GT, left, right, prec) {
    name_ = "GT";
    ftype_ = expr_type::OP;
    rtype_ = expr_type::BOOL_OP;
}

void* gt_predicate::accept(expression_visitor &fep) {
    //left_->accept(rank+1, fep);
    //right_->accept(rank+1, fep);
    return fep.visit(shared_from_this());
    // TODO: do binary stuff here
}

ge_predicate::ge_predicate(const expr left, const expr right, bool prec, bool not_)
        : binary_predicate(expr_op::GE, left, right, prec) {
    name_ = "GE";
    ftype_ = expr_type::OP;
    rtype_ = expr_type::BOOL_OP;
}

void* ge_predicate::accept(expression_visitor &fep) {
    //left_->accept(rank+1, fep);
    //right_->accept(rank+1, fep);
    return fep.visit(shared_from_this());
    // TODO: do binary stuff here
}

lt_predicate::lt_predicate(const expr left, const expr right, bool prec, bool not_)
        : binary_predicate(expr_op::LT, left, right, prec) {
    name_ = "LT";
    ftype_ = expr_type::OP;
    rtype_ = expr_type::BOOL_OP;
}

void* lt_predicate::accept(expression_visitor &fep) {
    //left_->accept(rank+1, fep);
    //right_->accept(rank+1, fep);
    return fep.visit(shared_from_this());
    // TODO: do binary stuff here
}

le_predicate::le_predicate(const expr left, const expr right, bool prec, bool not_)
        : binary_predicate(expr_op::LE, left, right, prec) {
    name_ = "LE";
    ftype_ = expr_type::OP;
    rtype_ = expr_type::BOOL_OP;
}

void* le_predicate::accept(expression_visitor &fep) {
    //left_->accept(rank+1, fep);
    //right_->accept(rank+1, fep);
    return fep.visit(shared_from_this());
    // TODO: do binary stuff here
}

call_predicate::call_predicate(const expr left, const expr right, bool prec, bool not_)
        : binary_predicate(expr_op::EQ, left, right, prec) {
    name_ = "CALL";
    ftype_ = expr_type::OP;
    if (not_) fop_ = expr_op::NEQ;
}

void* call_predicate::accept(expression_visitor &fep) {
    //left_->accept(rank+1, fep);
    //right_->accept(rank+1, fep);
    return fep.visit(shared_from_this());
    // TODO: do binary stuff here
}

and_predicate::and_predicate(const expr left, const expr right, bool prec)
        : binary_predicate(expr_op::AND, left, right, prec, true) {
    rtype_ = ftype_ = expr_type::BOOL_OP;
    name_ = "AND";
}

void* and_predicate::accept(expression_visitor &fep) {
    //left_->accept(rank+1, fep);
    //right_->accept(rank+1, fep);
    return fep.visit(shared_from_this());
    // TODO: do binary stuff here
}

or_predicate::or_predicate(const expr left, const expr right, bool prec)
        : binary_predicate(expr_op::OR, left, right, prec, true) {
    rtype_ = ftype_ = expr_type::BOOL_OP;
    name_ = "OR";
}

void* or_predicate::accept(expression_visitor &fep) {
    //left_->accept(rank+1, fep);
    //right_->accept(rank+1, fep);
    return fep.visit(shared_from_this());
    // TODO: do binary stuff here
}