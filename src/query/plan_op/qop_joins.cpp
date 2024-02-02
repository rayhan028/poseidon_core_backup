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

#include <functional>
#include <boost/container_hash/hash.hpp>
#include "qop_joins.hpp"
#include "profiling.hpp"
#include "expr_interpreter.hpp"

#define HASHER

void cross_join_op::dump(std::ostream &os) const { 
  os << "cross_join() - " << PROF_DUMP;
}

void cross_join_op::process_left(query_ctx &ctx, const qr_tuple &v) {
  PROF_PRE;
  uint64_t n = 0;
  for (auto &inp : input_) {
    auto res = concat(inp, v);
    consume_(ctx, res);
    n++;
  }
  PROF_POST(n);
}

void cross_join_op::process_right(query_ctx &ctx, const qr_tuple &v) {
  PROF_PRE0;
  input_.push_back(v);
  PROF_POST(0);
}

void cross_join_op::finish(query_ctx &ctx) { 
  if (++phases_ > 1)
    qop::default_finish(ctx); 
}

/* ------------------------------------------------------------------------ */

void nested_loop_join_op::dump(std::ostream &os) const { // TODO
  os << "nested_loop_join() - " << PROF_DUMP;
}

void nested_loop_join_op::process_left(query_ctx &ctx, const qr_tuple &v) {
  auto n = boost::get<node *>(v[left_right_nodes_.first]);
  auto nid = n->id();

  auto i = 0;
  for (auto id : join_ids_) {
    if (id == nid){ 
      auto res = concat(v, input_[i]);
      consume_(ctx, res);
    }
    i++;
  }
}

void nested_loop_join_op::process_right(query_ctx &ctx, const qr_tuple &v) {
  auto n = boost::get<node *>(v[left_right_nodes_.second]);
  auto nd = n->id();
  join_ids_.push_back(nd);
  input_.push_back(v);
}

void nested_loop_join_op::finish(query_ctx &ctx) { qop::default_finish(ctx); }

/* ------------------------------------------------------------------------ */

void hash_join_op::dump(std::ostream &os) const { // TODO
  os << "hash_join([" << lhs_var_->dump() << "," << rhs_var_->dump() << "]) - " << PROF_DUMP;
}

void hash_join_op::probe_phase(query_ctx &ctx, const qr_tuple &v) {
  // spdlog::info("probe phase");
  PROF_PRE;
  auto jval = get_var_value(ctx, v, rhs_var_);
  auto key = hasher(jval);
  
  auto n = 0u;
  std::shared_lock lock(m_);

  auto it = htable_.find(key);
  if (it != htable_.end()) {
    auto& vec = it->second;
    for (auto& jc : vec) {
    // spdlog::info("probe: {}", key);
      // std::cout << "jval: " << jval << " == jc: " << jc.first << std::endl;
      if (jval == jc.first) {
        auto res = concat(jc.second, v);
        consume_(ctx, res);
        n++;
      }
    }
  }
  // else spdlog::info("key not found: {}", key);
  PROF_POST(n);
}

void hash_join_op::build_phase(query_ctx &ctx, const qr_tuple &v) {
  // spdlog::info("build phase");
  PROF_PRE;
  auto jval = get_var_value(ctx, v, lhs_var_);
  auto key = hasher(jval);
  std::unique_lock lock(m_);
  auto it = htable_.find(key);
  if (it != htable_.end())
    it->second.push_back(std::make_pair(jval, v));
  else {
    std::vector<join_candidate> vec { std::make_pair(jval, v) };
    htable_.emplace(key, vec);
  }
  PROF_POST(0);
}

query_result hash_join_op::get_var_value(query_ctx& ctx, const qr_tuple& v, std::shared_ptr<variable> var) {
  auto inp = v[var->id_];
  query_result res;

  if (inp.which() == node_ptr_type || inp.which() == rship_ptr_type) {
    switch(var->result_type()) {
      case expr_type::INT: res = qv_(get_property_value<int>(ctx, v, var->id_, var->pcode_)); break; 
      case expr_type::UINT64: res = qv_(get_property_value<uint64_t>(ctx, v, var->id_, var->pcode_)); break; 
      case expr_type::DOUBLE: res = qv_(get_property_value<double>(ctx, v, var->id_, var->pcode_)); break; 
      case expr_type::STRING: res = qv_(get_property_value<std::string>(ctx, v, var->id_, var->pcode_)); break; 
      case expr_type::DATETIME: res = qv_(get_property_value<boost::posix_time::ptime>(ctx, v, var->id_, var->pcode_)); break; 
      default: break;
    }
  }
  else
    res = inp;
  return res;     
}


uint64_t hash_join_op::hasher(const query_result& q) {
  return boost::hash<query_result>()(q);
}

void hash_join_op::finish(query_ctx &ctx) { 
  if (++phases_ > 1)
    qop::default_finish(ctx); 
  }

/* ------------------------------------------------------------------------ */

void left_outer_join_op::dump(std::ostream &os) const {
  os << "left_outer_join(";
  if (ex_) 
    os << ex_->dump();
  os << ") - " << PROF_DUMP;
}

void left_outer_join_op::process_left(query_ctx &ctx, const qr_tuple &v) {
  PROF_PRE;
  bool dangling_tuple = true;
  uint64_t n = 0;
  for (auto &t : input_) {
    bool tp = false;
    if (ex_) {
      auto tv = concat(v, t);
      tp = interpret_bool_expression(ctx, ex_, tv);
    }
    else
      tp = pred_(v, t);
    if (tp) {
      dangling_tuple = false;
      auto res = concat(v, t);
      consume_(ctx, res);
      n++;
    }
  }
  if (/*!input_.empty() &&*/ dangling_tuple) {
    // TODO: determine size (width) of input_
    auto space = input_.empty() ? 1 : input_.front().size();
    qr_tuple nll(space, query_result(null_t(-1)));
    auto res = concat(v, nll);
    consume_(ctx, res);
    n++;
  }
  PROF_POST(n);
}

void left_outer_join_op::process_right(query_ctx &ctx, const qr_tuple &v) {
  PROF_PRE0;
  input_.push_back(v);
  PROF_POST(0);
}

void left_outer_join_op::finish(query_ctx &ctx) { 
  if (++phases_ > 1)
    qop::default_finish(ctx); 
}