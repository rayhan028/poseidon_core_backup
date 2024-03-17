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
#ifndef query_proc_hpp_
#define query_proc_hpp_

#include <any>
#include <string>

#include <boost/dll/import.hpp>

#include "query_ctx.hpp"
#include "query_batch.hpp"

#include "qinterp.hpp"

#ifdef USE_LLVM
#include "jit_engine.hpp"
#include "ir_generator.hpp"
#endif

#include "qresult_iterator.hpp"

/**
 * query_proc is the main entry class for the Poseidon query processor. It
 * encapsulates the parser, the query planner as well as the query compiler and
 * interpreter.
 */
class query_proc {
public:
  enum mode { Interpret, Compile, Adaptive };

  query_proc(query_ctx &ctx);
  ~query_proc() = default;

  bool parse_(const std::string &query);

  void set_execution_mode(mode m) { exec_mode_ = m; }
  mode execution_mode() const { return exec_mode_; }

  query_batch prepare_query(const std::string &query);

  qresult_iterator execute_query(const std::string &qstr,
                                 bool print_plan = false, bool as_string = false);

  template<typename... Args> 
  qresult_iterator exec_query(const std::string &qstr, Args... args) {
    return execute_query(fmt::format(qstr, args...));
  }

  std::size_t execute_and_output_query(const std::string &qstr,
                                       bool print_plan = false);

  void run_query(query_batch &plan);
  
  void abort_query();

  void abort_transaction();
  
  bool load_library(const std::string &lib_path);

private:
  void prepare_plan(query_batch &qplan, mode m);

  std::size_t qcnt_;
  query_ctx& qctx_;
  std::unique_ptr<qinterp> interp_;
  mode exec_mode_;

#ifdef USE_LLVM
  std::unique_ptr<jit_engine> jit_;
  std::unique_ptr<ir_generator> codegen_;

#endif
  std::shared_ptr<boost::dll::shared_library> udf_lib_;
};

#endif