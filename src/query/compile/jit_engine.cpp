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

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/DataLayout.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/Support/InitLLVM.h>
#include <llvm/Support/Error.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/InitLLVM.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Support/SourceMgr.h>

#include "jit_engine.hpp"
#include "jit_compiler.hpp"

#include "query_ctx.hpp"

using namespace llvm;

jit_engine::jit_engine(graph_db_ptr gdb) : gdb_(gdb) { 
    ExitOnError ExitOnErr;
    ExitOnErr.setBanner("Poseidon JIT Engine");

    InitializeNativeTarget();
    InitializeNativeTargetAsmPrinter();
    InitializeNativeTargetAsmParser();

    ExitOnError exitOnError;
    jit_ = std::make_unique<jit_compiler>(exitOnError);
}

bool jit_engine::load_code(const std::string& filename) {
    SMDiagnostic err;

    auto module = parseIRFile(filename, err, ctx_);

    if (!module) {
        spdlog::info("ERROR: cannot parse IR file: {} at line {}, column {}", err.getMessage(), err.getLineNo(), err.getColumnNo());
        return false;
    }
    if (auto jerr = jit_->add_module(std::move(module))) {
        spdlog::info("ERROR: cannot add module to JIT");
        return false;
    }
    return true;
}
    
bool jit_engine::add_module(std::unique_ptr<llvm::Module> m) {
    m->setDataLayout(jit_->get_data_layout());
    if (auto jerr = jit_->add_module(std::move(m))) {
        spdlog::info("ERROR: cannot add module to JIT");
        return false;
    }
    return true;
}

jit_engine::predicate_fptr jit_engine::get_predicate_function(const std::string& fname) {
  auto fptr = jit_->get_function_raw<predicate_fptr>(fname);
  return fptr ? *fptr: nullptr;
}

std::tuple<jit_engine::aggr_init_fptr, jit_engine::aggr_iterate_fptr, jit_engine::aggr_finish_fptr> jit_engine::get_aggregate_functions(const std::string& fname) {
    auto f1 = jit_->get_function_raw<aggr_init_fptr>(fname + "_init");
    auto f2 = jit_->get_function_raw<aggr_iterate_fptr>(fname + "_iterate");
    auto f3 = jit_->get_function_raw<aggr_finish_fptr>(fname + "_finish");
    auto fptr1 = f1 ? *f1 : nullptr;
    auto fptr2 = f2 ? *f2 : nullptr;
    auto fptr3 = f3 ? *f3 : nullptr;

    return std::make_tuple(fptr1, fptr2, fptr3);    
}
