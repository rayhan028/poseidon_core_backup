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

/* OLAP */
void gtpc_olap_1(graph_db_ptr &gdb, result_set &rs);
void gtpc_olap_2(graph_db_ptr &gdb, result_set &rs);
void gtpc_olap_3(graph_db_ptr &gdb, result_set &rs);
void gtpc_olap_4(graph_db_ptr &gdb, result_set &rs);
void gtpc_olap_5(graph_db_ptr &gdb, result_set &rs);
void gtpc_olap_6(graph_db_ptr &gdb, result_set &rs);
void gtpc_olap_7(graph_db_ptr &gdb, result_set &rs);
void gtpc_olap_8(graph_db_ptr &gdb, result_set &rs);
void gtpc_olap_9(graph_db_ptr &gdb, result_set &rs);
void gtpc_olap_10(graph_db_ptr &gdb, result_set &rs);
void gtpc_olap_11(graph_db_ptr &gdb, result_set &rs);
void gtpc_olap_12(graph_db_ptr &gdb, result_set &rs);
void gtpc_olap_13(graph_db_ptr &gdb, result_set &rs);
void gtpc_olap_14(graph_db_ptr &gdb, result_set &rs);
void gtpc_olap_15(graph_db_ptr &gdb, result_set &rs);
void gtpc_olap_16(graph_db_ptr &gdb, result_set &rs);
void gtpc_olap_17(graph_db_ptr &gdb, result_set &rs);
void gtpc_olap_18(graph_db_ptr &gdb, result_set &rs);
void gtpc_olap_19(graph_db_ptr &gdb, result_set &rs);
void gtpc_olap_20(graph_db_ptr &gdb, result_set &rs);
void gtpc_olap_21(graph_db_ptr &gdb, result_set &rs);
void gtpc_olap_22(graph_db_ptr &gdb, result_set &rs);

/* OLTP */
void gtpc_oltp_1(graph_db_ptr &gdb, result_set &rs);
void gtpc_oltp_2(graph_db_ptr &gdb, result_set &rs);
void gtpc_oltp_3(graph_db_ptr &gdb, result_set &rs);
void gtpc_oltp_4(graph_db_ptr &gdb, result_set &rs);
void gtpc_oltp_5(graph_db_ptr &gdb, result_set &rs);

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