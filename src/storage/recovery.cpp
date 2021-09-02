#include "recovery.hpp"

#include "nodes.hpp"
#include "relationships.hpp"
#include <iostream>

#ifdef QOP_RECOVERY
using namespace boost::posix_time;

query_argument_list::~query_argument_list() {

}

void query_argument_list::add(offset_t opid, offset_t value, offset_t type) {
    //args_.store({opid, value, type});
}

recovery_list::~recovery_list() {

}

void recovery_list::runtime_initialize() {
    /*std::cout << "RECOVER RESULTS" << std::endl;
    for(auto & v : results_) {
        std::cout << "[" << v.tuple_id_ << " "<< v.type_ << "], ";
    }
    std::cout << std::endl;*/
}

offset_t type_to_offset(query_result t) {
    if(t.type() == typeid(node*)){
        return 0;
    } else if(t.type() == typeid(relationship*)){
        return 1;
    } else if(t.type() == typeid(int)){
        return 2;
    } else if(t.type() == typeid(std::string)){
        return 3;
    } else if(t.type() == typeid(double)){
        return 4;
    } else if(t.type() == typeid(uint64_t)){
        return 5;
    } else if(t.type() == typeid(ptime)){
        return 6;
    }
}

uint64_t secondsSinceEpoch(ptime &time) {
    auto epoch = from_time_t(0);
    auto duration = time-epoch;
    return duration.total_seconds();
}

std::mutex rec_mtx;
std::vector<std::size_t> recovery_list::add(qr_tuple &&qr, dict &d, offset_t chunk) {
    std::lock_guard<std::mutex> lck(rec_mtx);
    std::vector<std::size_t> add_ids;
    for(auto & t : qr) {
        offset_t type;
        offset_t value;
        if(t.type() == typeid(node*)){
            type = 0;
            value = boost::get<node*>(t)->id();
        } else if(t.type() == typeid(relationship*)){
            type = 1;
            value = boost::get<relationship*>(t)->id();
        } else if(t.type() == typeid(int)){
            type = 2;
            value = boost::get<int>(t);
        } else if(t.type() == typeid(std::string)){
            auto str = boost::get<std::string>(t);
            value = d.lookup_string(str);
            if(value == 0) { // add string to dict
                value = d.insert(str);
            }
            type = 3;
        } else if(t.type() == typeid(double)){
            auto d = boost::get<double>(t);
            std::memcpy(&value, &d, sizeof(double));
            type = 4;
        } else if(t.type() == typeid(ptime)){
            auto time = boost::get<ptime>(t);
            value = secondsSinceEpoch(time);
            type = 6;
        } else if(t.type() == typeid(uint64_t)){
            auto u = boost::get<uint64_t>(t);
            value = u;
            type = 5;
        }

        intermediate_result ir;
        ir.tuple_id_ = tuple_cnt_.load();
        ir.res_ = value;
        ir.type_ = type;
        ir.chunk_ = chunk;

        auto store_id = results_.store(std::move(ir));
        
        add_ids.push_back(store_id.first);
    }
    std::atomic_fetch_add(&tuple_cnt_, 1);

    return add_ids;
}

void recovery_list::clear() {
    tuple_cnt_.store(0);
    results_.clear();
}

void recovery_list::update(std::size_t chunked_id, qr_tuple &qr, dict &d) {

}

intermediate_result &recovery_list::get(offset_t id) {
    if(results_.capacity() <= id)
        throw unknown_id();
    auto &r = results_.at(id);
    return r;
}

int recovery_list::get_stored_tuples() {
    return tuple_cnt_.load();
}
#endif