#include "query.hpp"

/**
 * A helper class for representing the tree of query operators (instances of qop).
 * NOTE: The root of this tree is the final operator producing the results but
 * not the scan.
 */
struct qop_node;
using qop_node_ptr = std::shared_ptr<qop_node>;

struct qop_node {
    qop_ptr qop_; // pointer to the actual query operator
    std::vector<qop_node_ptr> children_; // child nodes (publisher)

    qop_node(qop_ptr p) : qop_(p) {}

    bool is_binary() const { return qop_->is_binary(); }

    void print(std::ostream& os) {
        qop_->dump(os); 
        os << std::endl;
    }
};

/**
 * Recursively constructs a tree of qop_nodes representing a query plan for printing.
 * This is an upside-down tree where the scan operators are the leaf nodes.
 * The function returns a pair of (root node, current node).
 */
std::pair<qop_node_ptr, qop_node_ptr> build_qop_tree(qop_ptr root) {
    if (!root->has_subscriber()) {
        auto troot = std::make_shared<qop_node>(root);
        return std::make_pair(troot, troot);
    }
    auto np = build_qop_tree(root->subscriber());
    auto rn = std::make_shared<qop_node>(root);
    np.second->children_.push_back(rn);
    return std::make_pair(np.first, rn);
}

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
    // 1. find the first binary operator in tree
    qop_node_ptr bin_op = find_binary_qop(tree);
    if (! bin_op)
        return;

    // 2. try to find the same operator in master
    qop_node_ptr master_bin_op = find_matching_qop(master, bin_op);

    // 3. merge both trees on this operator
    master_bin_op->children_.push_back(bin_op);
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

void query::print_plan(std::ostream& os) {
    os << "----------------------------------------------------------------------\n";
    auto qop_tree = build_qop_tree(plan_head_);
    qop_tree.first->print(os);
    print_plan_helper(os, qop_tree.first, "");
    os << "----------------------------------------------------------------------\n";
}

void query::print_plans(std::initializer_list<query *> queries, std::ostream& os) {
    std::vector<qop_node_ptr> trees;
    for (auto &q : queries) {
        auto qop_tree = build_qop_tree(q->plan_head_);
        trees.push_back(qop_tree.first);
    }
    // merge trees
    for (auto i = 1u; i < trees.size(); i++) {
        merge_qop_trees(trees[0], trees[1]);
    }
    os << "##----------------------------------------------------------------------\n";
    trees[0]->print(os);
    print_plan_helper(os, trees[0], "");
    os << "##----------------------------------------------------------------------\n";

}