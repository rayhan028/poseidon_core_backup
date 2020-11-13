#ifndef queryc_hpp_
#define queryc_hpp_

#include "dict.hpp"
#include "qlang_grammar.hpp"
#include "ast.hpp"
#include "qop.hpp"
#include "qoperator.hpp"
#include "query_engine.hpp"

#include <tao/pegtl.hpp>
#include <tao/pegtl/contrib/parse_tree.hpp>

class queryc {

public:

  /**
   * Constructor for a new query compiler.
   */
  queryc(p_ptr<dict> &d) : dict_(d) {}

  void compile(const std::string &query);

  void compile(const std::string &query, algebra_optr& op);

  algebra_optr ast_to_algoptr(ast_op_ptr &ast, algebra_optr parent);

private:
  /**
   * Parses the given query string and returns an AST.
   */
  ast_op_ptr parse(const std::string &query);

  /**
   * Constructs a query execution plan from the given AST and
   * returns the root node.
   */
  void ast_to_plan(ast_op_ptr &ast);

  ast_op::op_type get_op_type(parse_tree_ptr& pn);
  prop_spec get_property_spec(parse_tree_ptr& pn);

  ast_op_ptr ptree_to_ast(parse_tree_ptr& pn);

  qop_ptr ast_to_qop(ast_op_ptr &ast, qop_ptr parent);

  p_ptr<dict> &dict_;
};

#endif