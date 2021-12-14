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

#ifndef update_hpp_
#define update_hpp_

#include "qop.hpp"

/**
 * create_node represents a query operator for creating a new node. The operator
 * can act as a root operator (via start) are also as subscriber.
 */
struct create_node : public qop {
  create_node(const std::string &l) : label(l) {}
  create_node(const std::string &l, const properties_t &p) : label(l), props(p) {}
  ~create_node() = default;

  void dump(std::ostream &os) const override;

  virtual void start(graph_db_ptr &gdb) override;
  void process(graph_db_ptr &gdb, const qr_tuple &v);

  std::string label;
  properties_t props;
};

/**
 * create_relationship represents a query operator for creating a new
 * relationship. The operator connects two nodes at any given positions 
 * in the query result.
 */
struct create_relationship : public qop {
  create_relationship(const std::string &l) : label(l) {}
  create_relationship(const std::string &l, const properties_t &p,
                      std::pair<int, int> src_des)
      : label(l), props(p), src_des_nodes_(src_des) {}
  ~create_relationship() = default;

  void dump(std::ostream &os) const override;

  void process(graph_db_ptr &gdb, const qr_tuple &v);

  std::string label;
  properties_t props;
  std::pair<int, int> src_des_nodes_;
};

/**
 * create_rship_on_join implements a binary operator for creating a new
 * relationship. The operator connects two nodes from two different queries:
 * One of the nodes (the source node by default) is at a given position in 
 * the left query and the other (the destination node by default) is at the 
 * last position in the right query. The default relationship direction can be
 * changed via a flag.
 */
struct create_rship_on_join : public qop {
  create_rship_on_join(const std::string &l) : label(l) {}
  create_rship_on_join(const std::string &l, const properties_t &p,
                      int pos, bool dir_flag)
      : label(l), props(p), l_node_pos(pos), src_to_des(dir_flag) {}
  ~create_rship_on_join() = default;

  void dump(std::ostream &os) const override;

  void process_left(graph_db_ptr &gdb, const qr_tuple &v);
  void process_right(graph_db_ptr &gdb, const qr_tuple &v);

  void finish(graph_db_ptr &gdb);

  std::string label;
  properties_t props;
  int l_node_pos;
  bool src_to_des;
  node *r_node_; 
};

/**
 * update_node represents a query operator for updating properties of a node
 * which is passed via consume parameters.
 */
struct update_node : public qop {
  update_node(std::size_t var, properties_t &p) : var_no_(var), props(p) {}
  update_node() : var_no_(0) {}
  ~update_node() = default;

  void dump(std::ostream &os) const override;

  void process(graph_db_ptr &gdb, const qr_tuple &v);

  std::size_t var_no_;
  properties_t props;
};

/**
 * detach_node implements an operator for deleting the last node in a query tuple.
 * All relationship objects connected to the node are also deleted.
 * The optional p specifies a node to be deleted at other positions in the tuple.
 */
struct detach_node : public qop {
  // detach_node(const std::vector<std::size_t> &r, std::size_t p)  : pos_(p), /*rels_(r)*/ {} 
  detach_node(std::size_t p)  : pos_(p) {}
  ~detach_node() = default;

  void dump(std::ostream &os) const override;

  void process(graph_db_ptr &gdb, const qr_tuple &v);

  std::size_t pos_;
  // std::vector<std::size_t> rels_;
};

/**
 * remove_node implements an operator for deleting the last node in a query tuple.
 * The optional p specifies a node to be deleted at other positions in the tuple.
 */
struct remove_node : public qop {
  remove_node(std::size_t p)  : pos_(p) {} 
  ~remove_node() = default;

  void dump(std::ostream &os) const override;

  void process(graph_db_ptr &gdb, const qr_tuple &v);

  std::size_t pos_;
};

/**
 * remove_rship implements an operator for deleting the last relationship in a query tuple.
 * The optional p specifies a relationship to be deleted at other positions in the tuple.
 */
struct remove_rship : public qop {
  remove_rship(std::size_t p)  : pos_(p) {} 
  ~remove_rship() = default;

  void dump(std::ostream &os) const override;

  void process(graph_db_ptr &gdb, const qr_tuple &v);

  std::size_t pos_;
};

#endif