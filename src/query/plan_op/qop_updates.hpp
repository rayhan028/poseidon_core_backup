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

#ifndef qop_updates_hpp_
#define qop_updates_hpp_

#include "qop.hpp"

/**
 * create_node represents a query operator for creating a new node. The operator
 * can act as a root operator (via start) are also as subscriber.
 */
struct create_node : public enable_shared<qop, create_node> {
  create_node(const std::string &l) : label(l) {  type_ = qop_type::create; }
  create_node(const std::string &l, const properties_t &p) : label(l), props(p) {
    type_ = qop_type::create;
    expr_in_properties_= check_for_expr_in_properties();
  }
  ~create_node() = default;

  void dump(std::ostream &os) const override;

  virtual void start(query_ctx &ctx) override;
  void process(query_ctx &ctx, const qr_tuple &v);

  void accept(qop_visitor& vis) override { 
    vis.visit(shared_from_this()); 
    if (has_subscriber())
      subscriber_->accept(vis);
  }

  bool check_for_expr_in_properties();
  properties_t eval_properties(query_ctx &ctx, const qr_tuple &v);
  
  std::string label;
  properties_t props;
  bool expr_in_properties_;
};

/**
 * create_relationship represents a query operator for creating a new
 * relationship. The operator connects two nodes at any given positions 
 * in the query result.
 */
struct create_relationship : public enable_shared<qop, create_relationship> {
  create_relationship(const std::string &l) : label(l) { type_ = qop_type::create; }
  create_relationship(const std::string &l, const properties_t &p,
                      std::pair<int, int> src_des)
      : label(l), props(p), src_des_nodes_(src_des) { type_ = qop_type::create; }
  ~create_relationship() = default;

  void dump(std::ostream &os) const override;

  void process(query_ctx &ctx, const qr_tuple &v);

  void accept(qop_visitor& vis) override { 
    vis.visit(shared_from_this()); 
    if (has_subscriber())
      subscriber_->accept(vis);
  }

  std::string label;
  properties_t props;
  std::pair<int, int> src_des_nodes_;
};

/**
 * update_node represents a query operator for updating properties of a node
 * which is passed via consume parameters.
 */
struct update_node : public enable_shared<qop, update_node> {
  update_node(std::size_t var, properties_t &p) : var_no_(var), props(p) { type_ = qop_type::update; }
  update_node() : var_no_(0) {}
  ~update_node() = default;

  void dump(std::ostream &os) const override;

  void process(query_ctx &ctx, const qr_tuple &v);

  void accept(qop_visitor& vis) override { 
    vis.visit(shared_from_this()); 
    if (has_subscriber())
      subscriber_->accept(vis);
  }

  std::size_t var_no_;
  properties_t props;
};

/**
 * detach_node implements an operator for deleting the last node in a query tuple.
 * All relationship objects connected to the node are also deleted.
 * The optional p specifies a node to be deleted at other positions in the tuple.
 */
struct detach_node : public enable_shared<qop, detach_node> {
  // detach_node(const std::vector<std::size_t> &r, std::size_t p)  : pos_(p), /*rels_(r)*/ {} 
  detach_node(std::size_t p = std::numeric_limits<std::size_t>::max())  : pos_(p) {}
  ~detach_node() = default;

  void dump(std::ostream &os) const override;

  void process(query_ctx &ctx, const qr_tuple &v);

  void accept(qop_visitor& vis) override { 
    vis.visit(shared_from_this()); 
    if (has_subscriber())
      subscriber_->accept(vis);
  }

  std::size_t pos_;
  // std::vector<std::size_t> rels_;
};

/**
 * remove_node implements an operator for deleting the last node in a query tuple.
 * The optional p specifies a node to be deleted at other positions in the tuple.
 */
struct remove_node : public enable_shared<qop, remove_node> {
  remove_node(std::size_t p = std::numeric_limits<std::size_t>::max())  : pos_(p) {} 
  ~remove_node() = default;

  void dump(std::ostream &os) const override;

  void process(query_ctx &ctx, const qr_tuple &v);

  void accept(qop_visitor& vis) override { 
    vis.visit(shared_from_this()); 
    if (has_subscriber())
      subscriber_->accept(vis);
  }

  std::size_t pos_;
};

/**
 * remove_relationship implements an operator for deleting the last relationship in a query tuple.
 * The optional p specifies a relationship to be deleted at other positions in the tuple.
 */
struct remove_relationship : public enable_shared<qop, remove_relationship> {
  remove_relationship(std::size_t p = std::numeric_limits<std::size_t>::max())  : pos_(p) {} 
  ~remove_relationship() = default;

  void dump(std::ostream &os) const override;

  void process(query_ctx &ctx, const qr_tuple &v);

  void accept(qop_visitor& vis) override { 
    vis.visit(shared_from_this()); 
    if (has_subscriber())
      subscriber_->accept(vis);
  }
  
  std::size_t pos_;
};

#endif