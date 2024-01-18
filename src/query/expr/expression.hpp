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
#ifndef expression_hpp_
#define expression_hpp_

#include <string>
#include <memory>
#include <iostream>
#include "defs.hpp"
#include "qresult_iterator.hpp"
// #include "filter_visitor.hpp"

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

enum class expr_op {
    UNKNOWN = 0,
    EQ = 1,
    NEQ = 2,
    LE = 3,
    LT = 4,
    GE = 5,
    GT = 6,
    AND = 7,
    OR = 8,
    NOT = 9,
    PLUS = 10,
    MINUS = 11,
    MULT = 12,
    DIV = 13,
    MOD = 14,
    REGEX = 15,
    CALL = 16,
    VARIABLE = 19,
    LITERAL = 20
};

enum class expr_type {
    UNKNOWN = 0,
    INT = 1,
    UINT64 = 2,
    DOUBLE = 3,
    STRING = 4,
    DATETIME = 5,
    BOOLEAN = 6,
    NODE = 7,
    RELATIONSHIP = 8,
    OP = 10
};

struct expression;
struct binary_expression;
struct math_expression;
struct binary_predicate;
struct fep_visitor;
struct number_literal;
struct variable;
struct key_token;
struct string_literal;
struct qparam_token;
struct time_literal;
struct func_call;
struct eq_predicate;
struct neq_predicate;
struct regex_predicate;
struct le_predicate;
struct lt_predicate;
struct ge_predicate;
struct gt_predicate;
struct and_predicate;
struct or_predicate;
using expr = std::shared_ptr<expression>;
using bin_expr = std::shared_ptr<binary_expression>;

struct expression_visitor {
protected:
    expression_visitor() = default;

public:
    virtual ~expression_visitor() = default;

    virtual void* visit(std::shared_ptr<number_literal> op) { return nullptr; }
    virtual void* visit(std::shared_ptr<variable> op) { return nullptr; }
    virtual void* visit(std::shared_ptr<string_literal> op) { return nullptr; }
    virtual void* visit(std::shared_ptr<time_literal> op) { return nullptr; }
    virtual void* visit(std::shared_ptr<func_call> op) { return nullptr; }
    virtual void* visit(std::shared_ptr<eq_predicate> op);   
    virtual void* visit(std::shared_ptr<neq_predicate> op);  
    virtual void* visit(std::shared_ptr<regex_predicate> op);  
    virtual void* visit(std::shared_ptr<le_predicate> op);
    virtual void* visit(std::shared_ptr<lt_predicate> op);
    virtual void* visit(std::shared_ptr<ge_predicate> op);
    virtual void* visit(std::shared_ptr<gt_predicate> op);
    virtual void* visit(std::shared_ptr<and_predicate> op);
    virtual void* visit(std::shared_ptr<or_predicate> op);
    virtual void* visit(std::shared_ptr<math_expression> op);
};

struct expression {
    expr_type ftype_;
    expr_type rtype_; // result type - deduced from the operands and the operator
    expr_op fop_;

    expression() : ftype_(expr_type::UNKNOWN), rtype_(expr_type::UNKNOWN), fop_(expr_op::LITERAL) {}
    virtual ~expression() {};

    virtual std::string dump() const = 0;

    virtual void* accept(expression_visitor& vis) = 0;

    std::string op_as_string(expr_op fop) const;

    expr_type result_type() const { return rtype_; }
};

struct number_literal : public expression, public std::enable_shared_from_this<number_literal> {
    int ivalue_;
    uint64_t lvalue_;
    double dvalue_;

    number_literal(int value = 0);
    number_literal(uint64_t value);
    number_literal(double value);

    std::string dump() const override;

    void* accept(expression_visitor& fep) override;
};

inline expr Int(int value = 0) { return std::make_shared<number_literal>(value); }
inline expr UInt64(uint64_t value) { return std::make_shared<number_literal>(value); }
inline expr Float(double value) { return std::make_shared<number_literal>(value); }

struct variable : public expression, std::enable_shared_from_this<variable> {
    unsigned int id_;
    std::string pname_;
    dcode_t pcode_;

    variable(unsigned int id, expr_type ty);
    variable(unsigned int id, const std::string& p, expr_type t);
    variable(unsigned int id, const std::string& p, dcode_t pc, expr_type ty);

    std::string dump() const override;
    void* accept(expression_visitor& fep) override;
};

inline expr Variable(unsigned int id, expr_type ty) { return std::make_shared<variable>(id, ty); }
inline expr Variable(unsigned int id, const std::string& p, expr_type ty) { return std::make_shared<variable>(id, p, ty); }
inline expr Variable(unsigned int id, const std::string& p, dcode_t pc, expr_type ty) { 
    return std::make_shared<variable>(id, p, pc, ty); 
}

struct string_literal : public expression, std::enable_shared_from_this<string_literal> {
    std::string str_;

    string_literal(std::string str);

    std::string dump() const override;

    void *accept(expression_visitor& fep) override;
};

inline expr Str(std::string value = 0) { return std::make_shared<string_literal>(value); }

struct qparam_token : public expression, std::enable_shared_from_this<qparam_token> {
    std::string str_;

    qparam_token(std::string str);

    std::string dump() const override;

    void* accept(expression_visitor& fep) override;
};

inline expr QParam(std::string value = 0) { return std::make_shared<qparam_token>(value); }

struct time_literal : public expression, std::enable_shared_from_this<time_literal> {
    boost::posix_time::ptime time_;

    time_literal(boost::posix_time::ptime time);

    std::string dump() const override;

    void* accept(expression_visitor& fep) override;
};

inline expr Time(boost::posix_time::ptime time) { return std::make_shared<time_literal>(time); }

struct func_call : public expression, std::enable_shared_from_this<func_call> {
    std::string func_prefix_;
    std::string func_name_;
    std::vector<expr> param_list_;
    std::function<query_result(query_ctx&, query_result&)> func1_ptr_;
    std::function<query_result(query_ctx&, query_result&, query_result&)> func2_ptr_;
    bool is_deterministic_;

    func_call(const std::string& fn, const std::vector<expr>& pl) : 
        func_prefix_(""), func_name_(fn), param_list_(pl), func1_ptr_(nullptr), func2_ptr_(nullptr), is_deterministic_(true) {
            fop_ = expr_op::CALL;
        }

   func_call(const std::string& pfx, const std::string& fn, const std::vector<expr>& pl) : 
        func_prefix_(pfx), func_name_(fn), param_list_(pl), func1_ptr_(nullptr), func2_ptr_(nullptr), is_deterministic_(true) {
            fop_ = expr_op::CALL;
        }

    std::string dump() const override;

    void* accept(expression_visitor &fep) override;

    /**
     * Return true if all args are literals and the function is deterministic. In this
     * case we can replace the function call by its result.
     */
    bool is_constant() const;

    /**
     * Returns a literal expression that replaces the function call.
    */
    expr replace_by_literal(query_ctx& ctx);
};

inline expr Fct(const std::string& fname, const std::vector<expr>& params) { 
    return std::make_shared<func_call>(fname, params); 
}

inline expr Fct(const std::string& pfx, const std::string& fname, const std::vector<expr>& params) { 
    return std::make_shared<func_call>(pfx, fname, params); 
}

#endif 