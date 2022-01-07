/*
 * Copyright (C) 2019-2022 DBIS Group - TU Ilmenau, All Rights Reserved.
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
#ifndef qparser_hpp_
#define qparser_hpp_

#include <string>

#include "ast.hpp"
#include "expression.hpp"

#include <tao/pegtl.hpp>
#include <tao/pegtl/contrib/parse_tree.hpp>

  /**
   * Removes leading and trailing quotation marks from a string.
   */
  std::string trim_string(const std::string& s);

  /**
   * Removes leading and trailing white spaces from a string.
   */
   std::string trim_ws(const std::string& str);

using parse_tree_ptr = std::unique_ptr<tao::pegtl::parse_tree::node>;

/**
 * qparser is the main class for parsing textual algebraic query specifications and producing an AST.
 */
class qparser {
public:
    qparser() = default;
    ~qparser() = default;


  /**
   * Parses the given query string and returns an AST.
   */
  ast_op_ptr parse(const std::string &query);

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
  expr parse_expression(parse_tree_ptr& tree);

  /**
   * Returns the property name from the variable name, 
   * e.g. $0.Id -> Id.
   */
  std::string extract_variable_name(const std::string& var_name);

  /**
   * TODO
   */
  expr parse_variable_name(parse_tree_ptr& tree);

  /**
   * TODO
   */
  expr parse_udf_func_expr(parse_tree_ptr& tree);

  /**
   * Returns the variable number from the variable name, 
   * e.g. $0.Id -> 0.
   */
  uint32_t parse_tuple_id(const std::string& var_name);

  udf_spec parse_udf(parse_tree_ptr& tree);

   /**
    * Parses given string and returns the correspondig sort order 
    * (expects only ASC and DESC).
    */
   simple_proj_spec::sort_order parse_sort_order(const std::string &s);

};

#endif