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
#ifndef jit_engine_hpp_
#define jit_engine_hpp_

#include <llvm/IR/LLVMContext.h>

#include "defs.hpp"
#include "graph_db.hpp"
#include "query_ctx.hpp"
#include "qresult_iterator.hpp"
#include "jit_compiler.hpp"


/**
 * A class for loading and executing LLVM IR code used in Poseidon queries.
 */
class jit_engine {
public:
    /**
     * Function type for predicates in filter operator.
     */
    using predicate_fptr = bool(*)(const query_ctx*, const qr_tuple*);

    /**
     * Function types for init, iterate, finish functions in aggregate operator.
    */
    using aggr_init_fptr = void(*)(uint8_t*, uint32_t);
    using aggr_iterate_fptr = void(*)(const query_ctx*, uint8_t*, uint32_t, const qr_tuple*);
    using aggr_finish_fptr = void(*)(const query_ctx*, uint8_t*, uint32_t);

    jit_engine(graph_db_ptr gdb);
    ~jit_engine() = default;

    /**
     * Returns the LLVM context for IR code generation.
     */
    llvm::LLVMContext& get_context() { return ctx_; }

    /**
     * Load IR code from a file - used mainly for debugging purpose.
    */
    bool load_code(const std::string& filename);

    /**
     * Add an IR module to the engine. The module has to be created before
     * using ir_generator.
     */
    bool add_module(std::unique_ptr<llvm::Module> m);

    /**
     * Return a function with the given name implementing a filter predicate.
     */
    predicate_fptr get_predicate_function(const std::string& fname);

    /**
     * Return the triple of functions implementing the aggregate functions. 
     * The name is the prefix of the functions where _init, _iterate, and _finish
     * is added.
     */
    std::tuple<aggr_init_fptr, aggr_iterate_fptr, aggr_finish_fptr> 
        get_aggregate_functions(const std::string& fname);

    /**
     * Remove all generated modules from the JIT - used after the query was finished.
     */
    void clear() { /*auto res = jit_->clear();*/ }
    
private:
    graph_db_ptr gdb_; // the graph database
    llvm::LLVMContext ctx_; // the LLVM context
    std::unique_ptr<jit_compiler> jit_; // the actual LLVM JIT compiler
};

#endif