/**
 * This file provides external methods and structures, used in the query operator processing.
 * Foreach method exists a equivalent LLVM IR prototype and FunctionCallee object.
 */

#ifndef PJIT_GLOBAL_DEFINITIONS_HPP
#define PJIT_GLOBAL_DEFINITIONS_HPP
#include <dict/dict.hpp>
#include <storage/graph_db.hpp>
#include <storage/nodes.hpp>
#include <defs.hpp>
#include <iostream>
#include <qop.hpp>

using consumer_ty = std::function<void(graph_db*, int, int**, int*, int, int*)>;
using consumer_fct_type = void(*)(graph_db*, int, int**, int*, int, int*, int**, int);
using finish_fct_type = void(*)(result_set*);

/**
 * The type for the generated query function.
 */ 
using start_ty = void(*)(graph_db*, int, int, transaction_ptr, int, std::vector<int>*, result_set*, int**, finish_fct_type, uint64_t, uint64_t**);
typedef int* qrl_ty;
typedef std::array<int*, 100> qr_arr;

void call_consumer_function(consumer_fct_type consumer, graph_db* gdb, int tid, int** qr, int* rs, int size, int* tyvec, int** consumer_);

extern "C" void printNode(node *n);


extern "C" void stupid();

/**
 * Filter expression types.
 */
enum class FTYPE {
    INT = 0,
    DOUBLE = 1,
    STRING = 2,
    DATE = 3,
    TIME = 4,
    BOOLEAN = 5,
};

struct qres {
    int *res;
    int type;
    bool is_null;
};

extern "C" void consumerDummy(qres *qr);

extern "C" void print_int(int i);

struct nn {
    nn *next;
    nn *prev;
    int x;
    qres *res;
};

struct qr_list {
    nn *head;
    nn *tail;
    int size;
};

extern "C" void list_size(qr_list *list);

/**
 * Function to obtain the iterator of a node vector
 */
extern "C" chunked_vec<node, NODE_CHUNK_SIZE>::range_iter *get_vec_begin(node_list *vec, size_t first, size_t last);

/**
 * Function to obtain the next iterator of a node vector
 */
extern "C" chunked_vec<node, NODE_CHUNK_SIZE>::range_iter *get_vec_next(chunked_vec<node, NODE_CHUNK_SIZE>::range_iter *it);

/**
 * Boolean function to check if the end of the node vector is reached
 */
extern "C" bool vec_end_reached(node_list &vec, chunked_vec<node, NODE_CHUNK_SIZE>::range_iter *it);

/**
 * Function to obtain the iterator of a relationship vector
 */
extern "C" chunked_vec<relationship, RSHIP_CHUNK_SIZE>::iter get_vec_begin_r(relationship_list &vec);

/**
 * Function to obtain the next iterator of a relationship vector
 */
extern "C" chunked_vec<relationship, RSHIP_CHUNK_SIZE>::iter *get_vec_next_r(chunked_vec<relationship, RSHIP_CHUNK_SIZE>::iter *it);

/**
 * Boolean function to check if the end of the relationship vector is reached
 */
extern "C" bool vec_end_reached_r(relationship_list &vec, chunked_vec<relationship, RSHIP_CHUNK_SIZE>::iter it);

/**
 * Function to lookup a label
 */
extern "C" dcode_t dict_lookup_label(graph_db *gdb, char *label);

/**
 * Obtains the pointer to a node from a node vector iterator
 */
extern "C" node *get_node_from_it(chunked_vec<node, NODE_CHUNK_SIZE>::range_iter *it);

/**
 * Obtains the pointer to a relationship from a relationship vector iterator
 */
extern "C" relationship *get_rship_from_it(chunked_vec<relationship, RSHIP_CHUNK_SIZE>::iter *it);

/**
 * Returns a pointer to the node chunked vector of a given graph
 */
extern "C" chunked_vec<node, NODE_CHUNK_SIZE> *gdb_get_nodes(graph_db *gdb);

/**
 * Returns a pointer to the relationship chunked vector of a given graph
 */
extern "C" chunked_vec<relationship, RSHIP_CHUNK_SIZE> *gdb_get_rships(graph_db *gdb);

extern "C" void test_ints(uint64_t a, uint64_t b);

/**
 * Return the relationship pointer with the given id
 */
extern "C" relationship *rship_by_id(graph_db *gdb, offset_t id);

/**
 * Return the node pointer with the given id
 */
extern "C" node *node_by_id(graph_db *gdb, offset_t id);

/**
 * Returns the dictionary code of a given string
 */
extern "C" dcode_t gdb_get_dcode(graph_db *gdb, char *property);

/**
 * Returns the property item at the given position in the property set
 */
extern "C" const property_set *pset_get_item_at(graph_db *gdb, offset_t id);

/**
 * Init of the transaction processing
 */
extern "C" xid_t get_tx(transaction_ptr);

/**
 * Checks if the current node is valid for transactional processing
 */
extern "C" node * get_valid_node(graph_db *gdb, node * n, transaction_ptr tx);

/**
 * Returns the size of the rhs tuple list of a join
 */
extern "C" int get_join_vec_size(std::vector<qr_arr>* vec);

/**
 * Returns the a pointer to a tuple at a given position in the rhs tuple list of a join
 */
extern "C" int** get_join_vec_arr(std::vector<qr_arr>* vec, int idx);

extern "C" void check_qr(int** qr);

class Collector {
    int called_;
    std::vector<std::string> results;
    std::mutex mut;


public:
    Collector() {
        called_ = 0;
    }

    void collect(int **rl);
    static graph_db *gdb;
};

class Joiner {
    using left_table = std::vector<qr_list *>;
    left_table left_input_;
    left_table::iterator cur_pos_;

public:
    Joiner() {
        cur_pos_ = left_input_.begin();

    }

    void insert(qr_list *res);

    int i = 0;

    qr_list *consume();

};

/**
 * Method for the handling of the tuple collection.
 */
extern "C" void collect(graph_db *gdb, int **qr, result_set * rs, int qr_size, std::vector<int> *types);

extern "C" void join_insert_left(std::vector<qr_arr>* vec, int **qrl);

extern "C" qr_list *join_consume_left();

/**
 * Method that processes the actual projection on a tuple result
 */
extern "C" void apply_pexpr(graph_db *gdb, const char *key, FTYPE val_type, int *qr, int idx, std::vector<int> types, int *ret);

/**
 * Function to lookup a given dictionary code
 */
extern "C" const char* lookup_dc(graph_db *gdb, dcode_t dc);

extern "C" void join_vec_insert(std::vector<int*> *inputs, int* res);

extern "C" void merge_type_vec(std::vector<int*> *lhs, std::vector<int*> *rhs);

//extern "C" void get_nodes(graph_db gdb, consumer_fct_type consumer);

/**
 * Function for the creation of a node with given properties
 */
extern "C" node* create_node(graph_db *gdb, char *label, properties_t *props);

/**
 * Function for the creation of a relationship with given properties
 */
extern "C" relationship* create_rship(graph_db *gdb, char *label, node *n1, node *n2, properties_t *props);

/**
 * Functions to handle the variable from/to relationship operator
 */
extern "C" void foreach_variable_from(graph_db *gdb, dcode_t label, int min, int max, consumer_fct_type consumer,
                                 int oid, int **qr, int *rs, int size, int *ty, int **call_map_arg, int offset);

extern "C" void foreach_variable_to(graph_db *gdb, dcode_t label, int min, int max, consumer_fct_type consumer,
                                      int oid, int **qr, int *rs, int size, int *ty, int **call_map_arg, int offset);


extern std::map<int, std::function<std::string(graph_db*, int*)>> con_map;

/**
 * Thread local storage of intermediate projection results
 */
extern thread_local std::map<int, std::string> str_result;
extern thread_local std::map<int, boost::posix_time::ptime> time_result;

/**
 * Functions for the materilization of tuple results
 */
extern "C" void mat_reg_value(graph_db *gdb, int *reg, int type);
extern "C" void collect_tuple(result_set *rs);
extern "C" qr_tuple *obtain_mat_tuple();
extern "C" void mat_node(node *n, qr_tuple *qr);
extern "C" void mat_rship(relationship *r, qr_tuple *qr);
extern "C" void collect_tuple_join(int jid, qr_tuple *qr);
extern "C" qr_tuple *get_join_tp_at(int jid, int pos);
extern "C" node *get_node_res_at(qr_tuple *tuple, int pos);
extern "C" relationship *get_rship_res_at(qr_tuple *tuple, int pos);
extern "C" int get_mat_res_size(int jid);

extern "C" node *index_get_node(graph_db *gdb, char *label, char *prop, uint64_t value);

#endif //PJIT_GLOBAL_DEFINITIONS_HPP
