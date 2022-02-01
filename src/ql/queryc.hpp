/*
 * Copyright (C) 2019-2021 DBIS Group - TU Ilmenau, All Rights Reserved.
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
#ifndef queryc_hpp_
#define queryc_hpp_

#include "qlang_grammar.hpp"
#include "ast.hpp"
#include "qop.hpp"
#include "query.hpp"
#include "query_set.hpp"


#ifdef USE_LLVM
#include "qoperator.hpp"
#include "qcompiler.hpp"

#endif 

#include <tao/pegtl.hpp>
#include <tao/pegtl/contrib/parse_tree.hpp>

namespace ph = std::placeholders;
using parse_tree_ptr = std::unique_ptr<tao::pegtl::parse_tree::node>;

/**
 * queryc ist the query compiler for Poseidon. It accepts a simple 
 * algebraic textual query langugage and produces via an AST either
 * (a) an ahead-of-time compiled query plan of connected query operator 
 *     implementations (instances of the class qop_ptr) that can be
 *     executed or
 * (b) an intermediate plan consisting of qop_ptr instances that
 *     is used to generate IR code for LLVM.
 */
class queryc {

public:

  /**
   * Constructor for a new query compiler.
   */
  queryc() = default;

#ifdef USE_LLVM

  /**
   * TODO
   */
  qop_ptr ast_to_algoptr(ast_op_ptr &ast, qop_ptr parent);

  /**
   * Executes the given query plan.
   */
  void exec_plan(qop_ptr &plan, graph_db_ptr &gdb);
#endif
  /**
   *
   */
  query_set generate_qex_plan(graph_db_ptr &gdb, const std::string &qstr);

  /**
   * Parses the given query string and returns an AST.
   */
  ast_op_ptr parse(const std::string &query);

  /**
   * Parses the given query string and saves the plan with given name.
   */
  void parse_and_save_plan(const std::string &name, const std::string &query);

  /**
   * Executes the given query by name.
   */
  void exec_plan(const std::string &qname, graph_db_ptr &gdb);

private:
  /**
   * Derives the operator type for the AST node from the
   * parse tree.
   */
  ast_op::op_type get_op_type(parse_tree_ptr& pn);

  /**
   * 
   */
  proj_spec get_property_spec(parse_tree_ptr& pn);

  /**
   * Extracts an aggregate specification (function name, variable name, data type) 
   * from the property tree.
   */
  aggr_spec get_aggregate_spec(parse_tree_ptr& pn);

  /**
   * Extracts a JSON property (key-value pair) from the property tree.
   */
  jproperty get_json_property(parse_tree_ptr& pn);

  /**
   * Construct an AST from the parse tree.
   */
  ast_op_ptr ptree_to_ast(parse_tree_ptr& pn);

  /**
   * TODO
   */
  std::pair<qop_ptr, qop_ptr> ast_to_qex(ast_op_ptr &ast, graph_db_ptr& gdb, std::vector<qop_ptr>& sources);

#ifdef USE_LLVM
  /**
   * TODO
   */
  expr parse_expression(parse_tree_ptr& tree);
#endif 

  /**
   * Returns the property name from the variable name, 
   * e.g. $0.Id -> Id.
   */
  std::string parse_variable_name(const std::string& var_name);

  /**
   * Returns the variable number from the variable name, 
   * e.g. $0.Id -> 0.
   */
  uint32_t parse_tuple_id(const std::string& var_name);

  udf_spec parse_udf(parse_tree_ptr& tree);

  /**
   * Removes leading and trailing quotation marks from a string.
   */
  std::string trim_string(const std::string& s);

  /**
   * Removes leading and trailing white spaces from a string.
   */
   std::string trim_ws(const std::string& str);

   /**
    * Parses given string and returns the correspondig sort order 
    * (expects only ASC and DESC).
    */
   simple_proj_spec::sort_order parse_sort_order(const std::string &s);

  /**
   *
   */
  properties_t jprops_to_props(const jproperty_list& jprops);

  template <typename T>
  qop_ptr qop_append(qop_ptr parent, std::shared_ptr<T> qop) { 
    if (parent != nullptr)
      parent->connect(qop, std::bind(&T::process, dynamic_cast<T *>(qop.get()), ph::_1, ph::_2));
    return qop;
  }

#ifdef USE_LLVM
  std::map<std::string, qop_ptr> query_plans_;
#else
  std::map<std::string, query> query_plan_;
#endif

};

#endif