#ifndef tpc_hpp_
#define tpc_hpp_

#include "graph_db.hpp"
#include "qop.hpp"
#include "query.hpp"
#include "thread_pool.hpp"

#include <queue>

// #define CREATE_INDEX

// #define RUN_INDEXED
// #define RUN_PARALLEL
// #define PRINT_RESULT

namespace pj = builtin;

/* data */
void load_gtpcc_data(graph_db_ptr &graph, const std::string& path, bool strict = true);
void load_gtpcc_data(graph_db_ptr &graph,
                    std::vector<std::string> &node_files,
                    std::vector<std::string> &rship_files, bool strict = true);

/* index */
void create_gtpcc_index(graph_db_ptr &graph);
void gtpcc_fptree_recovery(graph_db_ptr &graph);

#endif