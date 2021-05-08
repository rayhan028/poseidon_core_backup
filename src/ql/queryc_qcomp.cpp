/**
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

#include "queryc.hpp"


algebra_optr queryc::compile_to_plan(const std::string &query) {
    auto ast = parse(query);
    if (!ast) 
      throw query_execution_error();
    auto collect = Collect(true);
    return ast_to_algoptr(ast, collect);
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
      auto fexpr = ast->get_param<expr>(0);
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
      auto pr_list = ast->get_param<proj_spec_list>(0);
      auto aggr_list = ast->get_param<aggr_spec_list>(1);
   
      // Group attributes
      std::vector<unsigned int> group_ids;
      std::vector<std::pair<std::string, int>> aggrs;
      for(auto & ag : aggr_list) {
          auto tp_pos = parse_tuple_id(ag.aname);
          group_ids.push_back(tp_pos);
          aggrs.push_back({ag.afunc, tp_pos});
      }

      auto aggr = Aggr(aggrs, parent);
      auto grby = GroupBy(group_ids, aggr);

      // Project attributes
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
      op = Project(pr_exprs, grby);  
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