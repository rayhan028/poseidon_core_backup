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
#include <boost/date_time/posix_time/posix_time.hpp>

expr_codegen::expr_codegen(ir_generator& ir, std::unique_ptr<llvm::Module>& module, llvm::Function *start) : 
    gen_(ir), module_(module), start_(start) {
}

/**
 * Generates the code for a numeric constant in an expression.
 */
void* expr_codegen::visit(std::shared_ptr<number_literal> op) {
    if (op->ftype_ == expr_type::INT)
        return llvm::ConstantInt::get(gen_.get_context(), llvm::APInt(64, op->ivalue_, true));
    else if (op->ftype_ == expr_type::UINT64)
        return llvm::ConstantInt::get(gen_.get_context(), llvm::APInt(64, op->lvalue_, true));
    else
        return llvm::ConstantFP::get(gen_.get_context(), llvm::APFloat(op->dvalue_));
}

/**
 * Generates the code for a variable in an expression like $0 or $1.name.
 */
void* expr_codegen::visit(std::shared_ptr<variable> op) { 
    // TODO: handle other types than int
    // TODO: handle variables of other types than node* or relationship*
    llvm::Value *val1 = llvm::ConstantInt::get(gen_.get_context(), llvm::APInt(32, op->id_, true));
    llvm::Value *val2 = llvm::ConstantInt::get(gen_.get_context(), llvm::APInt(32, op->pcode_, true));
    llvm::FunctionCallee callee;

    if (op->pcode_ == UNKNOWN_CODE) {
        switch(op->result_type()) {
        case expr_type::INT:
            callee = gen_.extern_func(module_, "qr_get_int"); break;
        case expr_type::DOUBLE:
            callee = gen_.extern_func(module_, "qr_get_double"); break;
        case expr_type::STRING:
            callee = gen_.extern_func(module_, "qr_get_string"); break;
        case expr_type::DATETIME:
            callee = gen_.extern_func(module_, "qr_get_ptime"); break;
        case expr_type::UINT64:
            callee = gen_.extern_func(module_, "qr_get_uint64"); break;
        default:
            // TODO
            break;
        }
        return gen_.get_builder()->CreateCall(callee, { start_->getArg(1), val1 });
    }
    else {
        switch (op->result_type()) {
        case expr_type::INT:
            callee = gen_.extern_func(module_, "get_int_property_value"); break;
        case expr_type::DOUBLE:
            callee = gen_.extern_func(module_, "get_double_property_value"); break;
        case expr_type::STRING:
            callee = gen_.extern_func(module_, "get_string_property_value"); break;
        case expr_type::DATETIME:
            callee = gen_.extern_func(module_, "get_ptime_property_value"); break;
        case expr_type::UINT64:
            callee = gen_.extern_func(module_, "get_uint64_property_value"); break;
        default:
            // TODO
            break;
        }
        if (!callee) {
            spdlog::info("unknown get_???_property_value for property of type '{}'", (int)op->result_type());
            abort();
        }
        return gen_.get_builder()->CreateCall(callee, { start_->getArg(0), start_->getArg(1), val1, val2 });
    }
}

void* expr_codegen::visit(std::shared_ptr<string_literal> op) { 
    return gen_.get_builder()->CreateGlobalStringPtr(op->str_);
}

void* expr_codegen::visit(std::shared_ptr<time_literal> op) {
    using namespace boost::posix_time;

    static ptime epoch(boost::gregorian::date(1970, 1, 1));
    auto msecs = (op->time_ - epoch).total_milliseconds();

    return llvm::ConstantInt::get(gen_.get_context(), llvm::APInt(64, msecs, true));    
}
    
void* expr_codegen::visit(std::shared_ptr<func_call> op) { return nullptr; }

/**
 * Generates the code for a == predicate.
 */
void* expr_codegen::visit(std::shared_ptr<eq_predicate> op) {
   // TODO: handle other types than int
    // spdlog::info("visit eq_predicate: {}", (int)op->result_type());
    auto lhs = static_cast<llvm::Value*>(op->left_->accept(*this));
    auto rhs = static_cast<llvm::Value*>(op->right_->accept(*this));
    auto dbl_ty = llvm::Type::getDoubleTy(gen_.get_context());
    if (lhs->getType() == dbl_ty || rhs->getType() == dbl_ty)
        return gen_.get_builder()->CreateFCmpOEQ(lhs, rhs);
    else if (op->left_->ftype_ == expr_type::STRING) {
        llvm::FunctionCallee callee = gen_.extern_func(module_, "string_compare");
        auto fcall = gen_.get_builder()->CreateCall(callee, { lhs, rhs });
        auto val_0 = llvm::ConstantInt::get(gen_.get_context(), llvm::APInt(32, 0, true));
        return gen_.get_builder()->CreateICmpEQ(fcall, val_0);
    }
    else
        return gen_.get_builder()->CreateICmpEQ(lhs, rhs);
}  

/**
 * Generates the code for a <> predicate.
 */
void* expr_codegen::visit(std::shared_ptr<neq_predicate> op) {
   // TODO: handle other types than int
    auto lhs = static_cast<llvm::Value*>(op->left_->accept(*this));
    auto rhs = static_cast<llvm::Value*>(op->right_->accept(*this));
    auto dbl_ty = llvm::Type::getDoubleTy(gen_.get_context());
    if (lhs->getType() == dbl_ty || rhs->getType() == dbl_ty)
        return gen_.get_builder()->CreateFCmpONE(lhs, rhs);
    else if (op->left_->ftype_ == expr_type::STRING) {
        llvm::FunctionCallee callee = gen_.extern_func(module_, "string_compare");
        auto fcall = gen_.get_builder()->CreateCall(callee, { lhs, rhs });
        auto val_0 = llvm::ConstantInt::get(gen_.get_context(), llvm::APInt(32, 0, true));
        return gen_.get_builder()->CreateICmpNE(fcall, val_0);
    }
    else 
        return gen_.get_builder()->CreateICmpNE(lhs, rhs);
}  

/**
 * Generates the code for a <= predicate.
 */
void* expr_codegen::visit(std::shared_ptr<le_predicate> op) {
   // TODO: handle other types than int
    auto lhs = static_cast<llvm::Value*>(op->left_->accept(*this));
    auto rhs = static_cast<llvm::Value*>(op->right_->accept(*this));
    auto dbl_ty = llvm::Type::getDoubleTy(gen_.get_context());
    if (lhs->getType() == dbl_ty || rhs->getType() == dbl_ty)
        return gen_.get_builder()->CreateFCmpOLE(lhs, rhs);
    else if (op->left_->ftype_ == expr_type::STRING) {
        llvm::FunctionCallee callee = gen_.extern_func(module_, "string_compare");
        auto fcall = gen_.get_builder()->CreateCall(callee, { lhs, rhs });
        auto val_0 = llvm::ConstantInt::get(gen_.get_context(), llvm::APInt(32, 0, true));
        return gen_.get_builder()->CreateICmpSLE(fcall, val_0);
    }
    else
        return gen_.get_builder()->CreateICmpSLE(lhs, rhs);
}

/**
 * Generates the code for a < predicate.
 */ 
void* expr_codegen::visit(std::shared_ptr<lt_predicate> op) {
   // TODO: handle other types than int
    auto lhs = static_cast<llvm::Value*>(op->left_->accept(*this));
    auto rhs = static_cast<llvm::Value*>(op->right_->accept(*this));
    auto dbl_ty = llvm::Type::getDoubleTy(gen_.get_context());
    if (lhs->getType() == dbl_ty || rhs->getType() == dbl_ty)
        return gen_.get_builder()->CreateFCmpOLT(lhs, rhs);
    else if (op->left_->ftype_ == expr_type::STRING) {
        llvm::FunctionCallee callee = gen_.extern_func(module_, "string_compare");
        auto fcall = gen_.get_builder()->CreateCall(callee, { lhs, rhs });
        auto val_0 = llvm::ConstantInt::get(gen_.get_context(), llvm::APInt(32, 0, true));
        return gen_.get_builder()->CreateICmpSLT(fcall, val_0);
    }
    else
        return gen_.get_builder()->CreateICmpSLT(lhs, rhs);
}

/**
 * Generates the code for a >= predicate.
 */
void* expr_codegen::visit(std::shared_ptr<ge_predicate> op) {
   // TODO: handle other types than int
    auto lhs = static_cast<llvm::Value*>(op->left_->accept(*this));
    auto rhs = static_cast<llvm::Value*>(op->right_->accept(*this));
    auto dbl_ty = llvm::Type::getDoubleTy(gen_.get_context());
    if (lhs->getType() == dbl_ty || rhs->getType() == dbl_ty)
        return gen_.get_builder()->CreateFCmpOGE(lhs, rhs);
    else if (op->left_->ftype_ == expr_type::STRING) {
        llvm::FunctionCallee callee = gen_.extern_func(module_, "string_compare");
        auto fcall = gen_.get_builder()->CreateCall(callee, { lhs, rhs });
        auto val_0 = llvm::ConstantInt::get(gen_.get_context(), llvm::APInt(32, 0, true));
        return gen_.get_builder()->CreateICmpSGE(fcall, val_0);
    }
    else
        return gen_.get_builder()->CreateICmpSGE(lhs, rhs);
}

/**
 * Generates the code for a > predicate.
 */
void* expr_codegen::visit(std::shared_ptr<gt_predicate> op) {
   // TODO: handle other types than int
    auto lhs = static_cast<llvm::Value*>(op->left_->accept(*this));
    auto rhs = static_cast<llvm::Value*>(op->right_->accept(*this));
    auto dbl_ty = llvm::Type::getDoubleTy(gen_.get_context());
    if (lhs->getType() == dbl_ty || rhs->getType() == dbl_ty)
        return gen_.get_builder()->CreateFCmpOGT(lhs, rhs);
    else if (op->left_->ftype_ == expr_type::STRING) {
        llvm::FunctionCallee callee = gen_.extern_func(module_, "string_compare");
        auto fcall = gen_.get_builder()->CreateCall(callee, { lhs, rhs });
        auto val_0 = llvm::ConstantInt::get(gen_.get_context(), llvm::APInt(32, 0, true));
        return gen_.get_builder()->CreateICmpSGT(fcall, val_0);
    }
    else
        return gen_.get_builder()->CreateICmpSGT(lhs, rhs);
}

void* expr_codegen::visit(std::shared_ptr<and_predicate> op) {
    auto lhs = static_cast<llvm::Value*>(op->left_->accept(*this));
    auto rhs = static_cast<llvm::Value*>(op->right_->accept(*this));
 /*   auto i32_ty = llvm::Type::getInt32Ty(gen_.get_context());
    auto lhs32 = gen_.get_builder()->CreateIntCast(lhs, i32_ty, true);
    auto rhs32 = gen_.get_builder()->CreateIntCast(rhs, i32_ty, true);
    auto and_op = gen_.get_builder()->CreateAnd({ lhs32, rhs32 });
    auto val_0 = llvm::ConstantInt::get(gen_.get_context(), llvm::APInt(32, 0, true));
    return gen_.get_builder()->CreateICmpNE(and_op, val_0);
    */
    return gen_.get_builder()->CreateLogicalAnd(lhs, rhs);

}

void* expr_codegen::visit(std::shared_ptr<or_predicate> op) {
    auto lhs = static_cast<llvm::Value*>(op->left_->accept(*this));
    auto rhs = static_cast<llvm::Value*>(op->right_->accept(*this));
    /*
    auto i32_ty = llvm::Type::getInt32Ty(gen_.get_context());
    auto lhs32 = gen_.get_builder()->CreateIntCast(lhs, i32_ty, true);
    auto rhs32 = gen_.get_builder()->CreateIntCast(rhs, i32_ty, true);

    auto or_op = gen_.get_builder()->CreateOr({ lhs32, rhs32 });
    auto val_0 = llvm::ConstantInt::get(gen_.get_context(), llvm::APInt(32, 0, true));
    return gen_.get_builder()->CreateICmpNE(or_op, val_0);
    */
   return gen_.get_builder()->CreateLogicalOr(lhs, rhs);
}

void* expr_codegen::visit(std::shared_ptr<regex_predicate> op) {
    auto lhs = static_cast<llvm::Value*>(op->left_->accept(*this));
    auto rhs = llvm::ConstantInt::get(gen_.get_context(), llvm::APInt(64, (uint64_t) &(op->re_), true));

    llvm::FunctionCallee callee = gen_.extern_func(module_, "regex_match");
    return gen_.get_builder()->CreateCall(callee, { lhs, rhs });
}  

void* expr_codegen::visit(std::shared_ptr<math_expression> op) {
    llvm::Value *res = nullptr;
    auto lhs = static_cast<llvm::Value*>(op->left_->accept(*this));
    auto rhs = static_cast<llvm::Value*>(op->right_->accept(*this));
    
    if (op->result_type() == expr_type::INT || op->result_type() == expr_type::UINT64) {
        switch (op->fop_) {
        case expr_op::PLUS:
        spdlog::info("createAdd");
            res = gen_.get_builder()->CreateAdd(lhs, rhs);
            break;
        case expr_op::MINUS:
            res = gen_.get_builder()->CreateSub(lhs, rhs);
            break;
        case expr_op::MULT:
            res = gen_.get_builder()->CreateMul(lhs, rhs);
            break;
        case expr_op::DIV:
            if (op->result_type() == expr_type::UINT64)
                res = gen_.get_builder()->CreateUDiv(lhs, rhs);
            else
                res = gen_.get_builder()->CreateSDiv(lhs, rhs);
            break;
        case expr_op::MOD:
            if (op->result_type() == expr_type::UINT64)
                res = gen_.get_builder()->CreateURem(lhs, rhs);
            else
                res = gen_.get_builder()->CreateSRem(lhs, rhs);
            break;
        default:
            break;
        }
    }
    else if (op->result_type() == expr_type::DOUBLE) {
    }
    else if (op->result_type() == expr_type::UINT64) {
    }
    return res;
}
