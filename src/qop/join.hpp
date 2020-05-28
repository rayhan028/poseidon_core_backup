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

#ifndef join_hpp_
#define join_hpp_

#include "qop.hpp"

/**
 * cross_join implements a binary operator for constructing the cartesian
 * product of the results provided by the two input query operators.
 */
struct cross_join : public qop {
  cross_join() = default;
  ~cross_join() = default;

  void dump(std::ostream &os) const override;

  void process_left(graph_db_ptr &gdb, const qr_tuple &v);
  void process_right(graph_db_ptr &gdb, const qr_tuple &v);

  void finish(graph_db_ptr &gdb);

private:
  std::list<qr_tuple> input_;
};

  /**
   * left_outerjoin implements a left outerjoin operator for merging tuples
   * of two queries if there exists a relationship defined by an object 
   * (at a given position) in the left tuple as the source node and an 
   * object (at a given position) in the right tuple as the destination 
   * node. Dangling tuples are padded with "[0]{}" 
   */
struct left_outerjoin : public qop {
  left_outerjoin(std::pair<int, int> src_des) : src_des_nodes_(src_des) {} 
  ~left_outerjoin() = default;

  void dump(std::ostream &os) const override;

  void process_left(graph_db_ptr &gdb, const qr_tuple &v);
  void process_right(graph_db_ptr &gdb, const qr_tuple &v);

  void finish(graph_db_ptr &gdb);

private:
  std::list<qr_tuple> input_;
  std::pair<int, int> src_des_nodes_;
};

  /**
   * TODO
   */
struct nested_loop_join : public qop {
  nested_loop_join(std::pair<int, int> pos) : left_right_nodes_(pos) {} 
  ~nested_loop_join() = default;

  void dump(std::ostream &os) const override;

  void process_left(graph_db_ptr &gdb, const qr_tuple &v);
  void process_right(graph_db_ptr &gdb, const qr_tuple &v);

  void finish(graph_db_ptr &gdb);

private:
  std::vector<qr_tuple> input_;
  std::vector<node::id_t> join_ids_;
  std::pair<int, int> left_right_nodes_;
};

  /**
   * TODO
   */
struct hash_join : public qop {
  hash_join(std::pair<int, int> pos) : left_right_nodes_(pos) {} 
  ~hash_join() = default;

  void dump(std::ostream &os) const override;

  void build_phase(graph_db_ptr &gdb, const qr_tuple &v);
  void probe_phase(graph_db_ptr &gdb, const qr_tuple &v);

  void finish(graph_db_ptr &gdb);

private:
  std::pair<int, int> left_right_nodes_;

};

#endif