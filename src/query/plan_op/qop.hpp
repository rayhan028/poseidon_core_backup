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

#ifndef qop_hpp_
#define qop_hpp_

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <atomic>
#include <condition_variable>
#include <iterator>
#include <memory>
#include <set>
#include <unordered_map>
#include <vector>

#include "binary_expression.hpp"
#include "defs.hpp"
#include "expression.hpp"
#include "graph_db.hpp"
#include "nodes.hpp"
#include "profiling.hpp"
#include "properties.hpp"
#include "qop_visitor.hpp"
#include "qresult_iterator.hpp"
#include "relationships.hpp"
// #include "query_ctx.hpp"

template <typename T> std::vector<T> append(const std::vector<T> &v, T t) {
  std::vector<T> v2;
  v2.reserve(v.size() + 1);
  v2.insert(v2.end(), v.begin(), v.end());
  v2.push_back(t);
  return v2;
}

template <typename T>
std::vector<T> concat(const std::vector<T> &v1, const std::vector<T> &v2) {
  std::vector<T> v;
  v.reserve(v1.size() + v2.size());
  std::copy(v1.begin(), v1.end(), std::back_inserter(v));
  std::copy(v2.begin(), v2.end(), std::back_inserter(v));
  return v;
}

enum class qop_type {
  none,            // 0
  any,             // 1
  scan,            // 2
  project,         // 3
  printer,         // 4
  is_property,     // 5
  filter,          // 6
  foreach_rship,   // 7
  expand,          // 8
  node_has_label,  // 9
  union_all,
  except,
  cross_join,
  left_join,
  hash_join,
  nest_loop_join,
  sort,
  limit,
  distinct,
  collect,
  aggregate,
  order_by,
  group_by,
  create,
  store,
  start,
  end
};

/**
 * Directions of relationships.
 */
enum class RSHIP_DIR { FROM = 0, TO = 1, ALL = 2 };

/**
 * Directions of the expand operator.
 */
enum class EXPAND { IN, OUT };

/**
 * Functions to get a property value (defined by var=pos and property name) from
 * a qr_tuple.
 */
p_item get_property_value(query_ctx &ctx, const qr_tuple &v, std::size_t var,
                          const std::string &prop);
p_item get_property_value(query_ctx &ctx, const qr_tuple &v, std::size_t var,
                          dcode_t pkey);

template <typename T>
T get_property_value(query_ctx &ctx, const qr_tuple &v, std::size_t var,
                     const std::string &prop);
template <typename T>
T get_property_value(query_ctx &ctx, const qr_tuple &v, std::size_t var,
                     dcode_t pkey);

query_result get_var_value(query_ctx &ctx, const qr_tuple &v,
                           std::shared_ptr<variable> var);

struct qop;
using qop_ptr = std::shared_ptr<qop>;

template <class Base, class Derived>
struct enable_shared : public Base {   
    std::shared_ptr<Derived> shared_from_this() {   
          return std::static_pointer_cast<Derived>(
                                          Base::shared_from_this());
    }
};

/**
 * qop represents the abstract base class for all query operators. It implements
 * a push-style query engine where the execution is initiated by calling the
 * start method and results of an operator are forwarded to the subsequent
 * operator by invoking its consume method. This consume method as well as the
 * finish method are invoked via a signal/slot mechanism implemented via the
 * connect method.
 */
struct qop : public std::enable_shared_from_this<qop> {
  friend class query_planner;

  /**
   * function pointer for a function called as subscriber consume function, i.e.
   * a function which  processes the list of graph elements (nodes,
   * relationships) given in vector v and forwards the results to the next
   * operator. These elements are contained in the graph gdb.
   */
  using consume_func = std::function<void(query_ctx &, const qr_tuple &)>;

  /**
   * function pointer for a function called as subscriber finish function, i.e.
   * a function which is called when all results have been processed.
   */
  using finish_func = std::function<void(query_ctx &)>;

  /**
   * Constructor.
   */
  qop()
      : priority_(0), subscriber_(nullptr), parent_(nullptr), consume_(nullptr),
        finish_(nullptr) {}

  /**
   * Destructor.
   */
  virtual ~qop() = default;

  /**
   * Starts the processing of the whole query. This method is implemented only
   * in producer operators such as node_scan.
   */
  virtual void start(query_ctx &ctx) {}
  virtual void start(query_ctx &ctx, const qr_tuple &v) {}

  /**
   * Prints a description of the operator and recursively calls
   * dump of the subscribers.
   */
  virtual void dump(std::ostream &os) const {}

  void connect(qop_ptr op) { 
    op->parent_ = this; // shared_from_this();
    subscriber_ = op; 
  }

  /**
   * Registers the subscriber by initializing the subscriber_ pointer and
   * registering its consume function. The finish function is set to the default
   * function.
   */
  void connect(qop_ptr op, consume_func cf) {
    op->parent_ = this; // shared_from_this();
    subscriber_ = op;
    consume_ = cf;
    finish_ = std::bind(&qop::default_finish, subscriber_.get(),
                        std::placeholders::_1);
  }

  /**
   * Registers the subscriber by initializing the subscriber_ pointer and
   * registering its consume and finish functions.
   */
  void connect(qop_ptr op, consume_func cf, finish_func ff) {
    op->parent_ = this; // shared_from_this();
    subscriber_ = op;
    consume_ = cf;
    finish_ = ff;
  }

  void connect_consume(consume_func cf) {
    consume_ = cf;
  }

  /**
   * This method is called after all results have been processed by the producer
   * operator. The default behavior is to propagate this event to all
   * subscribers, but operators such as ORDER_BY can use it to implement their
   * own behavior.
   */
  void default_finish(query_ctx &ctx) {
    if (finish_)
      finish_(ctx);
  }

  /**
   * Returns true if the operator has already a subscriber.
   */
  inline bool has_subscriber() const { return subscriber_.get() != nullptr; }

  /**
   * Return the current subscriber.
   */
  inline qop_ptr subscriber() { return subscriber_; }

  /**
   * Return true if this operator is a binary operator (join, union etc.)
   */
  virtual bool is_binary() const { return false; }

  inline uint64_t hasher(const query_result &q) {
    return boost::hash<query_result>()(q);
  }

  inline void consume(query_ctx &ctx, const qr_tuple &tup) { consume_(ctx, tup); }
  
  PROF_ACCESSOR;

  /**
   * Accept method for generic visitor
   */
  virtual void accept(qop_visitor &vis) = 0;

  qop_type type_;

  std::size_t priority_;

protected:
  qop_ptr subscriber_;   // pointer to the subsequent operator which receives and
                         // processes the results
  qop* parent_;
  consume_func consume_; // pointer to the subscriber's consume function
  finish_func finish_;   // pointer to the subscriber's finish function

  PROF_DATA;
};

/**
 * is_property is a query operator for filtering nodes/relationships based on
 * their properties. For this purpose, the name of the property and a predicate
 * function for checking the value of this property have to be given.
 */
struct is_property : public enable_shared<qop, is_property> {
  is_property(const std::string &p, std::function<bool(const p_item &)> pred)
      : property(p), pcode(0), predicate(pred) {
    type_ = qop_type::is_property;
  }
  ~is_property() = default;

  void dump(std::ostream &os) const override;

  void process(query_ctx &ctx, const qr_tuple &v);

  void accept(qop_visitor &vis) override {
    vis.visit(shared_from_this());
    if (has_subscriber())
      subscriber_->accept(vis);
  }

  std::string property;
  dcode_t pcode;
  std::function<bool(const p_item &)> predicate;
};

/**
 * node_has_label is a query operator representing a filter for nodes which have
 * the given label.
 */
struct node_has_label : public enable_shared<qop, node_has_label> {
  node_has_label(const std::vector<std::string> &l) : labels(l), lcode(0) {
    type_ = qop_type::node_has_label;
  }
  node_has_label(const std::string &l) : label(l), lcode(0) {
    type_ = qop_type::node_has_label;
  }
  ~node_has_label() = default;

  void dump(std::ostream &os) const override;

  void process(query_ctx &ctx, const qr_tuple &v);

  void accept(qop_visitor &vis) override {
    vis.visit(shared_from_this());
    if (has_subscriber())
      subscriber_->accept(vis);
  }

  std::vector<std::string> labels;
  std::string label;
  dcode_t lcode;
};

struct expand : public enable_shared<qop, expand> {
  expand(EXPAND dir) : dir_(dir) { type_ = qop_type::expand; }

  void accept(qop_visitor &vis) override {
    vis.visit(shared_from_this());
    if (has_subscriber())
      subscriber_->accept(vis);
  }

  EXPAND dir_;
  std::string label;
};

/**
 * get_from_node is a query operator for retrieving the node at the FROM side of
 * a relationship which is given in the last element of vector v.
 */
struct get_from_node : public expand {
  get_from_node() : expand(EXPAND::IN) {}

  void dump(std::ostream &os) const override;

  void process(query_ctx &ctx, const qr_tuple &v);

  void accept(qop_visitor &vis) override {
    vis.visit(shared_from_this());
    if (has_subscriber())
      subscriber_->accept(vis);
  }
};

/**
 * get_to_node is a query operator for retrieving the node at the TO side of
 * a relationship which is given in the last element of vector v.
 */
struct get_to_node : public expand {
  get_to_node() : expand(EXPAND::OUT) {}

  void dump(std::ostream &os) const override;

  void process(query_ctx &ctx, const qr_tuple &v);

  void accept(qop_visitor &vis) override {
    vis.visit(shared_from_this());
    if (has_subscriber())
      subscriber_->accept(vis);
  }
};

/**
 * printer is a query operator to output the query results collected in the
 * vector v to standard output.
 */
struct printer : public enable_shared<qop, printer> {
  printer() : ntuples_(0), output_width_(0) { type_ = qop_type::printer; }

  void dump(std::ostream &os) const override;

  void process(query_ctx &ctx, const qr_tuple &v);

  void finish(query_ctx &ctx);

  void accept(qop_visitor &vis) override {
    vis.visit(shared_from_this());
    if (has_subscriber())
      subscriber_->accept(vis);
  }

  std::size_t ntuples_;
  std::size_t output_width_;
  mutable std::mutex m_;
};

/**
 * limit_result is a query operator for producing only the given number of
 * results.
 */
struct limit_result : public enable_shared<qop, limit_result>  {
  limit_result(std::size_t n) : num_(n), processed_(0) {
    type_ = qop_type::limit;
  }
  ~limit_result() = default;

  void dump(std::ostream &os) const override;

  void process(query_ctx &ctx, const qr_tuple &v);

  void accept(qop_visitor &vis) override {
    vis.visit(shared_from_this());
    if (has_subscriber())
      subscriber_->accept(vis);
  }

  std::size_t num_;
  std::atomic_ulong processed_;
};

extern result_set::sort_spec_list sort_spec_;
/**
 * order_by implements an operator for sorting results either by giving a
 * comparison function or a specificaton of sorting criteria.
 */
struct order_by : public enable_shared<qop, order_by> {
  order_by(const result_set::sort_spec_list &spec) {
    type_ = qop_type::order_by;
    sort_spec_ = spec;
  }

  order_by(std::function<bool(const qr_tuple &, const qr_tuple &)> func)
  //: cmp_func_(func)
  {
    cmp_func_ = func;
    type_ = qop_type::order_by;
  }
  ~order_by() = default;

  void dump(std::ostream &os) const override;

  void process(query_ctx &ctx, const qr_tuple &v);

  void finish(query_ctx &ctx);

  void accept(qop_visitor &vis) override {
    vis.visit(shared_from_this());
    if (has_subscriber())
      subscriber_->accept(vis);
  }

  static void sort(result_set *rs) {
    query_ctx ctx; // TODO!!!!
    if (cmp_func_ != nullptr)
      rs->sort(ctx, cmp_func_);
    else {
      rs->sort(ctx, sort_spec_);
    }
  }

  result_set results_;
  static std::function<bool(const qr_tuple &, const qr_tuple &)> cmp_func_;
};

/**
 * distinct_tuples implements an operator for outputing distinct
 * result tuples.
 */
struct distinct_tuples : public enable_shared<qop, distinct_tuples> {
  distinct_tuples() { type_ = qop_type::distinct; }
  ~distinct_tuples() = default;

  void dump(std::ostream &os) const override;

  void process(query_ctx &ctx, const qr_tuple &v);

  void accept(qop_visitor &vis) override {
    vis.visit(shared_from_this());
    if (has_subscriber())
      subscriber_->accept(vis);
  }

  std::mutex m_;
  std::set<std::string> keys_;
};

/**
 * filter_op implements an operator that filters a tuple
 * based on a predicate function.
 */
struct filter_op : public enable_shared<qop, filter_op> {
  filter_op(std::function<bool(const qr_tuple &)> func)
      : pred_func1_(func), pred_func_(nullptr) {
    type_ = qop_type::filter;
  }
  filter_op(const expr &ex) : ex_(ex), pred_func_(nullptr) {
    type_ = qop_type::filter;
  }

  ~filter_op() = default;

  void dump(std::ostream &os) const override;

  void process(query_ctx &ctx, const qr_tuple &v);

  void finish(query_ctx &ctx);

  expr get_expression() { return ex_; }

  void accept(qop_visitor &vis) override {
    vis.visit(shared_from_this());
    if (has_subscriber())
      subscriber_->accept(vis);
  }

  std::function<bool(const qr_tuple &)> pred_func1_;
  std::function<bool(const qr_tuple &, expr &)> pred_func2_;
  expr ex_;

  using predicate_fptr = bool (*)(const query_ctx *, const qr_tuple *);
  predicate_fptr pred_func_;
};

/**
 * Operator for printing the content of a result set.
 */
std::ostream &operator<<(std::ostream &os, const result_set &rs);

/**
 * collect_result is a query operator for collecting results of a query: either
 * to check the results or to further process the data. Note, that all result
 * values are represented as strings.
 */
struct collect_result : public enable_shared<qop, collect_result> {
  collect_result(result_set &res, bool as_string = false) : results_(res), as_string_(as_string) { type_ = qop_type::collect; }
  ~collect_result() = default;

  void dump(std::ostream &os) const override;

  void process(query_ctx &ctx, const qr_tuple &v);

  void finish(query_ctx &ctx);

  void accept(qop_visitor &vis) override {
    vis.visit(shared_from_this());
    if (has_subscriber())
      subscriber_->accept(vis);
  }

  result_set &results_;

private:
  bool as_string_;
  std::mutex collect_mtx;
};

#endif
