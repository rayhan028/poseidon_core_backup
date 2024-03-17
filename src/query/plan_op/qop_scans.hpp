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

#ifndef qop_scans_hpp_
#define qop_scans_hpp_

#include "qop.hpp"

/**
 * node_scan represents a query operator for scanning all nodes of a graph
 * (optionally with a given label). All these nodes are then forwarded to the
 * subscriber.
 */
struct node_scan : public enable_shared<qop, node_scan> /*public qop, public std::enable_shared_from_this<node_scan>*/ {
  node_scan(const std::string &l) : label(l), ranged(false) { type_ = qop_type::scan;  }
  node_scan(const std::string &l, std::map<std::size_t, std::vector<std::size_t>> &range_map) : label(l), 
        ranges(range_map), ranged(true) { type_ = qop_type::scan;  }
  node_scan(const std::vector<std::string> &l) : labels(l), ranged(false) { type_ = qop_type::scan;  }
  node_scan() = default;
  ~node_scan() = default;

  void dump(std::ostream &os) const override;

  virtual void start(query_ctx &ctx) override;

  void accept(qop_visitor& vis) override { 
    vis.visit(shared_from_this()); 
    if (has_subscriber())
      subscriber_->accept(vis);
  }

  std::string label;
  std::vector<std::string> labels;
  std::map<std::size_t, std::vector<std::size_t>> ranges;
  bool ranged;
};

/**
 * relationship_scan represents a query operator for scanning all relationships of a graph
 * (optionally with a given label). All these relationships are then forwarded to the
 * subscriber.
 */
struct relationship_scan : public enable_shared<qop, relationship_scan> /*public qop, public std::enable_shared_from_this<relationship_scan>*/ {
  relationship_scan(const std::string &l) : label(l), ranged(false) { type_ = qop_type::scan;  }
  relationship_scan(const std::string &l, std::map<std::size_t, std::vector<std::size_t>> &range_map) : label(l), 
        ranges(range_map), ranged(true) { type_ = qop_type::scan;  }
  relationship_scan(const std::vector<std::string> &l) : labels(l), ranged(false) { type_ = qop_type::scan;  }
  relationship_scan() = default;
  ~relationship_scan() = default;

  void dump(std::ostream &os) const override;

  virtual void start(query_ctx &ctx) override;

  void accept(qop_visitor& vis) override { 
    vis.visit(shared_from_this()); 
    if (has_subscriber())
      subscriber_->accept(vis);
  }

  std::string label;
  std::vector<std::string> labels;
  std::map<std::size_t, std::vector<std::size_t>> ranges;
  bool ranged;
};

/**
 * index_scan represents a query operator for scanning an index on nodes for
 * a given property key/value. All the matching nodes are then forwarded to the
 * subscriber.
 */
struct index_scan : public enable_shared<qop, index_scan> /*public qop, public std::enable_shared_from_this<index_scan>*/ {
  index_scan(index_id ix, uint64_t k) : idx(ix), key(k) { type_ = qop_type::scan;  }
  index_scan(std::list<index_id> &ixs, uint64_t k) : key(k), idxs(ixs) { type_ = qop_type::scan;  }
  ~index_scan() = default;

  void dump(std::ostream &os) const override;

  virtual void start(query_ctx &ctx) override;
  
  void accept(qop_visitor& vis) override { 
    vis.visit(shared_from_this()); 
    if (has_subscriber())
      subscriber_->accept(vis);
  }

  index_id idx;
  uint64_t key;
  std::list<index_id> idxs;
};

struct start_pipeline : public enable_shared<qop, start_pipeline> {
  start_pipeline() { type_ = qop_type::start;  }
  ~start_pipeline() = default;

  void dump(std::ostream &os) const override;

  void start(query_ctx &ctx, const qr_tuple &v) override;
  
  void accept(qop_visitor& vis) override { 
    vis.visit(shared_from_this()); 
    if (has_subscriber())
      subscriber_->accept(vis);
  }
};

struct node_by_id : public enable_shared<qop, node_by_id> {
  node_by_id(node::id_t id) : id_(id) { type_ = qop_type::node_by_id; }  
  ~node_by_id() = default;

  void dump(std::ostream &os) const override;

  void start(query_ctx &ctx) override;
  
  void accept(qop_visitor& vis) override { 
    vis.visit(shared_from_this()); 
    if (has_subscriber())
      subscriber_->accept(vis);
  }

  node::id_t id_;
};

#endif