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

#include <functional>
#include <iostream>
#include <vector>
#include "qop.hpp"
#include "join.hpp"
#include "update.hpp"
#include "qplanner.hpp"
#include "qparser.hpp"

query_set qplanner::transform(graph_db_ptr& gdb, ast_op_ptr op_tree) {
    std::vector<qop_ptr> sources;
  
    auto qop_p = ast_to_qset(op_tree, gdb, sources);
    query_set qset;
    for (auto& src : sources) {
        query q(gdb, src);
        q.print_plan(std::cout); 
        qset.add(q);
    }
    return qset;
}


properties_t qplanner::jprops_to_props(const jproperty_list& jprops) {
  properties_t props;
  for (auto& jp : jprops) {
    props[jp.pname] = jp.pval;
  }
  return props;
}

std::pair<qop_ptr, qop_ptr> qplanner::ast_to_qset(ast_op_ptr &ast, graph_db_ptr& gdb, std::vector<qop_ptr>& sources) {
  qop_ptr qop;
  std::pair<qop_ptr, qop_ptr> res, res2;

  if (!ast->is_source()) {
    res = ast_to_qset(ast->children_[0], gdb, sources);
    /* TODO */
    if (ast->children_.size() == 2) {
      res2 = ast_to_qset(ast->children_[1], gdb, sources);
    }    
  }
  // std::cout << "ast_to_qex: " << ast->op_ << std::endl;
  switch (ast->op_) {
    case ast_op::node_scan:
      // TODO: handle also params like [ 'label1', 'label2' ]
      if (ast->num_params() == 0)
        qop = std::make_shared<scan_nodes>();
      else if (ast->num_params() == 1)
        qop = std::make_shared<scan_nodes>(trim_string(ast->get_param<std::string>(0)));
      sources.push_back(qop);
      return std::make_pair(qop, qop);
      break;
    case ast_op::limit:
    {
      auto qp = std::make_shared<limit_result>(ast->get_param<int>(0));
      qop = qop_append(res2.first ? res2.second : res.second, qp);
    }
      break;
    case ast_op::filter:
    {
      auto ex = ast->get_param<expr>(0);
      auto qp = std::make_shared<filter_tuple>(ex);
      qop = qop_append(res2.first ? res2.second : res.second, qp);
    }
      break;
    case ast_op::foreach_rship:
      if (ast->get_param<std::string>(0) == "TO") {
        auto qp = std::make_shared<foreach_to_relationship>(ast->get_param<std::string>(1));
        qop = qop_append(res2.first ? res2.second : res.second, qp);
      }
      else if (ast->get_param<std::string>(0) == "FROM") {
        auto qp = std::make_shared<foreach_from_relationship>(ast->get_param<std::string>(1));
        qop = qop_append(res2.first ? res2.second : res.second, qp);
      }
      else if (ast->get_param<std::string>(0) == "ALL") {
        // TODO
      }
      break;
    case ast_op::expand:
    {
      if (ast->get_param<std::string>(0) == "IN") {
        auto qp = std::make_shared<get_from_node>();
        qop = qop_append(res2.first ? res2.second : res.second, qp);
      }
      else if (ast->get_param<std::string>(0) == "OUT") {
        auto qp = std::make_shared<get_to_node>();
        qop = qop_append(res2.first ? res2.second : res.second, qp);
      }
      if (ast->params_.size() > 1) {
        auto qp2 = std::make_shared<node_has_label>(ast->get_param<std::string>(1));
        qop = qop_append(qop, qp2);
      }
    }
      break;
    case ast_op::project:
      {
        // TODO: projection expression
        projection::expr_list pexprs;
        auto plist = ast->get_param<proj_spec_list>(0);
        for (auto& pex : plist) {
          if (pex.which() == 0) {
            // simple_proj_spec
            auto spj = boost::get<simple_proj_spec>(pex);
            auto pv_id = qparser::extract_tuple_id(spj.pname);
            auto pv_name = qparser::extract_variable_name(spj.pname);

            // std::cout << "projection: " << pv_id << " . " << pv_name << " : " << spj.ptype << std::endl;
            // note we need 'capture by value' here for pv_name, otherwise pv_name gets out of scope
            if (spj.ptype == "string")
              pexprs.push_back(projection::expr(pv_id, ([=](auto res) { return builtin::string_property(res, pv_name); } )));
            else if (spj.ptype == "int")
              pexprs.push_back(projection::expr(pv_id, ([=](auto res) { return builtin::int_property(res, pv_name); } )));
            else if (spj.ptype == "double")
              pexprs.push_back(projection::expr(pv_id, ([=](auto res) { return builtin::double_property(res, pv_name); } )));
            else if (spj.ptype == "uint64")
              pexprs.push_back(projection::expr(pv_id, ([=](auto res) { return builtin::uint64_property(res, pv_name); } )));
            else if (spj.ptype == "datetime")
              pexprs.push_back(projection::expr(pv_id, ([=](auto res) { return builtin::ptime_property(res, pv_name); } )));
          }  
          else {
            // udf_spec
            auto upj = boost::get<udf_spec>(pex);
            std::cout << "projection-udf: " << upj.fname << std::endl;
          }
        }
        auto qp = std::make_shared<projection>(pexprs);
        qop = qop_append(res2.first ? res2.second : res.second, qp);
      }
      break;
    case ast_op::sort:
      break;
    case ast_op::union_all:
      break;
    case ast_op::count:
      break;
    case ast_op::group_by:
      break;
    case ast_op::hash_join:
      break;
    case ast_op::leftouter_join:
    {
      // TODO: outerjoin_on_rship or outerjoin_on_node??
      // TODO: identify src and des positions
      std::cout << "join: " << sources.size() << std::endl;
      auto qp = std::make_shared<left_outerjoin_on_rship>(std::make_pair(1, 2));
      auto parent = res2.first ? res2.second : res.second;
      parent->connect(qp, std::bind(&left_outerjoin_on_rship::process_left, qp.get(), ph::_1, ph::_2));
      qop_ptr parent2 = sources.at(sources.size()-2);
      while (parent2->has_subscriber()) {
        parent2 = parent2->subscriber();
      }
      parent2->connect(qp, std::bind(&left_outerjoin_on_rship::process_right, qp.get(), ph::_1, ph::_2));
      qop = qp;
    }
      break;
    case ast_op::create_node:
    {
      properties_t props = jprops_to_props(ast->get_param<jproperty_list>(1));
      auto qp = std::make_shared<create_node>(ast->get_param<std::string>(0), props);
      qop = qop_append(res2.first ? res2.second : res.second, qp);
    }
      break;
    case ast_op::create_rship:
     {
       // TODO: handle directions
      properties_t props;
      if (ast->params_.size() > 4) 
        props = jprops_to_props(ast->get_param<jproperty_list>(4));
      auto qp = std::make_shared<create_relationship>(ast->get_param<std::string>(3), props, 
        std::make_pair(ast->get_param<int>(1), ast->get_param<int>(2)));
      qop = qop_append(res2.first ? res2.second : res.second, qp);
    }
      break;
    case ast_op::algo:
      if (ast->get_param<std::string>(0) == "SPSP") {
        // TODO
      }
      break;
    default:
      break;
  } 

  if (res2.first) std::cout << "return res2\n";
  return std::make_pair(res.first, qop);
} 