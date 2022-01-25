#ifndef RADIX_INDEX_HPP
#define RADIX_INDEX_HPP

#include "qoperator.hpp"
#include "dict.hpp"
//#include "materialized_view.hpp"


#include <array>
#include <bitset>
#include <cstdint>
#include <memory>

#ifdef USE_PMDK
#include <libpmemobj++/container/array.hpp>
#include <libpmemobj++/container/vector.hpp>
#include <libpmemobj++/utils.hpp>
#include <libpmemobj++/shared_mutex.hpp>
#include <libpmemobj++/persistent_ptr.hpp>
#endif

struct radix_child;
struct radix_root;
struct radix_op_child;

using tree_ptr =
#ifdef USE_PMDK
pmem::obj::persistent_ptr<radix_child>;
#else
std::shared_ptr<radix_child>;
#endif

enum class node_type {
    root = 0,
    op = 1,
    arg = 2,
    dir = 3,
    list = 4
};

struct radix_child {
protected:
    uint32_t type;
    std::bitset<32> bitmap;
    std::array<tree_ptr, 32> children;
    radix_child() = default;
    radix_child(uint32_t t) : type(t) {}

public:
    void insert_op(tree_ptr child, uint32_t type);
    void insert_arg(tree_ptr child, uint32_t type);
    
    bool exists(uint32_t id) {
        return bitmap.test(id);
    }

    tree_ptr get_op(uint32_t opid) {
        if(bitmap.test(opid)) {
            return children[opid];
        }
        return nullptr;
    }

    bool is_of_type(node_type t) {
        return type == static_cast<uint32_t>(t);
    }

    tree_ptr get_arg() {
        if(exists(2)) return children[2];
        return nullptr;
    }

    void insert_leaf(tree_ptr leaf) {
        bitmap.set(0);
        children[0] = leaf;
    }

    tree_ptr get_leaf() {
        if(bitmap.test(0)) {
            return children[0];
        }
        return nullptr;
    }

};

struct radix_root : public radix_child {
    //uint8_t type;
    radix_root() = default;
    radix_root(uint32_t type) : radix_child(type) {}
};

struct radix_op_child : public radix_child {
    //uint8_t type;
    uint32_t opid;

    radix_op_child() = default;
    radix_op_child(uint32_t t) : radix_child(t) {}
};

struct radix_arg_child : public radix_child {
    //uint8_t type;
    uint32_t arg;

    radix_arg_child() = default;
    radix_arg_child(uint32_t t, uint32_t a) : radix_child(t), arg(a) {}
};

using mv_ptr =
#ifdef USE_PMDK
    pmem::obj::persistent_ptr<materialized_view>;
#else
    std::shared_ptr<materialized_view>;
#endif

struct radix_leaf : public radix_child {
    mv_ptr mv_data;

    radix_leaf(dict_ptr &dct) {
#ifdef USE_PMDK
    mv_data = pmem::obj::make_persistent<materialized_view>(dct);
#else
    mv_data = std::make_shared<materialized_view>(dct);
#endif
    }
};

struct radix_tree {
    // INSERT
    mv_ptr insert(algebra_optr query);
    
    // GET
    mv_ptr get_mv(algebra_optr query);
private:
    tree_ptr insert_op(algebra_optr op, tree_ptr parent);
    tree_ptr insert_at_node(algebra_optr op, tree_ptr parent);
    tree_ptr insert_args_at(tree_ptr prev, dict_ptr dct, algebra_optr op);
    mv_ptr insert_leaf(tree_ptr parent);

    bool check_direction(uint32_t dir, tree_ptr &last);
    bool check_arguments(algebra_optr op, tree_ptr &parent);
    mv_ptr get_leaf(tree_ptr parent);

    tree_ptr root;
    dict_ptr dict_;

};



#endif