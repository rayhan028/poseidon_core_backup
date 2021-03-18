#include "recovery.hpp"

#include "nodes.hpp"
#include "relationships.hpp"
#include <iostream>

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
    }
}

void recovery_list::add(qr_tuple &&qr, std::size_t chunk) {
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
            type = 3;
        } else if(t.type() == typeid(double)){
            auto d = boost::get<int>(t);
            std::memcpy(&value, &d, sizeof(double));
            type = 4;
        }
        results_.store({tuple_cnt_.load(), value, type});
    }
    std::atomic_fetch_add(&tuple_cnt_, 1);
}