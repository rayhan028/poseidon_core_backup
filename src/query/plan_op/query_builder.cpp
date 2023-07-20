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

#include "query_builder.hpp"
#include "qop_joins.hpp"
// #include "lua_poseidon.hpp"
#include "qop_updates.hpp"
#include "qop_aggregates.hpp"
#include "query_printer.hpp"
#include <memory>

namespace ph = std::placeholders;

query_builder::query_builder(query_ctx& ctx, qop_ptr qop) : ctx_(ctx) {
  plan_head_ = qop;
  // initialize plan_tail_
  plan_tail_ = qop;
  while (plan_tail_->has_subscriber())
    plan_tail_ = plan_tail_->subscriber();
}

query_builder &query_builder::operator=(const query_builder &q) {
  ctx_ = q.ctx_;
  plan_head_ = q.plan_head_;
  plan_tail_ = q.plan_tail_;
  return *this;
}

query_builder &query_builder::append_op(qop_ptr op, qop::consume_func cf) {
  if (!plan_head_)
    plan_head_ = op;
  else
    plan_tail_->connect(op, cf);
  plan_tail_ = op;

  return *this;
}

query_builder &query_builder::append_op(qop_ptr op, qop::consume_func cf, qop::finish_func ff) {
  if (!plan_head_)
    plan_head_ = op;
  else
    plan_tail_->connect(op, cf, ff);
  plan_tail_ = op;

  return *this;
}

query_builder &query_builder::all_nodes(const std::string &label) {  
  plan_head_ = plan_tail_ = std::make_shared<scan_nodes>(label);
  return *this;
}

query_builder &query_builder::all_nodes(std::map<std::size_t, std::vector<std::size_t>> &range_map, const std::string &label) {
  plan_head_ = plan_tail_ = std::make_shared<scan_nodes>(label, range_map);
  return *this;
}

query_builder &query_builder::nodes_where(const std::string &label, const std::string &key,
                          std::function<bool(const p_item &)> pred) {
  plan_head_ = plan_tail_ = std::make_shared<scan_nodes>(label);
  auto op = std::make_shared<is_property>(key, pred);
  return append_op(op,
                   std::bind(&is_property::process, op.get(), ph::_1, ph::_2));
}

query_builder &query_builder::nodes_where(const std::vector<std::string> &labels, const std::string &key,
                          std::function<bool(const p_item &)> pred) {
  plan_head_ = plan_tail_ = std::make_shared<scan_nodes>(labels);
  auto op = std::make_shared<is_property>(key, pred);
  return append_op(op,
                   std::bind(&is_property::process, op.get(), ph::_1, ph::_2));
}

query_builder &query_builder::nodes_where_indexed(const std::string &label, const std::string &prop, uint64_t val) {
  auto idx = ctx_.gdb_->get_index(label, prop);
  plan_head_ = plan_tail_ = std::make_shared<index_scan>(idx, val);
  return *this;
}

query_builder &query_builder::nodes_where_indexed(const std::vector<std::string> &labels,
                                  const std::string &prop, uint64_t val) {
  std::list<index_id> idxs;
  for (auto &label : labels) {
    auto idx = ctx_.gdb_->get_index(label, prop);
    idxs.push_back(idx);
  }
  plan_head_ = plan_tail_ = std::make_shared<index_scan>(idxs, val);
  return *this;
}

query_builder &query_builder::to_relationships(const std::string &label, int pos) {
  auto op = std::make_shared<foreach_to_relationship>(label, pos);
  return append_op(op, std::bind(&foreach_to_relationship::process, op.get(),
                                 ph::_1, ph::_2));
}

query_builder &query_builder::to_relationships(std::pair<int, int> range,
                               const std::string &label, int pos) {
  auto op = std::make_shared<foreach_variable_to_relationship>(
      label, range.first, range.second, pos);
  return append_op(op, std::bind(&foreach_variable_to_relationship::process,
                                 op.get(), ph::_1, ph::_2));
}

query_builder &query_builder::from_relationships(const std::string &label, int pos) {
  auto op = std::make_shared<foreach_from_relationship>(label, pos);
  return append_op(op, std::bind(&foreach_from_relationship::process, op.get(),
                                 ph::_1, ph::_2));
}

query_builder &query_builder::from_relationships(std::pair<int, int> range,
                                 const std::string &label, int pos) {
  auto op = std::make_shared<foreach_variable_from_relationship>(
      label, range.first, range.second, pos);
  return append_op(op, std::bind(&foreach_variable_from_relationship::process,
                                 op.get(), ph::_1, ph::_2));
}

query_builder &query_builder::all_relationships(const std::string &label, int pos) {
  auto op = std::make_shared<foreach_all_relationship>(label, pos);
  return append_op(op, std::bind(&foreach_all_relationship::process, op.get(),
                                 ph::_1, ph::_2));
}

query_builder &query_builder::all_relationships(std::pair<int, int> range,
                                 const std::string &label, int pos) {
  auto op = std::make_shared<foreach_variable_all_relationship>(
      label, range.first, range.second, pos);
  return append_op(op, std::bind(&foreach_variable_all_relationship::process,
                                 op.get(), ph::_1, ph::_2));
}

query_builder &query_builder::property(const std::string &key,
                       std::function<bool(const p_item &)> pred) {
  auto op = std::make_shared<is_property>(key, pred);
  return append_op(op,
                   std::bind(&is_property::process, op.get(), ph::_1, ph::_2));
}

query_builder &query_builder::filter(const expr &ex) {
  auto op = std::make_shared<filter_tuple>(ex);
  return append_op(op,
                   std::bind(&filter_tuple::process, op.get(), ph::_1, ph::_2));
}

query_builder &query_builder::to_node(const std::string &label) {
  auto op = std::make_shared<get_to_node>();
  append_op(op, std::bind(&get_to_node::process, op.get(), ph::_1, ph::_2));
  if (!label.empty()) {
    auto op2 = std::make_shared<node_has_label>(label);
    return append_op(
        op2, std::bind(&node_has_label::process, op2.get(), ph::_1, ph::_2));
  }
  return *this;
}

query_builder &query_builder::to_node(const std::vector<std::string> &labels) {
  auto op = std::make_shared<get_to_node>();
  append_op(op, std::bind(&get_to_node::process, op.get(), ph::_1, ph::_2));
  auto op2 = std::make_shared<node_has_label>(labels);
  return append_op(
      op2, std::bind(&node_has_label::process, op2.get(), ph::_1, ph::_2));
}

query_builder &query_builder::from_node(const std::string &label) {
  auto op = std::make_shared<get_from_node>();
  append_op(op, std::bind(&get_from_node::process, op.get(), ph::_1, ph::_2));
  if (!label.empty()) {
    auto op2 = std::make_shared<node_has_label>(label);
    return append_op(
        op2, std::bind(&node_has_label::process, op2.get(), ph::_1, ph::_2));
  }
  return *this;
}

query_builder &query_builder::from_node(const std::vector<std::string> &labels) {
  auto op = std::make_shared<get_from_node>();
  append_op(op, std::bind(&get_from_node::process, op.get(), ph::_1, ph::_2));
  auto op2 = std::make_shared<node_has_label>(labels);
  return append_op(
      op2, std::bind(&node_has_label::process, op2.get(), ph::_1, ph::_2));
}

query_builder &query_builder::has_label(const std::string &label) {
  auto op = std::make_shared<node_has_label>(label);
  return append_op(
      op, std::bind(&node_has_label::process, op.get(), ph::_1, ph::_2));
}

query_builder &query_builder::has_label(const std::vector<std::string> &labels) {
  auto op = std::make_shared<node_has_label>(labels);
  return append_op(
      op, std::bind(&node_has_label::process, op.get(), ph::_1, ph::_2));
}

query_builder &query_builder::limit(std::size_t n) {
  auto op = std::make_shared<limit_result>(n);
  return append_op(op,
                   std::bind(&limit_result::process, op.get(), ph::_1, ph::_2));
}

query_builder &query_builder::rship_exists(std::pair<int, int> src_des, bool append_null) {
  auto op = std::make_shared<nodes_connected>(src_des, append_null);
  return append_op(op,
                   std::bind(&nodes_connected::process, op.get(), ph::_1, ph::_2));
}

query_builder &query_builder::print() {
  auto op = std::make_shared<printer>();
  return append_op(op, std::bind(&printer::process, op.get(), ph::_1, ph::_2));
}

query_builder &query_builder::collect(result_set &rs) {
  auto op = std::make_shared<collect_result>(rs);
  return append_op(
      op, std::bind(&collect_result::process, op.get(), ph::_1, ph::_2),
      std::bind(&collect_result::finish, op.get(), ph::_1));
}

query_builder &query_builder::finish() {
  auto op = std::make_shared<end_pipeline>();
  return append_op(op, std::bind(&end_pipeline::process, op.get()));
}

query_builder &query_builder::project(const projection::expr_list &exprs) {
  auto op = std::make_shared<projection>(exprs);
  return append_op(op,
                   std::bind(&projection::process, op.get(), ph::_1, ph::_2));
}

query_builder &query_builder::project(std::vector<projection_expr> prexpr) {
  auto op = std::make_shared<projection>(prexpr);
  return append_op(op,
                   std::bind(&projection::process, op.get(), ph::_1, ph::_2));  
}

query_builder &query_builder::orderby(std::function<bool(const qr_tuple &, const qr_tuple &)> cmp) {
  auto op = std::make_shared<order_by>(cmp);
  return append_op(op, std::bind(&order_by::process, op.get(), ph::_1, ph::_2),
                   std::bind(&order_by::finish, op.get(), ph::_1));
}

query_builder &query_builder::groupby(const std::vector<group_by::group>& grps, const std::vector<group_by::expr>& exprs) {
  auto op = std::make_shared<group_by>(grps, exprs);
  return append_op(op, std::bind(&group_by::process, op.get(), ph::_1, ph::_2),
                   std::bind(&group_by::finish, op.get(), ph::_1));
}

query_builder &query_builder::aggr(const std::vector<aggregate::expr>& exprs) {
  auto op = std::make_shared<aggregate>(exprs);
  return append_op(op, std::bind(&aggregate::process, op.get(), ph::_1, ph::_2),
                   std::bind(&aggregate::finish, op.get(), ph::_1));

}

/*
query_builder &
query_builder::groupby(const std::vector<std::size_t> &pos) {
  auto op = std::make_shared<group_by>(pos);
  return append_op(op, std::bind(&group_by::process, op.get(), ph::_1, ph::_2),
                   std::bind(&group_by::finish, op.get(), ph::_1));
}

query_builder &
query_builder::groupby(const std::vector<std::size_t> &pos,
  const std::vector<std::pair<std::string, std::size_t>> &aggrs) {
  auto op = std::make_shared<group_by>(pos, aggrs);
  return append_op(op, std::bind(&group_by::process, op.get(), ph::_1, ph::_2),
                   std::bind(&group_by::finish, op.get(), ph::_1));
}

query_builder &
query_builder::groupby(std::list<qr_tuple> &grps, const std::vector<std::size_t> &pos,
    const std::vector<std::pair<std::string, std::size_t>> &aggrs) {
  auto op = std::make_shared<group_by>(grps, pos, aggrs);
  return append_op(op, std::bind(&group_by::process, op.get(), ph::_1, ph::_2),
                   std::bind(&group_by::finish, op.get(), ph::_1));
}
*/

query_builder &
query_builder::distinct() {
  auto op = std::make_shared<distinct_tuples>();
  return append_op(op, std::bind(&distinct_tuples::process, op.get(), ph::_1, ph::_2));
}

query_builder &
query_builder::where_qr_tuple(std::function<bool(const qr_tuple &)> pred) {
  auto op = std::make_shared<filter_tuple>(pred);
  return append_op(op,
                   std::bind(&filter_tuple::process, op.get(), ph::_1, ph::_2));
}

query_builder &
query_builder::append_to_qr_tuple(std::function<query_result(const qr_tuple &)> func) {
  auto op = std::make_shared<qr_tuple_append>(func);
  return append_op(op,
                   std::bind(&qr_tuple_append::process, op.get(), ph::_1, ph::_2));
}

query_builder &query_builder::union_all(query_builder &other) {
  auto op = std::make_shared<union_all_qres>();
  other.append_op(
      op, std::bind(&union_all_qres::process_right, op.get(), ph::_1, ph::_2));
  return append_op(
      op, std::bind(&union_all_qres::process_left, op.get(), ph::_1, ph::_2),
      std::bind(&union_all_qres::finish, op.get(), ph::_1));
}

query_builder &query_builder::union_all(std::initializer_list<query_builder *> queries) {
  auto op = std::make_shared<union_all_qres>();
  for (auto &q : queries)
    q->append_op(
        op, std::bind(&union_all_qres::process_right, op.get(), ph::_1, ph::_2));
  return append_op(
      op, std::bind(&union_all_qres::process_left, op.get(), ph::_1, ph::_2),
      std::bind(&union_all_qres::finish, op.get(), ph::_1));
}

query_builder &query_builder::count() {
  auto op = std::make_shared<count_result>();
  return append_op(op,
                   std::bind(&count_result::process, op.get(), ph::_1, ph::_2),
                   std::bind(&count_result::finish, op.get(), ph::_1));
}

query_builder &query_builder::crossjoin(query_builder &other) {
  auto op = std::make_shared<cross_join>(other.plan_head());
  other.append_op(
      op, std::bind(&cross_join::process_right, op.get(), ph::_1, ph::_2));
  return append_op(
      op, std::bind(&cross_join::process_left, op.get(), ph::_1, ph::_2),
      std::bind(&cross_join::finish, op.get(), ph::_1));
}

query_builder &query_builder::join_on_node(std::pair<int, int> left_right, query_builder &other) {
  auto op = std::make_shared<nested_loop_join>(left_right, other.plan_head());
  other.append_op(
      op, std::bind(&nested_loop_join::process_right, op.get(), ph::_1, ph::_2));
  return append_op(
      op, std::bind(&nested_loop_join::process_left, op.get(), ph::_1, ph::_2),
      std::bind(&nested_loop_join::finish, op.get(), ph::_1));
}

query_builder &query_builder::hashjoin_on_node(std::pair<int, int> left_right, query_builder &other) {
  auto op = std::make_shared<hash_join>(left_right, other.plan_head());
  other.append_op(
      op, std::bind(&hash_join::build_phase, op.get(), ph::_1, ph::_2));
  return append_op(
      op, std::bind(&hash_join::probe_phase, op.get(), ph::_1, ph::_2),
      std::bind(&hash_join::finish, op.get(), ph::_1));
}

query_builder &query_builder::outerjoin_on_node(const std::pair<int, int> &left_right, query_builder &other) {
  auto op = std::make_shared<left_outerjoin_on_node>(left_right, other.plan_head());
  other.append_op(
      op, std::bind(&left_outerjoin_on_node::process_right, op.get(), ph::_1, ph::_2));
  return append_op(
      op, std::bind(&left_outerjoin_on_node::process_left, op.get(), ph::_1, ph::_2),
      std::bind(&left_outerjoin_on_node::finish, op.get(), ph::_1));
}

query_builder &query_builder::outerjoin(query_builder &other, std::function<bool(const qr_tuple &, const qr_tuple &)> pred) {
  auto op = std::make_shared<left_outerjoin>(pred);
  other.append_op(
      op, std::bind(&left_outerjoin::process_right, op.get(), ph::_1, ph::_2),
      std::bind(&left_outerjoin::finish, op.get(), ph::_1));
  return append_op(
      op, std::bind(&left_outerjoin::process_left, op.get(), ph::_1, ph::_2),
      std::bind(&left_outerjoin::finish, op.get(), ph::_1));
}

query_builder &query_builder::join_on_rship(std::pair<int, int> src_des, query_builder &other) {
  auto op = std::make_shared<rship_join>(src_des);
  other.append_op(
      op, std::bind(&rship_join::process_right, op.get(), ph::_1, ph::_2));
  return append_op(
      op, std::bind(&rship_join::process_left, op.get(), ph::_1, ph::_2),
      std::bind(&rship_join::finish, op.get(), ph::_1));
}

query_builder &query_builder::outerjoin_on_rship(std::pair<int, int> src_des, query_builder &other) {
  auto op = std::make_shared<left_outerjoin_on_rship>(src_des);
  other.append_op(
      op, std::bind(&left_outerjoin_on_rship::process_right, op.get(), ph::_1, ph::_2));
  return append_op(
      op, std::bind(&left_outerjoin_on_rship::process_left, op.get(), ph::_1, ph::_2),
      std::bind(&left_outerjoin_on_rship::finish, op.get(), ph::_1));
}

query_builder &query_builder::algo_shortest_path(std::pair<std::size_t, std::size_t> start_stop,
                      rship_predicate rpred, bool bidirectional, bool all_spaths) {
  auto op = std::make_shared<shortest_path_opr>(start_stop, rpred, bidirectional, all_spaths);
  return append_op(op,
                   std::bind(&shortest_path_opr::process, op.get(), ph::_1, ph::_2));
}

query_builder &query_builder::algo_weighted_shortest_path(std::pair<std::size_t, std::size_t> start_stop,
            rship_predicate rpred, rship_weight weight, bool bidirectional, bool all_spaths) {
  auto op = std::make_shared<weighted_shortest_path_opr>(start_stop, rpred, weight,
                                                          bidirectional, all_spaths);
  return append_op(op,
                   std::bind(&weighted_shortest_path_opr::process, op.get(), ph::_1, ph::_2));
}

#ifdef USE_GUNROCK
query_builder &query_builder::gunrock_bfs(std::size_t start, bool bidirectional) {
  auto op = std::make_shared<gunrock_bfs_opr>(start, bidirectional);
  return append_op(op,
                   std::bind(&gunrock_bfs_opr::process, op.get(), ph::_1, ph::_2));
}

query_builder &query_builder::gunrock_sssp(std::size_t start, rship_weight weight, bool bidirectional) {
  auto op = std::make_shared<gunrock_sssp_opr>(start, weight, bidirectional);
  return append_op(op,
                   std::bind(&gunrock_sssp_opr::process, op.get(), ph::_1, ph::_2));
}

query_builder &query_builder::gunrock_pr(bool bidirectional) {
  auto op = std::make_shared<gunrock_pr_opr>(bidirectional);
  return append_op(op,
                   std::bind(&gunrock_pr_opr::process, op.get(), ph::_1, ph::_2));
}
#endif

query_builder &query_builder::algo_k_weighted_shortest_path(std::pair<std::size_t, std::size_t> start_stop,
      std::size_t k, rship_predicate rpred, rship_weight weight, bool bidirectional) {
  auto op = std::make_shared<k_weighted_shortest_path_opr>(start_stop, k, rpred, weight, bidirectional);
  return append_op(op,
                   std::bind(&k_weighted_shortest_path_opr::process, op.get(), ph::_1, ph::_2));
}

query_builder &query_builder::csr(rship_weight weight, bool bidirectional, std::size_t pos) {
  auto op = std::make_shared<csr_data>(weight, bidirectional, pos);
  return append_op(op,
                   std::bind(&csr_data::process, op.get(), ph::_1, ph::_2));
}

/*
query_builder &query_builder::call_lua(const std::string &proc_name,
                       const std::vector<std::size_t> &params) {
  auto op = std::make_shared<call_lua_procedure>(graph_db_, proc_name, params);
  return append_op(
      op, std::bind(&call_lua_procedure::process, op.get(), ph::_1, ph::_2));
}
*/

query_builder &query_builder::create(const std::string &label, const properties_t &props) {
  auto op = std::make_shared<create_node>(label, props);
  return append_op(op,
                   std::bind(&create_node::process, op.get(), ph::_1, ph::_2));
}

query_builder &query_builder::create_rship(std::pair<int, int> src_des, const std::string &label,
                           const properties_t &props) {
  auto op = std::make_shared<create_relationship>(label, props, src_des);
  return append_op(
      op, std::bind(&create_relationship::process, op.get(), ph::_1, ph::_2));
}

query_builder &query_builder::create_rship(query_builder &other, int l_node_pos, const std::string &label,
                        const properties_t &props, bool src_to_des) {
  auto op = std::make_shared<create_rship_on_join>(label, props, l_node_pos, src_to_des);
  other.append_op(
      op, std::bind(&create_rship_on_join::process_right, op.get(), ph::_1, ph::_2));
  return append_op(
      op, std::bind(&create_rship_on_join::process_left, op.get(), ph::_1, ph::_2),
      std::bind(&create_rship_on_join::finish, op.get(), ph::_1));
}

query_builder &query_builder::update(std::size_t var, properties_t &props) {
  auto op = std::make_shared<update_node>(var, props);
  return append_op(op,
                   std::bind(&update_node::process, op.get(), ph::_1, ph::_2));
}

query_builder &query_builder::delete_detach(const std::size_t pos) {
  auto op = std::make_shared<detach_node>(pos);
  return append_op(op,
                   std::bind(&detach_node::process, op.get(), ph::_1, ph::_2));
}

query_builder &query_builder::delete_node(const std::size_t pos) {
  auto op = std::make_shared<remove_node>(pos);
  return append_op(op,
                   std::bind(&remove_node::process, op.get(), ph::_1, ph::_2));
}

query_builder &query_builder::delete_rship(const std::size_t pos) {
  auto op = std::make_shared<remove_rship>(pos);
  return append_op(op,
                   std::bind(&remove_rship::process, op.get(), ph::_1, ph::_2));
}

void query_builder::start(query_ctx& ctx) { 
  assert(ctx.gdb_->get_dictionary());
  plan_head_->start(ctx); 
}

void query_builder::start(query_ctx& ctx, std::initializer_list<query_builder *> queries) {
  for (auto &q : queries) {
    q->start(ctx);
  }
}

void query_builder::print_plan(std::ostream& os) {
    os << ">>---------------------------------------------------------------------->>\n";
    auto qop_tree = build_qop_tree(plan_head_);
    qop_tree.first->print(os);
    print_plan_helper(os, qop_tree.first, "");
    os << "<<----------------------------------------------------------------------<<\n";
}

void query_builder::print_plans(std::initializer_list<query_builder *> queries, std::ostream& os) {
    std::vector<qop_node_ptr> trees;
    for (auto &q : queries) {
        auto qop_tree = build_qop_tree(q->plan_head_);
        trees.push_back(qop_tree.first);
    }

    std::list<qop_node_ptr> bin_ops;
    for (auto& t : trees) {
      collect_binary_ops(t, bin_ops);
    }
    // merge trees
    for (auto i = 1u; i < trees.size(); i++) {
        merge_qop_trees(trees[0], trees[i], bin_ops);
    }
    os << ">>---------------------------------------------------------------------->>\n";
    trees[0]->print(os);
    print_plan_helper(os, trees[0], "");
    os << "<<----------------------------------------------------------------------<<\n";
}


#ifdef QOP_RECOVERY
query_builder &
query_builder::recover_results() {
  plan_head_ = plan_tail_ = std::make_shared<recover_scan>();
  return *this;
}

query_builder &
query_builder::continue_scan(std::map<std::size_t, std::size_t> &cp, const std::string &label) {
  plan_head_ = plan_tail_ = std::make_shared<continue_scan_nodes>(cp, label);
  return *this;
}

query_builder &
query_builder::persist() {
  auto op = std::make_shared<persist_result>();
  return append_op(op,
                   std::bind(&persist_result::process, op.get(), ph::_1, ph::_2));
}

query_builder &
query_builder::pgroupby(const std::vector<std::size_t> &pos,
  const std::vector<std::pair<std::string, std::size_t>> &aggrs) {
  auto op = std::make_shared<persistent_group_by>(pos, aggrs);
  return append_op(op, std::bind(&persistent_group_by::process, op.get(), ph::_1, ph::_2),
                   std::bind(&persistent_group_by::finish, op.get(), ph::_1));
}

query_builder &
query_builder::crash(std::size_t n) {
  auto op = std::make_shared<crash_at>(n);
  return append_op(op,
                   std::bind(&crash_at::process, op.get(), ph::_1, ph::_2));
}

void query_builder::extract_args() {
  std::map<offset_t, offset_t> args_map;
  offset_t opid = 0;
  if(auto ns = std::dynamic_pointer_cast<scan_nodes>(plan_head_)) {
    if(ns->labels.empty()) {
        offset_t lc = ctx_.gdb_->get_dictionary()->lookup_string(ns->label);
        args_map[opid++] = lc;
    } else {
      for(auto & l : ns->labels) {
        offset_t lc = ctx_.gdb_->get_dictionary()->lookup_string(l);
        args_map[opid++] = lc;
      }
    }
  } else if(auto fr = std::dynamic_pointer_cast<foreach_from_relationship>(plan_head_)) {
    if(!fr->label.empty()) {
        offset_t lc = ctx_.gdb_->get_dictionary()->lookup_string(fr->label);
        args_map[opid] = lc;
    }
    opid++;
  } else if(auto tr = std::dynamic_pointer_cast<foreach_to_relationship>(plan_head_)) {
    if(!tr->label.empty()) {
        offset_t lc = ctx_.gdb_->get_dictionary()->lookup_string(fr->label);
        args_map[opid] = lc;
    }
    opid++;
  }
}

#endif