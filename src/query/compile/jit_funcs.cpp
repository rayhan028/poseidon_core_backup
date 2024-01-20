/*
 * Copyright (C) 2019-2023 DBIS Group - TU Ilmenau, All Rights Reserved.
 *
 * This file is part of the Poseidon package.
 *
 * Poseidon is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Poseidon is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Poseidon. If not, see <http://www.gnu.org/licenses/>.
 */
#include "exceptions.hpp"
#include "jit_funcs.hpp"

void print_node(query_ctx *ctx, node *n) {
    std::cout << "node @" << n->id() << ":" << ctx->gdb_->get_string(n->node_label) << std::endl;
}

int get_node_property_int_value(query_ctx *ctx, node *n, dcode_t label) {
    auto item = ctx->gdb_->get_node_properties()->property_value(n->property_list, label);
    return item.get<int>();
}

int get_int_property_value(query_ctx *ctx, qr_tuple* v, std::size_t i, dcode_t label) {
    if (v->at(i).which() == node_ptr_type) {
        auto n = boost::get<node *>(v->at(i));
        auto item = ctx->gdb_->get_node_properties()->property_value(n->property_list, label);
        if (item.typecode() != p_item::p_int)
            spdlog::info("unexpected property type: {}", item.typecode());
        return item.get<int>();     
    }
    else if (v->at(i).which() == rship_ptr_type) {
        auto r = boost::get<relationship *>(v->at(i));
        auto item = ctx->gdb_->get_rship_properties()->property_value(r->property_list, label);
        return item.get<int>(); 
    }
    throw invalid_typecast();
}

uint64_t get_uint64_property_value(query_ctx *ctx, qr_tuple* v, std::size_t i, dcode_t label) {
    if (v->at(i).which() == node_ptr_type) {
        auto n = boost::get<node *>(v->at(i));
        auto item = ctx->gdb_->get_node_properties()->property_value(n->property_list, label);
        return item.get<uint64_t>();     
    }
    else if (v->at(i).which() == rship_ptr_type) {
        auto r = boost::get<relationship *>(v->at(i));
        auto item = ctx->gdb_->get_rship_properties()->property_value(r->property_list, label);
        return item.get<uint64_t>(); 
    }
    spdlog::info("unexpected variable: ${} type={}", i, v->at(i).which());
    throw invalid_typecast();
}

uint64_t get_ptime_property_value(query_ctx *ctx, qr_tuple* v, std::size_t i, dcode_t label) {
    using namespace boost::posix_time;
    ptime pt;
    if (v->at(i).which() == node_ptr_type) {
        auto n = boost::get<node *>(v->at(i));
        auto item = ctx->gdb_->get_node_properties()->property_value(n->property_list, label);
        pt = item.get<ptime>();     
    }
    else if (v->at(i).which() == rship_ptr_type) {
        auto r = boost::get<relationship *>(v->at(i));
        auto item = ctx->gdb_->get_rship_properties()->property_value(r->property_list, label);
        pt = item.get<ptime>(); 
    }
    else
        throw invalid_typecast();

    static ptime epoch(boost::gregorian::date(1970, 1, 1));
    return (pt - epoch).total_milliseconds();
}

double get_double_property_value(query_ctx *ctx, qr_tuple* v, std::size_t i, dcode_t label) {
    if (v->at(i).which() == node_ptr_type) {
        auto n = boost::get<node *>(v->at(i));
        auto item = ctx->gdb_->get_node_properties()->property_value(n->property_list, label);
        return item.get<double>();     
    }
    else if (v->at(i).which() == rship_ptr_type) {
        auto r = boost::get<relationship *>(v->at(i));
        auto item = ctx->gdb_->get_rship_properties()->property_value(r->property_list, label);
        return item.get<double>(); 
    }
    throw invalid_typecast();
}

const char* get_string_property_value(query_ctx *ctx, qr_tuple* v, std::size_t i, dcode_t label) {
    if (v->at(i).which() == node_ptr_type) {
        auto n = boost::get<node *>(v->at(i));
        auto item = ctx->gdb_->get_node_properties()->property_value(n->property_list, label);
        auto code = item.get<dcode_t>();
        return ctx->get_string(code);
    }
    else if (v->at(i).which() == rship_ptr_type) {
        auto r = boost::get<relationship *>(v->at(i));
        auto item = ctx->gdb_->get_rship_properties()->property_value(r->property_list, label);
        auto code = item.get<dcode_t>();
        return ctx->get_string(code);
    }
    throw invalid_typecast();
}

uint64_t get_node_property_uint64_value(query_ctx *ctx, node *n, dcode_t label) {
    auto item = ctx->gdb_->get_node_properties()->property_value(n->property_list, label);
    return item.get<uint64_t>();
}

dcode_t get_node_property_string_value(query_ctx *ctx, node *n, dcode_t label) {
    auto item = ctx->gdb_->get_node_properties()->property_value(n->property_list, label);
    std::cout << "item: " << item << std::endl;
    return item.get<dcode_t>();
}
