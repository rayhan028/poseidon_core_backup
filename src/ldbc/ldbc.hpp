#ifndef ldbc_hpp_
#define ldbc_hpp_

#include "graph_db.hpp"
#include "qop.hpp"
#include "thread_pool.hpp"

#include <queue>

// #define SF_1
#define SF_10
#define CREATE_INDEX
#define PRINT_RESULT
#define IU_RESULT

using param_val = boost::variant<uint64_t, std::string, int>;
using params_tuple = std::vector<param_val>;

/* interactive short queries */
void ldbc_is_query_1(graph_db_ptr &gdb, result_set &rs, uint64_t personId);
void ldbc_is_query_2_p(graph_db_ptr &gdb, result_set &rs, uint64_t personId);
void ldbc_is_query_2_c(graph_db_ptr &gdb, result_set &rs, uint64_t personId);
void ldbc_is_query_3(graph_db_ptr &gdb, result_set &rs, uint64_t personId);
void ldbc_is_query_4_p(graph_db_ptr &gdb, result_set &rs, uint64_t messageId);
void ldbc_is_query_4_c(graph_db_ptr &gdb, result_set &rs, uint64_t messageId);
void ldbc_is_query_5_p(graph_db_ptr &gdb, result_set &rs, uint64_t messageId);
void ldbc_is_query_5_c(graph_db_ptr &gdb, result_set &rs, uint64_t messageId);
void ldbc_is_query_6_p(graph_db_ptr &gdb, result_set &rs, uint64_t messageId);
void ldbc_is_query_6_c(graph_db_ptr &gdb, result_set &rs, uint64_t messageId);
void ldbc_is_query_7_p(graph_db_ptr &gdb, result_set &rs, uint64_t messageId);
void ldbc_is_query_7_c(graph_db_ptr &gdb, result_set &rs, uint64_t messageId);

/* interactive update queries */
void ldbc_iu_query_1(graph_db_ptr &gdb, result_set &rs, params_tuple &params);
void ldbc_iu_query_2(graph_db_ptr &gdb, result_set &rs, params_tuple &params);
void ldbc_iu_query_3(graph_db_ptr &gdb, result_set &rs, params_tuple &params);
void ldbc_iu_query_4(graph_db_ptr &gdb, result_set &rs, params_tuple &params);
void ldbc_iu_query_5(graph_db_ptr &gdb, result_set &rs, params_tuple &params);
void ldbc_iu_query_6(graph_db_ptr &gdb, result_set &rs, params_tuple &params);
void ldbc_iu_query_7(graph_db_ptr &gdb, result_set &rs, params_tuple &params);
void ldbc_iu_query_8(graph_db_ptr &gdb, result_set &rs, params_tuple &params);

/* graphalytics algorithms */
void bfs(graph_db_ptr &gdb, result_set &rs, uint64_t id);

void load_snb_data(graph_db_ptr &graph, const std::string& path, bool strict = true);

void load_snb_data(graph_db_ptr &graph,
                    std::vector<std::string> &node_files,
                    std::vector<std::string> &rship_files, bool strict = true);

void fptree_recovery(graph_db_ptr &graph);

#endif