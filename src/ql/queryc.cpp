#include <iostream>
#include <memory>

#include <tao/pegtl/contrib/parse_tree.hpp>
#include <tao/pegtl/contrib/parse_tree_to_dot.hpp>

#include "queryc.hpp"

namespace pegtl = tao::pegtl;

void queryc::compile(const std::string &query) {
    auto ast = parse(query);
    if (ast) {
        ast_to_plan(ast);
    }
}

ast_op_ptr queryc::parse(const std::string &query) {
  pegtl::memory_input<> in(query, "");

  parse_tree_ptr ptree;

  try {
    ptree = pegtl::parse_tree::parse<qlang::qoperator,
                                    qlang::my_selector, qlang::my_control>(in);
    //if (ptree)
  	//  pegtl::parse_tree::print_dot( std::cout, *ptree);
  } catch (const pegtl::parse_error &e) {
    const auto p = e.positions.front();
    std::cerr << e.what() << std::endl
              << in.line_at(p) << std::endl
              << std::string(p.byte_in_line, ' ') << '^' << std::endl;
    return nullptr;
  }

  auto ast_node = ptree_to_ast(ptree->children.front());
  print_ast(ast_node);
  return ast_node;
}

ast_op::op_type queryc::get_op_type(parse_tree_ptr& pn) {
  for (auto& n : pn->children) {
    if (n->is<qlang::op_name>()) {
      auto name = n->string();
      if (name == "Filter")
        return ast_op::filter;
      else if (name == "NodeScan")
        return ast_op::node_scan;
      else if (name == "Limit")
        return ast_op::limit;
      else if (name == "Join")
        return ast_op::join;
      // TODO
    }
  }
  return ast_op::unknown;
}

ast_op_ptr queryc::ptree_to_ast(parse_tree_ptr& pn) {
  ast_op::op_type otype = get_op_type(pn);
  auto nptr = std::make_shared<ast_op>(otype);  
  for (auto &n : pn->children) {
    if (n->is<qlang::param>()) {
      auto& param = n->children.front();
      if (param->is<qlang::qoperator>()) {
        nptr->add_child(ptree_to_ast(param));
      }
      else if (param->is<qlang::literal_string>()) {
        nptr->add_param(param->string());
      }
      else if (param->is<qlang::integer>()) {
        nptr->add_param(std::stoi(param->string()));
      }
      else if (param->is<qlang::expression>()) {
        nptr->add_param(std::move(param));
      }
    }
  }
  return nptr;
}

void queryc::ast_to_plan(ast_op_ptr &ast) {
  if (!ast)
    return;
}
