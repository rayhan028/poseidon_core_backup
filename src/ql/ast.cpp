#include "ast.hpp"
#include <boost/hana.hpp>

std::ostream& operator<<(std::ostream& os, ast_op& op) {
    switch(op.op_) {
        case ast_op::node_scan:
            os << "NodeScan("; break;
        case ast_op::filter:
            os << "Filter("; break;
        case ast_op::foreach_rship:
            os << "ForeachRelationship("; break;
        case ast_op::expand:
            os << "Expand("; break;
        case ast_op::project:
            os << "Project("; break;
        case ast_op::limit:
            os << "Limit("; break;
        case ast_op::join:
            os << "Join("; break;
    }
    auto my_visitor = boost::hana::overload(
      [&](int i) { os << i; },
      [&](const std::string &s) { os << s; },
      [&](const parse_tree_ptr& expr) { os << expr->string(); });

   for (auto& p : op.params_) {
        boost::apply_visitor(my_visitor, p);
        std::cout << " ";
    }
    os << ")";
    return os;
}

void _print_ast(ast_op_ptr root, const std::string& prefix) {
    if (!root) return;

    bool hasFirst = (root->children_.size() >= 1);
    bool hasSecond = (root->children_.size() == 2);

    if (!hasFirst && !hasSecond)
        return;

    std::cout << prefix;
    std::cout << ((hasFirst  && hasSecond) ? "├── " : "");
    std::cout << ((!hasFirst && hasSecond) ? "└── " : "");

    if (hasSecond) {
        auto& rchild = root->children_[1];
        bool printStrand = (hasFirst && hasSecond && (rchild->children_.size() > 0));
        std::string newPrefix = prefix + (printStrand ? "│   " : "    ");
        std::cout << *rchild << std::endl;
        _print_ast(rchild, newPrefix);
    }

    if (hasFirst) {
        std::cout << (hasSecond ? prefix : "") << "└── " << *(root->children_[0]) << std::endl;
        _print_ast(root->children_[0], prefix + "    ");
    }
}


void print_ast(ast_op_ptr root) {
    std::cout << *root << std::endl;
    _print_ast(root, "");
    std::cout << std::endl;
}
