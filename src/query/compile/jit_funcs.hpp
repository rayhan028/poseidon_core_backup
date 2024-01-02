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
#ifndef jit_funcs_hpp_
#define jit_funcs_hpp_

#include "defs.hpp"
#include "query_ctx.hpp"
#include "qresult_iterator.hpp"
#include "nodes.hpp"

void print_node(query_ctx *ctx, node *n);

int get_node_property_int_value(query_ctx *ctx, node *n, dcode_t label);

uint64_t get_node_property_uint64_value(query_ctx *ctx, node *n, dcode_t label);

dcode_t get_node_property_string_value(query_ctx *ctx, node *n, dcode_t label);

node* qr_get_node(qr_tuple* v, std::size_t i);

#endif