#include <iostream>
#include <boost/variant.hpp>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>

#include "defs.hpp"
#include "graph_db.hpp"
#include "graph_pool.hpp"
#include "ldbc.hpp"
#include "config.h"

#include <cassert>
#include "threadsafe_queue.hpp"

#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"

#define IU_RESULT

// ------------------------------------------------------------------------------------------------------------------------

void ldbc_del_query_1(graph_db_ptr &gdb, result_set &rs, params_tuple &params) {
;
}