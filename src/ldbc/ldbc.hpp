#ifndef ldbc_hpp_
#define ldbc_hpp_

#include "graph_db.hpp"
#include "qop.hpp"

void run_ldbc_queries(graph_db_ptr &gdb);

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
void ldbc_is_query_7(graph_db_ptr &gdb, result_set &rs, uint64_t commentId);



/* interactive update queries */
/*void ldbc_iu_query_1(graph_db_ptr &gdb, result_set &rs);
void ldbc_iu_query_2(graph_db_ptr &gdb, result_set &rs);
void ldbc_iu_query_3(graph_db_ptr &gdb, result_set &rs);
void ldbc_iu_query_4(graph_db_ptr &gdb, result_set &rs, std::vector<uint64_t>& uint64_props,
                      std::vector<std::string>& str_props);
void ldbc_iu_query_5(graph_db_ptr &gdb, result_set &rs, std::vector<uint64_t>& uint64_props,
                      std::vector<std::string>& str_props);
void ldbc_iu_query_6(graph_db_ptr &gdb, result_set &rs, std::vector<uint64_t>& uint64_props,
                      std::vector<std::string>& str_props, std::vector<int>& int_props);
void ldbc_iu_query_7(graph_db_ptr &gdb, result_set &rs);
void ldbc_iu_query_8(graph_db_ptr &gdb, result_set &rs);*/

void ldbc_iu_query_1(graph_db_ptr &gdb, std::vector<uint64_t>& uint64_props,
                      std::vector<std::string>& str_props, std::vector<int>& int_props);
void ldbc_iu_query_2(graph_db_ptr &gdb, std::vector<uint64_t>& uint64_props,
                      std::vector<std::string>& str_props, std::vector<int>& int_props);
void ldbc_iu_query_3(graph_db_ptr &gdb, std::vector<uint64_t>& uint64_props,
                      std::vector<std::string>& str_props, std::vector<int>& int_props);
void ldbc_iu_query_4(graph_db_ptr &gdb, std::vector<uint64_t>& uint64_props,
                      std::vector<std::string>& str_props, std::vector<int>& int_props);
void ldbc_iu_query_5(graph_db_ptr &gdb, std::vector<uint64_t>& uint64_props,
                      std::vector<std::string>& str_props, std::vector<int>& int_props);
void ldbc_iu_query_6(graph_db_ptr &gdb, std::vector<uint64_t>& uint64_props,
                      std::vector<std::string>& str_props, std::vector<int>& int_props);
void ldbc_iu_query_7(graph_db_ptr &gdb, std::vector<uint64_t>& uint64_props,
                      std::vector<std::string>& str_props, std::vector<int>& int_props);
void ldbc_iu_query_8(graph_db_ptr &gdb, std::vector<uint64_t>& uint64_props,
                      std::vector<std::string>& str_props, std::vector<int>& int_props);

#endif