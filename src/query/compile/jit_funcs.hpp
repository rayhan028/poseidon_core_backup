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

#include <cstring>
#include <regex>
#include "defs.hpp"
#include "query_ctx.hpp"
#include "qresult_iterator.hpp"
#include "nodes.hpp"

/**
 * This module provides a set of functions which are used as external functions in the
 * generated IR code.
 */

/* -----------------------------------------------------------------------------------*/

/**
 * Print the given node - only a helper function for debugging purposes.
 */
void print_node(query_ctx *ctx, node *n);

/**
 * Returns the value of an integer property named by label of a node/relationship pointer 
 * in the query_tuple v at index i.
 */
int get_int_property_value(query_ctx *ctx, qr_tuple* v, std::size_t i, dcode_t label);

/**
 * Returns the value of a double property named by label of a node/relationship pointer 
 * in the query_tuple v at index i.
 */
double get_double_property_value(query_ctx *ctx, qr_tuple* v, std::size_t i, dcode_t label);

/**
 * Returns the value of an uint64 property named by label of a node/relationship pointer 
 * in the query_tuple v at index i.
 */
uint64_t get_uint64_property_value(query_ctx *ctx, qr_tuple* v, std::size_t i, dcode_t label);

/**
 * Returns the value of an ptime property named by label of a node/relationship pointer 
 * in the query_tuple v at index i.
 */
uint64_t get_ptime_property_value(query_ctx *ctx, qr_tuple* v, std::size_t i, dcode_t label);

/**
 * Returns the value of a string property named by label of a node/relationship pointer 
 * in the query_tuple v at index i.
 */
const char* get_string_property_value(query_ctx *ctx, qr_tuple* v, std::size_t i, dcode_t label);

/**
 * Returns the int value of the query tuple v at index i.
 */
inline int qr_get_int(qr_tuple* v, std::size_t i) { return boost::get<int>(v->at(i)); }

/**
 * Returns the uint64 value of the query tuple v at index i.
 */
inline uint64_t qr_get_uint64(qr_tuple* v, std::size_t i) { return boost::get<uint64_t>(v->at(i)); }

/**
 * Returns the double value of the query tuple v at index i.
 */
inline double qr_get_double(qr_tuple* v, std::size_t i) { return boost::get<double>(v->at(i)); }

/**
 * Compares two strings represented by char *.
*/
inline int string_compare(uint8_t *s1, uint8_t *s2) { return std::strcmp((char *)s1, (char *)s2); }

inline int regex_match(uint8_t *s, uint8_t *re) { return std::regex_match((char *)s,  *((std::regex *)re)); }

int get_node_property_int_value(query_ctx *ctx, node *n, dcode_t label);

uint64_t get_node_property_uint64_value(query_ctx *ctx, node *n, dcode_t label);

dcode_t get_node_property_string_value(query_ctx *ctx, node *n, dcode_t label);

inline node* qr_get_node(qr_tuple* v, std::size_t i) { return boost::get<node*>(v->at(i)); }

#endif