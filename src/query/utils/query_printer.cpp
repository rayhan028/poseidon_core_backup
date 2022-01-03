/*
 * Copyright (C) 2019-2022 DBIS Group - TU Ilmenau, All Rights Reserved.
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
#include "query.hpp"
#include "query_printer.hpp"

std::pair<qop_node_ptr, qop_node_ptr> build_qop_tree(qop_ptr root) {
    // std::cout << "build_qop_tree: "; root->dump(std::cout); std::cout << std::endl;
    if (!root->has_subscriber()) {
        auto troot = std::make_shared<qop_node>(root);
        return std::make_pair(troot, troot);
    }
    auto np = build_qop_tree(root->subscriber());
    auto rn = std::make_shared<qop_node>(root);
    np.second->children_.push_back(rn);
    return std::make_pair(np.first, rn);
}

/**
 * Try to find the first binary operator in the tree.
 * We assume that the tree is actually only a list.
 */
qop_node_ptr find_binary_qop(qop_node_ptr tree) {
    // we know that this tree is only a list!
    while (tree != nullptr) {
        if (tree->is_binary()) 
            return tree;
        assert(tree->children_.size() <= 1);
        tree = tree->children_.empty() ? nullptr : tree->children_.front();
    }
    return nullptr;
}

qop_node_ptr find_matching_qop(qop_node_ptr tree, qop_node_ptr node) {
    if (tree->qop_ == node->qop_)
        return tree;
    for (auto& qp : tree->children_) {
        auto tp = find_matching_qop(qp, node);
        if (tp)
            return tp;
    }
    return nullptr;
}

void merge_qop_trees(qop_node_ptr master, qop_node_ptr tree) {
    // std::cout << "merge_qop_trees... "; master->print(); tree->print(); 
    // 1. find the first binary operator in tree
    qop_node_ptr bin_op = find_binary_qop(tree);
    if (! bin_op)
        return;

    // std::cout << "merge_qop_trees: found .."; bin_op->print();
    // 2. try to find the corresponding operator in master
    qop_node_ptr master_bin_op = find_matching_qop(master, bin_op);

    // 3. merge both trees on this operator
    if (!master_bin_op)
        return;

    // std::cout << "merge_qop_trees: found .."; master_bin_op->print();
    
    // std::cout << "merge: ";
    // bin_op->children_.front()->print();
    // std::cout << " --- ";
    // master_bin_op->print();

    master_bin_op->children_.push_back(bin_op->children_.front());
}

// --------------------------------------------------------------------------------------

void print_plan_helper(std::ostream& os, qop_node_ptr root, const std::string& prefix) {
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
        rchild->print(os);
        print_plan_helper(os, rchild, newPrefix);
    }

    if (hasFirst) {
        os << (hasSecond ? prefix : "") << "└── ";
        root->children_[0]->print(os);
        print_plan_helper(os, root->children_[0], prefix + "    ");
    }
}

