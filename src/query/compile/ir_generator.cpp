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

    auto qctx_ty = llvm::StructType::create(ctx_, "query_ctx");
    qctx_ptr_ty = llvm::PointerType::get(qctx_ty, 0);

    auto qr_tuple_ty = llvm::StructType::create(ctx_, "qr_tuple");
    qr_tuple_ptr_ty = llvm::PointerType::get(qr_tuple_ty, 0);

    auto node_ty = llvm::StructType::create(ctx_, "node"); 
    node_ptr_ty = llvm::PointerType::get(node_ty, 0);

    auto i32_ty = llvm::Type::getInt32Ty(ctx_);
    auto i64_ty = llvm::Type::getInt64Ty(ctx_);
    auto double_ty = llvm::Type::getDoubleTy(ctx_);

    auto i8_ty  = llvm::Type::getInt8Ty(ctx_);
    auto i8_ptr_ty = llvm::PointerType::get(i8_ty, 0);

    // node* qr_get_node(qr_tuple*, std::size_t)
    auto qr_get_node_func_ty = llvm::FunctionType::get(node_ptr_ty, {qr_tuple_ptr_ty, i32_ty}, false);
    func_types_["qr_get_node"] = qr_get_node_func_ty;

    // int get_node_property_int_value(query_ctx *, node *, dcode_t)
    auto get_node_property_int_value_func_ty = llvm::FunctionType::get(i64_ty, { qctx_ptr_ty, node_ptr_ty, i32_ty}, false);
    func_types_["get_node_property_int_value"] = get_node_property_int_value_func_ty;

    // int get_int_property_value(query_ctx *ctx, qr_tuple* v, std::size_t i, dcode_t label)
    auto get_int_property_value_func_ty = llvm::FunctionType::get(i64_ty, { qctx_ptr_ty, qr_tuple_ptr_ty, i32_ty, i32_ty}, false);
    func_types_["get_int_property_value"] = get_int_property_value_func_ty;

    // uint64_t get_uint64_property_value(query_ctx *ctx, qr_tuple* v, std::size_t i, dcode_t label)
    auto get_uint64_property_value_func_ty = llvm::FunctionType::get(i64_ty, { qctx_ptr_ty, qr_tuple_ptr_ty, i32_ty, i32_ty}, false);
    func_types_["get_uint64_property_value"] = get_uint64_property_value_func_ty;

    // double get_double_property_value(query_ctx *ctx, qr_tuple* v, std::size_t i, dcode_t label)
    auto get_double_property_value_func_ty = llvm::FunctionType::get(double_ty, { qctx_ptr_ty, qr_tuple_ptr_ty, i32_ty, i32_ty}, false);
    func_types_["get_double_property_value"] = get_double_property_value_func_ty;

    // const char* get_string_property_value(query_ctx *ctx, qr_tuple* v, std::size_t i, dcode_t label)
    auto get_string_property_value_func_ty = llvm::FunctionType::get(i8_ptr_ty, { qctx_ptr_ty, qr_tuple_ptr_ty, i32_ty, i32_ty}, false);
    func_types_["get_string_property_value"] = get_string_property_value_func_ty;

    auto string_compare_ty = llvm::FunctionType::get(i32_ty, { i8_ptr_ty, i8_ptr_ty }, false);
    func_types_["string_compare"] = string_compare_ty;
}

llvm::FunctionCallee ir_generator::extern_func(std::unique_ptr<llvm::Module>& module, const std::string& fct_name) {
    return module->getOrInsertFunction(fct_name, func_types_[fct_name]);
}

std::unique_ptr<llvm::Module> ir_generator::generate(expr ex, const std::string& fct_name) {
    auto module = std::make_unique<llvm::Module>("MyModule", ctx_);

    auto target_triple = LLVMGetDefaultTargetTriple();
    module->setTargetTriple(target_triple);
 
    std::vector<llvm::Type*> params {qctx_ptr_ty, qr_tuple_ptr_ty};

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

std::unique_ptr<llvm::Module> ir_generator::generate(std::shared_ptr<aggregate> aggr, const std::string& fct_name) {
    auto i8_ty = llvm::Type::getInt8Ty(ctx_);
    auto i32_ty = llvm::Type::getInt32Ty(ctx_);
    auto i64_ty = llvm::Type::getInt64Ty(ctx_);
    auto double_ty = llvm::Type::getDoubleTy(ctx_);
    auto i8_ptr_ty = llvm::PointerType::get(i8_ty, 0);

    std::vector<llvm::Type*> elements;
    for (auto& ex : aggr->aggr_exprs_) {
        if (ex.func != aggregate::expr::f_avg) {
            switch(ex.aggr_type) {
            case int_type:
            case uint64_type:
            case node_ptr_type:
            case rship_ptr_type:
                elements.push_back(i64_ty); break;
            case double_type:
                elements.push_back(double_ty); break;
            default:
                break;
            }
        }
        else { // f_avg
            // 1. sum 
            switch(ex.aggr_type) {
            case int_type:
            case uint64_type:
            case node_ptr_type:
            case rship_ptr_type:
                elements.push_back(i64_ty);
                break;
            case double_type:
                elements.push_back(double_ty);
                break;
            default:
                break;
            }    
            // 2. count   
            elements.push_back(i64_ty);
            // 3. avg     
            elements.push_back(double_ty);
        }
    }
    auto aggr_ty = llvm::StructType::create(llvm::ArrayRef<llvm::Type *>(elements), "Agg");
    auto aggr_ptr_ty = llvm::PointerType::get(aggr_ty, 0);


    auto module = std::make_unique<llvm::Module>("MyModule3", ctx_);
    auto target_triple = LLVMGetDefaultTargetTriple();
    module->setTargetTriple(target_triple);

    auto int_0 = llvm::ConstantInt::get(ctx_, llvm::APInt(32, 0, true));
    auto long_0 = llvm::ConstantInt::get(ctx_, llvm::APInt(64, 0, true));
    auto long_min = llvm::ConstantInt::get(ctx_, llvm::APInt(64, std::numeric_limits<int64_t>::min(), true));
    auto long_max = llvm::ConstantInt::get(ctx_, llvm::APInt(64, std::numeric_limits<int64_t>::max(), true));
    auto double_0 = llvm::ConstantFP::get(ctx_, llvm::APFloat(0.0));
    auto double_min = llvm::ConstantFP::get(ctx_, llvm::APFloat(std::numeric_limits<double>::min()));
    auto double_max = llvm::ConstantFP::get(ctx_, llvm::APFloat(std::numeric_limits<double>::max()));

    // init function
    {
        std::vector<llvm::Type*> params {i8_ptr_ty, i32_ty};
        llvm::FunctionType *ft = llvm::FunctionType::get(llvm::Type::getVoidTy(ctx_), params, false);
        llvm::Function *start = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, fct_name + "_init", module.get());

        llvm::BasicBlock *bb = llvm::BasicBlock::Create(ctx_, "", start);
        builder_->SetInsertPoint(bb);
        auto base_ptr = builder_->CreateBitCast(start->getArg(0), aggr_ptr_ty); // bitcast -> base_ptr

        int idx = 0;
        for (auto& ex : aggr->aggr_exprs_) {
            switch(ex.func) {
            case aggregate::expr::f_sum:
            case aggregate::expr::f_count:
            {
                llvm::ArrayRef<llvm::Value*> indices {
                    int_0, llvm::ConstantInt::get(ctx_, llvm::APInt(32, idx++, true)), 
                };
                auto ptr = builder_->CreateInBoundsGEP(aggr_ty, base_ptr, indices);
                if (ex.aggr_type == double_type && ex.func == aggregate::expr::f_sum)
                    builder_->CreateStore(double_0, ptr); 
                else
                    builder_->CreateStore(long_0, ptr);
                break;
            }
            case aggregate::expr::f_min:
            {
                llvm::ArrayRef<llvm::Value*> indices {
                    int_0, llvm::ConstantInt::get(ctx_, llvm::APInt(32, idx++, true)), 
                };
                auto ptr = builder_->CreateInBoundsGEP(aggr_ty, base_ptr, indices);
                if (ex.aggr_type == double_type)
                    builder_->CreateStore(double_max, ptr); 
                else
                    builder_->CreateStore(long_max, ptr); 
                break;
            }
            case aggregate::expr::f_max:
            {
                llvm::ArrayRef<llvm::Value*> indices {
                    int_0, llvm::ConstantInt::get(ctx_, llvm::APInt(32, idx++, true)), 
                };
                auto ptr = builder_->CreateInBoundsGEP(aggr_ty, base_ptr, indices);
                if (ex.aggr_type == double_type)
                    builder_->CreateStore(double_min, ptr); 
                else
                    builder_->CreateStore(long_min, ptr); 
                break;
            }
            case aggregate::expr::f_avg:
            {
                llvm::ArrayRef<llvm::Value*> indices1 {
                    int_0, llvm::ConstantInt::get(ctx_, llvm::APInt(32, idx++, true)), 
                };              
                auto ptr1 = builder_->CreateInBoundsGEP(aggr_ty, base_ptr, indices1); 
                builder_->CreateStore(long_0, ptr1); 
               
                llvm::ArrayRef<llvm::Value*> indices2 {
                    int_0, llvm::ConstantInt::get(ctx_, llvm::APInt(32, idx++, true)), 
                };                
                auto ptr2 = builder_->CreateInBoundsGEP(aggr_ty, base_ptr, indices2); 
                builder_->CreateStore(long_0, ptr2); 

               llvm::ArrayRef<llvm::Value*> indices3 {
                    int_0, llvm::ConstantInt::get(ctx_, llvm::APInt(32, idx++, true)), 
                };
                auto ptr3 = builder_->CreateInBoundsGEP(aggr_ty, base_ptr, indices3); 
                builder_->CreateStore(double_0, ptr3); 
               
                break;
            }
            default:
                break;
            }

        }

        builder_->CreateRetVoid();
    }

    // iterate function
    {
        std::vector<llvm::Type*> params {qctx_ptr_ty, i8_ptr_ty, i32_ty, qr_tuple_ptr_ty};
        llvm::FunctionType *ft = llvm::FunctionType::get(llvm::Type::getVoidTy(ctx_), params, false);
        llvm::Function *start = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, fct_name + "_iterate", module.get());

        llvm::BasicBlock *bb = llvm::BasicBlock::Create(ctx_, "", start);
        builder_->SetInsertPoint(bb);

        auto base_ptr = builder_->CreateBitCast(start->getArg(1), aggr_ptr_ty); // bitcast -> base_ptr

        auto idx = 0u;
        for (auto& ex : aggr->aggr_exprs_) {
            switch(ex.func) {
            case aggregate::expr::f_count:
                generate_count_iterate(ex, aggr_ty, base_ptr, idx++);
                break;
            case aggregate::expr::f_min:
                generate_min_iterate(module, ex, start, aggr_ty, base_ptr, idx++);
                break;
            case aggregate::expr::f_max:
                generate_max_iterate(module, ex, start, aggr_ty, base_ptr, idx++);
                break;

            case aggregate::expr::f_sum:
                generate_sum_iterate(module, ex, start, aggr_ty, base_ptr, idx++);
                break;
            case aggregate::expr::f_avg:
                generate_avg_iterate(module, ex, start, aggr_ty, base_ptr, idx);
                idx += 3;
                break;
            default:
                break;
            }
        }

        builder_->CreateRetVoid();
    }

    // finish function
    {
        std::vector<llvm::Type*> params {qctx_ptr_ty, i8_ptr_ty, i32_ty};
        llvm::FunctionType *ft = llvm::FunctionType::get(llvm::Type::getVoidTy(ctx_), params, false);
        llvm::Function *start = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, fct_name + "_finish", module.get());

        llvm::BasicBlock *bb = llvm::BasicBlock::Create(ctx_, "", start);
        builder_->SetInsertPoint(bb);

        auto base_ptr = builder_->CreateBitCast(start->getArg(1), aggr_ptr_ty); // bitcast -> base_ptr

        auto idx = 0u;
        for (auto& ex : aggr->aggr_exprs_) {
            if (ex.func != aggregate::expr::f_avg) {
                idx++;
                continue;
            }
            llvm::ArrayRef<llvm::Value*> indices1 {
                int_0, llvm::ConstantInt::get(ctx_, llvm::APInt(32, idx++, true)), 
            };
            auto ptr1 = builder_->CreateInBoundsGEP(aggr_ty, base_ptr, indices1); // GEP
            auto val1 = builder_->CreateLoad(ptr1);

            llvm::ArrayRef<llvm::Value*> indices2 {
                int_0, llvm::ConstantInt::get(ctx_, llvm::APInt(32, idx++, true)), 
            };
            auto ptr2 = builder_->CreateInBoundsGEP(aggr_ty, base_ptr, indices2); // GEP
            auto val2 = builder_->CreateLoad(ptr2);
            auto fval2 = builder_->CreateSIToFP(val2, double_ty);

            llvm::Value *res = nullptr;
            if (ex.aggr_type != double_type) {
                auto fval1 = builder_->CreateSIToFP(val1, double_ty);
                res = builder_->CreateFDiv(fval1, fval2);
            }
            else 
                res = builder_->CreateFDiv(val1, fval2);

            llvm::ArrayRef<llvm::Value*> indices3 {
                int_0, llvm::ConstantInt::get(ctx_, llvm::APInt(32, idx++, true)), 
            };
            auto ptr3 = builder_->CreateInBoundsGEP(aggr_ty, base_ptr, indices3); // GEP
            builder_->CreateStore(res, ptr3); // store
        }

        builder_->CreateRetVoid();
    }

    return module;     
}

void ir_generator::generate_count_iterate(aggregate::expr& ex, llvm::StructType *aggr_ty, llvm::Value *base_ptr, uint32_t idx) {
    auto ptr = builder_->CreateInBoundsGEP(aggr_ty, base_ptr, {
        llvm::ConstantInt::get(ctx_, llvm::APInt(32, 0, true)), 
        llvm::ConstantInt::get(ctx_, llvm::APInt(32, idx, true))}); // GEP
    auto aggr_val = builder_->CreateLoad(ptr);
    auto res = builder_->CreateAdd(llvm::ConstantInt::get(ctx_, llvm::APInt(64, 1, true)), aggr_val);
    builder_->CreateStore(res, ptr); // store
}

void ir_generator::generate_sum_iterate(std::unique_ptr<llvm::Module>& module, aggregate::expr& ex, 
    llvm::Function *start, llvm::StructType *aggr_ty, llvm::Value *base_ptr, uint32_t idx) {
    llvm::Value *pval = generate_get_value(module, ex, start);

    auto ptr = builder_->CreateInBoundsGEP(aggr_ty, base_ptr, {
        llvm::ConstantInt::get(ctx_, llvm::APInt(32, 0, true)), 
        llvm::ConstantInt::get(ctx_, llvm::APInt(32, idx, true))}); // GEP
    auto aggr_val = builder_->CreateLoad(ptr);
    auto res = builder_->CreateAdd(pval, aggr_val);
    builder_->CreateStore(res, ptr); // store
}

void ir_generator::generate_min_iterate(std::unique_ptr<llvm::Module>& module, aggregate::expr& ex, 
    llvm::Function *start, llvm::StructType *aggr_ty, llvm::Value *base_ptr, uint32_t idx) {
    llvm::Value *pval = generate_get_value(module, ex, start);

    auto ptr = builder_->CreateInBoundsGEP(aggr_ty, base_ptr, {
        llvm::ConstantInt::get(ctx_, llvm::APInt(32, 0, true)), 
        llvm::ConstantInt::get(ctx_, llvm::APInt(32, idx, true))}); // GEP
    auto aggr_val = builder_->CreateLoad(ptr);
    // TODO: compare!!
    auto res = builder_->CreateAdd(pval, aggr_val);
    builder_->CreateStore(res, ptr); // store
}

void ir_generator::generate_max_iterate(std::unique_ptr<llvm::Module>& module, aggregate::expr& ex, 
    llvm::Function *start, llvm::StructType *aggr_ty, llvm::Value *base_ptr, uint32_t idx) {
    llvm::Value *pval = generate_get_value(module, ex, start);

    auto ptr = builder_->CreateInBoundsGEP(aggr_ty, base_ptr, {
        llvm::ConstantInt::get(ctx_, llvm::APInt(32, 0, true)), 
        llvm::ConstantInt::get(ctx_, llvm::APInt(32, idx, true))}); // GEP
    auto aggr_val = builder_->CreateLoad(ptr);
    // TODO: compare!!
    auto res = builder_->CreateAdd(pval, aggr_val);
    builder_->CreateStore(res, ptr); // store
}

void ir_generator::generate_avg_iterate(std::unique_ptr<llvm::Module>& module, aggregate::expr& ex, 
    llvm::Function *start, llvm::StructType *aggr_ty, llvm::Value *base_ptr, uint32_t idx) {
    llvm::Value *pval = generate_get_value(module, ex, start);

    auto ptr1 = builder_->CreateInBoundsGEP(aggr_ty, base_ptr, {
        llvm::ConstantInt::get(ctx_, llvm::APInt(32, 0, true)), 
        llvm::ConstantInt::get(ctx_, llvm::APInt(32, idx, true))}); // GEP
    auto aggr_val1 = builder_->CreateLoad(ptr1);
    auto res1 = builder_->CreateAdd(pval, aggr_val1);
    builder_->CreateStore(res1, ptr1); // store

    auto ptr2 = builder_->CreateInBoundsGEP(aggr_ty, base_ptr, {
        llvm::ConstantInt::get(ctx_, llvm::APInt(32, 0, true)), 
        llvm::ConstantInt::get(ctx_, llvm::APInt(32, idx+1, true))}); // GEP
    auto aggr_val2 = builder_->CreateLoad(ptr2);
    auto res2 = builder_->CreateAdd(llvm::ConstantInt::get(ctx_, llvm::APInt(64, 1, true)), aggr_val2);
    builder_->CreateStore(res2, ptr2); // store
}

llvm::Value *ir_generator::generate_get_value(std::unique_ptr<llvm::Module>& module, aggregate::expr& ex, 
    llvm::Function *start) {
    llvm::Value *res = nullptr;
    if (ex.pkey == UNKNOWN_CODE) {
        //$i refers to a primitive value
        llvm::FunctionCallee callee;
        switch(ex.aggr_type) {
        case int_type:
            callee = extern_func(module, "qr_get_int");
            break;
        case uint64_type:
            callee = extern_func(module, "qr_get_uint64");
            break;
        case double_type:
            callee = extern_func(module, "qr_get_double");
            break;
        default:
            break;
        }
        llvm::Value *val = llvm::ConstantInt::get(ctx_, llvm::APInt(32, ex.var, true));
        res = builder_->CreateCall(callee, { start->getArg(3), val});
    }
    else {
        // $i.property: $i refers to node* or relationship*
        // TODO: handle other types
        llvm::Value *val1 = llvm::ConstantInt::get(ctx_, llvm::APInt(32, ex.var, true));
        llvm::Value *val2 = llvm::ConstantInt::get(ctx_, llvm::APInt(32, ex.pkey, true));
        llvm::FunctionCallee callee;
        
        switch(ex.aggr_type) {
        case double_type:
            callee = extern_func(module, "get_double_property_value");
            break;
        case uint64_type:
            callee = extern_func(module, "get_uint64_property_value");
            break;
        case int_type:
            callee = extern_func(module, "get_int_property_value");
            break;
        default:
            break;
        }        
        res = builder_->CreateCall(callee, { start->getArg(0), start->getArg(3), val1, val2 });
    }
    return res;
}

void ir_generator::dump(std::unique_ptr<llvm::Module>& module) {
    std::string errors;
    llvm::raw_string_ostream out(errors);

    if (llvm::verifyModule(*module, &out)) {
        spdlog::info("ERROR in code generation: {}", out.str());
    }

    module->print(llvm::outs(), nullptr);
}