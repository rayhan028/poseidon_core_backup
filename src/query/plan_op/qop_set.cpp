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
