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

#include <llvm/ExecutionEngine/Orc/CompileUtils.h>
#include <llvm/ExecutionEngine/Orc/ExecutionUtils.h>
#include <llvm/ExecutionEngine/Orc/ThreadSafeModule.h>
#include <llvm/ExecutionEngine/Orc/Core.h>
#include <llvm/ExecutionEngine/SectionMemoryManager.h>
#include <llvm/IR/Mangler.h>
#include <llvm/Support/Debug.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Bitcode/BitcodeWriter.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/PassManager.h>
#include <llvm/Support/Debug.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Transforms/InstCombine/InstCombine.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Transforms/Scalar/GVN.h>
#include <llvm/Transforms/Utils.h>
#include <llvm/Transforms/IPO.h>
#include <llvm/Transforms/AggressiveInstCombine/AggressiveInstCombine.h>
#include <llvm/Transforms/Vectorize.h>
#include <llvm/CodeGen/Passes.h>
#include <llvm/Analysis/CallGraphSCCPass.h>

#include <cstdio>

#include "jit_compiler.hpp"
#include "jit_funcs.hpp"

// #include "global_definitions.hpp"


using namespace llvm;
using namespace llvm::orc;

/**
 * Helper class to manage the IR code optimization passes and optimization level
 */
class ir_optimize {
public:
    ir_optimize(unsigned level) { pmb_.OptLevel = level; }

    /**
     * Process the given optimization passes
     */
    llvm::Expected<llvm::orc::ThreadSafeModule>
    operator()(llvm::orc::ThreadSafeModule TSM,
               const llvm::orc::MaterializationResponsibility &);

private:
    llvm::PassManagerBuilder pmb_;
};

Expected<ThreadSafeModule>
ir_optimize::operator()(ThreadSafeModule tsm, const MaterializationResponsibility &) {
    Module &mod = *tsm.getModuleUnlocked();

    // mod.print(llvm::errs(), nullptr);

    legacy::FunctionPassManager fpm(&mod);
    fpm.add(createPromoteMemoryToRegisterPass());
    fpm.add(createCFGSimplificationPass());
    fpm.add(createLCSSAPass());
    fpm.add(createLoopDeletionPass());
    fpm.add(createDeadStoreEliminationPass());
    fpm.add(createInstructionCombiningPass());
    pmb_.populateFunctionPassManager(fpm);
    fpm.doInitialization();

    for (Function &F : mod)
        fpm.run(F);
    fpm.doFinalization();

    legacy::PassManager mpm;
    pmb_.populateModulePassManager(mpm);
    
    std::error_code ec;
    llvm::raw_fd_ostream ostr("module", ec, llvm::sys::fs::F_None);
    WriteBitcodeToFile(mod, ostr);
    ostr.flush();

    return std::move(tsm);
}

jit_compiler::jit_compiler(ExitOnError ExitOnErr) : ctx_(std::make_unique<LLVMContext>()),
          es_(std::make_unique<ExecutionSession>()),
          tm_(create_target_machine(ExitOnErr)),
          E_ERR(ExitOnErr),
          objLinkingLayer_(*es_, create_memory_manager_ftor()),
          compileLayer_(*es_, objLinkingLayer_, std::make_unique<SimpleCompiler>(*tm_)),
          optimizeLayer_(*es_, compileLayer_),
          mainJD_(es_->createBareJITDylib("main")) {

    auto dl = get_data_layout();
    cantFail(DynamicLibrarySearchGenerator::GetForCurrentProcess(dl.getGlobalPrefix()));
            
    SymbolMap s_map;
    MangleAndInterner mangle(*es_, dl);

    s_map[mangle("print_node")] = JITEvaluatedSymbol(
                pointerToJITTargetAddress(&print_node), JITSymbolFlags::Exported);
    s_map[mangle("qr_get_node")] = JITEvaluatedSymbol(
                pointerToJITTargetAddress(&qr_get_node), JITSymbolFlags::Exported);
    s_map[mangle("get_node_property_int_value")] = JITEvaluatedSymbol(
                pointerToJITTargetAddress(&get_node_property_int_value), JITSymbolFlags::Exported);
    s_map[mangle("get_node_property_uint64_value")] = JITEvaluatedSymbol(
                pointerToJITTargetAddress(&get_node_property_uint64_value), JITSymbolFlags::Exported);
    s_map[mangle("get_node_property_string_value")] = JITEvaluatedSymbol(
                pointerToJITTargetAddress(&get_node_property_string_value), JITSymbolFlags::Exported);

    ExitOnErr(mainJD_.define(absoluteSymbols(s_map)));
}

jit_compiler::~jit_compiler() {
    if (auto err = es_->endSession())
        es_->reportError(std::move(err));
}


Error jit_compiler::add_module(std::unique_ptr<Module> mod) {
    optimizeLayer_.setTransform(ir_optimize(3));
    auto tracker = mainJD_.getDefaultResourceTracker();
    return optimizeLayer_.add(tracker, ThreadSafeModule(std::move(mod), ctx_));
}


std::unique_ptr<TargetMachine> jit_compiler::create_target_machine(llvm::ExitOnError ExitOnErr) {
    auto jtmp = ExitOnErr(JITTargetMachineBuilder::detectHost());
	
    auto tm = jtmp.createTargetMachine();
    if (tm) {
        tm->get()->setFastISel(true);
	}
    return ExitOnErr(std::move(tm));
}

using GetMemoryManagerFunction_T = RTDyldObjectLinkingLayer::GetMemoryManagerFunction;

GetMemoryManagerFunction_T jit_compiler::create_memory_manager_ftor() {
    return []() -> GetMemoryManagerFunction_T::result_type {
        return std::make_unique<SectionMemoryManager>();
    };
}

std::string jit_compiler::mangle(llvm::StringRef unmangled_name) {
    std::string mangled_name;

    DataLayout dl = get_data_layout();
    raw_string_ostream mangled_name_stream(mangled_name);
    Mangler::getNameWithPrefix(mangled_name_stream, unmangled_name, dl);

    return mangled_name;
}

Error jit_compiler::apply_data_layout(llvm::Module &mod) {
    DataLayout dl = tm_->createDataLayout();
    if(mod.getDataLayout().isDefault())
        mod.setDataLayout(dl);

    if(mod.getDataLayout() != dl)
        return make_error<StringError>("Added modules have incompatible data layouts",
                inconvertibleErrorCode());
    return Error::success();
}

Expected<JITTargetAddress> jit_compiler::get_function_addr(llvm::StringRef name) {
    SymbolStringPtr name_ptr = es_->intern(mangle(name));
    JITDylibSearchOrder JDs{{&mainJD_, JITDylibLookupFlags::MatchAllSymbols}};
    Expected<JITEvaluatedSymbol> symb = es_->lookup(JDs, name_ptr);
    if(!symb)
        return symb.takeError();
    JITTargetAddress addr = symb->getAddress();

    return addr;
}

std::unique_ptr<DynamicLibrarySearchGenerator> jit_compiler::create_host_process_resolver() {
    char prefix = tm_->createDataLayout().getGlobalPrefix();
    auto gen = DynamicLibrarySearchGenerator::GetForCurrentProcess(prefix);
    if(!gen) {
        es_->reportError(gen.takeError());
        return nullptr;
    }

    if(!*gen) {
        return nullptr;
    }

    return std::move(*gen);
}
