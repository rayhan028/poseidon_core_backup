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
graph_db_ptr g;
//std::vector<params_tuple_bi> q1_params = {{time_from_string(std::string("2017-04-14 01:51:21.746"))}};
//std::vector<params_tuple_bi> q2_params = {{time_from_string(std::string("2011-04-14 01:51:21.746"))}};

bool q1_filter_cdate(int *prop_ptr) {
    auto prop = (p_item*)prop_ptr;
    return (*reinterpret_cast<const ptime *>(prop->value_)) < time_from_string(std::string("2017-04-14 01:51:21.746"));
    
}

int q1_get_node_property(node* pr) {
    auto nd = g->get_node_description(pr->id()); 
    return get_property<int>(nd.properties, "length").value();
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
    if(duration.contains(d)) std::cout << "TRUE1" << std::endl;
    return duration.contains(d) ? true : false;
}

bool q2_filter_cdate_2(int *prop_ptr) {
    auto prop = (p_item*)prop_ptr;
    auto d = *reinterpret_cast<const ptime *>(prop->value_);
    auto dt1 = time_from_string(std::string("2011-04-14 01:51:21.746"));
    time_period duration1(dt1, hours(24*100));
    auto dt2 = duration1.last();
    time_period duration2(dt2, hours(24*100));
    //if(duration2.contains(d)) std::cout << "TRUE2" << std::endl;
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

bool q3_filter_cntry(int *prop_ptr) {
    auto prop = (p_item*)prop_ptr;
    auto c1 = *(reinterpret_cast<const dcode_t *>(prop->value_));
    auto c2 = g->get_dictionary()->lookup_string("United_States");
    if(c1 == c2)
        std::cout << "Found" << std::endl;
    return c1 == c2;
}

bool q3_filter_tgclass(int *prop_ptr) {
    auto prop = (p_item*)prop_ptr;
    auto c1 = *(reinterpret_cast<const dcode_t *>(prop->value_));
    auto c2 = g->get_dictionary()->lookup_string("TennisPlayer");
    return c1 == c2;

}


#endif
