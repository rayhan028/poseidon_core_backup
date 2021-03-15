#include <iostream>
#include <memory>

#include <boost/algorithm/string.hpp>

#include <tao/pegtl/contrib/parse_tree.hpp>
#include <tao/pegtl/contrib/parse_tree_to_dot.hpp>

#include "queryc.hpp"

namespace pegtl = tao::pegtl;
namespace ph = std::placeholders;

std::string trim_string(const std::string& s) {
  std::string s2 = s;
  if (s2[0] == '\'' || s2[0] == '"')
    s2 = s2.substr(1, s2.size()-2);
  return s2;
}

std::string trim_ws(const std::string& str) {
  auto first = str.find_first_not_of(' ');
  if (std::string::npos == first) {
    return str;
  }
  auto last = str.find_last_not_of(' ');
  return str.substr(first, (last - first + 1));
}

void queryc::compile(const std::string &query) {
    auto ast = parse(query);
    if (ast) {
        ast_to_plan(ast);
    }
}

algebra_optr queryc::compile_to_plan(const std::string &query) {
    auto ast = parse(query);
    if (!ast) 
      throw query_execution_error();
    auto collect = Collect(true);
    return ast_to_algoptr(ast, collect);
}

ast_op_ptr queryc::parse(const std::string &query) {
  pegtl::memory_input<> in(query, "");

  parse_tree_ptr ptree;

  try {
    ptree = pegtl::parse_tree::parse<qlang::qoperator,
                                    qlang::my_selector, qlang::my_control>(in);
    if (ptree)
  	  pegtl::parse_tree::print_dot(std::cout, *ptree); 
    else {
      std::cerr << "uknown parse error" << std::endl;
      return nullptr;
    }
  } catch (const pegtl::parse_error &e) {
    const auto p = e.positions().front();
    std::cerr << e.what() << std::endl
              << in.line_at(p) << std::endl
              << std::string(p.column, ' ') << '^' << std::endl;
    return nullptr;
  }
  auto ast_node = ptree_to_ast(ptree->children.front());
  print_ast(ast_node);
  return ast_node;
}

ast_op::op_type queryc::get_op_type(parse_tree_ptr& pn) {
  for (auto& n : pn->children) {
    if (n->is_type<qlang::op_name>()) {
      auto name = n->string();
      if (name == "Filter")
        return ast_op::filter;
      else if (name == "NodeScan")
        return ast_op::node_scan;
      else if (name == "Limit")
        return ast_op::limit;
      else if (name == "Sort")
        return ast_op::sort;
      else if (name == "GroupBy")
        return ast_op::group_by;
      else if (name == "HashJoin")
        return ast_op::hash_join;
      else if (name == "LeftOuterJoin")
        return ast_op::leftouter_join;
      else if (name == "ForeachRelationship")
        return ast_op::foreach_rship;
      else if (name == "Expand")
        return ast_op::expand;
      else if (name == "Project")
        return ast_op::project;
      else if (name == "Create") {
        if (pn->children.size() > 1) {
          auto& sibling = pn->children[1];
          auto& pattern = sibling->children.front();
          if (pattern->is_type<qlang::node_pattern>()) {
            return ast_op::create_node;
          }
          else if (pattern->is_type<qlang::rship_pattern>()) {
            return ast_op::create_rship;
          }
        }
        return ast_op::unknown;
      }
      // TODO
    }
  }
  return ast_op::unknown;
}

ast_op_ptr queryc::ptree_to_ast(parse_tree_ptr& pn) {
  assert(pn->is_type<qlang::qoperator>());
  ast_op::op_type otype = get_op_type(pn);
  auto nptr = std::make_shared<ast_op>(otype);  
  for (auto &n : pn->children) {
    if (n->is_type<qlang::param>()) {
      assert(n->children.size() > 0);
      auto& param = n->children.front();
      if (param->is_type<qlang::qoperator>()) {
        nptr->add_child(ptree_to_ast(param));
      }
      else if (param->is_type<qlang::literal_string>()) {
        nptr->add_param(param->string());
      }
      else if (param->is_type<qlang::directions>()) {
        nptr->add_param(param->string());
      }
      else if (param->is_type<qlang::integer>()) {
        nptr->add_param(std::stoi(param->string()));
      }
      else if (param->is_type<qlang::expression>()) {
        nptr->add_param(std::move(param));
      }
      else if (param->is_type<qlang::proj_array>()) {
        proj_spec_list plist;
        for (auto& p : param->children) {
          auto pspec = get_property_spec(p);
          plist.push_back(pspec);
        }
        nptr->add_param(plist);
      }
      else if (param->is_type<qlang::node_pattern>()) {
        auto& p = param->children.front();
        // p->children[0] > node_or_rship_label
        nptr->add_param(p->children[0]->string());
        // p->children[1] -> prop_list
        jproperty_list plist;
        for (auto& prop : p->children[1]->children) {
          auto pspec = get_json_property(prop);
          plist.push_back(pspec);
        }
        nptr->add_param(plist);
      }  
      else if (param->is_type<qlang::rship_pattern>()) {
        auto d1 = param->children[1]->string();
        auto d2 = param->children[3]->string();
        if (d1 == "-" && d2 == "->")
          nptr->add_param(std::string("->"));
        else if (d1 == "<-" && d2 == "-")
          nptr->add_param(std::string("<-"));
        else if (d1 == "<-" && d2 == "->")
          nptr->add_param(std::string("<->"));
        else
          nptr->add_param(std::string("-"));
        
        auto& p0 = param->children[0];
        nptr->add_param(p0->string());
        auto& p = param->children[2];
        auto& p1 = param->children[4];
        nptr->add_param(p1->string());
        // p->children[0] > node_or_rship_label
        nptr->add_param(p->children[0]->string());
        // p->children[1] -> prop_list
        jproperty_list plist;
        for (auto& prop : p->children[1]->children) {
          auto pspec = get_json_property(prop);
          plist.push_back(pspec);
        }
        nptr->add_param(plist);
      }
      else 
        std::cerr << "UNKNOWN param type!" << std::endl;
    }
  }
  return nptr;
}

proj_spec queryc::get_property_spec(parse_tree_ptr& pn) {
  assert (pn->is_type<qlang::proj_expr>());
  std::vector<std::string> s;
  boost::split(s, pn->string(), boost::is_any_of(":"));
  return proj_spec{ s[0], s[1] };
}

jproperty queryc::get_json_property(parse_tree_ptr& pn) {
  assert (pn->is_type<qlang::property>());
  std::vector<std::string> s;
  boost::split(s, pn->string(), boost::is_any_of(":"));
  return jproperty{ s[0], s[1] };
}

void queryc::ast_to_plan(ast_op_ptr &ast) {
  if (!ast)
    return;

  auto qop = ast_to_qop(ast, nullptr);
}

qop_ptr queryc::ast_to_qop(ast_op_ptr &ast, qop_ptr parent) {
  std::cout << "ast_to_qop: " << ast->op_ << std::endl;
  qop_ptr qop;
  switch (ast->op_) {
    case ast_op::node_scan:
      qop = std::make_shared<scan_nodes>(ast->get_param<std::string>(0));
      break;
    case ast_op::limit:
      qop = std::make_shared<limit_result>(ast->get_param<int>(0));
      break;
  /*  case ast_op::filter:
      qop = std::make_shared<is_property>(??);
      if (parent)
        parent->connect(qop, std::bind(&is_property::process,
                                dynamic_cast<is_property *>(qop.get()), ph::_1,
                                ph::_2));
      break;*/
    default:
      break;
  } 
  if (!ast->is_source()) {
    auto qop1 = ast_to_qop(ast->children_[0], qop);
    if (ast->children_.size() == 2) {
      auto qop2 = ast_to_qop(ast->children_[1], qop);
    }    
  }
  return qop;
} 

std::string parse_variable_name(std::string var_name) {
  auto dot_pos = var_name.find(".");
  return var_name.substr(dot_pos+1);
}

unsigned parse_tuple_id(std::string var_name) {
  auto dot_pos = var_name.find(".");
  return std::stoi(var_name.substr(1, dot_pos-1));
}

expr parse_filter_expression(ast_op_ptr &ast) {
/* TODO: more complex filter expressions
  * currently, only simple (binary) expressions are supported
  * e.g. Age >= 42
  */

  // process lhs of expression
  auto fe_expr = ast->get_param(0);

  auto lhs_key = std::move(fe_expr->children[0]);
  
  unsigned int lhs_qr_id = 0;

  if(lhs_key->is_type<qlang::variable_name>()) {
      auto lhs_id = std::move(lhs_key->children[0]);
      lhs_qr_id = std::stoi(lhs_id->string());
  }

  // extract the actual key after $X. in string
  auto lhs_var_name = parse_variable_name(lhs_key->string());
  auto key_se = Key(lhs_qr_id, lhs_var_name);


  auto rhs_value = std::move(fe_expr->children[2]);
  expr value_se;
  if (is_int(rhs_value->string())) {
      auto n = std::stoi(rhs_value->string()); // TODO: find better solution
      value_se = Int(n);
  } else if(rhs_value->is_type<qlang::variable_name>()){
      auto rhs_var_name = parse_variable_name(lhs_key->string());
      auto rhs_id = std::move(rhs_value->children[0]);
      auto rhs_qr_id = std::stoi(rhs_id->string());
      value_se = Key(rhs_qr_id, rhs_var_name);
  }

  auto fe_op = std::move(fe_expr->children[1]);
  expr op_se;

  if (boost::equals(fe_op->string(), "==")) {
      op_se = EQ(key_se, value_se);
  }

  return op_se;
}

algebra_optr queryc::ast_to_algoptr(ast_op_ptr &ast, algebra_optr parent) {
  algebra_optr op;
  switch(ast->op_) {
    case ast_op::node_scan:
      op = Scan(trim_string(ast->get_param<std::string>(0)), parent);
      break;
    case ast_op::foreach_rship:
    {
      auto rship_dir_str = ast->get_param<std::string>(0);
      RSHIP_DIR rship_dir = RSHIP_DIR::FROM;

      if(boost::iequals(rship_dir_str, "FROM")) {
        rship_dir = RSHIP_DIR::FROM;
      }
      else if(boost::iequals(rship_dir_str, "TO")) {
        rship_dir = RSHIP_DIR::TO;
      }

      auto rship_label = trim_string(ast->get_param<std::string>(1));

      op = ForeachRship(rship_dir, {}, rship_label, parent);
    }
      break;
    case ast_op::expand:
    {
      auto expand_dir_str = ast->get_param<std::string>(0);
      EXPAND expand_dir = EXPAND::IN;

      if(boost::iequals(expand_dir_str, "IN")) {
        expand_dir = EXPAND::IN;
      } else if(boost::iequals(expand_dir_str, "OUT")) {
        expand_dir = EXPAND::OUT;
      }

      auto expand_label = trim_string(ast->get_param<std::string>(1));

      op = Expand(expand_dir, expand_label, parent);
    }
      break;
    case ast_op::limit:
      op = Limit(ast->get_param<int>(0), parent);
      break;
    case ast_op::filter:
    {
      auto fexpr = parse_filter_expression(ast);

      op = Filter(fexpr, parent);
    }
      break;
    case ast_op::project:
    {
        auto pr_list = ast->get_param<proj_spec_list>(0);
        std::vector<pr_expr> pr_exprs;

        for(auto & p : pr_list) {
            FTYPE type = FTYPE::INT;
            if (boost::iequals(p.ptype, "int")) {
              type = FTYPE::INT;
            } else if (boost::iequals(p.ptype, "string")) {
              type = FTYPE::STRING;
            } else if (boost::iequals(p.ptype, "uint64")) {
              type = FTYPE::UINT64;
            } /// TODO: improve type handling

            auto pv_id = parse_tuple_id(p.pname);
            auto pv_name = parse_variable_name(p.pname);
            pr_exprs.push_back({pv_id, pv_name, type});
        }
        
        op = Project(pr_exprs, parent);
      break;
    }
    case ast_op::hash_join:
    {
      break;        
    }
    case ast_op::leftouter_join:
    {
      break;        
    }    
    case ast_op::sort:
    {
      break;        
    }  
    case ast_op::group_by:
    {
      break;        
    }     
    default:
      break;
  }
  if(!ast->is_source()) {
    op = ast_to_algoptr(ast->children_[0], op);
    //if(ast->children_.size() == 2) {
      //auto qop2 = ast_to_algoptr(ast->children_[1], op);
    //}
  }
  return op;
}