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

#include <boost/thread/barrier.hpp>
#include <boost/hana.hpp>


using consumer_ty = std::function<void(graph_db*, int, int**, int*, int, int*)>;
using consumer_fct_type = void(*)(graph_db*, int, int**, int*, int, int*, int**, int);
using finish_fct_type = void(*)(result_set*);

/**
 * The type for the generated query function.
 */ 
using start_ty = void(*)(graph_db*, int, int, transaction_ptr, int, std::vector<int>*, result_set*, int**, finish_fct_type, uint64_t, uint64_t**);


/**
 * Tuple result types
 * TODO: consistent type id mapping for all classes
 */
enum class FTYPE {
    INT = 0,
    DOUBLE = 1,
    STRING = 2,
    DATE = 3,
    TIME = 4,
    BOOLEAN = 5,
    UINT64 = 6
};

/**
 * Function to obtain the iterator of a node vector
 */
 chunked_vec<node, NODE_CHUNK_SIZE>::range_iter *get_vec_begin(node_list *vec, size_t first, size_t last);

/**
 * Function to obtain the next iterator of a node vector
 */
 chunked_vec<node, NODE_CHUNK_SIZE>::range_iter *get_vec_next(chunked_vec<node, NODE_CHUNK_SIZE>::range_iter *it);

/**
 * Boolean function to check if the end of the node vector is reached
 */
 bool vec_end_reached(node_list &vec, chunked_vec<node, NODE_CHUNK_SIZE>::range_iter *it);

/**
 * Function to obtain the iterator of a relationship vector
 */
chunked_vec<relationship, RSHIP_CHUNK_SIZE>::iter get_vec_begin_r(relationship_list &vec);

/**
 * Function to obtain the next iterator of a relationship vector
 */
chunked_vec<relationship, RSHIP_CHUNK_SIZE>::iter *get_vec_next_r(chunked_vec<relationship, RSHIP_CHUNK_SIZE>::iter *it);

/**
 * Boolean function to check if the end of the relationship vector is reached
 */
bool vec_end_reached_r(relationship_list &vec, chunked_vec<relationship, RSHIP_CHUNK_SIZE>::iter it);

/**
 * Function to lookup a label
 */
 dcode_t dict_lookup_label(graph_db *gdb, char *label);

/**
 * Obtains the pointer to a node from a node vector iterator
 */
 node *get_node_from_it(chunked_vec<node, NODE_CHUNK_SIZE>::range_iter *it);

/**
 * Obtains the pointer to a relationship from a relationship vector iterator
 */
 relationship *get_rship_from_it(chunked_vec<relationship, RSHIP_CHUNK_SIZE>::iter *it);

/**
 * Returns a pointer to the node chunked vector of a given graph
 */
 chunked_vec<node, NODE_CHUNK_SIZE> *gdb_get_nodes(graph_db *gdb);

/**
 * Returns a pointer to the relationship chunked vector of a given graph
 */
 chunked_vec<relationship, RSHIP_CHUNK_SIZE> *gdb_get_rships(graph_db *gdb);

 void test_ints(uint64_t a, uint64_t b);

/**
 * Return the relationship pointer with the given id
 */
 relationship *rship_by_id(graph_db *gdb, offset_t id);

/**
 * Return the node pointer with the given id
 */
 node *node_by_id(graph_db *gdb, offset_t id);

/**
 * Returns the dictionary code of a given string
 */
 dcode_t gdb_get_dcode(graph_db *gdb, char *property);

/**
 * Returns the property item at the given position in the property set
 */
 const property_set *pset_get_item_at(graph_db *gdb, offset_t id);

/**
 * Init of the transaction processing
 */
 xid_t get_tx(transaction_ptr);

/**
 * Checks if the current node is valid for transactional processing
 */
 node * get_valid_node(graph_db *gdb, node * n, transaction_ptr tx);

/**
 * Applies a given Projection on a node result and writes the result at a memory address, given by the caller.
 */
 void apply_pexpr_node(graph_db *gdb, const char *key, FTYPE val_type, int *qr, int *ret);

/**
 * Applies a given Projection on a relationship result and writes the result at a memory address, given by the caller.
 */
 void apply_pexpr_rship(graph_db *gdb, const char *key, FTYPE val_type, int *qr, int *ret);

/**
 * External function to count all potential 1-hop relationships, used by the variable ForeachRelationship operator
 */
 int count_potential_o_hop(graph_db *gdb, offset_t rship_id);

/**
 * Allocates a queue in order to scan all relationships recursively
 */
 std::list<std::pair<relationship::id_t, std::size_t>> retrieve_fev_queue();

/**
 * Insert the relationship id and the hop count to the FEV queue 
 */
 void insert_fev_rship(std::list<std::pair<relationship::id_t, std::size_t>> &queue, relationship::id_t rid, std::size_t hop);

/**
 * Returns true when the queue, used by the FEV operator, is empty
 */
 bool fev_queue_empty(std::list<std::pair<relationship::id_t, std::size_t>> &queue);

/**
 * Method that processes the actual projection on a tuple result
 */
 void apply_pexpr(graph_db *gdb, const char *key, FTYPE val_type, int *qr, int idx, std::vector<int> types, int *ret);

/**
 * Function to lookup a given dictionary code
 */
 const char* lookup_dc(graph_db *gdb, dcode_t dc);

// void get_nodes(graph_db gdb, consumer_fct_type consumer);

/**
 * Function for the creation of a node with given properties
 */
 node* create_node(graph_db *gdb, char *label, properties_t *props);

/**
 * Function for the creation of a relationship with given properties
 */
 relationship* create_rship(graph_db *gdb, char *label, node *n1, node *n2, properties_t *props);

/**
 * Functions to handle the variable from relationship operator
 */
 void foreach_variable_from(graph_db *gdb, dcode_t label, int min, int max, consumer_fct_type consumer,
                                 int oid, int **qr, int *rs, int size, int *ty, int **call_map_arg, int offset);

/**
 * Functions to handle the variable to relationship operator
 */
 void foreach_variable_to(graph_db *gdb, dcode_t label, int min, int max, consumer_fct_type consumer,
                                      int oid, int **qr, int *rs, int size, int *ty, int **call_map_arg, int offset);


extern std::map<int, std::function<std::string(graph_db*, int*)>> con_map;

/**
 * Thread local storage to store the Projection of a tuple. These values are
 * materialized to the result_set.
 * //TODO: remove map
 */
extern thread_local std::map<int, uint64_t> uint_result;
extern thread_local std::map<int, std::string> str_result;
extern thread_local std::map<int, boost::posix_time::ptime> time_result;

/**
 * Function to transform a register value into the appropriate type and materialize to 
 * thread local storage.
 */
 void mat_reg_value(graph_db *gdb, int *reg, int type);

/**
 * collect_tuple inserts the tuple from thread_local storage into the given result_set.  
 * If print is true, the tuple will be printed to the standard output
 */
 void collect_tuple(result_set *rs, bool print);

qr_tuple &get_qr_tuple();

/**
 * obtain_mat_tuple returns a thread local tuple storage used to materialize the
 * rhs side of a join.
 */
 qr_tuple *obtain_mat_tuple();

/**
 * mat_node materialize a node to a thread local tuple storage
 */
 void mat_node(node *n, qr_tuple *qr);

/**
 * mat_rship materialize a rship to a thread local tuple storage
 */
 void mat_rship(relationship *r, qr_tuple *qr);

/**
 * collect_tuple_join inserts the thread local tuple storage to a list of
 * the appropriate join operation with the id jid
 */
 void collect_tuple_join(int jid, qr_tuple *qr);

/**
 * get_join_tp_at returns a tuple from the join list at the given position
 */
 qr_tuple *get_join_tp_at(int jid, int pos);

/**
 * get_node_res_at returns a ptr to the node from the tuple at the given postion
 */
 node *get_node_res_at(qr_tuple *tuple, int pos);

/**
 * get_rship_res_at returns a ptr to the rship from the tuple at the given postion
 */
 relationship *get_rship_res_at(qr_tuple *tuple, int pos);

/**
 * get_mat_res_size returns the size of the materialized rhs list of a join with the id = jid
 */
 int get_mat_res_size(int jid);

/**
 * index_get_node is a helper method in order to process a index scan for a specific node
 */
 node *index_get_node(graph_db *gdb, char *label, char *prop, uint64_t value);

extern thread_local std::vector<relationship*> fev_rship_list;
extern thread_local std::vector<relationship*>::iterator fev_list_iter;
extern thread_local std::string grpkey_buffer;

 void foreach_from_variable_rship(graph_db *gdb, dcode_t lcode, node *n, std::size_t min, std::size_t max);

 relationship *get_next_rship_fev();

 bool fev_list_end();

/**
 * Methods to process the group_by operation
 */
 void get_node_grpkey(node* n, unsigned pos);
 void get_rship_grpkey(relationship* r, unsigned pos);
 void get_int_grpkey(int i, unsigned pos);
 void get_double_grpkey(int* d_ptr, unsigned pos);
 void get_string_grpkey(int* str_ptr, unsigned pos);
 void get_time_grpkey(int* time_ptr, unsigned pos);
 void add_to_group();
 void finish_group_by(result_set* rs);
 void clear_mat_tuple();
 qr_tuple* grp_demat_at(int index);
 int get_grp_rs_count();

/**
 * Methods for the dematerialization of the tuple to IR registers
 */
 int int_to_reg(qr_tuple* qr, int pos);
 int str_to_reg(qr_tuple* qr, int pos);
 node* node_to_reg(qr_tuple* qr, int pos);
 relationship* rship_to_reg(qr_tuple* qr, int pos);
 int time_to_reg(qr_tuple* qr, int pos);

/**
 * Methods for the aggregation processing
 */
 void init_grp_aggr();
 int get_group_count();
 int get_total_group_count();
 int get_group_sum_int(int pos);
 double get_group_sum_double(int pos);
 uint64_t get_group_sum_uint(int pos);

 void append_to_tuple(query_result qr);

void insert_join_id_input(int jid, offset_t id);
offset_t get_join_id_at(int jid, int pos);

#endif //PJIT_GLOBAL_DEFINITIONS_HPP
