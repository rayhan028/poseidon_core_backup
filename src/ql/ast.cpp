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

#include "ast.hpp"
#include <boost/hana.hpp>

std::ostream& operator<<(std::ostream& os, const proj_spec_list& plist) {
    os << "[ ";
    for (auto i = 0u; i < plist.size(); i++) {
        auto& pi = plist[i];
        os << pi.pname;
        if (pi.porder != proj_spec::None)
            os << " " << pi.porder;
        if (i < plist.size()-1) os << ", ";
    }
    os << " ]";
    return os;
}

std::ostream& operator<<(std::ostream& os, const jproperty_list& plist) {
    os << "{ ";
    for (auto i = 0u; i < plist.size(); i++) {
        auto& pi = plist[i];
        os << pi.pname << ": " << pi.pval;
        if (i < plist.size()-1) os << ", ";
    }
    os << " }";
    return os;
}

std::ostream& operator<<(std::ostream& os, const aggr_spec_list& alist) {
    os << "[ ";
    for (auto i = 0u; i < alist.size(); i++) {
        auto& ai = alist[i];
        os << ai.afunc << "(" << ai.aname << "):" << ai.atype;
        if (i < alist.size()-1) os << ", ";
    }
    os << " ]";
    return os;
}

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
        case ast_op::append:
            os << "Append("; break;
        case ast_op::project:
            os << "Project("; break;
        case ast_op::limit:
            os << "Limit("; break;
        case ast_op::sort:
            os << "Sort("; break;
        case ast_op::group_by:
            os << "GroupBy("; break;
        case ast_op::hash_join:
            os << "HashJoin("; break;
        case ast_op::leftouter_join:
            os << "LeftOuterJoin("; break;
        case ast_op::cross_join:
            os << "CrossJoin("; break;
        case ast_op::create_node:
            os << "CreateNode("; break;
        case ast_op::create_rship:
            os << "CreateRelationship("; break;
        case ast_op::end:
            os << "CreateRelationship("; break;
        default:
            break;
    }
    auto my_visitor = boost::hana::overload(
      [&](int i) { os << i; },
      [&](const std::string &s) { os << s; },
      #ifdef USE_LLVM
      [&](const expr& expr) { if (expr) os << expr->dump(); },
      #endif
      [&](const proj_spec_list& plist) { os << plist; },
      [&](const aggr_spec_list& alist) { os << alist; },
      [&](const jproperty_list& plist) { os << plist; });

   for (const auto& p : op.params_) {
        boost::apply_visitor(my_visitor, p);
        os << " ";
    }
    os << ")";
    return os;
}

void _print_ast(std::ostream& os, ast_op_ptr root, const std::string& prefix) {
    if (!root) return;

    bool hasFirst = (root->children_.size() >= 1);
    bool hasSecond = (root->children_.size() == 2);

    if (!hasFirst && !hasSecond)
        return;

    os << prefix;
    os << ((hasFirst  && hasSecond) ? "├── " : "");
    os << ((!hasFirst && hasSecond) ? "└── " : "");

    if (hasSecond) {
        auto& rchild = root->children_[1];
        bool printStrand = (hasFirst && hasSecond && (rchild->children_.size() > 0));
        std::string newPrefix = prefix + (printStrand ? "│   " : "    ");
        os << *rchild << std::endl;
        _print_ast(os, rchild, newPrefix);
    }

    if (hasFirst) {
        os << (hasSecond ? prefix : "") << "└── " << *(root->children_[0]) << std::endl;
        _print_ast(os, root->children_[0], prefix + "    ");
    }
}


void print_ast(ast_op_ptr root) {
    std::cout << *root << std::endl;
    _print_ast(std::cout, root, "");
    std::cout << std::endl;
}

void ast_to_stream(ast_op_ptr root, std::ostream& os) {
    os << *root << std::endl;
    _print_ast(os, root, "");
}
