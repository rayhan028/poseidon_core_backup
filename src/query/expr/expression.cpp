/*
 * Copyright (C) 2019-2024 DBIS Group - TU Ilmenau, All Rights Reserved.
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

#include "expression.hpp"
#include "binary_expression.hpp"
#include "qresult.hpp"

void* expression_visitor::visit(std::shared_ptr<math_expression> op) { 
    if (op->left_) op->left_->accept(*this);
    if (op->right_) op->right_->accept(*this);
    return nullptr; 
} 

void* expression_visitor::visit(std::shared_ptr<eq_predicate> op) { 
    if (op->left_) op->left_->accept(*this);
    if (op->right_) op->right_->accept(*this);
    return nullptr; 
} 

void* expression_visitor::visit(std::shared_ptr<neq_predicate> op) { 
    if (op->left_) op->left_->accept(*this);
    if (op->right_) op->right_->accept(*this);
    return nullptr; 
} 

void* expression_visitor::visit(std::shared_ptr<regex_predicate> op) { 
    if (op->left_) op->left_->accept(*this);
    if (op->right_) op->right_->accept(*this);
    return nullptr; 
} 
void* expression_visitor::visit(std::shared_ptr<le_predicate> op) { 
    if (op->left_) op->left_->accept(*this);
    if (op->right_) op->right_->accept(*this);
    return nullptr; 
} 

void* expression_visitor::visit(std::shared_ptr<lt_predicate> op) {     
    if (op->left_) op->left_->accept(*this);
    if (op->right_) op->right_->accept(*this);
    return nullptr; 
} 

void* expression_visitor::visit(std::shared_ptr<ge_predicate> op) { 
    if (op->left_) op->left_->accept(*this);
    if (op->right_) op->right_->accept(*this);
    return nullptr; 
} 
void* expression_visitor::visit(std::shared_ptr<gt_predicate> op) { 
    if (op->left_) op->left_->accept(*this);
    if (op->right_) op->right_->accept(*this);
    return nullptr; 
} 

void* expression_visitor::visit(std::shared_ptr<and_predicate> op) {
    if (op->left_) op->left_->accept(*this);
    if (op->right_) op->right_->accept(*this);
    return nullptr; 
} 
void* expression_visitor::visit(std::shared_ptr<or_predicate> op) {
    if (op->left_) op->left_->accept(*this);
    if (op->right_) op->right_->accept(*this);
    return nullptr; 
} 

std::string expression::op_as_string(expr_op fop) const {
    switch (fop) {
        case expr_op::EQ:
            return " == ";
        case expr_op::NEQ:
            return " != ";
        case expr_op::LE:
            return " <= ";
        case expr_op::LT:
            return " < ";
        case expr_op::GE:
            return " >= ";
        case expr_op::GT:
            return " > ";
        case expr_op::AND:
            return " && ";
        case expr_op::OR:
            return " || ";
        case expr_op::NOT:
            return " ! ";
        case expr_op::REGEX:
            return " =~ ";
        default:
            return " ?? ";
    }
}

number_literal::number_literal(int value) : ivalue_(value) {
    rtype_ = ftype_ = expr_type::INT;
}

number_literal::number_literal(uint64_t value) : lvalue_(value) {
    rtype_ = ftype_ = expr_type::UINT64;
}

number_literal::number_literal(double value) : dvalue_(value) {
    rtype_ = ftype_ = expr_type::DOUBLE;
}

std::string number_literal::dump() const {
    switch (ftype_) {
        case expr_type::INT:
            return std::to_string(ivalue_);
        case expr_type::UINT64:
            return std::to_string(lvalue_);
        case expr_type::DOUBLE:
            return std::to_string(dvalue_);
        default:
            return "UNKNOWN";
    }
}

void* number_literal::accept(expression_visitor &fep) {
    return fep.visit(shared_from_this());
}

variable::variable(unsigned int id, expr_type ty) : id_(id), pname_(""), pcode_(UNKNOWN_CODE) {
    ftype_ = rtype_ = ty; 
    fop_ = expr_op::VARIABLE;
}

variable::variable(unsigned int id, const std::string& p, expr_type ty) : id_(id), pname_(p), pcode_(UNKNOWN_CODE) {
    ftype_ = rtype_ = ty; 
    fop_ = expr_op::VARIABLE;
}

variable::variable(unsigned int id, const std::string& p, dcode_t pc, expr_type ty) : id_(id), pname_(p), pcode_(pc) {
    ftype_ = rtype_ = ty; 
    fop_ = expr_op::VARIABLE;
}

std::string variable::dump() const {
    auto suffix = pname_.empty() ? "" : (std::string(".") + pname_);
    return std::string("$") + std::to_string(id_) + suffix;
}

void* variable::accept(expression_visitor& fep) {
    return fep.visit(shared_from_this());
}

time_literal::time_literal(boost::posix_time::ptime time) : time_(time) {
    rtype_ = ftype_ = expr_type::DATETIME;
}

void* time_literal::accept(expression_visitor &fep) {
    return fep.visit(shared_from_this());
}

std::string time_literal::dump() const {
    return boost::posix_time::to_simple_string(time_);
}

string_literal::string_literal(std::string str) : str_(str) {
    rtype_ = ftype_ = expr_type::STRING;
}

std::string string_literal::dump() const {
    return str_;
}

void* string_literal::accept(expression_visitor &fep) {
    return fep.visit(shared_from_this());
}

qparam_token::qparam_token(std::string str) : str_(str) {
    ftype_ = expr_type::STRING; // TODO
}

std::string qparam_token::dump() const {
    return str_;
}

void* qparam_token::accept(expression_visitor &fep) {
    // TODO - should not happen, but replaced before
    return nullptr;
}

std::string func_call::dump() const {
    std::string params;
    for (auto i = 0u; i < param_list_.size(); i++) {
        params += param_list_[i]->dump();
        if (i < param_list_.size()-1)
            params += ", ";
    }
    return func_name_ + "(" + params + ")";
}

void* func_call::accept(expression_visitor &fep) {
    for (auto& p : param_list_)    
        p->accept(fep);
    return fep.visit(shared_from_this());
}

bool func_call::is_constant() const {
    if (!is_deterministic_)
        return false;
    
    bool res = true;
    for (auto& p : param_list_) {
        res &= p->fop_ == expr_op::LITERAL; 
    }
    return res;
}

class literal_visitor : public expression_visitor {
public:
    literal_visitor() = default;

    void* visit(std::shared_ptr<number_literal> op) override { 
        if (op->ftype_ == expr_type::INT)
            res_= qv_(op->ivalue_);
        else if (op->ftype_ == expr_type::UINT64)
            res_ = qv_(op->lvalue_);
        else
            res_ = qv_(op->dvalue_); 
            return nullptr; 
    }
    void* visit(std::shared_ptr<string_literal> op) override { res_ = qv_(op->str_); return nullptr; }
    void* visit(std::shared_ptr<time_literal> op) override { res_ = qv_(op->time_); return nullptr; }

    query_result res_;
};

query_result literal_expr_to_qresult(expr ex) {
    literal_visitor vis;
    ex->accept(vis);
    return vis.res_;
}

expr qresult_to_expr(const query_result& v) {
    if (v.which() == int_type)
        return Int(qv_get_int(v));
    else if (v.which() == double_type)
        return Float(qv_get_double(v));
    else if (v.which() == uint64_type)
        return Float(qv_get_uint64(v));
    else if (v.which() == ptime_type)
        return Time(qv_get_ptime(v));
    else if (v.which() == string_type) 
        return Str(qv_get_string(v));
    else
        return expr();
}

expr func_call::replace_by_literal(query_ctx& ctx) {
    auto arg1 = literal_expr_to_qresult(param_list_[0]);
    if (func1_ptr_ != nullptr) {
        auto res = func1_ptr_(ctx, arg1);
        return qresult_to_expr(res);
    }
    else if (func2_ptr_ != nullptr) {
        auto arg2 = literal_expr_to_qresult(param_list_[1]);
        auto res = func2_ptr_(ctx, arg1, arg2);  
        return qresult_to_expr(res);
    }
    else {
        spdlog::info("ERROR: Invalid function pointer in function '{}'", func_name_);
    }
}
