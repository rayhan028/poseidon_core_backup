#include <iostream>
#include <memory>

#include <tao/pegtl/contrib/parse_tree.hpp>
#include <tao/pegtl/contrib/parse_tree_to_dot.hpp>

#include "queryc.hpp"

namespace pegtl = tao::pegtl;

void queryc::compile(const std::string &query) {
    auto ast = parse(query);
    if (ast) {
	      pegtl::parse_tree::print_dot( std::cout, *ast );
        ast_to_plan(ast);
    }
}

queryc::parse_tree_ptr
queryc::parse(const std::string &query) {
  pegtl::memory_input<> in(query, "");

  try {
    return pegtl::parse_tree::parse<qlang::qoperator,
                                    qlang::my_selector, qlang::my_control>(in);
  } catch (const pegtl::parse_error &e) {
    const auto p = e.positions.front();
    std::cerr << e.what() << std::endl
              << in.line_at(p) << std::endl
              << std::string(p.byte_in_line, ' ') << '^' << std::endl;
  }
  return nullptr;
}

void queryc::ast_to_plan(queryc::parse_tree_ptr &ast) {
  if (!ast)
    return;
}
