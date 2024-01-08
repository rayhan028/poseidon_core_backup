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
#ifndef expr_codegen_hpp_
#define expr_codegen_hpp_

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>

#include "expression.hpp"
#include "binary_expression.hpp"
#include "ir_generator.hpp"

/**
 * expr_codegen encapsulates the IR code generation for expressions. It implements an expression visitor.
 */
class expr_codegen : public expression_visitor, public std::enable_shared_from_this<expr_codegen> {
public:
    expr_codegen(ir_generator& gen, std::unique_ptr<llvm::Module>& module, llvm::Function *start);

    void* visit(std::shared_ptr<number_literal> op) override;
    void* visit(std::shared_ptr<variable> op) override;
    void* visit(std::shared_ptr<string_literal> op) override;
    void* visit(std::shared_ptr<time_literal> op) override;
    void* visit(std::shared_ptr<func_call> op) override;
    void* visit(std::shared_ptr<eq_predicate> op) override;  
    void* visit(std::shared_ptr<neq_predicate> op) override;  
    void* visit(std::shared_ptr<le_predicate> op) override;
    void* visit(std::shared_ptr<lt_predicate> op) override;
    void* visit(std::shared_ptr<ge_predicate> op) override;
    void* visit(std::shared_ptr<gt_predicate> op) override;
    void* visit(std::shared_ptr<and_predicate> op) override;
    void* visit(std::shared_ptr<or_predicate> op) override;

private:
    ir_generator& gen_; // the actual code generator
    std::unique_ptr<llvm::Module>& module_; // the LLVM module to which the generated code belongs
    llvm::Function *start_; // the function to which the generated code belongs
};

#endif