#ifndef ldbc_hpp_
#define ldbc_hpp_

#include "graph_db.hpp"
#include "qop.hpp"

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

#endif