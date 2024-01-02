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

class ir_generator {
public:
    ir_generator(llvm::LLVMContext& ctx);
    ~ir_generator() = default;

    std::unique_ptr<llvm::Module> generate(expr ex, const std::string& fct_name);

    void dump(std::unique_ptr<llvm::Module>& module);

    llvm::LLVMContext& get_context() { return ctx_; }
    std::shared_ptr<llvm::IRBuilder<>> get_builder() { return builder_; }

    llvm::FunctionCallee extern_func(std::unique_ptr<llvm::Module>& module, const std::string& fct_name);

    llvm::Type *qctx_ptr_type;
    llvm::Type *qr_tuple_ptr_type;
    llvm::Type *node_ptr_type;

private:
    llvm::LLVMContext& ctx_;
    std::shared_ptr<llvm::IRBuilder<>> builder_;
    std::map<std::string, llvm::FunctionType *> func_types_;
 };

#endif