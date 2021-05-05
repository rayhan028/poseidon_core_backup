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
#include <iostream>
#include <memory>

#include <boost/algorithm/string.hpp>

#include <tao/pegtl/contrib/parse_tree.hpp>
#include <tao/pegtl/contrib/parse_tree_to_dot.hpp>

#include "queryc.hpp"

namespace pegtl = tao::pegtl;


/* -------------------------------------------------------------------------------- */
/*                               Helper methods                                     */
/* -------------------------------------------------------------------------------- */   
expr parse_expression(parse_tree_ptr& tree);

std::string queryc::trim_string(const std::string& s) {
  std::string s2 = s;
  if (s2[0] == '\'' || s2[0] == '"')
    s2 = s2.substr(1, s2.size()-2);
  return s2;
}

std::string queryc::trim_ws(const std::string& str) {
  auto first = str.find_first_not_of(' ');
  if (std::string::npos == first) {
    return str;
  }
  auto last = str.find_last_not_of(' ');
  return str.substr(first, (last - first + 1));
}

proj_spec::sort_order queryc::parse_sort_order(const std::string &s) {
  if (s == "ASC")
    return proj_spec::Asc;
  else if (s == "DESC")
    return proj_spec::Desc;
  else
    return proj_spec::None;
}

std::string queryc::parse_variable_name(const std::string& var_name) {
  auto dot_pos = var_name.find(".");
  return var_name.substr(dot_pos + 1);
}

uint32_t queryc::parse_tuple_id(const std::string& var_name) {
  auto dot_pos = var_name.find(".");
  return std::stoi(var_name.substr(1, dot_pos - 1));
}

proj_spec queryc::get_property_spec(parse_tree_ptr& pn) {
  assert (pn->is_type<qlang::proj_expr>());
  std::vector<std::string> s;
  boost::split(s, pn->string(), boost::is_any_of(": "));
  return s.size() == 2 ? proj_spec{ s[0], s[1], proj_spec::None } : proj_spec{ s[0], s[1], parse_sort_order(s[2]) };
}

aggr_spec queryc::get_aggregate_spec(parse_tree_ptr& pn) {
  assert (pn->is_type<qlang::func_expr>());
  assert (pn->children.size() == 3);
  auto fname = pn->children[0]->string();
  auto vname = pn->children[1]->string();
  auto dtype = pn->children[2]->string();
  return aggr_spec{ fname, vname, dtype };
}

jproperty queryc::get_json_property(parse_tree_ptr& pn) {
  assert (pn->is_type<qlang::property>());
  std::vector<std::string> s;
  boost::split(s, pn->string(), boost::is_any_of(":"));
  return jproperty{ s[0], s[1] };
}

properties_t queryc::jprops_to_props(const jproperty_list& jprops) {
  properties_t props;
  for (auto& jp : jprops) {
    props[jp.pname] = jp.pval;
  }
  return props;
}

/* -------------------------------------------------------------------------------- */
/*                    Methods for parsing and AST construction                      */
/* -------------------------------------------------------------------------------- */   

/**
 * Parse the given query string and construct an abstract syntax tree (AST).
 */
ast_op_ptr queryc::parse(const std::string &query) {
  pegtl::memory_input<> in(query, "");

  parse_tree_ptr ptree;

  try {
    ptree = pegtl::parse_tree::parse<qlang::qoperator,
                                    qlang::my_selector, qlang::my_control>(in);
    if (!ptree) {
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
  // pegtl::parse_tree::print_dot(std::cout, *ptree); 
  auto ast_node = ptree_to_ast(ptree->children.front());
  // print_ast(ast_node);
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
        // nptr->add_param(std::move(param));
        nptr->add_param(parse_expression(param));
      }
      else if (param->is_type<qlang::proj_array>()) {
        proj_spec_list plist;
        for (auto& p : param->children) {
          auto pspec = get_property_spec(p);
          plist.push_back(pspec);
        }
        nptr->add_param(plist);
      }
      else if (param->is_type<qlang::func_array>()) {
        aggr_spec_list alist;
        for (auto& p : param->children) {
          auto aspec = get_aggregate_spec(p);
          alist.push_back(aspec);
        }
        nptr->add_param(alist);
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
        if (p->children.size() > 1) {
          jproperty_list plist;
          for (auto& prop : p->children[1]->children) {
            auto pspec = get_json_property(prop);
            plist.push_back(pspec);
          }
          nptr->add_param(plist);
        }
      }
      else 
        std::cerr << "UNKNOWN param type: " << param->string() << std::endl;
    }
  }
  return nptr;
}

/* -------------------------------------------------------------------------------- */

expr queryc::parse_expression(parse_tree_ptr& tree) {
  auto& lhs_key = tree->children[0];
  
  unsigned int lhs_qr_id = 0;

  if (lhs_key->is_type<qlang::variable_name>()) {
      lhs_qr_id = std::stoi(lhs_key->children[0]->string());
  }

  // extract the actual key after $X. in string
  auto lhs_var_name = parse_variable_name(lhs_key->string());
  auto key_se = Key(lhs_qr_id, lhs_var_name);

  auto& rhs_value = tree->children[2];
  expr value_se;
  if (is_int(rhs_value->string())) {
      auto n = std::stoi(rhs_value->string()); // TODO: find better solution
      value_se = Int(n);
  } else if (rhs_value->is_type<qlang::variable_name>()){
      auto rhs_var_name = parse_variable_name(lhs_key->string());
      auto rhs_qr_id = std::stoi(rhs_value->children[0]->string());
      value_se = Key(rhs_qr_id, rhs_var_name);
  }

  auto& fe_op = tree->children[1];
  expr op_se;

  if (boost::equals(fe_op->string(), "==")) {
      op_se = EQ(key_se, value_se);
  }

  return op_se;
}

