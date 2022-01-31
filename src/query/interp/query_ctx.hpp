#ifndef query_ctx_hpp_
#define query_ctx_hpp_

#include "defs.hpp"
#include "graph_db.hpp"

/**
 * query_ctx represents the query context which is passed to query operators and user-defined functions
 * during execution.
 */

struct query_ctx {
    query_ctx(graph_db_ptr& gdb) : gdb_(gdb) {}
    graph_db_ptr gdb_;
};

#endif