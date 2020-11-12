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

#define HASHER

void cross_join::dump(std::ostream &os) const { // TODO
  os << "cross_join()=>";
  if (subscriber_)
    subscriber_->dump(os);
}

void cross_join::process_left(graph_db_ptr &gdb, const qr_tuple &v) {
  for (auto &inp : input_) {
    auto res = concat(v, inp);
    consume_(gdb, res);
  }
}

void cross_join::process_right(graph_db_ptr &gdb, const qr_tuple &v) {
  input_.push_back(v);
}

void cross_join::finish(graph_db_ptr &gdb) { qop::default_finish(gdb); }

/* ------------------------------------------------------------------------ */

void left_outerjoin::dump(std::ostream &os) const { // TODO
  os << "left_outerjoin()=>";
  if (subscriber_)
    subscriber_->dump(os);
}

void left_outerjoin::process_left(graph_db_ptr &gdb, const qr_tuple &v) {
  auto src = boost::get<node *>(v[src_des_nodes_.first]);
  bool dangling_tuple = true;

  for (auto &inp : input_) {
    auto des = boost::get<node *>(inp[src_des_nodes_.second]);
    gdb->foreach_from_relationship_of_node((*src), [&](auto &r) {
      if (r.to_node_id() == des->id()){
        dangling_tuple = false;
        auto res = append(concat(v, inp), query_result(&r));
        consume_(gdb, res);
      }
    });
  }
  if (dangling_tuple){
    qr_tuple nll(input_.front().size(), query_result(null_t(-1)));
    auto res = append(concat(v, nll), query_result(null_t(-1)));
    consume_(gdb, res);
  }
}

void left_outerjoin::process_right(graph_db_ptr &gdb, const qr_tuple &v) {
  input_.push_back(v);
}

void left_outerjoin::finish(graph_db_ptr &gdb) { qop::default_finish(gdb); }

/* ------------------------------------------------------------------------ */

void nested_loop_join::dump(std::ostream &os) const { // TODO
  os << "nested_loop_join()=>";
  if (subscriber_)
    subscriber_->dump(os);
}

void nested_loop_join::process_left(graph_db_ptr &gdb, const qr_tuple &v) {
  auto n = boost::get<node *>(v[left_right_nodes_.first]);
  auto nid = n->id();

  auto i = 0;
  for (auto id : join_ids_) {
    if (id == nid){
      auto res = concat(v, input_[i]);
      consume_(gdb, res);
    }
    i++;
  }
}

void nested_loop_join::process_right(graph_db_ptr &gdb, const qr_tuple &v) {
  auto n = boost::get<node *>(v[left_right_nodes_.second]);
  auto nd = n->id();
  join_ids_.push_back(nd);
  input_.push_back(v);
}

void nested_loop_join::finish(graph_db_ptr &gdb) { qop::default_finish(gdb); }

/* ------------------------------------------------------------------------ */

void hash_join::dump(std::ostream &os) const { // TODO
  os << "hash_join()=>";
  if (subscriber_)
    subscriber_->dump(os);
}

void hash_join::probe_phase(graph_db_ptr &gdb, const qr_tuple &v) {
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
      consume_(gdb, res);
    }
    i++;
  }
}

void hash_join::build_phase(graph_db_ptr &gdb, const qr_tuple &v) {
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
void hash_join::finish(graph_db_ptr &gdb) { qop::default_finish(gdb); }