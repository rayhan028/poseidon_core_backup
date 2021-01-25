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

namespace pj = builtin;
using namespace boost::program_options;
using namespace boost::posix_time;

using param_val_bi = boost::variant<uint64_t, std::string, int, boost::posix_time::ptime>;
using params_tuple_bi = std::vector<param_val>;

//std::vector<params_tuple_bi> q1_params = {{time_from_string(std::string("2017-04-14 01:51:21.746"))}};
//std::vector<params_tuple_bi> q2_params = {{time_from_string(std::string("2011-04-14 01:51:21.746"))}};

bool q1_filter_cdate(int *prop_ptr) {
    auto prop = (p_item*)prop_ptr;
    return (*reinterpret_cast<const ptime *>(prop->value_)) < time_from_string(std::string("2017-04-14 01:51:21.746"));
    
}

int q1_get_node_property(projection::pr_result pr) {
    return boost::get<int>(pj::int_property(pr, "length"));
}

int q1_group_msg_len(node * n) {
    auto len = q1_get_node_property(n);
    return (len >= 0 && len < 40) ? 0 :
                (len >= 40 && len < 80) ? 1 :
                    (len >= 80 && len < 160) ? 2 : 3; 
}

bool q2_filter_cdate_1(int *prop_ptr) {
    auto prop = (p_item*)prop_ptr;
    auto d = *reinterpret_cast<const ptime *>(prop->value_);
    auto dt = time_from_string(std::string("2011-04-14 01:51:21.746"));
    time_period duration(dt, hours(24*100));
    return duration.contains(d) ? true : false;
}

bool q2_filter_cdate_2(int *prop_ptr) {
    auto prop = (p_item*)prop_ptr;
    auto d = *reinterpret_cast<const ptime *>(prop->value_);
    auto dt1 = time_from_string(std::string("2011-04-14 01:51:21.746"));
    time_period duration1(dt1, hours(24*100));
    auto dt2 = duration1.last();
    time_period duration2(dt2, hours(24*100));
    return duration2.contains(d) ? true : false;
}

thread_local query_result qr;
query_result* q2_compute_diff(qr_tuple &q) {
    auto cnt = boost::get<uint64_t>(q.at(1));
    auto nxt_cnt = boost::get<uint64_t>(q.at(2));
    uint64_t diff = cnt > nxt_cnt ? cnt - nxt_cnt : nxt_cnt - cnt;
    qr = query_result(diff);
    return &qr;        
}


#endif
