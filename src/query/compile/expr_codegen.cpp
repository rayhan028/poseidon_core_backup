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
#include "expr_codegen.hpp"

expr_codegen::expr_codegen(ir_generator& ir, std::unique_ptr<llvm::Module>& module, llvm::Function *start) : 
    gen_(ir), module_(module), start_(start) {
}

void* expr_codegen::visit(std::shared_ptr<number_token> op) {
    spdlog::info("expr_codegen: number_token: lvalue={}, ivalue={} (type={})", op->lvalue_, op->ivalue_, (int)op->ftype_);
    if (op->ftype_ == expr_type::INT)
        return llvm::ConstantInt::get(gen_.get_context(), llvm::APInt(64, op->ivalue_, true));
    else if (op->ftype_ == expr_type::UINT64)
        return llvm::ConstantInt::get(gen_.get_context(), llvm::APInt(64, op->lvalue_, true));
    else
        return llvm::ConstantFP::get(gen_.get_context(), llvm::APFloat(op->dvalue_));
}

void* expr_codegen::visit(std::shared_ptr<variable> op) {
    spdlog::info("expr_codegen: variable");    
    llvm::FunctionCallee qr_get_node_func = gen_.extern_func(module_, "qr_get_node");
    llvm::Value *val1 = llvm::ConstantInt::get(gen_.get_context(), llvm::APInt(32, op->id_, true));
    auto node_ptr = gen_.get_builder()->CreateCall(qr_get_node_func, { start_->getArg(1), val1});

    llvm::FunctionCallee get_node_property_int_value_func = gen_.extern_func(module_, "get_node_property_int_value");
    llvm::Value *val2 = llvm::ConstantInt::get(gen_.get_context(), llvm::APInt(32, op->pcode_, true));
    return gen_.get_builder()->CreateCall(get_node_property_int_value_func, { start_->getArg(0), node_ptr, val2 });
}

void* expr_codegen::visit(std::shared_ptr<str_token> op) {}

void* expr_codegen::visit(std::shared_ptr<time_token> op) {}
    
void* expr_codegen::visit(std::shared_ptr<fct_call> op) {}

void* expr_codegen::visit(std::shared_ptr<func_call> op) {}

void* expr_codegen::visit(std::shared_ptr<eq_predicate> op) {
    spdlog::info("expr_codegen: eq_predicate");
    auto lhs = static_cast<llvm::Value*>(op->left_->accept(*this));
    auto rhs = static_cast<llvm::Value*>(op->right_->accept(*this));
    return gen_.get_builder()->CreateICmpEQ(lhs, rhs);
}  

void* expr_codegen::visit(std::shared_ptr<le_predicate> op) {
    spdlog::info("expr_codegen: le_predicate");
    auto lhs = static_cast<llvm::Value*>(op->left_->accept(*this));
    auto rhs = static_cast<llvm::Value*>(op->right_->accept(*this));
    return gen_.get_builder()->CreateICmpSLE(lhs, rhs);
}
    
void* expr_codegen::visit(std::shared_ptr<lt_predicate> op) {
    spdlog::info("expr_codegen: lt_predicate");
    auto lhs = static_cast<llvm::Value*>(op->left_->accept(*this));
    auto rhs = static_cast<llvm::Value*>(op->right_->accept(*this));
    return gen_.get_builder()->CreateICmpSLT(lhs, rhs);

}

void* expr_codegen::visit(std::shared_ptr<ge_predicate> op) {
    spdlog::info("expr_codegen: ge_predicate");
    auto lhs = static_cast<llvm::Value*>(op->left_->accept(*this));
    auto rhs = static_cast<llvm::Value*>(op->right_->accept(*this));
    return gen_.get_builder()->CreateICmpSGE(lhs, rhs);

}

void* expr_codegen::visit(std::shared_ptr<gt_predicate> op) {
    spdlog::info("expr_codegen: gt_predicate");
    auto lhs = static_cast<llvm::Value*>(op->left_->accept(*this));
    auto rhs = static_cast<llvm::Value*>(op->right_->accept(*this));
    return gen_.get_builder()->CreateICmpSGT(lhs, rhs);
}

void* expr_codegen::visit(std::shared_ptr<and_predicate> op) {}

void* expr_codegen::visit(std::shared_ptr<or_predicate> op) {}

void* expr_codegen::visit(std::shared_ptr<call_predicate> op) {}