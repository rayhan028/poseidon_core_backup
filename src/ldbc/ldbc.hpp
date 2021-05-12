#ifndef ldbc_hpp_
#define ldbc_hpp_

#include "graph_db.hpp"
#include "qop.hpp"
#include "query.hpp"
#include "thread_pool.hpp"

#include <queue>

#define SF_1
// #define SF_10
#define CREATE_INDEX

#define RUN_INDEXED

// #define RUN_PARALLEL
 #define PRINT_RESULT

namespace pj = builtin;

using param_val = boost::variant<uint64_t, std::string, int, boost::posix_time::ptime>;
using params_tuple = std::vector<param_val>;

/* interactive short queries */
void ldbc_is_query_1(graph_db_ptr &gdb, result_set &rs, uint64_t personId);
void ldbc_is_query_2(graph_db_ptr &gdb, result_set &rs, uint64_t personId);
void ldbc_is_query_2_p(graph_db_ptr &gdb, result_set &rs, uint64_t personId);
void ldbc_is_query_2_c(graph_db_ptr &gdb, result_set &rs, uint64_t personId);
void ldbc_is_query_3(graph_db_ptr &gdb, result_set &rs, uint64_t personId);
void ldbc_is_query_4(graph_db_ptr &gdb, result_set &rs, uint64_t messageId);
void ldbc_is_query_4_p(graph_db_ptr &gdb, result_set &rs, uint64_t messageId);
void ldbc_is_query_4_c(graph_db_ptr &gdb, result_set &rs, uint64_t messageId);
void ldbc_is_query_5(graph_db_ptr &gdb, result_set &rs, uint64_t messageId);
void ldbc_is_query_5_p(graph_db_ptr &gdb, result_set &rs, uint64_t messageId);
void ldbc_is_query_5_c(graph_db_ptr &gdb, result_set &rs, uint64_t messageId);
void ldbc_is_query_6(graph_db_ptr &gdb, result_set &rs, uint64_t messageId);
void ldbc_is_query_6_p(graph_db_ptr &gdb, result_set &rs, uint64_t messageId);
void ldbc_is_query_6_c(graph_db_ptr &gdb, result_set &rs, uint64_t messageId);
void ldbc_is_query_7(graph_db_ptr &gdb, result_set &rs, uint64_t messageId);
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

/* business intelligence queries */
void ldbc_bi_query_1(graph_db_ptr &gdb, result_set &rs, params_tuple &params);
void ldbc_bi_query_2(graph_db_ptr &gdb, result_set &rs, params_tuple &params);
void ldbc_bi_query_3(graph_db_ptr &gdb, result_set &rs, params_tuple &params);
void ldbc_bi_query_4(graph_db_ptr &gdb, result_set &rs, params_tuple &params);
void ldbc_bi_query_5(graph_db_ptr &gdb, result_set &rs, params_tuple &params);
void ldbc_bi_query_6(graph_db_ptr &gdb, result_set &rs, params_tuple &params);
void ldbc_bi_query_7(graph_db_ptr &gdb, result_set &rs, params_tuple &params);
void ldbc_bi_query_8(graph_db_ptr &gdb, result_set &rs, params_tuple &params);
void ldbc_bi_query_9(graph_db_ptr &gdb, result_set &rs, params_tuple &params);
void ldbc_bi_query_10(graph_db_ptr &gdb, result_set &rs, params_tuple &params);
void ldbc_bi_query_11(graph_db_ptr &gdb, result_set &rs, params_tuple &params);
void ldbc_bi_query_12(graph_db_ptr &gdb, result_set &rs, params_tuple &params);
void ldbc_bi_query_13(graph_db_ptr &gdb, result_set &rs, params_tuple &params);
void ldbc_bi_query_14(graph_db_ptr &gdb, result_set &rs, params_tuple &params);
void ldbc_bi_query_15(graph_db_ptr &gdb, result_set &rs, params_tuple &params);
void ldbc_bi_query_16(graph_db_ptr &gdb, result_set &rs, params_tuple &params);
void ldbc_bi_query_17(graph_db_ptr &gdb, result_set &rs, params_tuple &params);
void ldbc_bi_query_18(graph_db_ptr &gdb, result_set &rs, params_tuple &params);
void ldbc_bi_query_19(graph_db_ptr &gdb, result_set &rs, params_tuple &params);
void ldbc_bi_query_20(graph_db_ptr &gdb, result_set &rs, params_tuple &params);

void recover_ldbc_bi_query_1(graph_db_ptr &gdb, result_set &rs, std::map<std::size_t, std::size_t> &cp, 
    std::map<std::size_t, std::vector<std::size_t>> &range_map, 
    std::list<qr_tuple> rec, params_tuple &params);

void recover_ldbc_bi_query_9(graph_db_ptr &gdb, result_set &rs, std::map<std::size_t, std::size_t> &cp, 
    std::map<std::size_t, std::vector<std::size_t>> &range_map, 
    std::list<qr_tuple> rec, params_tuple &params);

/* interactive delete queries */
void ldbc_del_query_1(graph_db_ptr &gdb, result_set &rs, params_tuple &params);
void ldbc_del_query_2(graph_db_ptr &gdb, result_set &rs, params_tuple &params);
void ldbc_del_query_3(graph_db_ptr &gdb, result_set &rs, params_tuple &params);
void ldbc_del_query_4(graph_db_ptr &gdb, result_set &rs, params_tuple &params);
void ldbc_del_query_5(graph_db_ptr &gdb, result_set &rs, params_tuple &params);
void ldbc_del_query_6(graph_db_ptr &gdb, result_set &rs, params_tuple &params);
void ldbc_del_query_7(graph_db_ptr &gdb, result_set &rs, params_tuple &params);
void ldbc_del_query_8(graph_db_ptr &gdb, result_set &rs, params_tuple &params);

/* data */
void load_snb_data(graph_db_ptr &graph, const std::string& path, bool strict = true);
void load_snb_data(graph_db_ptr &graph,
                    std::vector<std::string> &node_files,
                    std::vector<std::string> &rship_files, bool strict = true);

/* index */
void fptree_recovery(graph_db_ptr &graph);

/* qps */
void ldbc_is_qp2_query_1(graph_db_ptr &gdb, result_set &rs, uint64_t personId);
void ldbc_is_qp2_query_2_p(graph_db_ptr &gdb, result_set &rs, uint64_t personId);
void ldbc_is_qp2_query_2_c(graph_db_ptr &gdb, result_set &rs, uint64_t personId);
void ldbc_is_qp2_query_3(graph_db_ptr &gdb, result_set &rs, uint64_t personId);
void ldbc_is_qp2_query_7_p(graph_db_ptr &gdb, result_set &rs, uint64_t personId);
void ldbc_is_qp2_query_7_c(graph_db_ptr &gdb, result_set &rs, uint64_t personId);
void ldbc_is_qp3_query_2_c(graph_db_ptr &gdb, result_set &rs, uint64_t personId);
void ldbc_is_qp3_query_7_p(graph_db_ptr &gdb, result_set &rs, uint64_t personId);
void ldbc_is_qp3_query_7_c(graph_db_ptr &gdb, result_set &rs, uint64_t personId);
void ldbc_is_qp4_query_2_c(graph_db_ptr &gdb, result_set &rs, uint64_t personId);
void ldbc_is_qp4_query_7_p(graph_db_ptr &gdb, result_set &rs, uint64_t personId);
void ldbc_is_qp4_query_7_c(graph_db_ptr &gdb, result_set &rs, uint64_t personId);
void ldbc_is_qp5_query_2_c(graph_db_ptr &gdb, result_set &rs, uint64_t personId);

#endif