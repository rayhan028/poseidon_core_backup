/*
 * Copyright (C) 2019-2021 DBIS Group - TU Ilmenau, All Rights Reserved.
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

#ifndef query_set_hpp_
#define query_set_hpp_

#include "query_builder.hpp"

namespace ph = std::placeholders;

/**
 * A query set combines multiple queries producing a joint result. This is
 * necessary because we use a push-based approach where each scan is represented
 * by a separate query object.
 */
class query_set {
public:
  query_set() = default;

  void add(query_builder &q) { queries_.push_back(q); }
  std::size_t size() const { return queries_.size(); }
  query_builder& front() { return queries_.front(); }
  query_builder &at(std::size_t i) { return queries_[i];  }
  bool empty() const { return queries_.empty(); }

  void accept(qop_visitor& visitor);
  
  void append_printer();
  void append_collect(result_set& rs);

  /**
   * Start the execution of the query.
   */
  void start();

 /**
   * Print the query plan.
   */
  void print_plan(std::ostream& os = std::cout);

private:
  std::vector<query_builder> queries_;
};

#endif