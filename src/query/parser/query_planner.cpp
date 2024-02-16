/*
 * Copyright (C) 2019-2023 DBIS Group - TU Ilmenau, All Rights Reserved.
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
#include "query_planner.hpp"
#include "binary_expression.hpp"
#include "expression.hpp"
#include "properties.hpp"
#include "qop.hpp"
#include "qop_aggregates.hpp"
#include "qop_algorithm.hpp"
#include "qop_builtins.hpp"
#include "qop_joins.hpp"
#include "qop_projection.hpp"
#include "qop_relationships.hpp"
#include "qop_scans.hpp"
#include "qop_set.hpp"
#include "qop_updates.hpp"

namespace ph = std::placeholders;

uint32_t query_planner::extract_tuple_id(const std::string &var_name) {
  auto dot_pos = var_name.find(".");
  return std::stoi(var_name.substr(1, dot_pos - 1));
}

std::string query_planner::trim_string(const std::string &s) {
  std::string s2 = s;
  if (s2[0] == '\'' || s2[0] == '"')
    s2 = s2.substr(1, s2.size() - 2);
  return s2;
}

query_batch query_planner::get_query_plan() {
  query_batch qbatch;

  for (auto qop : sources_) {
    query_pipeline q(qop);
    qbatch.add(q);
  }

  return qbatch;
}

std::any query_planner::visitQuery(poseidonParser::QueryContext *ctx) {
  sources_.clear();
  return visit(ctx->query_operator());
}

std::any
query_planner::visitNode_scan_op(poseidonParser::Node_scan_opContext *ctx) {
  qop_ptr op = nullptr;

  auto p = ctx->scan_param();
  if (p == nullptr)
    op = std::make_shared<node_scan>();
  else if (p->STRING_() != nullptr)
    op = std::make_shared<node_scan>(trim_string(p->STRING_()->getText()));
  else if (p->scan_list() != nullptr) {
    std::vector<std::string> label_list;
    for (auto &s : p->scan_list()->STRING_()) {
      label_list.push_back(trim_string(s->getText()));
    }
    op = std::make_shared<node_scan>(label_list);
  }
  sources_.push_back(op);
  return std::make_any<qop_ptr>(op);
}

std::any
query_planner::visitRship_scan_op(poseidonParser::Rship_scan_opContext *ctx) {
  qop_ptr op = nullptr;

  auto p = ctx->scan_param();
  if (p == nullptr)
    op = std::make_shared<relationship_scan>();
  else if (p->STRING_() != nullptr)
    op = std::make_shared<relationship_scan>(
        trim_string(p->STRING_()->getText()));
  else if (p->scan_list() != nullptr) {
    std::vector<std::string> label_list;
    for (auto &s : p->scan_list()->STRING_()) {
      label_list.push_back(trim_string(s->getText()));
    }
    op = std::make_shared<relationship_scan>(label_list);
  }
  sources_.push_back(op);
  return std::make_any<qop_ptr>(op);
}

std::any
query_planner::visitIndex_scan_op(poseidonParser::Index_scan_opContext *ctx) {
  qop_ptr op = nullptr;

  auto p = ctx->index_scan_param();
  auto label = trim_string(p->STRING_(0)->getText());
  auto property = trim_string(p->STRING_(1)->getText());

  if (!qctx_.gdb_->has_index(label, property))
    throw unknown_index();

  auto v = p->value();
  uint64_t key = 0;
  if (v->INTEGER() != nullptr)
    key = std::stoi(v->INTEGER()->getText());
  else if (v->STRING_() != nullptr) {
    auto str = v->STRING_()->getText();
    key = qctx_.gdb_->get_code(str);
  }
  auto idx_id = qctx_.gdb_->get_index(label, property);
  op = std::make_shared<index_scan>(idx_id, key);
  sources_.push_back(op);
  return std::make_any<qop_ptr>(op);
}

std::any query_planner::visitFilter_op(poseidonParser::Filter_opContext *ctx) {
  auto ch = visit(ctx->query_operator());
  auto child_op = std::any_cast<qop_ptr>(ch);

  auto ex = visit(ctx->logical_expr());
  auto qp = std::make_shared<filter_op>(std::any_cast<expr>(ex));
  auto qop = qop_append(child_op, qp);

  return std::make_any<qop_ptr>(qop);
}

std::any
query_planner::visitProject_op(poseidonParser::Project_opContext *ctx) {
  projection::pexpr_list pex_list;
  projection::udf_list prj_udf_list;

  for (auto &pexpr : ctx->proj_list()->proj_expr()) {
    if (pexpr->variable() != nullptr) {
      auto pex = pexpr->variable();
      auto var = pex->Var()->getText();
      auto var_id = extract_tuple_id(var);
      std::string attr;
      if (pex->Identifier_() != nullptr)
        attr = pex->Identifier_()->getText();
      auto attr_type = pex->type_spec();
      if (attr.empty()) {
        // handle cases where attr is empty
        pex_list.push_back(projection::pexpr{var_id, "", prj::forward});
      } else {
        if (attr_type->StringType_() != nullptr) {
          pex_list.push_back(
              projection::pexpr{var_id, attr, prj::string_property});
        } else if (attr_type->IntType_() != nullptr) {
          pex_list.push_back(
              projection::pexpr{var_id, attr, prj::int_property});
        } else if (attr_type->DoubleType_() != nullptr) {
          pex_list.push_back(
              projection::pexpr{var_id, attr, prj::double_property});
        } else if (attr_type->Uint64Type_() != nullptr) {
          pex_list.push_back(
              projection::pexpr{var_id, attr, prj::uint64_property});
        } else if (attr_type->DateType_() != nullptr) {
          pex_list.push_back(
              projection::pexpr{var_id, attr, prj::date_property});
        }
      }
    } else if (pexpr->additive_expr() != nullptr) {
      auto e = visit(pexpr->additive_expr());
      auto ex = std::any_cast<expr>(e);
      pex_list.push_back(projection::pexpr{0, "", prj::arithmetic_expr, ex});
    } else if (pexpr->case_expr() != nullptr) {
      auto e = visit(pexpr->case_expr()->logical_expr());
      auto ex = std::any_cast<expr>(e);
      auto res1 = pexpr->case_expr()->case_result()[0];
      expr res_ex1, res_ex2;
      if (res1->variable() != nullptr) {
        res_ex1 = std::any_cast<expr>(visit(res1->variable()));
      } else if (res1->value() != nullptr) {
        res_ex1 = std::any_cast<expr>(visit(res1->value()));
      }
      auto res2 = pexpr->case_expr()->case_result()[1];
      if (res2->variable() != nullptr) {
        res_ex2 = std::any_cast<expr>(visit(res2->variable()));
      } else if (res2->value() != nullptr) {
        res_ex2 = std::any_cast<expr>(visit(res2->value()));
      }
      spdlog::debug("CASE: {} | {} | {}", ex->dump(), res_ex1->dump(),
                    res_ex2->dump());
      pex_list.push_back(
          projection::pexpr{0, "", prj::case_expr, ex, res_ex1, res_ex2});
    } else if (pexpr->function_call() != nullptr) {
      // handle UDFs
      auto fc = pexpr->function_call();
      auto prefix = fc->prefix()->getText();
      auto fc_name = fc->Identifier_()->getText();
      auto fc_params = fc->param_list()->param();
      // TODO: handle UDFs with more than one parameter
      if (prefix == "pb") {
        auto p_idx = extract_tuple_id(fc_params[0]->Var()->getText());
        auto prop_name = fc_params[0]->Identifier_() != nullptr
                             ? fc_params[0]->Identifier_()->getText()
                             : "";

        // TODO: handle all builtin functions in the same way
        if (fc_name == "label") {
          assert(fc_params.size() == 1);
          pex_list.push_back(projection::pexpr{p_idx, "", prj::label});
        } else if (fc_name == "year") {
          assert(fc_params.size() == 1);
          pex_list.push_back(
              projection::pexpr{p_idx, prop_name, prj::function});
          prj_udf_list.push_back([=](auto ctx, auto res) {
            return builtin::pr_year(res, prop_name);
          });
        } else if (fc_name == "substr") {
          assert(fc_params.size() == 3);
          auto start = std::stoi(fc_params[1]->value()->INTEGER()->getText());
          auto len = std::stoi(fc_params[2]->value()->INTEGER()->getText());
          pex_list.push_back(
              projection::pexpr{p_idx, prop_name, prj::function});
          prj_udf_list.push_back([=](auto ctx, auto res) {
            return builtin::substring(res, prop_name, start, len);
          });

        } else
          throw query_processing_error("unknown builtin function");
      } else if (prefix == "udf") {
        if (!udf_lib_ || !udf_lib_->is_loaded())
          throw udf_not_found();
        // spdlog::info("trying to find udf '{}'...", fc_name);
        // auto fc_func = udf_lib_->get<user_defined_func1>(fc_name);
        auto fc_func =
            udf_lib_->get<query_result(query_ctx *, void *)>(fc_name);
        auto &pm = fc_params[0];
        if (pm->value() != nullptr) {
          abort();
          if (pm->value()->STRING_()) {
            // TODO:
            // param_list.push_back(Str(trim_string(pm->value()->getText())));
          } else if (pm->value()->INTEGER()) {
            // TODO:
            // param_list.push_back(Int(std::stoi(pm->value()->getText())));
          } else if (pm->value()->FLOAT()) {
            // TODO:
            // param_list.push_back(Float(std::stof(pm->value()->getText())));
          }
        } else {
          auto p_idx = extract_tuple_id(pm->Var()->getText());
          pex_list.push_back(projection::pexpr{p_idx, "", prj::function});
          prj_udf_list.push_back(
              [=](auto ctx, auto res) { return fc_func(&ctx, &res); });
          // TODO: pexprs.push_back(projection::expr(p_idx, ([=](auto ctx, auto
          // res) { return fc_func(&ctx, &res); } )));
        }
      } else {
        // TODO: throw undefined prefix
        throw query_processing_error("invalid function prefix");
      }
    }
  }
  auto qp = std::make_shared<projection>(pex_list, prj_udf_list);

  auto ch = visit(ctx->query_operator());
  auto child_op = std::any_cast<qop_ptr>(ch);
  auto qop = qop_append(child_op, qp);

  return std::make_any<qop_ptr>(qop);
}

std::any query_planner::visitLimit_op(poseidonParser::Limit_opContext *ctx) {
  auto param = ctx->INTEGER()->getText();
  auto limit = std::stoi(param);
  auto qp = std::make_shared<limit_result>(limit);

  auto ch = visit(ctx->query_operator());
  auto child_op = std::any_cast<qop_ptr>(ch);
  auto qop = qop_append(child_op, qp);

  return std::make_any<qop_ptr>(qop);
}

std::any query_planner::visitExpand_op(poseidonParser::Expand_opContext *ctx) {
  qop_ptr qop = nullptr;
  auto param = ctx->expand_dir()->getText();

  auto ch = visit(ctx->query_operator());
  auto child = std::any_cast<qop_ptr>(ch);

  if (param == "IN") {
    auto qp = std::make_shared<get_from_node>();
    qop = qop_append(child, qp);
  } else if (param == "OUT") {
    auto qp = std::make_shared<get_to_node>();
    qop = qop_append(child, qp);
  }

  if (ctx->STRING_() != nullptr) {
    auto label = trim_string(ctx->STRING_()->getText());
    auto qp2 = std::make_shared<node_has_label>(label);
    qop = qop_append(qop, qp2);
  }

  return std::make_any<qop_ptr>(qop);
}

std::any query_planner::visitSort_op(poseidonParser::Sort_opContext *ctx) {
  result_set::sort_spec_list sort_list;

  for (auto &sexpr : ctx->sort_list()->sort_expr()) {
    auto vidx = extract_tuple_id(sexpr->Var()->getText());
    auto tspec = sexpr->type_spec();
    std::size_t cmp_type = 0;
    if (tspec->IntType_() != nullptr)
      cmp_type = 2;
    else if (tspec->DoubleType_() != nullptr)
      cmp_type = 3;
    else if (tspec->Uint64Type_() != nullptr)
      cmp_type = 5;
    else if (tspec->StringType_() != nullptr)
      cmp_type = 4;
    else if (tspec->DateType_() != nullptr)
      cmp_type = 6;

    auto s_order = sexpr->sort_spec()->DescOrder_() != nullptr
                       ? result_set::sort_spec::Desc
                       : result_set::sort_spec::Asc;
    sort_list.push_back(result_set::sort_spec(vidx, cmp_type, s_order));
  }
  auto ch = visit(ctx->query_operator());
  auto child = std::any_cast<qop_ptr>(ch);

  auto qp = std::make_shared<order_by>(sort_list);
  auto qop = qop_append2(child, qp);

  return std::make_any<qop_ptr>(qop);
}

std::any query_planner::visitForeach_relationship_op(
    poseidonParser::Foreach_relationship_opContext *ctx) {
  qop_ptr qop = nullptr;
  int origin_idx = std::numeric_limits<int>::max();

  auto dir = ctx->rship_dir();
  auto label = trim_string(ctx->STRING_()->getText());
  auto cardinality = ctx->rship_cardinality();
  auto rship_src = ctx->rship_source_var();

  auto ch = visit(ctx->query_operator());
  auto child = std::any_cast<qop_ptr>(ch);

  int m1 = 0, m2 = 0;
  if (cardinality != nullptr) {
    m1 = std::stoi(cardinality->INTEGER()[0]->getText());
    m2 = std::stoi(cardinality->INTEGER()[1]->getText());
  }

  if (rship_src != nullptr)
    origin_idx = extract_tuple_id(rship_src->Var()->getText());

  if (dir->FromDir_() != nullptr) {
    if (cardinality != nullptr) {
      auto qp = std::make_shared<foreach_variable_from_relationship>(
          label, m1, m2, origin_idx);
      qop = qop_append(child, qp);
    } else {
      auto qp = std::make_shared<foreach_from_relationship>(label, origin_idx);
      qop = qop_append(child, qp);
    }
  } else if (dir->ToDir_() != nullptr) {
    if (cardinality != nullptr) {
      auto qp = std::make_shared<foreach_variable_to_relationship>(
          label, m1, m2, origin_idx);
      qop = qop_append(child, qp);
    } else {
      auto qp = std::make_shared<foreach_to_relationship>(label, origin_idx);
      qop = qop_append(child, qp);
    }
  } else if (dir->AllDir_() != nullptr) {
    if (cardinality != nullptr) {
      auto qp = std::make_shared<foreach_variable_all_relationship>(
          label, m1, m2, origin_idx);
      qop = qop_append(child, qp);
    } else {
      auto qp = std::make_shared<foreach_all_relationship>(label, origin_idx);
      qop = qop_append(child, qp);
    }
  }
  return std::make_any<qop_ptr>(qop);
}

std::any
query_planner::visitCrossjoin_op(poseidonParser::Crossjoin_opContext *ctx) {
  auto qop = std::make_shared<cross_join_op>();
  auto ch1 = visit(ctx->query_operator()[0]);
  auto ch2 = visit(ctx->query_operator()[1]);
  auto child1 = std::any_cast<qop_ptr>(ch1);
  auto child2 = std::any_cast<qop_ptr>(ch2);

  child1->connect(
      qop, std::bind(&cross_join_op::process_right, qop.get(), ph::_1, ph::_2),
      std::bind(&cross_join_op::finish, qop.get(), ph::_1));
  child2->connect(
      qop, std::bind(&cross_join_op::process_left, qop.get(), ph::_1, ph::_2),
      std::bind(&cross_join_op::finish, qop.get(), ph::_1));
  // execute child1 before child2
  child1->priority_ = 1;
  child2->priority_ = 0;
  return std::make_any<qop_ptr>(qop);
}

std::any query_planner::visitLeftouterjoin_op(
    poseidonParser::Leftouterjoin_opContext *ctx) {
  // extract expression
  auto ex = visit(ctx->logical_expr());
  auto qop = std::make_shared<left_outer_join_op>(std::any_cast<expr>(ex));

  auto ch1 = visit(ctx->query_operator()[0]);
  auto ch2 = visit(ctx->query_operator()[1]);
  auto child1 = std::any_cast<qop_ptr>(ch1);
  auto child2 = std::any_cast<qop_ptr>(ch2);

  child2->connect(
      qop,
      std::bind(&left_outer_join_op::process_right, qop.get(), ph::_1, ph::_2),
      std::bind(&left_outer_join_op::finish, qop.get(), ph::_1));
  child1->connect(
      qop,
      std::bind(&left_outer_join_op::process_left, qop.get(), ph::_1, ph::_2),
      std::bind(&left_outer_join_op::finish, qop.get(), ph::_1));
  // execute child2 before child1
  child1->priority_ = 0;
  child2->priority_ = 1;
  return std::make_any<qop_ptr>(qop);
}

std::any
query_planner::visitHashjoin_op(poseidonParser::Hashjoin_opContext *ctx) {
  auto ch1 = visit(ctx->query_operator()[0]);
  auto ch2 = visit(ctx->query_operator()[1]);
  auto child1 = std::any_cast<qop_ptr>(ch1);
  auto child2 = std::any_cast<qop_ptr>(ch2);

  auto lhs = std::any_cast<expr>(visit(ctx->variable()[0]));
  auto rhs = std::any_cast<expr>(visit(ctx->variable()[1]));

  auto qop =
      std::make_shared<hash_join_op>(std::dynamic_pointer_cast<variable>(lhs),
                                     std::dynamic_pointer_cast<variable>(rhs));

  child1->connect(
      qop, std::bind(&hash_join_op::build_phase, qop.get(), ph::_1, ph::_2),
      std::bind(&hash_join_op::finish, qop.get(), ph::_1));
  child2->connect(
      qop, std::bind(&hash_join_op::probe_phase, qop.get(), ph::_1, ph::_2),
      std::bind(&hash_join_op::finish, qop.get(), ph::_1));
  // execute child1 before child2
  child1->priority_ = 1;
  child2->priority_ = 0;
  return std::make_any<qop_ptr>(qop);
}

std::any
query_planner::visitAggregate_op(poseidonParser::Aggregate_opContext *ctx) {
  std::vector<aggregate::expr> aggrs;
  auto ch = visit(ctx->query_operator());
  auto child = std::any_cast<qop_ptr>(ch);

  auto aggr_list = ctx->aggregate_list()->aggr_expr();
  for (auto &aggr : aggr_list) {
    auto expr = aggr->variable();
    auto v_id = extract_tuple_id(expr->Var()->getText());
    auto v_name =
        expr->Identifier_() == nullptr ? "" : expr->Identifier_()->getText();
    auto tspec = expr->type_spec();
    aggregate::expr::func_t aggr_func = aggregate::expr::f_count;
    qr_type aggr_type = int_type;

    if (aggr->aggr_func()->Count_() != nullptr)
      aggr_func = aggregate::expr::f_count;
    else if (aggr->aggr_func()->Sum_() != nullptr)
      aggr_func = aggregate::expr::f_sum;
    else if (aggr->aggr_func()->Avg_() != nullptr)
      aggr_func = aggregate::expr::f_avg;
    else if (aggr->aggr_func()->Min_() != nullptr)
      aggr_func = aggregate::expr::f_min;
    else if (aggr->aggr_func()->Max_() != nullptr)
      aggr_func = aggregate::expr::f_max;

    if (tspec->IntType_() != nullptr)
      aggr_type = int_type;
    else if (tspec->DoubleType_() != nullptr)
      aggr_type = double_type;
    else if (tspec->StringType_() != nullptr)
      aggr_type = string_type;
    else if (tspec->StringType_() != nullptr)
      aggr_type = string_type;
    else if (tspec->Uint64Type_() != nullptr)
      aggr_type = uint64_type;
    aggrs.push_back(aggregate::expr{aggr_func, v_id, v_name, aggr_type});
  }

  auto qp = std::make_shared<aggregate>(aggrs, qctx_.get_dictionary());
  auto qop = qop_append2(child, qp);
  return std::make_any<qop_ptr>(qop);
}

std::any query_planner::visitUnion_op(poseidonParser::Union_opContext *ctx) {
  auto qop = std::make_shared<union_all_op>();

  auto ch1 = visit(ctx->query_operator()[0]);
  auto ch2 = visit(ctx->query_operator()[1]);
  auto child1 = std::any_cast<qop_ptr>(ch1);
  auto child2 = std::any_cast<qop_ptr>(ch2);
  child1->connect(
      qop, std::bind(&union_all_op::process_right, qop.get(), ph::_1, ph::_2),
      std::bind(&union_all_op::finish, qop.get(), ph::_1));
  child2->connect(
      qop, std::bind(&union_all_op::process_left, qop.get(), ph::_1, ph::_2),
      std::bind(&union_all_op::finish, qop.get(), ph::_1));

  return std::make_any<qop_ptr>(qop);
}

std::any query_planner::visitExcept_op(poseidonParser::Except_opContext *ctx) {
  auto ch1 = visit(ctx->query_operator()[0]);
  auto ch2 = visit(ctx->query_operator()[1]);
  auto child1 = std::any_cast<qop_ptr>(ch1);
  auto child2 = std::any_cast<qop_ptr>(ch2);

  auto lhs = std::any_cast<expr>(visit(ctx->variable()[0]));
  auto rhs = std::any_cast<expr>(visit(ctx->variable()[1]));

  auto qop =
      std::make_shared<except_op>(std::dynamic_pointer_cast<variable>(lhs),
                                  std::dynamic_pointer_cast<variable>(rhs));

  child1->connect(
      qop, std::bind(&except_op::process_left, qop.get(), ph::_1, ph::_2),
      std::bind(&except_op::finish, qop.get(), ph::_1));
  child2->connect(
      qop, std::bind(&except_op::process_right, qop.get(), ph::_1, ph::_2),
      std::bind(&except_op::finish, qop.get(), ph::_1));
  // execute child2 before child1
  child1->priority_ = 0;
  child2->priority_ = 1;
  return std::make_any<qop_ptr>(qop);
}

std::any
query_planner::visitGroup_by_op(poseidonParser::Group_by_opContext *ctx) {
  std::vector<aggregate::expr> aggrs;
  std::vector<group_by::group> grps;

  auto ch = visit(ctx->query_operator());
  auto child = std::any_cast<qop_ptr>(ch);

  auto grp_list = ctx->grouping_list()->grouping_expr();
  for (auto &gexpr : grp_list) {
    auto gex = gexpr->variable();
    auto var = gex->Var()->getText();
    auto var_id = extract_tuple_id(var);
    std::string attr;
    if (gex->Identifier_() != nullptr)
      attr = gex->Identifier_()->getText();
    auto tspec = gex->type_spec();
    qr_type grp_type = int_type;
    if (tspec->IntType_() != nullptr)
      grp_type = int_type;
    else if (tspec->DoubleType_() != nullptr)
      grp_type = double_type;
    else if (tspec->StringType_() != nullptr)
      grp_type = string_type;
    else if (tspec->Uint64Type_() != nullptr)
      grp_type = uint64_type;
    else if (tspec->DateType_() != nullptr)
      grp_type = ptime_type;
    grps.push_back(group_by::group{var_id, attr, grp_type});
  }

  auto aggr_list = ctx->aggregate_list()->aggr_expr();
  for (auto &aggr : aggr_list) {
    auto expr = aggr->variable();
    auto v_id = extract_tuple_id(expr->Var()->getText());
    std::string v_name;
    if (expr->Identifier_() != nullptr)
      v_name = expr->Identifier_()->getText();
    auto tspec = expr->type_spec();

    aggregate::expr::func_t aggr_func = aggregate::expr::f_count;
    qr_type aggr_type = int_type;

    if (aggr->aggr_func()->Count_() != nullptr)
      aggr_func = aggregate::expr::f_count;
    else if (aggr->aggr_func()->Sum_() != nullptr)
      aggr_func = aggregate::expr::f_sum;
    else if (aggr->aggr_func()->Avg_() != nullptr)
      aggr_func = aggregate::expr::f_avg;
    else if (aggr->aggr_func()->Min_() != nullptr)
      aggr_func = aggregate::expr::f_min;
    else if (aggr->aggr_func()->Max_() != nullptr)
      aggr_func = aggregate::expr::f_max;

    if (tspec->IntType_() != nullptr)
      aggr_type = int_type;
    else if (tspec->DoubleType_() != nullptr)
      aggr_type = double_type;
    else if (tspec->StringType_() != nullptr)
      aggr_type = string_type;
    aggrs.push_back(aggregate::expr{aggr_func, v_id, v_name, aggr_type});
  }

  auto qp = std::make_shared<group_by>(grps, aggrs, qctx_.get_dictionary());
  auto qop = qop_append2(child, qp);
  return std::make_any<qop_ptr>(qop);
}

std::any
query_planner::visitDistinct_op(poseidonParser::Distinct_opContext *ctx) {
  auto qp = std::make_shared<distinct_tuples>();

  auto ch = visit(ctx->query_operator());
  auto child_op = std::any_cast<qop_ptr>(ch);
  auto qop = qop_append(child_op, qp);

  return std::make_any<qop_ptr>(qop);
}

std::any query_planner::visitMatch_op(poseidonParser::Match_opContext *ctx) {
  return visit(ctx->path_pattern());
}

std::any
query_planner::visitPath_pattern(poseidonParser::Path_patternContext *ctx) {
  // construct a node_scan with an optional filter
  auto ch = visit(ctx->node_pattern());
  auto node_op = std::any_cast<qop_ptr>(ch);

  // get the sequence of foreach_relationship + expand
  auto paths = ctx->path_component();
  auto origin_idx = 0;
  for (auto &p : paths) {
    /*
     * 1. Rship_pattern
     */
    auto label = p->rship_pattern()->Identifier_().back()->getText();
    auto dir = p->rship_pattern()->dir_spec();
    //  handle direction of relationship
    auto card_spec = p->rship_pattern()->cardinality_spec();

    unsigned int m1 = std::numeric_limits<unsigned int>::min();
    unsigned int m2 = std::numeric_limits<unsigned int>::max();
    if (card_spec != nullptr) {
      m1 = std::stoi(card_spec->min_cardinality()->INTEGER()->getText());
      if (card_spec->max_cardinality() != nullptr)
        m2 = std::stoi(card_spec->max_cardinality()->INTEGER()->getText());
      spdlog::info("min:max = {}/{}", m1, m2);
      if (dir[0]->no_dir() != nullptr && dir[1]->right_dir() != nullptr) {
        // -[]->
        auto rship_op = std::make_shared<foreach_variable_from_relationship>(
            label, m1, m2, origin_idx);
        node_op = qop_append(node_op, rship_op);
        auto get_op = std::make_shared<get_to_node>();
        node_op = qop_append(node_op, get_op);
      } else if (dir[0]->left_dir() != nullptr && dir[1]->no_dir() != nullptr) {
        // <-[]-
        auto rship_op = std::make_shared<foreach_variable_to_relationship>(
            label, m1, m2, origin_idx);
        node_op = qop_append(node_op, rship_op);
        auto get_op = std::make_shared<get_from_node>();
        node_op = qop_append(node_op, get_op);
      } else if (dir[0]->no_dir() != nullptr && dir[1]->no_dir() != nullptr) {
        // -[]-
        auto rship_op = std::make_shared<foreach_variable_all_relationship>(
            label, m1, m2, origin_idx);
        node_op = qop_append(node_op, rship_op);
        auto get_op = std::make_shared<get_to_node>();
        node_op = qop_append(node_op, get_op);
      } else {
        throw query_processing_error("invalid relationship direction");
      }
    } else {
      if (dir[0]->no_dir() != nullptr && dir[1]->right_dir() != nullptr) {
        // -[]->
        auto rship_op =
            std::make_shared<foreach_from_relationship>(label, origin_idx);
        node_op = qop_append(node_op, rship_op);
        auto get_op = std::make_shared<get_to_node>();
        node_op = qop_append(node_op, get_op);
      } else if (dir[0]->left_dir() != nullptr && dir[1]->no_dir() != nullptr) {
        // <-[]-
        auto rship_op =
            std::make_shared<foreach_to_relationship>(label, origin_idx);
        node_op = qop_append(node_op, rship_op);
        auto get_op = std::make_shared<get_from_node>();
        node_op = qop_append(node_op, get_op);
      } else if (dir[0]->no_dir() != nullptr && dir[1]->no_dir() != nullptr) {
        // -[]-
        auto rship_op =
            std::make_shared<foreach_all_relationship>(label, origin_idx);
        node_op = qop_append(node_op, rship_op);
        auto get_op = std::make_shared<get_to_node>();
        node_op = qop_append(node_op, get_op);
      } else {
        throw query_processing_error("invalid relationship direction");
      }
    }
    origin_idx++;
    /*
     * 2. Node_pattern
     */
    label = trim_string(p->node_pattern()->Identifier_().back()->getText());
    auto hl_op = std::make_shared<node_has_label>(label);
    node_op = qop_append(node_op, hl_op);
    origin_idx++;
  }
  return std::make_any<qop_ptr>(node_op);
}

expr query_planner::property_list_to_expr(properties_t &plist) {
  expr ex;
  for (auto &p : plist) {
    auto prop = p;
    expr_type lhs_type = expr_type::UNKNOWN;
    std::any &val = prop.second;
    expr rhs_expr;
    if (val.type() == typeid(int)) {
      rhs_expr = Int(std::any_cast<int>(val));
      lhs_type = expr_type::INT;
    } else if (val.type() == typeid(double) || val.type() == typeid(float)) {
      rhs_expr = Float(std::any_cast<double>(val));
      lhs_type = expr_type::DOUBLE;
    } else if (val.type() == typeid(std::string)) {
      rhs_expr = Str(std::any_cast<std::string>(val));
      lhs_type = expr_type::STRING;
    } else if (val.type() == typeid(long)) {
      rhs_expr = Int(std::any_cast<long>(val));
      lhs_type = expr_type::UINT64;
    } else
      spdlog::info("ERROR in property_list_to_expr: rhs not handled {}",
                   val.type().name());

    auto pcode = qctx_.get_code(prop.first);
    auto lhs_expr = Variable(0, prop.first, pcode, lhs_type);

    expr ex2 = EQ(lhs_expr, rhs_expr);
    if (ex)
      ex = AND(ex, ex2);
    else
      ex = ex2;
  }
  return ex;
}

std::any
query_planner::visitNode_pattern(poseidonParser::Node_patternContext *ctx) {
  auto label = ctx->Identifier_().back()->getText();
  if (ctx->property_list() == nullptr) {
    auto op = std::make_shared<node_scan>(trim_string(label));
    sources_.push_back(op);
    return std::make_any<qop_ptr>(op);
  }

  auto pl = visit(ctx->property_list());
  auto props = std::any_cast<properties_t>(pl);

  if (props.size() == 1) {
    // check whether we can process the node pattern with an index scan
    auto pname = props.begin()->first;
    spdlog::debug("lookup index {}.{}", label, pname);

    if (qctx_.gdb_ && qctx_.gdb_->has_index(label, pname)) {
      auto idx_id = qctx_.gdb_->get_index(label, pname);
      std::any &val = props.begin()->second;
      auto key = std::any_cast<int>(val);
      auto op = std::make_shared<index_scan>(idx_id, key);
      sources_.push_back(op);
      return std::make_any<qop_ptr>(op);
    }
  }
  auto op = std::make_shared<node_scan>(trim_string(label));
  sources_.push_back(op);

  // build a filter if a property list is given and an index couldn't be used
  auto ex = property_list_to_expr(props);
  auto qop = std::make_shared<filter_op>(ex);
  auto op2 = qop_append(op, qop);
  return std::make_any<qop_ptr>(op2);
}

std::any query_planner::visitCreate_op(poseidonParser::Create_opContext *ctx) {
  qop_ptr child = nullptr, qop = nullptr;
  ;
  if (ctx->query_operator() != nullptr) {
    auto ch = visit(ctx->query_operator());
    child = std::any_cast<qop_ptr>(ch);
  }
  if (ctx->create_node() != nullptr) {
    auto res = visit(ctx->create_node());
    auto qp = std::any_cast<std::shared_ptr<create_node>>(res);
    qop = !child ? qp : qop_append(child, qp);
  } else if (ctx->create_rship() != nullptr) {
    auto res = visit(ctx->create_rship());
    auto qp = std::any_cast<std::shared_ptr<create_relationship>>(res);
    qop = !child ? qp : qop_append(child, qp);
  }

  if (!child)
    sources_.push_back(qop);
  return std::make_any<qop_ptr>(qop);
}

std::any
query_planner::visitCreate_node(poseidonParser::Create_nodeContext *ctx) {
  properties_t props;
  if (ctx->property_list() != nullptr) {
    auto pl = visit(ctx->property_list());
    props = std::any_cast<properties_t>(pl);
  }
  auto label = trim_string(ctx->Identifier_().back()->getText());
  auto qp = std::make_shared<create_node>(label, props);
  return std::make_any<std::shared_ptr<create_node>>(qp);
}

std::any
query_planner::visitCreate_rship(poseidonParser::Create_rshipContext *ctx) {
  properties_t props;
  if (ctx->property_list() != nullptr) {
    auto pl = visit(ctx->property_list());
    props = std::any_cast<properties_t>(pl);
  }
  auto label = trim_string(ctx->Identifier_().back()->getText());
  auto from = extract_tuple_id(ctx->node_var()[0]->Var()->getText());
  auto to = extract_tuple_id(ctx->node_var()[1]->Var()->getText());
  auto nodes = std::make_pair(from, to);
  auto qp = std::make_shared<create_relationship>(label, props, nodes);
  return std::make_any<std::shared_ptr<create_relationship>>(qp);
}

std::any
query_planner::visitRemove_node_op(poseidonParser::Remove_node_opContext *ctx) {
  auto ch = visit(ctx->query_operator());
  auto child = std::any_cast<qop_ptr>(ch);

  auto qp = std::make_shared<remove_node>();
  auto qop = qop_append(child, qp);
  return std::make_any<qop_ptr>(qop);
}

std::any query_planner::visitRemove_relationship_op(
    poseidonParser::Remove_relationship_opContext *ctx) {
  auto ch = visit(ctx->query_operator());
  auto child = std::any_cast<qop_ptr>(ch);

  auto qp = std::make_shared<remove_relationship>();
  auto qop = qop_append(child, qp);
  return std::make_any<qop_ptr>(qop);
}

std::any
query_planner::visitDetach_node_op(poseidonParser::Detach_node_opContext *ctx) {
  auto ch = visit(ctx->query_operator());
  auto child = std::any_cast<qop_ptr>(ch);

  auto qp = std::make_shared<detach_node>();
  auto qop = qop_append(child, qp);
  return std::make_any<qop_ptr>(qop);
}

std::any
query_planner::visitAlgorithm_op(poseidonParser::Algorithm_opContext *ctx) {
  auto ch = visit(ctx->query_operator());
  auto child = std::any_cast<qop_ptr>(ch);

  auto algo_name = ctx->Identifier_()->getText();
  auto mode = ctx->call_mode()->TupleMode_() != nullptr ? algorithm_op::m_tuple
                                                        : algorithm_op::m_set;
  algorithm_op::param_list args;
  if (ctx->algo_param_list() != nullptr) {
    auto pl = visit(ctx->algo_param_list());
    args = std::any_cast<algorithm_op::param_list>(pl);
  }

  auto qp = std::make_shared<algorithm_op>(algo_name, mode, args);
  auto qop = qop_append(child, qp);
  return std::make_any<qop_ptr>(qop);
}

std::any query_planner::visitAlgo_param_list(
    poseidonParser::Algo_param_listContext *ctx) {
  algorithm_op::param_list args;
  for (auto &p : ctx->algo_param()) {
    std::any res;
    if (p->value()->INTEGER()) {
      try {
        res = std::make_any<int>(std::stoi(p->value()->INTEGER()->getText()));
      } catch (std::out_of_range &exc) {
        // if we are out of range we try to parse a uint64_t value
        res = std::make_any<uint64_t>(
            std::stoull(p->value()->INTEGER()->getText()));
      }
    } else if (p->value()->FLOAT())
      res = std::make_any<double>(std::stod(p->value()->FLOAT()->getText()));
    else if (p->value()->STRING_())
      res = std::make_any<std::string>(
          trim_string(p->value()->STRING_()->getText()));
    else
      assert("Algorithm parameter not handled");
    args.push_back(res);
  }
  return std::make_any<algorithm_op::param_list>(args);
}

std::any
query_planner::visitProperty_list(poseidonParser::Property_listContext *ctx) {
  properties_t props;
  for (auto &p : ctx->property()) {
    auto pname = p->Identifier_()->getText();
    std::any pval;
    if (p->value()->INTEGER())
      pval = std::stoi(p->value()->INTEGER()->getText());
    else if (p->value()->FLOAT())
      pval = std::stod(p->value()->FLOAT()->getText());
    else if (p->value()->STRING_())
      pval = trim_string(p->value()->STRING_()->getText());
    props.insert({pname, pval});
  }
  return std::make_any<properties_t>(props);
}

std::any
query_planner::visitLogical_expr(poseidonParser::Logical_exprContext *ctx) {
  int num_ex = ctx->boolean_expr().size();
  auto le = visit(ctx->boolean_expr(0));
  auto lhs_expr = std::any_cast<expr>(le);
  for (auto i = 1; i < num_ex; i++) {
    auto re = visit(ctx->boolean_expr(i));
    auto rhs_expr = std::any_cast<expr>(re);
    auto or_expr = OR(lhs_expr, rhs_expr);
    lhs_expr = or_expr;
  }
  return std::make_any<expr>(lhs_expr);
}

std::any
query_planner::visitBoolean_expr(poseidonParser::Boolean_exprContext *ctx) {
  int num_ex = ctx->equality_expr().size();
  auto le = visit(ctx->equality_expr(0));
  auto lhs_expr = std::any_cast<expr>(le);
  for (auto i = 1; i < num_ex; i++) {
    auto re = visit(ctx->equality_expr(i));
    auto rhs_expr = std::any_cast<expr>(re);
    auto and_expr = AND(lhs_expr, rhs_expr);
    lhs_expr = and_expr;
  }
  return std::make_any<expr>(lhs_expr);
}

std::any
query_planner::visitEquality_expr(poseidonParser::Equality_exprContext *ctx) {
  int num_ex = ctx->relational_expr().size();
  auto le = visit(ctx->relational_expr(0));
  auto lhs_expr = std::any_cast<expr>(le);
  for (auto i = 1; i < num_ex; i++) {
    auto re = visit(ctx->relational_expr(i));
    auto rhs_expr = std::any_cast<expr>(re);
    auto eq_expr = (ctx->EQUALS(i - 1) != nullptr) ? EQ(lhs_expr, rhs_expr)
                                                   : NEQ(lhs_expr, rhs_expr);
    lhs_expr = eq_expr;
  }
  return std::make_any<expr>(lhs_expr);
}

std::any query_planner::visitRelational_expr(
    poseidonParser::Relational_exprContext *ctx) {
  int num_ex = ctx->additive_expr().size();
  auto le = visit(ctx->additive_expr(0));
  auto lhs_expr = std::any_cast<expr>(le);
  for (auto i = 1; i < num_ex; i++) {
    auto re = visit(ctx->additive_expr(i));
    auto rhs_expr = std::any_cast<expr>(re);
    if (ctx->LT(i - 1) != nullptr) {
      auto mult_expr = LT(lhs_expr, rhs_expr);
      lhs_expr = mult_expr;
    } else if (ctx->LTEQ(i - 1) != nullptr) {
      auto mult_expr = LE(lhs_expr, rhs_expr);
      lhs_expr = mult_expr;
    } else if (ctx->GT(i - 1) != nullptr) {
      auto mult_expr = GT(lhs_expr, rhs_expr);
      lhs_expr = mult_expr;
    } else if (ctx->GTEQ(i - 1) != nullptr) {
      auto mult_expr = GE(lhs_expr, rhs_expr);
      lhs_expr = mult_expr;
    } else if (ctx->REGEX(i - 1) != nullptr) {
      if (rhs_expr->ftype_ != expr_type::STRING)
        throw query_processing_error("invalid regular expression");
      auto re_expr = RE(lhs_expr, rhs_expr);
      lhs_expr = re_expr;
    }
  }
  return std::make_any<expr>(lhs_expr);
}

std::any
query_planner::visitAdditive_expr(poseidonParser::Additive_exprContext *ctx) {
  int num_ex = ctx->multiplicative_expr().size();
  auto le = visit(ctx->multiplicative_expr(0));
  auto lhs_expr = std::any_cast<expr>(le);
  for (auto i = 1; i < num_ex; i++) {
    auto re = visit(ctx->multiplicative_expr(i));
    auto rhs_expr = std::any_cast<expr>(re);
    auto add_expr = (ctx->PLUS_(i - 1) != nullptr) ? PLUS(lhs_expr, rhs_expr)
                                                   : MINUS(lhs_expr, rhs_expr);
    lhs_expr = add_expr;
  }
  return std::make_any<expr>(lhs_expr);
}

std::any query_planner::visitMultiplicative_expr(
    poseidonParser::Multiplicative_exprContext *ctx) {
  int num_ex = ctx->unary_expr().size();
  auto le = visit(ctx->unary_expr(0));
  auto lhs_expr = std::any_cast<expr>(le);
  for (auto i = 1; i < num_ex; i++) {
    auto re = visit(ctx->unary_expr(i));
    auto rhs_expr = std::any_cast<expr>(re);
    if (ctx->MULT(i - 1) != nullptr) {
      auto mult_expr = MULT(lhs_expr, rhs_expr);
      lhs_expr = mult_expr;
    } else if (ctx->DIV(i - 1) != nullptr) {
      auto mult_expr = DIV(lhs_expr, rhs_expr);
      lhs_expr = mult_expr;
    } else if (ctx->MOD(i - 1) != nullptr) {
      auto mult_expr = MOD(lhs_expr, rhs_expr);
      lhs_expr = mult_expr;
    }
  }
  return std::make_any<expr>(lhs_expr);
}

std::any
query_planner::visitUnary_expr(poseidonParser::Unary_exprContext *ctx) {
  auto ex = visit(ctx->primary_expr());
  auto un_expr = std::any_cast<expr>(ex);
  return std::make_any<expr>((ctx->NOT() != nullptr) ? /*NOT*/ (un_expr)
                                                     : un_expr);
}

std::any
query_planner::visitPrimary_expr(poseidonParser::Primary_exprContext *ctx) {
  std::any res;

  if (ctx->logical_expr() != nullptr)
    res = visit(ctx->logical_expr());
  else if (ctx->value() != nullptr) {
    res = visit(ctx->value());
  } else if (ctx->variable() != nullptr) {
    res = visit(ctx->variable());
    /*
    auto var_id = extract_tuple_id(ctx->variable()->Var()->getText());
    auto var_type = typespec_to_exprtype(ctx->variable()->type_spec());
    // Identifier_ could be empty
    if (ctx->variable()->Identifier_() != nullptr) {
        auto attr = ctx->variable()->Identifier_()->getText();
        auto pcode = qctx_.get_code(attr);
        res = std::make_any<expr>(Variable(var_id, attr, pcode, var_type));
    }
    else
        res = std::make_any<expr>(Variable(var_id, var_type));
    */
  } else if (ctx->function_call() != nullptr) {
    // handle UDFs - TODO: should be combined with code in visitProject_op
    auto fc = ctx->function_call();
    auto prefix = fc->prefix()->getText();
    auto fc_name = fc->Identifier_()->getText();
    auto fc_params = fc->param_list()->param();
    std::vector<expr> param_list;

    for (auto i = 0u; i < fc_params.size(); i++) {
      auto &pm = fc_params[i];
      if (pm->value() != nullptr) {
        if (pm->value()->STRING_()) {
          param_list.push_back(Str(trim_string(pm->value()->getText())));
        } else if (pm->value()->INTEGER()) {
          param_list.push_back(Int(std::stoi(pm->value()->getText())));
        } else if (pm->value()->FLOAT()) {
          param_list.push_back(Float(std::stof(pm->value()->getText())));
        }
      } else {
        auto p_idx = extract_tuple_id(pm->Var()->getText());
        auto p_attr = pm->Identifier_();
        auto p_type = typespec_to_exprtype(pm->type_spec());
        // add parameter variable
        param_list.push_back(Variable(
            p_idx, p_attr != nullptr ? p_attr->getText() : "", p_type));
      }
    }
    if (prefix != "")
      res = std::make_any<expr>(Fct(prefix, fc_name, param_list));
    else
      res = std::make_any<expr>(Fct(fc_name, param_list));
  }
  return res;
}

std::any query_planner::visitVariable(poseidonParser::VariableContext *ctx) {
  std::any res;

  auto var_id = extract_tuple_id(ctx->Var()->getText());
  auto var_type = typespec_to_exprtype(ctx->type_spec());
  // Identifier_ could be empty
  if (ctx->Identifier_() != nullptr) {
    auto attr = ctx->Identifier_()->getText();
    auto pcode = qctx_.get_code(attr);
    res = std::make_any<expr>(Variable(var_id, attr, pcode, var_type));
  } else
    res = std::make_any<expr>(Variable(var_id, var_type));
  return res;
}

std::any query_planner::visitValue(poseidonParser::ValueContext *ctx) {
  std::any res;

  if (ctx->INTEGER()) {
    try {
      res = std::make_any<expr>(Int(std::stoi(ctx->INTEGER()->getText())));
    } catch (std::out_of_range &exc) {
      // if we are out of range we try to parse a uint64_t value
      res = std::make_any<expr>(UInt64(std::stoull(ctx->INTEGER()->getText())));
    }
  } else if (ctx->FLOAT())
    res = std::make_any<expr>(Float(std::stod(ctx->FLOAT()->getText())));
  else if (ctx->STRING_())
    res = std::make_any<expr>(Str(trim_string(ctx->STRING_()->getText())));
  else
    assert("Primary expression not handled");
  return res;
}

expr_type query_planner::typespec_to_exprtype(
    poseidonParser::Type_specContext *var_type) {
  expr_type ex_ty = expr_type::UNKNOWN;
  if (var_type->IntType_() != nullptr)
    ex_ty = expr_type::INT;
  else if (var_type->DoubleType_() != nullptr)
    ex_ty = expr_type::DOUBLE;
  else if (var_type->Uint64Type_() != nullptr)
    ex_ty = expr_type::UINT64;
  else if (var_type->StringType_() != nullptr)
    ex_ty = expr_type::STRING;
  else if (var_type->DateType_() != nullptr)
    ex_ty = expr_type::DATETIME;
  else if (var_type->NodeResultType_() != nullptr)
    ex_ty = expr_type::NODE;
  else if (var_type->RshipResultType_() != nullptr)
    ex_ty = expr_type::RELATIONSHIP;
  return ex_ty;
}
