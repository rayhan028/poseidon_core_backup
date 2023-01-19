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
#include "query_ctx.hpp"

query_set qplanner::transform(query_ctx& ctx, ast_op_ptr op_tree) {
    std::vector<qop_ptr> sources;
  
    // auto qop_p = ast_to_qset(op_tree, gdb, sources);
    auto qop_p = ast_to_qplan(ctx.gdb_, op_tree, sources);
    query_set qset;
    for (auto& src : sources) {
        query q(ctx, src);
        qset.add(q);
    }
    return qset;
}

void qplanner::add_udf_library(std::shared_ptr<boost::dll::shared_library> udf_lib) {
  udf_lib_ = udf_lib;
}

properties_t qplanner::jprops_to_props(const jproperty_list& jprops) {
  properties_t props;
  for (auto& jp : jprops) {
    props[jp.pname] = jp.pval;
  }
  return props;
}

qop_ptr qplanner::node_scan_to_qplan(ast_op_ptr ast) {
  qop_ptr qop = nullptr;

  switch (ast->num_params()) {
  case 0:
    qop = std::make_shared<scan_nodes>();
    break;
  case 1:
    qop = std::make_shared<scan_nodes>(trim_string(ast->get_param<std::string>(0)));
    break;
  default:
    // handle also params like [ 'label1', 'label2' ]
    std::vector<std::string> label_list;
    for (auto i = 0u; i < ast->num_params(); i++)
      label_list.push_back(trim_string(ast->get_param<std::string>(i)));
    qop = std::make_shared<scan_nodes>(label_list);
  }
  return qop;
}

qop_ptr qplanner::foreach_rship_to_qplan(ast_op_ptr ast, qop_ptr child) {
  qop_ptr qop = nullptr;
  std::size_t dir_param = 0;
  int origin_idx = std::numeric_limits<int>::max();
      
  if (ast->params_[0].which() == 2) { // expression, e.g. $0
    auto p0 = ast->get_param<expr>(0);
    if (p0->ftype_ == FOP_TYPE::KEY) {
      origin_idx = dynamic_cast<key_token *>(p0.get())->qr_id_;
    }
    dir_param = 1;
  }
  
  if (ast->get_param<std::string>(dir_param) == "TO") {
    if (ast->num_params() == dir_param + 2) {
      auto qp = std::make_shared<foreach_to_relationship>(ast->get_param<std::string>(dir_param + 1), origin_idx);
      qop = qop_append(child, qp);
    }
    else if (ast->num_params() == dir_param + 4) {
      int m1 = ast->get_param<int64_t>(dir_param + 2);
      int m2 = ast->get_param<int64_t>(dir_param + 3);
      auto qp = std::make_shared<foreach_variable_to_relationship>(ast->get_param<std::string>(dir_param + 1), 
          m1, m2, origin_idx);
      qop = qop_append(child, qp);
    }
  }
  else if (ast->get_param<std::string>(dir_param) == "FROM") {
    if (ast->num_params() == dir_param + 2) {
      auto qp = std::make_shared<foreach_from_relationship>(ast->get_param<std::string>(dir_param + 1), origin_idx);
      qop = qop_append(child, qp);
    }
    else if (ast->num_params() == dir_param + 4) {
      int m1 = ast->get_param<int64_t>(dir_param + 2);
      int m2 = ast->get_param<int64_t>(dir_param + 3);
      auto qp = std::make_shared<foreach_variable_from_relationship>(ast->get_param<std::string>(dir_param + 1), 
          m1, m2, origin_idx);
      qop = qop_append(child, qp);
    }
  }
  else if (ast->get_param<std::string>(dir_param) == "ALL") {
    // TODO
  }
  return qop;
}

qop_ptr qplanner::expand_to_qplan(ast_op_ptr ast, qop_ptr child) {
  qop_ptr qop = nullptr;
  if (ast->get_param<std::string>(0) == "IN") {
    auto qp = std::make_shared<get_from_node>();
    qop = qop_append(child, qp);
  }
  else if (ast->get_param<std::string>(0) == "OUT") {
    auto qp = std::make_shared<get_to_node>();
    qop = qop_append(child, qp);
  }
  if (ast->params_.size() > 1) {
    auto qp2 = std::make_shared<node_has_label>(ast->get_param<std::string>(1));
    qop = qop_append(qop, qp2);
  }
  return qop;
}

qop_ptr qplanner::project_to_qplan(ast_op_ptr ast, qop_ptr child) {
  qop_ptr qop = nullptr;
  projection::expr_list pexprs;
  std::vector<projection_expr> prexprs;
  auto plist = ast->get_param<proj_spec_list>(0);
  for (auto& pex : plist) {
    if (pex.which() == 0) {
      // simple_proj_spec
      auto spj = boost::get<simple_proj_spec>(pex);
      auto pv_id = qparser::extract_tuple_id(spj.pname);
      auto pv_name = qparser::extract_variable_name(spj.pname);

      // std::cout << "projection: " << pv_id << " . " << pv_name << " : " << spj.ptype << std::endl;
      // note we need 'capture by value' here for pv_name, otherwise pv_name gets out of scope
      if (spj.ptype == "string") {
        pexprs.push_back(projection::expr(pv_id, ([=](auto ctx, auto res) { return builtin::string_property(res, pv_name); } )));
        prexprs.push_back({pv_id, pv_name, result_type::string});
      } else if (spj.ptype == "int") {
        pexprs.push_back(projection::expr(pv_id, ([=](auto ctx, auto res) { return builtin::int_property(res, pv_name); } )));
        prexprs.push_back({pv_id, pv_name, result_type::integer});
      } else if (spj.ptype == "double") {
        pexprs.push_back(projection::expr(pv_id, ([=](auto ctx, auto res) { return builtin::double_property(res, pv_name); } )));
        prexprs.push_back({pv_id, pv_name, result_type::double_t});
      } else if (spj.ptype == "uint64") {
        pexprs.push_back(projection::expr(pv_id, ([=](auto ctx, auto res) { return builtin::uint64_property(res, pv_name); } )));
        prexprs.push_back({pv_id, pv_name, result_type::uint64});
      } else if (spj.ptype == "datetime") {
        pexprs.push_back(projection::expr(pv_id, ([=](auto ctx, auto res) { return builtin::ptime_property(res, pv_name); } )));
        prexprs.push_back({pv_id, pv_name, result_type::date});
      }
    }  
    else {
      // udf_spec
      auto upj = boost::get<udf_spec>(pex);
      auto func_name = upj.fname.substr(5); // get rid of udf::
      if (upj.pname_list.size() == 1) { 
        auto pv_id = qparser::extract_tuple_id(upj.pname_list[0]);
        auto func = udf_lib_->get<query_result(query_ctx*, void*)>(func_name);
        pexprs.push_back(projection::expr(pv_id, ([=](auto ctx, auto res) { return func(&ctx, &res); } )));
        prexprs.push_back({func}); 
      }
      else {
        // TODO: handle UDFs with more than one parameter
      }
    }
  }
  auto qp = std::make_shared<projection>(pexprs);
  qp->prexpr_ = prexprs;
  return qop_append(child, qp);
}

qop_ptr qplanner::sort_to_qplan(graph_db_ptr gdb, ast_op_ptr ast, qop_ptr child) {
  auto slist = ast->get_param<proj_spec_list>(0);
  result_set::sort_spec_list sort_list;

    for (auto& pex : slist) {
      if (pex.which() == 0) {
        // simple_proj_spec
        auto spj = boost::get<simple_proj_spec>(pex); 
        auto pv_name = qparser::extract_variable_name(spj.pname);
        result_set::sort_spec spc;

        if (spj.porder == simple_proj_spec::Asc)
          spc.s_order = result_set::sort_spec::Asc;
        else
          spc.s_order = result_set::sort_spec::Desc;

        spc.vidx = qparser::extract_tuple_id(spj.pname);
        spc.pcode = gdb->get_code(pv_name);
        if (spj.ptype == "uint64")
          spc.cmp_type = 5;
        else if (spj.ptype == "string")
          spc.cmp_type = 4; 
        else if (spj.ptype == "int")
          spc.cmp_type = 2; 
        else if (spj.ptype == "double")
          spc.cmp_type = 3; 
        else if (spj.ptype == "datetime")
          spc.cmp_type = 6;            
        sort_list.push_back(spc);
      }
    }
  auto qp = std::make_shared<order_by>(sort_list);
  return qop_append2(child, qp);
}

qop_ptr qplanner::aggregate_to_qplan(ast_op_ptr ast, qop_ptr child) {
  std::vector<aggregate::expr> aggrs; 

  assert (ast->num_params() == 1);
  auto aggr_list = ast->get_param<aggr_spec_list>(0);
  for (auto& aggr : aggr_list) {
    auto v_id = qparser::extract_tuple_id(aggr.aname);
    auto v_name = qparser::extract_variable_name(aggr.aname);

    aggregate::expr::func_t aggr_func = aggregate::expr::f_count;
    std::size_t aggr_type = 2;

    if (aggr.afunc == "count")
      aggr_func = aggregate::expr::f_count;
    else if (aggr.afunc == "sum")
      aggr_func = aggregate::expr::f_sum;

    if (aggr.atype == "int")
      aggr_type = 2;
    else if (aggr.atype == "double")
      aggr_type = 3;
    else if (aggr.atype == "string")
      aggr_type = 4;
    aggrs.push_back(aggregate::expr{ aggr_func, v_id, v_name, aggr_type });
  }

  auto qp = std::make_shared<aggregate>(aggrs);
  return qop_append2(child, qp);
}


qop_ptr qplanner::groupby_to_qplan(ast_op_ptr ast, qop_ptr child) {
  std::cout << "groupby: nparams = " << ast->num_params() << std::endl;
  std::vector<std::size_t> groups;
  std::vector<std::pair<std::string, std::size_t>> aggrs;

  if (ast->num_params() == 1) {
    auto aggr_list = ast->get_param<aggr_spec_list>(0);
    for (auto& aggr : aggr_list) {
      std::cout << "---->> " << aggr.aname << std::endl;
      auto v_id = qparser::extract_tuple_id(aggr.aname);
      auto v_name = qparser::extract_variable_name(aggr.aname);

      aggrs.push_back(std::make_pair(aggr.afunc, v_id));
    }
  }
  else if (ast->num_params() == 2) {
    auto grp_list = ast->get_param<proj_spec_list>(0);
    auto aggr_list = ast->get_param<aggr_spec_list>(1);
  }
  auto qp = std::make_shared<group_by>(groups, aggrs);
  return qop_append(child, qp);
}

qop_ptr qplanner::union_to_qplan(ast_op_ptr ast, qop_ptr child1, qop_ptr child2) {
  auto qop = std::make_shared<union_all_qres>();
  child1->connect(qop, std::bind(&union_all_qres::process_right, qop.get(), ph::_1, ph::_2));
  child2->connect(qop, std::bind(&union_all_qres::process_left, qop.get(), ph::_1, ph::_2));
  return qop;
}

qop_ptr qplanner::cross_join_to_qplan(ast_op_ptr ast, qop_ptr child1, qop_ptr child2) {
  auto qp = std::make_shared<cross_join>();
  child1->connect(qp, std::bind(&cross_join::process_right, qp.get(), ph::_1, ph::_2));
  child2->connect(qp, std::bind(&cross_join::process_left, qp.get(), ph::_1, ph::_2));
  return qp;
}

qop_ptr qplanner::leftouter_join_to_qplan(ast_op_ptr ast, qop_ptr child1, qop_ptr child2) {
  auto qp = std::make_shared<left_outerjoin>(ast->get_param<expr>(0));
  child1->connect(qp, std::bind(&left_outerjoin::process_right, qp.get(), ph::_1, ph::_2));
  child2->connect(qp, std::bind(&left_outerjoin::process_left, qp.get(), ph::_1, ph::_2));
  return qp;
}

qop_ptr qplanner::ast_to_qplan(graph_db_ptr& gdb, ast_op_ptr ast, std::vector<qop_ptr>& sources) {
  qop_ptr child1 = nullptr, child2 = nullptr;
  qop_ptr qop = nullptr;

  // We need the transformed child nodes because we want to add
  // the current operator represented by ast as subscriber 
  if (!ast->is_source()) {
    child1 = ast_to_qplan(gdb, ast->children_[0], sources);
    // child2 is used only for binary operators
    child2 = ast->children_.size() == 2 ? ast_to_qplan(gdb, ast->children_[1], sources) : nullptr;
  }
  switch (ast->op_) {
    case ast_op::node_scan:
      qop = node_scan_to_qplan(ast);
      sources.push_back(qop);
      return qop;
      break;
    case ast_op::filter: {
      auto qp = std::make_shared<filter_tuple>(ast->get_param<expr>(0));
      qop = qop_append(child1, qp);
      break;
    }
    case ast_op::limit: {
      auto qp = std::make_shared<limit_result>(ast->get_param<int64_t>(0));
      qop = qop_append(child1, qp);
      break;
    }
    case ast_op::foreach_rship:
      qop = foreach_rship_to_qplan(ast, child1);
      break;
    case ast_op::expand:
      qop = expand_to_qplan(ast, child1);
      break;
    case ast_op::project:
      qop = project_to_qplan(ast, child1);
      break;
    case ast_op::sort:
      qop = sort_to_qplan(gdb, ast, child1);
      break;
    case ast_op::aggregate:
      qop = aggregate_to_qplan(ast, child1);
      break;
    case ast_op::group_by:
      qop = groupby_to_qplan(ast, child1);
      break;
    case ast_op::union_all:
      qop = union_to_qplan(ast, child1, child2);
      break;
    case ast_op::cross_join:
      qop = cross_join_to_qplan(ast, child1, child2);
      break;
    case ast_op::leftouter_join:
      qop = leftouter_join_to_qplan(ast, child1, child2);
      break;
    case ast_op::create_node: {
      properties_t props = jprops_to_props(ast->get_param<jproperty_list>(1));
      auto qp = std::make_shared<create_node>(ast->get_param<std::string>(0), props);
      qop = qop_append(child1, qp);
      if (ast->is_source())
        sources.push_back(qp);
      break;
    }
    case ast_op::create_rship: {
      // TODO: handle directions
      properties_t props;
      if (ast->params_.size() > 4) 
        props = jprops_to_props(ast->get_param<jproperty_list>(4));
      auto qp = std::make_shared<create_relationship>(ast->get_param<std::string>(3), props, 
        std::make_pair<int, int>(ast->get_param<int64_t>(1), ast->get_param<int64_t>(2)));
      qop = qop_append(child1, qp);
      if (ast->is_source())
        sources.push_back(qp);
      break;
    }
    default:      
      std::cerr << "ERROR: op-type not handled: " <<  ast->op_ << std::endl;
      break;
  }
  return qop;
}
