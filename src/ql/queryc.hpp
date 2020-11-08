#ifndef queryc_hpp_
#define queryc_hpp_

#include "dict.hpp"
#include "qlang_grammar.hpp"
#include "ast.hpp"

#include <tao/pegtl.hpp>
#include <tao/pegtl/contrib/parse_tree.hpp>

class queryc {
using parse_tree_ptr = std::unique_ptr<tao::pegtl::parse_tree::node>;

public:

  /**
   * Constructor for a new query compiler.
   */
  queryc(p_ptr<dict> &d) : dict_(d) {}

  void compile(const std::string &query);

private:
  /**
   * Parses the given query string and returns a parse tree.
   */
  parse_tree_ptr parse(const std::string &query);

  /**
   * Constructs a query execution plan from the given AST and
   * returns the root node.
   */
  void ast_to_plan(parse_tree_ptr &ast);

  ast_op::op_type get_op_type(parse_tree_ptr& pn);
  ast_op_ptr ptree_to_ast(parse_tree_ptr& pn);

    p_ptr<dict> &dict_;
};

#endif