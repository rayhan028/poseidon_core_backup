/*
 * Copyright (C) 2019-2024 DBIS Group - TU Ilmenau, All Rights Reserved.
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

#ifndef qop_set_hpp_
#define qop_set_hpp_

#include "qop.hpp"

/**
 * union_all_op implements an operator that unions all the
 * query tuples of the left query pipeline and the right query
 * pipeline(s).
 */
struct union_all_op : public qop, public std::enable_shared_from_this<union_all_op> {
  union_all_op() : init_(true), phases_(0) { type_ = qop_type::union_all; }
  ~union_all_op() = default;

  void dump(std::ostream &os) const override;

  void process_left(query_ctx &ctx, const qr_tuple &v);
  void process_right(query_ctx &ctx, const qr_tuple &v);

  void finish(query_ctx &ctx);

  void accept(qop_visitor& vis) override { 
    vis.visit(shared_from_this()); 
    if (has_subscriber())
      subscriber_->accept(vis);
  }

  bool is_binary() const override { return true; }

  bool init_;
  std::size_t phases_;
  std::list<qr_tuple> res_;
};

struct except_op : public qop, public std::enable_shared_from_this<except_op> {
  except_op(std::shared_ptr<variable> l, std::shared_ptr<variable> r) : phases_(0), lhs_var_(l), rhs_var_(r) { type_ = qop_type::except; } 
  ~except_op() = default;

  void dump(std::ostream &os) const override;

  void process_left(query_ctx &ctx, const qr_tuple &v);
  void process_right(query_ctx &ctx, const qr_tuple &v);

  void finish(query_ctx &ctx);

  void accept(qop_visitor& vis) override { 
    vis.visit(shared_from_this()); 
    if (has_subscriber())
      subscriber_->accept(vis);
  }

  bool is_binary() const override { return true; }

  std::size_t phases_;

    std::shared_ptr<variable> lhs_var_, rhs_var_;
    std::set<query_result> subtrahend_;
  mutable std::shared_mutex m_;  
};

#endif