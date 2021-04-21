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

#ifndef qop_hpp_
#define qop_hpp_

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <memory>
#include <vector>
#include <set>
#include <iterator>
#include <condition_variable>
#include <unordered_map>

#include "defs.hpp"
#include "graph_db.hpp"
#include "nodes.hpp"
#include "relationships.hpp"
#include "shortest_path.hpp"
#include "profiling.hpp"

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

struct qop;
using qop_ptr = std::shared_ptr<qop>;

/**
 * qop represents the abstract base class for all query operators. It implements
 * a push-style query engine where the execution is initiated by calling the
 * start method and results of an operator are forwarded to the subsequent
 * operator by invoking its consume method. This consume method as well as the
 * finish method are invoked via a signal/slot mechanism implemented via the
 * connect method.
 */
struct qop {
  /**
   * function pointer for a function called as subscriber consume function, i.e.
   * a function which  processes the list of graph elements (nodes,
   * relationships) given in vector v and forwards the results to the next
   * operator. These elements are contained in the graph gdb.
   */
  using consume_func = std::function<void(graph_db_ptr &, const qr_tuple &)>;

  /**
   * function pointer for a function called as subscriber finish function, i.e.
   * a function which is called when all results have been processed.
   */
  using finish_func = std::function<void(graph_db_ptr &)>;

  /**
   * Constructor.
   */
  qop() : subscriber_(nullptr), consume_(nullptr), finish_(nullptr) {}

  /**
   * Destructor.
   */
  virtual ~qop() = default;

  /**
   * Starts the processing of the whole query. This method is implemented only
   * in producer operators such as scan_nodes.
   */
  virtual void start(graph_db_ptr &gdb) {}

  /**
   * Prints a description of the operator and recursively calls
   * dump of the subscribers.
   */
  virtual void dump(std::ostream &os) const {}

  /**
   * Registers the subscriber by initializing the subscriber_ pointer and
   * registering its consume function. The finish function is set to the default
   * function.
   */
  void connect(qop_ptr op, consume_func cf) {
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
    subscriber_ = op;
    consume_ = cf;
    finish_ = ff;
  }

  /**
   * This method is called after all results have been processed by the producer
   * operator. The default behavior is to propagate this event to all
   * subscribers, but operators such as ORDER_BY can use it to implement their
   * own behavior.
   */
  void default_finish(graph_db_ptr &gdb) {
    if (finish_)
      finish_(gdb);
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

  PROF_ACCESSOR;

protected:
  qop_ptr subscriber_; // pointer to the subsequent operator which receives and
                       // processes the results

  consume_func consume_; // pointer to the subscriber's consume function
  finish_func finish_;   // pointer to the subscriber's finish function
  PROF_DATA;
};

/**
 * scan_nodes represents a query operator for scanning all nodes of a graph
 * (optionally with a given label). All these nodes are then forwarded to the
 * subscriber.
 */
struct scan_nodes : public qop {
  scan_nodes(const std::string &l) : label(l) {}
  scan_nodes(const std::vector<std::string> &l) : labels(l) {}
  scan_nodes() = default;
  ~scan_nodes() = default;

  void dump(std::ostream &os) const override;

  virtual void start(graph_db_ptr &gdb) override;

  std::string label;
  std::vector<std::string> labels;
};

/**
 * index_scan represents a query operator for scanning an index on nodes for
 * a given property key/value. All the matching nodes are then forwarded to the
 * subscriber.
 */
struct index_scan : public qop {
  index_scan(index_id ix, uint64_t k) : idx(ix), key(k) {}
  index_scan(std::list<index_id> &ixs, uint64_t k) : key(k), idxs(ixs) {}
  ~index_scan() = default;

  void dump(std::ostream &os) const override;

  virtual void start(graph_db_ptr &gdb) override;

  index_id idx;
  uint64_t key;
  std::list<index_id> idxs;
};

/**
 * foreach_from_relationship is a query operator that scans all outgoing
 * relationships of a given node. This node is the last elment in the vector v
 * of the consume method.
 */
struct foreach_from_relationship : public qop {
  foreach_from_relationship(const std::string &l, int pos = std::numeric_limits<int>::max()) : label(l), lcode(0), npos(pos) {}
  ~foreach_from_relationship() = default;

  void dump(std::ostream &os) const override;

  void process(graph_db_ptr &gdb, const qr_tuple &v);

  std::string label;
  dcode_t lcode;
  int npos;
};

/**
 * foreach_variable_from_relationship is a query operator that scans all
 * outgoing relationships of a given node recursively within the given range.
 * This node is the last elment in the vector v of the consume method.
 */
struct foreach_variable_from_relationship : public qop {
  foreach_variable_from_relationship(const std::string &l, std::size_t min,
                                     std::size_t max, int pos = std::numeric_limits<int>::max())
      : label(l), lcode(0), min_range(min), max_range(max), npos(pos) {}
  ~foreach_variable_from_relationship() = default;

  void dump(std::ostream &os) const override;

  void process(graph_db_ptr &gdb, const qr_tuple &v);

  std::string label;
  dcode_t lcode;
  std::size_t min_range, max_range;
  int npos;
};

/**
 * foreach_bi_dir_relationship is a query operator that scans all outgoing
 * and incoming relationships of a given node. This node is the last elment 
 * in the vector v of the consume method or given by the index position pos.
 */
struct foreach_all_relationship : public qop {
  foreach_all_relationship(const std::string &l, int pos) : label(l), lcode(0), npos(pos) {}
  ~foreach_all_relationship() = default;

  void dump(std::ostream &os) const override;

  void process(graph_db_ptr &gdb, const qr_tuple &v);

  std::string label;
  dcode_t lcode;
  int npos;
};

/**
 * foreach_variable_bi_dir_relationship is a query operator that scans all
 * outgoing and incoming relationships of a given node recursively within the given range.
 * This node is the last elment in the vector v of the consume method or given by the
 * position index pos.
 */
struct foreach_variable_all_relationship : public qop {
  foreach_variable_all_relationship(const std::string &l, std::size_t min,
                                     std::size_t max, int pos)
      : label(l), lcode(0), min_range(min), max_range(max), npos(pos) {}
  ~foreach_variable_all_relationship() = default;

  void dump(std::ostream &os) const override;

  void process(graph_db_ptr &gdb, const qr_tuple &v);

  std::string label;
  dcode_t lcode;
  std::size_t min_range, max_range;
  int npos;
};

/**
 * foreach_to_relationship is a query operator that scans all incoming
 * relationships of a given node. This node is the last elment in the vector v
 * of the consume method.
 */
struct foreach_to_relationship : public qop {
  foreach_to_relationship(const std::string &l, int pos = std::numeric_limits<int>::max()) : label(l), lcode(0), npos(pos) {}
  ~foreach_to_relationship() = default;

  void dump(std::ostream &os) const override;

  void process(graph_db_ptr &gdb, const qr_tuple &v);

  std::string label;
  dcode_t lcode;
  int npos;
};

/**
 * foreach_variable_to_relationship is a query operator that scans all incoming
 * relationships of a given node recursively within the given range.
 * This node is the last elment in the vector v of the consume method.
 */
struct foreach_variable_to_relationship : public qop {
  foreach_variable_to_relationship(const std::string &l, std::size_t min,
                                   std::size_t max, int pos = std::numeric_limits<int>::max())
      : label(l), lcode(0), min_range(min), max_range(max), npos(pos) {}
  ~foreach_variable_to_relationship() = default;

  void dump(std::ostream &os) const override;

  void process(graph_db_ptr &gdb, const qr_tuple &v);

  std::string label;
  dcode_t lcode;
  std::size_t min_range, max_range;
  int npos;
};

/**
 * is_property is a query operator for filtering nodes/relationships based on
 * their properties. For this purpose, the name of the property and a predicate
 * function for checking the value of this property have to be given.
 */
struct is_property : public qop {
  is_property(const std::string &p, std::function<bool(const p_item &)> pred)
      : property(p), pcode(0), predicate(pred) {}
  ~is_property() = default;

  void dump(std::ostream &os) const override;

  void process(graph_db_ptr &gdb, const qr_tuple &v);

  std::string property;
  dcode_t pcode;
  std::function<bool(const p_item &)> predicate;
};

/**
 * node_has_label is a query operator representing a filter for nodes which have
 * the given label.
 */
struct node_has_label : public qop {
  node_has_label(const std::vector<std::string> &l) : labels(l), lcode(0) {}
  node_has_label(const std::string &l) : label(l), lcode(0) {}
  ~node_has_label() = default;

  void dump(std::ostream &os) const override;

  void process(graph_db_ptr &gdb, const qr_tuple &v);

  std::vector<std::string> labels;
  std::string label;
  dcode_t lcode;
};

/**
 * get_from_node is a query operator for retrieving the node at the FROM side of
 * a relationship which is given in the last element of vector v.
 */
struct get_from_node : public qop {
  get_from_node() = default;

  void dump(std::ostream &os) const override;

  void process(graph_db_ptr &gdb, const qr_tuple &v);
};

/**
 * get_to_node is a query operator for retrieving the node at the TO side of
 * a relationship which is given in the last element of vector v.
 */
struct get_to_node : public qop {
  get_to_node() = default;

  void dump(std::ostream &os) const override;

  void process(graph_db_ptr &gdb, const qr_tuple &v);
};

/**
 * printer is a query operator to output the query results collected in the
 * vector v to standard output.
 */
struct printer : public qop {
  printer() = default;

  void dump(std::ostream &os) const override;

  void process(graph_db_ptr &gdb, const qr_tuple &v);
};

/**
 * limit_result is a query operator for producing only the given number of
 * results.
 */
struct limit_result : public qop {
  limit_result(std::size_t n) : num_(n), processed_(0) {}
  ~limit_result() = default;

  void dump(std::ostream &os) const override;

  void process(graph_db_ptr &gdb, const qr_tuple &v);

  std::size_t num_, processed_;
};

/**
 * nodes_connected appends the relationship object between a source and a
 * destination node, whose positions in the query tuple are given by the
 * src_des pair.
 * When no relationship exist between them, the boolean b sets whether a
 * null_t is appended instead (true) or not (false)
 */
struct nodes_connected : public qop {
  nodes_connected(std::pair<int, int> src_des, bool b)  : append_null_(b), src_des_nodes_(src_des) {} 
  ~nodes_connected() = default;

  void dump(std::ostream &os) const override;

  void process(graph_db_ptr &gdb, const qr_tuple &v);

  bool append_null_;
  std::pair<int, int> src_des_nodes_;
};

/**
 * result_set is used to collect query results.
 */
struct result_set {
  using sort_spec = const std::vector<std::pair<std::size_t, bool>>;

  /**
   * Constructors.
   */
  result_set() = default;
  result_set(const result_set &rs) : data(rs.data) {}

  /**
   * Block the current thread until the result data is complete.
   */
  void wait();

  /**
   * Notify the waiting thread that the result is complete.
   */
  void notify();

  /**
   * Append the given element to the result set.
   */
  inline void append(const qr_tuple &elem) { data.push_back(elem); }

  /**
   * Sort the result by the given sort specification.
   */
  void sort(const sort_spec &spec);

  void sort(std::function<bool(const qr_tuple &, const qr_tuple &)> cmp);

  /**
   * Comparison operator.
   */
  bool operator==(const result_set &other) const;

  std::list<qr_tuple> data; // the result data

private:
  bool qr_compare(const qr_tuple &qr1, const qr_tuple &qr2,
                  const sort_spec &spec);

  // mutex and condition variable used to notify a waiting thread when the
  // result set is complete
  std::mutex m;
  std::condition_variable cond_var;
  std::atomic<bool> ready{false};
};

/**
 * order_by implements an operator for sorting results either by giving a
 * comparison function or a specificaton of sorting criteria.
 */
struct order_by : public qop {
  order_by(const result_set::sort_spec &spec) : sort_spec_(spec) {}
  order_by(std::function<bool(const qr_tuple &, const qr_tuple &)> func)
      : cmp_func_(func) {}
  ~order_by() = default;

  void dump(std::ostream &os) const override;

  void process(graph_db_ptr &gdb, const qr_tuple &v);

  void finish(graph_db_ptr &gdb);

  result_set results_;
  result_set::sort_spec sort_spec_;
  std::function<bool(const qr_tuple &, const qr_tuple &)> cmp_func_;
};

/**
 * group_by implements an operator for grouping tuples and optional aggregations
 * like count, sum, average, and percentage count.
 * The grouping keys are query result(s) given by their positions in the query tuple.
 * The aggregate type(s) and aggregate attribute(s) are given
 * as a string-integer pair. The aggregate types above are specified as "count",
 * "sum", "avg" and "pcount" respectively. The integer denotes the position of the
 * aggregate attribute in the tuples of grps. 
 */
struct group_by : public qop {
  group_by(const std::vector<std::size_t> &pos);
  group_by(const std::vector<std::size_t> &pos,
    const std::vector<std::pair<std::string, std::size_t>> &aggrs);
  ~group_by() = default;

  void dump(std::ostream &os) const override;

  void process(graph_db_ptr &gdb, const qr_tuple &v);

  void finish(graph_db_ptr &gdb);

  std::mutex m_;
  std::size_t grpkey_cnt_;
  std::vector<std::string> grpkey_set_;
  std::vector<std::size_t> grpkey_pos_;
  std::unordered_map<std::string, std::size_t> grpkey_map_;
  std::unordered_map<std::size_t, qr_tuple> grp_tpl_map_;
  std::vector<std::pair<std::string, std::size_t>> aggrs_;
  std::unordered_map<std::size_t, std::size_t> grp_size_map_;
};

/**
 * distinct_tuples implements an operator for outputing distinct
 * result tuples.
 */
struct distinct_tuples : public qop {
  distinct_tuples() = default;
  ~distinct_tuples() = default;

  void dump(std::ostream &os) const override;

  void process(graph_db_ptr &gdb, const qr_tuple &v);

  std::mutex m_;
  std::set<std::string> keys_;
};

/**
 * filter_tuple implements an operator that filters a tuple
 * based on a predicate function.
 */
struct filter_tuple : public qop {
  filter_tuple(std::function<bool(const qr_tuple &)> func)
      : pred_func_(func) {}
  ~filter_tuple() = default;

  void dump(std::ostream &os) const override;

  void process(graph_db_ptr &gdb, const qr_tuple &v);

  void finish(graph_db_ptr &gdb);

  std::function<bool(const qr_tuple &)> pred_func_;
};

/**
 * qr_tuple_append implements an operator that appends a query
 * result to a query tuple. The query result is computed from
 * already existing query results in the tuple.
 */
struct qr_tuple_append : public qop {
  qr_tuple_append(std::function<query_result(const qr_tuple &)> func)
      : func_(func) {}
  ~qr_tuple_append() = default;

  void dump(std::ostream &os) const override;

  void process(graph_db_ptr &gdb, const qr_tuple &v);

  void finish(graph_db_ptr &gdb);

  std::function<query_result(const qr_tuple &)> func_;
};

/**
 * union_all_qres implements an operator that unions all the
 * query tuples of the left query pipeline and the right query
 * pipeline(s).
 */
struct union_all_qres : public qop {
  union_all_qres() : init(true) {}
  // union_all_qres() = default;
  ~union_all_qres() = default;

  void dump(std::ostream &os) const override;

  void process_left(graph_db_ptr &gdb, const qr_tuple &v);
  void process_right(graph_db_ptr &gdb, const qr_tuple &v);

  void finish(graph_db_ptr &gdb);

  bool is_binary() const override { return true; }
  bool init;
  std::list<qr_tuple> res_;
};

/**
 * count_result implements an operator that counts the
 * query tuples of the query pipeline.
 */
struct count_result : public qop {
  count_result() : count_(0) {}
  ~count_result() = default;

  void dump(std::ostream &os) const override;

  void process(graph_db_ptr &gdb, const qr_tuple &v);

  void finish(graph_db_ptr &gdb);

  uint64_t count_;
};

/**
 * shortest_path_opr implements an operator that finds the
 * unweighted shortest path between two nodes.
 * all_spaths_ specfies if all shortest path of equal 
 * distance are searched.
 */
struct shortest_path_opr : public qop {
  shortest_path_opr(std::pair<std::size_t, std::size_t> uv,
    rship_predicate pred, bool bidir, bool all) : bidirectional_(bidir),
          all_spaths_(all), rpred_(pred), start_stop_(uv) {}
  ~shortest_path_opr() = default;

  void dump(std::ostream &os) const override;

  void process(graph_db_ptr &gdb, const qr_tuple &v);

  void finish(graph_db_ptr &gdb);

  path_item path_;
  bool bidirectional_;
  bool all_spaths_;
  rship_predicate rpred_;
  std::pair<std::size_t, std::size_t> start_stop_;
};

/**
 * weighted_shortest_path_opr implements an operator that finds the
 * weighted shortest path between two nodes.
 * all_spaths_ specfies if all shortest path of equal 
 * weight are searched.
 */
struct weighted_shortest_path_opr : public qop {
  weighted_shortest_path_opr(std::pair<std::size_t, std::size_t> uv, rship_predicate pred,
    rship_weight weight, bool bidir, bool all) : bidirectional_(bidir), all_spaths_(all),
      rpred_(pred), rweight_(weight), start_stop_(uv) {}
  ~weighted_shortest_path_opr() = default;

  void dump(std::ostream &os) const override;

  void process(graph_db_ptr &gdb, const qr_tuple &v);

  path_item path_;
  bool bidirectional_;
  bool all_spaths_;
  rship_predicate rpred_;
  rship_weight rweight_;
  std::pair<std::size_t, std::size_t> start_stop_;
};

/**
 * k_weighted_shortest_path_opr implements an operator that finds the
 * top k weighted shortest path between two nodes.
 */
struct k_weighted_shortest_path_opr : public qop {
  k_weighted_shortest_path_opr(std::pair<std::size_t, std::size_t> uv,
    std::size_t k, rship_predicate pred, rship_weight weight, bool b) : 
    k_(k), bidirectional_(b), rpred_(pred), rweight_(weight), start_stop_(uv) {}
  ~k_weighted_shortest_path_opr() = default;

  void dump(std::ostream &os) const override;

  void process(graph_db_ptr &gdb, const qr_tuple &v);

  std::size_t k_;
  path_item path_;
  bool bidirectional_;
  rship_predicate rpred_;
  rship_weight rweight_;
  std::pair<std::size_t, std::size_t> start_stop_;
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
struct collect_result : public qop {
  collect_result(result_set &res) : results_(res) {}
  ~collect_result() = default;

  void dump(std::ostream &os) const override;

  void process(graph_db_ptr &gdb, const qr_tuple &v);

  void finish(graph_db_ptr &gdb);

  result_set &results_;
};

/**
 * end_pipeline is a query operator to end a query pipeline without
 * collecting the query results.
 */
struct end_pipeline : public qop {
  end_pipeline() = default;

  void dump(std::ostream &os) const override;

  void process();
};

/**
 * Macro to simplify definition of arguments in project etc.
 * Usage: Instead of requiring to define a lambda expression
 *        we simply use PExpr_(my_func(res, ...))
 *        Note that res has to be used to refer to the query_result vector.
 */
#define PExpr_(i, func)                                                        \
  projection::expr {                                                           \
    i, [&](auto res) { return func; }                                          \
  }

#define PVar_(i)                                                               \
  projection::expr { i, nullptr }

/**
 * projection implements a project operator.
 */
struct projection : public qop {
  using pr_result = boost::variant<node_description, node *, rship_description,
                                   relationship *, int, double, 
                                   std::string, uint64_t, boost::posix_time::ptime, array_t, null_t>;

  struct expr {
    std::size_t vidx;
    std::function<query_result(pr_result)> func;
    expr() = default;
    expr(std::size_t i, std::function<query_result(pr_result)> f) : vidx(i), func(f) {}
  };

  using expr_list = std::vector<expr>;

  projection(const expr_list &exprs);

  void dump(std::ostream &os) const override;

  void process(graph_db_ptr &gdb, const qr_tuple &v);

  expr_list exprs_;
  std::size_t nvars_, npvars_;
  std::vector<std::size_t> var_map_;
  std::set<std::size_t> accessed_vars_;
};

/**
 *  namespace for builtin functions used in project, filter etc.
 */
namespace builtin {

query_result forward(projection::pr_result &res);


/**
 * Returns true if the node/relationship has the property specified by
 * the key. Otherwise, return false.
 */	
bool has_property(projection::pr_result &pv, const std::string &key);

/**
 * Returns true if the node/relationship has the given label specified. 
 * Otherwise, return false.
 */	
bool has_label(projection::pr_result &pv, const std::string &l);

/**
 * Return the integer value of the property of a node/relationship stored in
 * projection_result res and identified by the given key.
 */
query_result int_property(projection::pr_result &res, 
                 const std::string &key);

/**
 * Return the double value of the property of a node/relationship stored in
 * projection_result res and identified by the given key.
 */
query_result double_property(projection::pr_result &res, 
                       const std::string &key);

/**
 * Return the string value of the property of a node/relationship stored in
 * projection_result res and identified by the given key.
 */
query_result string_property(projection::pr_result &res, 
                            const std::string &key);

/**
 * Return the unsigned 64-bit integer value of the property of a node/relationship 
 * stored in projection_result res and identified by the given key.
 */
query_result uint64_property(projection::pr_result &res, 
                 const std::string &key);

/**
 * Return the ptime value of the property of a node/relationship 
 * stored in projection_result res and identified by the given key.
 */
query_result ptime_property(projection::pr_result &res, 
                 const std::string &key);

/**
 * Return the string representation of the date property of a node/relationship 
 * stored in projection_result res and identified by the given key.
 */
query_result pr_date(projection::pr_result &pv, 
                 const std::string &key);

/**
 * Return the year of the date property of a node/relationship 
 * stored in projection_result res and identified by the given key.
 */
query_result pr_year(projection::pr_result &pv, 
                 const std::string &key);

/**
 * Return the month of the date property of a node/relationship 
 * stored in projection_result res and identified by the given key.
 */
query_result pr_month(projection::pr_result &pv, 
                 const std::string &key);

/**
 * Return the string representation of a node/relationship stored in
 * projection_result res.
 */
std::string string_rep(projection::pr_result &res);

/**
 * Convert the given string value into an integer.
 */
int to_int(const std::string &s);

/**
 * Convert the given date value into its string representation.
 */
std::string int_to_datestring(int v);
std::string int_to_datestring(const query_result& v);

/**
 * Convert the given date string (2019-02-12) into an int value (posix time).
 */
int datestring_to_int(const std::string &d);

/**
 * Convert the given datetime value into its string representation.
 */
std::string int_to_dtimestring(int v);

std::string int_to_dtimestring(const query_result& v);

/**
 * Convert the given date+time string (2019-01-22T19:59:59.221+0000) into an int
 * value (posix time).
 */
int dtimestring_to_int(const std::string &d);

/*
CASE:
 return !string_property(res, 0, "content").empty() ?
    string_property(res, 0, "content") :
    string_property(res, 0, "imageFile"); },

*/

} // namespace builtin

#endif
