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

#ifndef jit_compiler_hpp_
#define jit_compiler_hpp_

#include <llvm/ADT/StringRef.h>
#include <llvm/ADT/Triple.h>
#include <llvm/ExecutionEngine/JITSymbol.h>
#include <llvm/ExecutionEngine/Orc/LLJIT.h>
#include <llvm/ExecutionEngine/Orc/Core.h>
#include <llvm/ExecutionEngine/Orc/IRCompileLayer.h>
#include <llvm/ExecutionEngine/Orc/IRTransformLayer.h>
#include <llvm/ExecutionEngine/Orc/RTDyldObjectLinkingLayer.h>
#include <llvm/ExecutionEngine/Orc/CompileOnDemandLayer.h>
#include <llvm/ExecutionEngine/Orc/CompileUtils.h>
#include <llvm/ExecutionEngine/JITEventListener.h>
#include <llvm/IR/DataLayout.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/Error.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Transforms/InstCombine/InstCombine.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Transforms/Scalar/GVN.h>
#include <llvm/ExecutionEngine/Orc/Core.h>
#include <llvm/ExecutionEngine/Orc/ThreadSafeModule.h>
#include <llvm/Support/Error.h>
#include <llvm/Transforms/IPO/PassManagerBuilder.h>
#include <functional>
#include <memory>
#include <string>
#include <iostream>

/**
 * The actual JIT compiler class, providing methods to compile
 * a LLVM module and obtaining the function pointer of generated functions
 */
class jit_compiler {
public:
    jit_compiler(llvm::ExitOnError exitOnError);

    ~jit_compiler();

    /**
     * Return the data layout for IR modules.
     */
    llvm::DataLayout get_data_layout() const { return tm_->createDataLayout(); }

    /**
     * Obtains a function pointer to a JITed function.
     */
    template<class Signature_t>
    llvm::Expected<std::function<Signature_t>> get_function(llvm::StringRef name) {
        if(auto addr = get_function_addr(name))
            return std::function<Signature_t>(llvm::jitTargetAddressToPointer<Signature_t *>(*addr));
        else
            return addr.takeError();
    }

    /**
     * Obtains a raw (C-style) function pointer of a JITed function.
     */
    template<class Signature_t>
    llvm::Expected<Signature_t> get_function_raw(llvm::StringRef name) {
        if(auto addr = get_function_addr(name))
            return llvm::jitTargetAddressToPointer<Signature_t>(*addr);
        else
            return addr.takeError();
    }

    /**
     * Get the TargetMachine triple.
     */
    const llvm::Triple &get_target_triple() const {  return tm_->getTargetTriple(); }

    /**
     * Returns the LLVM context of the target.
     */
    llvm::LLVMContext &get_context() { return *ctx_.getContext(); }

    /**
     * Return the target machine information of the current target.
     */
    llvm::TargetMachine &get_target_machine() { return *tm_; }

    /**
     * Compiles a IR module into machine code.
     */
    llvm::Error add_module(std::unique_ptr<llvm::Module> m);

    /**
     * Returns a global Exit object for error handling.
     */
    llvm::ExitOnError get_exit() { return E_ERR; }

    /**
     * String reference to function address used in order to obtain a function pointer.
     */
    llvm::Expected<llvm::JITTargetAddress> get_function_addr(llvm::StringRef name);

    llvm::Error clear();

private:
    /*
    * Target machine initialisation methods
    */
    std::unique_ptr<llvm::orc::DynamicLibrarySearchGenerator> create_host_process_resolver();
    
    std::unique_ptr<llvm::TargetMachine> create_target_machine(llvm::ExitOnError ExitOnErr);

    llvm::orc::RTDyldObjectLinkingLayer::GetMemoryManagerFunction create_memory_manager_ftor();

    std::string mangle(llvm::StringRef unmangled_name);

    llvm::Error apply_data_layout(llvm::Module &mod);

    /*
    * LLVM Context and machine information
    */
    llvm::orc::ThreadSafeContext ctx_; // LLVM context
    std::unique_ptr<llvm::orc::ExecutionSession> es_;
    std::unique_ptr<llvm::TargetMachine> tm_;
    llvm::ExitOnError E_ERR; // JIT compiler error object
    llvm::orc::RTDyldObjectLinkingLayer objLinkingLayer_; // LLVM IR transformation layers
    // std::unique_ptr<PJitObjectCache> objCache_; // cache for compiled query IR code

    llvm::orc::IRCompileLayer compileLayer_;
    llvm::orc::IRTransformLayer optimizeLayer_;

    llvm::orc::JITDylib &mainJD_; // The main JIT Dylib of the compiler
};

#endif