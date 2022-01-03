#ifndef query_printer_hpp_
#define query_printer_hpp_

#include <iostream>
#include <memory>
#include <vector>

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

    void print(std::ostream& os = std::cout) {
        qop_->dump(os); 
        os << std::endl;
    }
};

void print_plan_helper(std::ostream& os, qop_node_ptr root, const std::string& prefix);
void merge_qop_trees(qop_node_ptr master, qop_node_ptr tree);

/**
 * Recursively constructs a tree of qop_nodes representing a query plan for printing.
 * This is an upside-down tree where the scan operators are the leaf nodes.
 * The function returns a pair of (root node, current node).
 */
std::pair<qop_node_ptr, qop_node_ptr> build_qop_tree(qop_ptr root);

#endif