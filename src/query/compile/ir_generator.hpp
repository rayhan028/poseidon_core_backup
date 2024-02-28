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
#ifndef ir_generator_hpp_
#define ir_generator_hpp_

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>

#include <map>

#include "jit_engine.hpp"
#include "expression.hpp"
#include "qop.hpp"
#include "qop_aggregates.hpp"

/**
 * ir_generator encapsulates the IR code generation for query operators.
 */
class ir_generator {
public:
    ir_generator(llvm::LLVMContext& ctx);
    ~ir_generator() = default;

    /**
     * Generates a LLVM module providing a function for evaluating the expression of the given filter operator.
     */
    std::unique_ptr<llvm::Module> generate(expr ex, const std::string& fct_name);

    /**
     * Generates a LLVM module providing the functions (init, iterate, finish) for the given aggregate operator.
     */
    std::unique_ptr<llvm::Module> generate(std::shared_ptr<aggregate> aggr, const std::string& fct_name);

    /**
     * Prints the IR text of the given module.
     */
    void dump(std::unique_ptr<llvm::Module>& module);

    /**
     * Returns the LLVM context.
     */
    llvm::LLVMContext& get_context() { return ctx_; }

    /**
     * Returns the IR builder for code generation.
     */
    std::shared_ptr<llvm::IRBuilder<>> get_builder() { return builder_; }

    /**
     * Returns a handle to the external function given by fct_name.
     */
    llvm::FunctionCallee extern_func(std::unique_ptr<llvm::Module>& module, const std::string& fct_name);

    /**
     * Predefined (opaque) IR types for query_ctx*, qr_tuple*, and node*.
     */
    llvm::Type *qctx_ptr_ty;
    llvm::Type *qr_tuple_ptr_ty;
    llvm::Type *node_ptr_ty;

private:
    void generate_count_iterate(aggregate::expr& ex, llvm::StructType *aggr_ty, llvm::Value *base_ptr, uint32_t idx);
    void generate_sum_iterate(std::unique_ptr<llvm::Module>& module, aggregate::expr& ex, 
        llvm::Function *start, llvm::StructType *aggr_ty, llvm::Type *elem_ty, llvm::Value *base_ptr, uint32_t idx);
    void generate_avg_iterate(std::unique_ptr<llvm::Module>& module, aggregate::expr& ex, 
        llvm::Function *start, llvm::StructType *aggr_ty, llvm::Type *elem_ty, llvm::Value *base_ptr, uint32_t idx);
    void generate_min_iterate(std::unique_ptr<llvm::Module>& module, aggregate::expr& ex, 
        llvm::Function *start, llvm::StructType *aggr_ty, llvm::Value *base_ptr, uint32_t idx);
    void generate_max_iterate(std::unique_ptr<llvm::Module>& module, aggregate::expr& ex, 
        llvm::Function *start, llvm::StructType *aggr_ty, llvm::Value *base_ptr, uint32_t idx);
    llvm::Value *generate_get_value(std::unique_ptr<llvm::Module>& module, aggregate::expr& ex, llvm::Function *start);

    llvm::LLVMContext& ctx_;
    std::shared_ptr<llvm::IRBuilder<>> builder_;
    std::map<std::string, llvm::FunctionType *> func_types_;
 };

#endif