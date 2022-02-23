/*
 * Copyright (C) 2019-2020 DBIS Group - TU Ilmenau, All Rights Reserved.
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

#include "join.hpp"
#include "profiling.hpp"
#include "expr_interpreter.hpp"

#define HASHER

void cross_join::dump(std::ostream &os) const { 
  os << "cross_join() - " << PROF_DUMP;
}

void cross_join::process_left(query_ctx &ctx, const qr_tuple &v) {
  for (auto &inp : input_) {
    auto res = concat(inp, v);
    consume_(ctx, res);
  }
}

void cross_join::process_right(query_ctx &ctx, const qr_tuple &v) {
  input_.push_back(v);
}

void cross_join::finish(query_ctx &ctx) { qop::default_finish(ctx); }

/* ------------------------------------------------------------------------ */

void nested_loop_join::dump(std::ostream &os) const { // TODO
  os << "nested_loop_join() - " << PROF_DUMP;
}

void nested_loop_join::process_left(query_ctx &ctx, const qr_tuple &v) {
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

void nested_loop_join::process_right(query_ctx &ctx, const qr_tuple &v) {
  auto n = boost::get<node *>(v[left_right_nodes_.second]);
  auto nd = n->id();
  join_ids_.push_back(nd);
  input_.push_back(v);
}

void nested_loop_join::finish(query_ctx &ctx) { qop::default_finish(ctx); }

/* ------------------------------------------------------------------------ */

void hash_join::dump(std::ostream &os) const { // TODO
  os << "hash_join() - " << PROF_DUMP;
}

void hash_join::probe_phase(query_ctx &ctx, const qr_tuple &v) {
  auto n = boost::get<node *>(v[left_right_nodes_.first]);
  auto nid = n->id();
  #ifdef HASHER
  int bucket = int(hasher(nid) % BUCKETS);
  #else
  int bucket = int(nid % BUCKETS);
  #endif
  
  auto i = 0;
  for (auto id : join_ids_[bucket]) {
    if (id == nid){
      auto res = concat(v, input_[bucket][i]);
      consume_(ctx, res);
    }
    i++;
  }
}

void hash_join::build_phase(query_ctx &ctx, const qr_tuple &v) {
  auto n = boost::get<node *>(v[left_right_nodes_.second]);
  auto nd = n->id();

  #ifdef HASHER
  int bucket = int(hasher(nd) % BUCKETS);
  #else
  int bucket = int(nd % BUCKETS);
  #endif
  join_ids_[bucket].push_back(nd);
  input_[bucket].push_back(v);
}

uint64_t hash_join::hasher(uint64_t id){
  id = (id ^ (id >> 30)) * UINT64_C(0xbf58476d1ce4e5b9);
  id = (id ^ (id >> 27)) * UINT64_C(0x94d049bb133111eb);
  id = id ^ (id >> 31);
  return id;
}
void hash_join::finish(query_ctx &ctx) { qop::default_finish(ctx); }

/* ------------------------------------------------------------------------ */

void left_outerjoin_on_node::dump(std::ostream &os) const {
  os << "left_outerjoin_on_node() - " << PROF_DUMP;
}

void left_outerjoin_on_node::process_left(query_ctx &ctx, const qr_tuple &v) {
  auto n = boost::get<node *>(v[left_right_nodes_.first]);
  auto nid = n->id();

  auto i = 0;
  bool dangling_tuple = true;
  for (auto id : join_ids_) {
    if (id == nid){
      dangling_tuple = false;
      auto res = concat(v, input_[i]);
      consume_(ctx, res);
    }
    i++;
  }
  if (dangling_tuple) {
    qr_tuple nll(input_.front().size(), query_result(null_t(-1)));
    auto res = concat(v, nll);
    consume_(ctx, res);
  }
}

void left_outerjoin_on_node::process_right(query_ctx &ctx, const qr_tuple &v) {
  auto n = boost::get<node *>(v[left_right_nodes_.second]);
  auto nd = n->id();
  join_ids_.push_back(nd);
  input_.push_back(v);
}

void left_outerjoin_on_node::finish(query_ctx &ctx) { qop::default_finish(ctx); }

/* ------------------------------------------------------------------------ */

void left_outerjoin::dump(std::ostream &os) const {
  os << "left_outerjoin(";
  if (ex_) 
    os << ex_->dump();
  os << ") - " << PROF_DUMP;
}

void left_outerjoin::process_left(query_ctx &ctx, const qr_tuple &v) {
  PROF_PRE;
  bool dangling_tuple = true;
  uint64_t n = 0;
  for (auto &t : input_) {
    bool tp = false;
    if (ex_) {
      auto tv = concat(v, t);
      tp = interpret_expression(ctx, ex_, tv);
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
  if (dangling_tuple) {
    qr_tuple nll(input_.front().size(), query_result(null_t(-1)));
    auto res = concat(v, nll);
    consume_(ctx, res);
    n++;
  }
  PROF_POST(n);
}

void left_outerjoin::process_right(query_ctx &ctx, const qr_tuple &v) {
  PROF_PRE0;
  input_.push_back(v);
  PROF_POST(0);
}

void left_outerjoin::finish(query_ctx &ctx) { qop::default_finish(ctx); }

/* ------------------------------------------------------------------------ */

void left_outerjoin_on_rship::dump(std::ostream &os) const { // TODO
  os << "left_outerjoin() - " << PROF_DUMP;
}

void left_outerjoin_on_rship::process_left(query_ctx &ctx, const qr_tuple &v) {
  auto src = boost::get<node *>(v[src_des_nodes_.first]);
  bool dangling_tuple = true;

  for (auto &inp : input_) {
    bool found = false;
    auto des = boost::get<node *>(inp[src_des_nodes_.second]);
    ctx.gdb_->foreach_from_relationship_of_node((*src), [&](auto &r) {
      if (r.to_node_id() == des->id()){
        dangling_tuple = false;
        found = true;
      }
    });
    if (found) {
      auto res = concat(v, inp);
      consume_(ctx, res);
    }
  }
  if (dangling_tuple){
    qr_tuple nll(input_.front().size(), query_result(null_t(-1)));
    auto res = concat(v, nll);
    consume_(ctx, res);
  }
}

void left_outerjoin_on_rship::process_right(query_ctx &ctx, const qr_tuple &v) {
  input_.push_back(v);
}

void left_outerjoin_on_rship::finish(query_ctx &ctx) { qop::default_finish(ctx); }

/* ------------------------------------------------------------------------ */

void rship_join::dump(std::ostream &os) const { // TODO
  os << "rship_join() - " << PROF_DUMP;
}

void rship_join::process_left(query_ctx &ctx, const qr_tuple &v) {
  auto src = boost::get<node *>(v[src_des_nodes_.first]);

  for (auto &inp : input_) {
    auto des = boost::get<node *>(inp[src_des_nodes_.second]);
    ctx.gdb_->foreach_from_relationship_of_node((*src), [&](auto &r) {
      if (r.to_node_id() == des->id()){
        auto res = append(concat(v, inp), query_result(&r));
        consume_(ctx, res);
      }
    });
  }
}

void rship_join::process_right(query_ctx &ctx, const qr_tuple &v) {
  input_.push_back(v);
}

void rship_join::finish(query_ctx &ctx) { qop::default_finish(ctx); }

/* ------------------------------------------------------------------------ */