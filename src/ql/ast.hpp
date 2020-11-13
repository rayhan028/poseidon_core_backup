/*
 * Copyright (C) 2019-2020 DBIS Group - TU Ilmenau, All Rights Reserved.
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

#ifndef ast_hpp_
#define ast_hpp_

#include <memory>
#include <string>
#include <vector>
#include <iostream>

#include <boost/variant.hpp>
#include <tao/pegtl/contrib/parse_tree.hpp>

#include "qlang_grammar.hpp"

struct ast_op;

using ast_op_ptr = std::shared_ptr<ast_op>;
using parse_tree_ptr = std::unique_ptr<tao::pegtl::parse_tree::node>;

/**
 * prop_spec represents a property specification used in a projection clause.
 */
struct prop_spec {
    std::string pname; // property name in the form of $1.name
    std::string ptype; // property typename
};

using prop_spec_list = std::vector<prop_spec>;

/**
 * ast_op is used for representing query operators in the AST.
 */
struct ast_op {
    enum op_type { unknown, node_scan, filter, foreach_rship, expand, project, limit, join };
    using param_type = boost::variant<int, std::string, parse_tree_ptr, prop_spec_list>;

    /**
     * Constructor
     */
    ast_op(op_type ot) : op_(ot) {}

    /**
     * Add a single child node.
     */
    void add_child(ast_op_ptr c) { children_.push_back(c); }

    /**
     * Add two child nodes to the AST operator (used for binary operators only).
     */
    void add_children(ast_op_ptr c1, ast_op_ptr c2) { children_.push_back(c1); children_.push_back(c2); }

    /**
     * Add an integer parameter.
     */
    void add_param(int i) { params_.push_back(i); }

    /**
     * Add a string parameter.
     */
    void add_param(const std::string& s) { params_.push_back(s); }

    /**
     * Add an expression as parameter (used for conditions).
     */
    void add_param(parse_tree_ptr expr) { params_.push_back(std::move(expr)); }

    /**
     * Add a property specification list as parameter (used for projection).
     */
    void add_param(prop_spec_list& plist) { params_.push_back(plist); }

    /**
     * Return true if the AST operator is a source parameter.
     */
    bool is_source() const { return op_ == node_scan; }

    /**
     * Return the parameter at position i.
     */
    template<typename T>
    T get_param(std::size_t i) { return boost::get<T>(params_[i]); }
    parse_tree_ptr get_param(std::size_t i) { return std::move(boost::get<parse_tree_ptr>(params_[i])); }

    op_type op_;                       // operator type
    std::vector<ast_op_ptr> children_; // child nodes
    std::vector<param_type> params_;   // list of parameters of the operator
};

std::ostream& operator<<(std::ostream& os, ast_op& op);

void print_ast(ast_op_ptr root);

#endif