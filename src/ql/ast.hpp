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

struct ast_op {
    enum op_type { unknown, node_scan, filter, foreach_rship, expand, project, limit };
    using param_type = boost::variant<int, std::string, parse_tree_ptr>;

    ast_op(op_type ot) : op_(ot) {}
    void add_child(ast_op_ptr c) { children_.push_back(c); }
    void add_children(ast_op_ptr c1, ast_op_ptr c2) { children_.push_back(c1); children_.push_back(c2); }

    void add_param(int i) { params_.push_back(i); }
    void add_param(const std::string& s) { params_.push_back(s); }
    void add_param(parse_tree_ptr expr) { params_.push_back(std::move(expr)); }

    op_type op_;
    std::vector<ast_op_ptr> children_;
    std::vector<param_type> params_;
};

std::ostream& operator<<(std::ostream& os, ast_op& op);

void print_ast(ast_op_ptr root);

#endif