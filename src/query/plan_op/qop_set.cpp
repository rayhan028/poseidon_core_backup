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

#include "qop_set.hpp"
#include "qop_scans.hpp"

void union_all_op::dump(std::ostream &os) const {
  os << "union_all() - " << PROF_DUMP;
}

void union_all_op::process_left(query_ctx &ctx, const qr_tuple &v) {
  PROF_PRE;
  /*
  if (init_) {
    for (auto &r : res_)
      consume_(ctx, r);
    init_ = false;
  }
  */
  consume_(ctx, v);
  PROF_POST(1);
}

void union_all_op::process_right(query_ctx &ctx, const qr_tuple &v) {
  PROF_PRE;
  // res_.push_back(v);
  consume_(ctx, v);
  PROF_POST(1);
}

void union_all_op::finish(query_ctx &ctx) {
  if (++phases_ > 1)
    qop::default_finish(ctx);
}

/* ------------------------------------------------------------------------ */

void except_op::dump(std::ostream &os) const {
  os << "except([" << lhs_var_->dump() << "," << rhs_var_->dump() << "]) - "
     << PROF_DUMP;
}

void except_op::process_left(query_ctx &ctx, const qr_tuple &v) {
  bool have_result = false;
  PROF_PRE;
  auto minuend = get_var_value(ctx, v, lhs_var_);
  if (subtrahend_.find(minuend) == subtrahend_.end()) {
    consume_(ctx, v);
    have_result = true;
  }
  PROF_POST(have_result ? 1 : 0);
}

void except_op::process_right(query_ctx &ctx, const qr_tuple &v) {
  PROF_PRE;
  std::unique_lock lck(m_);
  subtrahend_.insert(get_var_value(ctx, v, rhs_var_));
  PROF_POST(0);
}

void except_op::finish(query_ctx &ctx) {
  if (++phases_ > 1)
    qop::default_finish(ctx);
}

/* ------------------------------------------------------------------------ */

exists_op::exists_op(qop_ptr sub, bool not_flag) : is_not_(not_flag), counter_(0) {
  namespace ph = std::placeholders;

  subquery_ = std::dynamic_pointer_cast<start_pipeline>(sub);
  // connect subquery_ with counter_
  tail_ = subquery_;
  while (tail_->has_subscriber()) {
    tail_ = tail_->subscriber();
  }
  tail_->connect_consume(std::bind(&exists_op::sub_query_processed, this,
                                 ph::_1, ph::_2));
  /*
  std::cout << "exists_op: tail: "; tail_->dump(std::cout); std::cout << std::endl;
  std::cout << "exists_op: subquery: "; subquery_->dump(std::cout); std::cout << std::endl;
  */
}

void exists_op::dump(std::ostream& os) const {
  if (is_not_)
    os << "not_exists([";
  else
    os << "exists([";

  os << "]) - " << PROF_DUMP;
}

void exists_op::process(query_ctx &ctx, const qr_tuple &v) {
  counter_ = 0;
  // send tuple to subquery  
  subquery_->start(ctx, v);

  if (is_not_ && counter_ == 0)
    consume_(ctx, v);
  else if (!is_not_ && counter_ > 0)
    consume_(ctx, v);
}

void exists_op::sub_query_processed(query_ctx &ctx, const qr_tuple &v) {
  counter_++;
}
