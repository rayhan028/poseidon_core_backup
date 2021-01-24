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
    // TODO: merge trees
}