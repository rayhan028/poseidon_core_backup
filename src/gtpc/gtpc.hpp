#ifndef gtpc_hpp_
#define gtpc_hpp_

#include "graph_db.hpp"
#include "qop.hpp"
#include "query.hpp"
#include "thread_pool.hpp"

#include <queue>

// #define CREATE_INDEX

// #define RUN_INDEXED
// #define RUN_PARALLEL
// #define PRINT_RESULT


using param_val = boost::variant<uint64_t, std::string, int,
                                 boost::posix_time::ptime, double>;
using params_tuple = std::vector<param_val>;

/* queries */
void gtpc_query_1(graph_db_ptr &gdb, result_set &rs);
void gtpc_query_2(graph_db_ptr &gdb, result_set &rs);
void gtpc_query_3(graph_db_ptr &gdb, result_set &rs);
void gtpc_query_4(graph_db_ptr &gdb, result_set &rs);
void gtpc_query_5(graph_db_ptr &gdb, result_set &rs);
void gtpc_query_6(graph_db_ptr &gdb, result_set &rs);
void gtpc_query_7(graph_db_ptr &gdb, result_set &rs);
void gtpc_query_8(graph_db_ptr &gdb, result_set &rs);
void gtpc_query_9(graph_db_ptr &gdb, result_set &rs);
void gtpc_query_10(graph_db_ptr &gdb, result_set &rs);
void gtpc_query_11(graph_db_ptr &gdb, result_set &rs);
void gtpc_query_12(graph_db_ptr &gdb, result_set &rs);
void gtpc_query_13(graph_db_ptr &gdb, result_set &rs);
void gtpc_query_14(graph_db_ptr &gdb, result_set &rs);
void gtpc_query_15(graph_db_ptr &gdb, result_set &rs);
void gtpc_query_16(graph_db_ptr &gdb, result_set &rs);
void gtpc_query_17(graph_db_ptr &gdb, result_set &rs);
void gtpc_query_18(graph_db_ptr &gdb, result_set &rs);
void gtpc_query_19(graph_db_ptr &gdb, result_set &rs);
void gtpc_query_20(graph_db_ptr &gdb, result_set &rs);
void gtpc_query_21(graph_db_ptr &gdb, result_set &rs);
void gtpc_query_22(graph_db_ptr &gdb, result_set &rs);

/* transactions */
void gtpc_transaction_1(graph_db_ptr &gdb, result_set &rs);
void gtpc_transaction_2(graph_db_ptr &gdb, result_set &rs);
void gtpc_transaction_3(graph_db_ptr &gdb, result_set &rs);
void gtpc_transaction_4(graph_db_ptr &gdb, result_set &rs);
void gtpc_transaction_5(graph_db_ptr &gdb, result_set &rs);

/* data */
void load_gtpc_data(graph_db_ptr &graph, const std::string& path, bool strict = true);
void load_gtpc_data(graph_db_ptr &graph, std::vector<std::string> &node_files,
                    std::vector<std::string> &rship_files, bool strict = true);

/* index */
void create_gtpc_index(graph_db_ptr &graph);
void gtpc_fptree_recovery(graph_db_ptr &graph);

/* utility */
std::string gen_last_name(int num);
int gen_random_uniform_int(int min, int max);
int gen_random_nonuniform_int(int A, int x, int y, int C);

#endif