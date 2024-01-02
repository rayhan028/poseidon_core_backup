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
#include <llvm/IR/Verifier.h>
#include <llvm/ADT/StringRef.h>

#include "ir_generator.hpp"
#include "expr_codegen.hpp"

ir_generator::ir_generator(llvm::LLVMContext& ctx) : ctx_(ctx) {
    builder_ = std::make_shared<llvm::IRBuilder<>>(ctx_);

    auto qctx_type = llvm::StructType::create(ctx_, "query_ctx");
    qctx_ptr_type = llvm::PointerType::get(qctx_type, 0);

    auto qr_tuple_type = llvm::StructType::create(ctx_, "qr_tuple");
    qr_tuple_ptr_type = llvm::PointerType::get(qr_tuple_type, 0);

    auto node_type = llvm::StructType::create(ctx_, "node"); 
    node_ptr_type = llvm::PointerType::get(node_type, 0);

    auto i32_type = llvm::Type::getInt32Ty(ctx_);
    auto i64_type = llvm::Type::getInt64Ty(ctx_);

    // node* qr_get_node(qr_tuple*, std::size_t)
    auto qr_get_node_func_type = llvm::FunctionType::get(node_ptr_type, {qr_tuple_ptr_type, i32_type}, false);
    func_types_["qr_get_node"] = qr_get_node_func_type;

    // int get_node_property_int_value(query_ctx *, node *, dcode_t)
    auto get_node_property_int_value_func_type = llvm::FunctionType::get(i64_type, { qctx_ptr_type, node_ptr_type, i32_type}, false);
    func_types_["get_node_property_int_value"] = get_node_property_int_value_func_type;
}

llvm::FunctionCallee ir_generator::extern_func(std::unique_ptr<llvm::Module>& module, const std::string& fct_name) {
    return module->getOrInsertFunction(fct_name, func_types_[fct_name]);
}

std::unique_ptr<llvm::Module> ir_generator::generate(expr ex, const std::string& fct_name) {
    auto module = std::make_unique<llvm::Module>(llvm::StringRef("MyModule2"), ctx_);

    auto target_triple = LLVMGetDefaultTargetTriple();
    module->setTargetTriple(target_triple);
 
    std::vector<llvm::Type*> params {qctx_ptr_type, qr_tuple_ptr_type};

    llvm::FunctionType *ft = llvm::FunctionType::get(llvm::Type::getInt1Ty(ctx_), params, false);
    llvm::Function *start = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, fct_name, module.get());

    llvm::BasicBlock *bb = llvm::BasicBlock::Create(ctx_, "init", start);
    llvm::BasicBlock *finish_false = llvm::BasicBlock::Create(ctx_, "finish_false", start);
    llvm::BasicBlock *finish_true = llvm::BasicBlock::Create(ctx_, "finish_true", start);

    builder_->SetInsertPoint(bb);
    // expression init  
    expr_codegen codegen(*this, module, start);
    llvm::Value *condition = static_cast<llvm::Value*>(ex->accept(codegen));

    builder_->CreateCondBr(condition, finish_true, finish_false);

    // expression finish
    builder_->SetInsertPoint(finish_false);
    builder_->CreateRet(llvm::ConstantInt::get(ctx_, llvm::APInt(1, 0, true)));

    builder_->SetInsertPoint(finish_true);
    builder_->CreateRet(llvm::ConstantInt::get(ctx_, llvm::APInt(1, 1, true)));

    return module;
}

void ir_generator::dump(std::unique_ptr<llvm::Module>& module) {
    std::string errors;
    llvm::raw_string_ostream out(errors);

    if (llvm::verifyModule(*module, &out)) {
        spdlog::info("ERROR in code generation: {}", out.str());
    }

    module->print(llvm::outs(), nullptr);
}