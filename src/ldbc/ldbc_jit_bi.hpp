#ifndef LDBC_JIT_BI_HPP
#define LDBC_JIT_BI_HPP

#include <iostream>
#include <numeric>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>

#include "defs.hpp"
#include "graph_db.hpp"
#include "graph_pool.hpp"
#include "ldbc.hpp"
#include "config.h"

#include "threadsafe_queue.hpp"

#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"

#include "ldbc_jit_reads.hpp"

using namespace boost::program_options;
using namespace boost::posix_time;

using param_val_bi = boost::variant<uint64_t, std::string, int, boost::posix_time::ptime>;
using params_tuple_bi = std::vector<param_val>;

//std::vector<params_tuple_bi> q1_params = {{time_from_string(std::string("2017-04-14 01:51:21.746"))}};
//std::vector<params_tuple_bi> q2_params = {{time_from_string(std::string("2011-04-14 01:51:21.746"))}};

#endif
