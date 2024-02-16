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

#ifndef query_batch_hpp_
#define query_batch_hpp_

#include "qop_visitor.hpp"
#include "query_pipeline.hpp"

/**
 * A query batch combines multiple queries producing a joint result. This is
 * necessary because we use a push-based approach where each scan is represented
 * by a separate query object.
 */
class query_batch {
public:
  query_batch() = default;

  void add(query_pipeline &q) { query_pipelines_.push_back(q); }
  std::size_t size() const { return query_pipelines_.size(); }
  query_pipeline &front() { return query_pipelines_.front(); }
  query_pipeline &at(std::size_t i) { return query_pipelines_[i]; }
  bool empty() const { return query_pipelines_.empty(); }

  void accept(qop_visitor &visitor);

  void append_printer();
  void append_collect(result_set &rs);

  /**
   * Start the execution of the query.
   */
  void start(query_ctx &ctx);

  /**
   * Print the query plan.
   */
  void print_plan(std::ostream &os = std::cout);

  void schedule_pipelines();

private:
  std::vector<query_pipeline> query_pipelines_;
  std::vector<std::size_t> pipeline_seq_;
};

#endif
