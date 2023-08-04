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

#ifndef query_builder_hpp_
#define query_builder_hpp_

#include <initializer_list>

#include "graph_db.hpp"
#include "query_ctx.hpp"
#include "qop.hpp"
#include "qop_aggregates.hpp"
#include "qop_analytics.hpp"

/**
 * The query_builder class allows to construct a plan for executing a graph query in dot
 * notation. The individual methods represent plan operators which can be
 * combined into a query. Example: auto q = query(graph) .all_nodes()
 *            .from_relationships()
 *            .to_node()
 *            .project({ PExpr_(0, builtin::int_property(res, "id")),
 *                       PExpr_(2, builtin::int_property(res, "id"))})
 *            .print();
 *   q.start();
 */
class query_builder {
  friend class query_set;
public:
  /**
   * Constructor for a query on the given graph database.
   */
  query_builder(query_ctx& ctx) : ctx_(ctx) {}

  query_builder(query_ctx& ctx, qop_ptr qop);

  /**
   * Default destructor.
   */
  ~query_builder() = default;

  query_builder &operator=(const query_builder &);

  /**
   * Add a scan over all nodes (optionally with the given label = type).
   */
  query_builder &all_nodes(const std::string &label = "");
  query_builder &all_nodes(std::map<std::size_t, std::vector<std::size_t>> &range_map, const std::string &label = "");

  /**
   * Add a scan over all nodes with the label which satisfy the given predicate
   * on the property with the given key.
   */
  query_builder &nodes_where(const std::string &label, const std::string &key,
                     std::function<bool(const p_item &)> pred);
  
  /**
   * Add a scan over all nodes with any of the given labels which satisfy the given predicate
   * on the property with the given key. This is for entity objects belonging to the same
   * abstract entity (e.g. Post and Comment are sub-classes of Message)
   */
  query_builder &nodes_where(const std::vector<std::string> &labels, const std::string &key,
                     std::function<bool(const p_item &)> pred);

  /**
   * Add an index scan over nodes where the key is equal to the given value. 
   */
  query_builder &nodes_where_indexed(const std::string &label, const std::string &prop, uint64_t val);

  query_builder &nodes_where_indexed(const std::vector<std::string> &labels,
                              const std::string &prop, uint64_t val);

  /**
   * Add an operator that scans all incoming relationships of the last node in
   * the query result. Optionally, 1) the given label of the relationship is
   * checked, too. 2) Nodes that were already explored, i.e. other than the frontier,
   * can also be re-explored, given their position.
   */
  query_builder &to_relationships(const std::string &label = "",
                          int pos = std::numeric_limits<int>::max());
  query_builder &to_relationships(std::pair<int, int> range,
                          const std::string &label = "",
                          int pos = std::numeric_limits<int>::max());

  /**
   * Add an operator that scans all outgoing relationships of the last node in
   * the query result. Optionally, 1) the given label of the relationship is
   * checked, too. 2) Nodes that were already explored, i.e. other than the frontier,
   * can also be re-explored, given their position.
   */
  query_builder &from_relationships(const std::string &label = "",
                            int pos = std::numeric_limits<int>::max());

  query_builder &from_relationships(std::pair<int, int> range,
                            const std::string &label = "",
                            int pos = std::numeric_limits<int>::max());

  /**
   * Add an operator that scans all outgoing and incoming relationships of the last node in
   * the query result. Optionally, 1) the given label of the relationship is
   * checked, too. 2) Nodes that were already explored, i.e. other than the frontier,
   * can also be re-explored, given their position.
   */
  query_builder &all_relationships(const std::string &label = "",
                            int pos = std::numeric_limits<int>::max());

  query_builder &all_relationships(std::pair<int, int> range,
                            const std::string &label = "",
                            int pos = std::numeric_limits<int>::max());

  /**
   * Add a filter operator for checking that the property with the given key
   * satisfies the predicate. This predicate is applied to the last
   * node/relationship that was processed, i.e. in a pattern like
   * (n1:Node)-[r:Relationship]->(n2:Node) it refers to the node n2.
   */
  query_builder &property(const std::string &key,
                  std::function<bool(const p_item &)> pred);

  query_builder &filter(const expr &ex);

  /**
   * Add an operator the retrieves the node at the destination side of the
   * currently processed relationship with an optional filter for label(s).
   */
  query_builder &to_node(const std::string &label = "");

  query_builder &to_node(const std::vector<std::string> &labels);

  /**
   * Add an operator the retrieves the node at the source side of the
   * currently processed relationship with an optional filter for label(s).
   */
  query_builder &from_node(const std::string &label = "");

  query_builder &from_node(const std::vector<std::string> &labels);

  /**
   * Add a filter operator that checks whether the last node/relationship
   * in the result has the given label.
   */
  query_builder &has_label(const std::string &label);

  /**
   * Add a filter operator that checks whether the last node/relationship
   * in the result has one of the given labels.
   */
  query_builder &has_label(const std::vector<std::string> &labels);

  /**
   * Add a limit operator that produces only the first n result elements.
   */
  query_builder &limit(std::size_t n);

  /**
   * Add an operator that appends the relationship object between a source and a
   * destination node, whose positions in the query tuple are given by the
   * src_des pair.
   * When no relationship exist between them, the boolean b sets whether a
   * null_t is appended instead (true) or not (false)
   */
  query_builder &rship_exists(std::pair<int, int> src_des, bool append_null = true);

  /**
   * Add a projection operator that applies the given list of projection
   * functions to the query result.
   */
  query_builder &project(const projection::expr_list &exprs);

  query_builder &project(std::vector<projection_expr> prexpr);

  /**
   * Add an operator for sorting the results.
   */
  query_builder &orderby(std::function<bool(const qr_tuple &, const qr_tuple &)> cmp);

  /**
   * Add an operator for grouping and optional aggregation. The positions of the 
   * grouping keys in the query result tuple are specified by the positions in 
   * the vector pos. The aggregation function name(s) and the position(s) of the 
   * attribute(s) (in the tuple) to be aggregated are given as the vector of 
   * string-int pairs aggrs.
   */
  /*
  query_builder &groupby(const std::vector<std::size_t> &pos);
  query_builder &groupby(const std::vector<std::size_t> &pos,
    const std::vector<std::pair<std::string, std::size_t>> &aggrs);
  query_builder &groupby(std::list<qr_tuple> &grps, const std::vector<std::size_t> &pos,
    const std::vector<std::pair<std::string, std::size_t>> &aggrs);
  */
  query_builder &groupby(const std::vector<group_by::group>& grps, const std::vector<group_by::expr>& exprs);

  query_builder &aggr(const std::vector<aggregate::expr>& exprs);

  /**
   * Add an operator for eliminating duplicates in result tuples.
   * Resulting tuples are distinct tuples.
   */
  query_builder &distinct();

  /**
   * Add an operator to filter projected result tuples based on the pred function.
   */
  query_builder &where_qr_tuple(std::function<bool(const qr_tuple &)> pred);

  /**
   * Add an operator that applies a function on multiple query results in the 
   * same query tuple and appends the result to the tuple.
   */
  query_builder &append_to_qr_tuple(std::function<query_result(const qr_tuple &)> func);

  /**
   * Add an operator to unions all the query tuples of the left query 
   * pipeline and the right query pipeline(s).
   */
  query_builder &union_all(query_builder &other);

  query_builder &union_all(std::initializer_list<query_builder *> queries);

  /**
   * Add an operator to count the number of tuples in the pipeline.
   */
  query_builder &count();

  /**
   * Add a print operator for outputting the query results to cout.
   */
  query_builder &print();

  /**
   * Add an operator for collecting the query results in the given result_set
   * object.
   */
  query_builder &collect(result_set &rs);

  /**
   * Ends a query pipeline.
   */
  query_builder &finish();

  /**
   * Add an operator for constructing the cartesian product of the query tuples 
   * of the left and right query pipelines.
   */
  query_builder &crossjoin(query_builder &other);

  /**
   * Add a nested loop join operator for merging tuples of two
   * query pipelines if the node at a given position in the left tuple
   * is the same as the node at another given position in the right tuple.
   * The node positions are specified by the pos pair. 
   */
  query_builder &join_on_node(std::pair<int, int> left_right, query_builder &other);

  /**
   * Add a hash join operator for merging tuples of two
   * query pipelines if the node at a given position in the left tuple
   * is the same as the node at another given position in the right tuple.
   * The node positions are specified by the pos pair. 
   */
  query_builder &hashjoin_on_node(std::pair<int, int> left_right, query_builder &other);

  /**
   * Add a left outerjoin operator for merging tuples of two queries based 
   * on the given join condition. Dangling tuples are padded with "null_val" 
   */
  query_builder &outerjoin(query_builder &other, std::function<bool(const qr_tuple &, const qr_tuple &)> pred);

  /**
   * Add a left outerjoin operator for merging tuples of two queries if the node
   * at a given position in the left tuple is the same as the node at another
   * given position in the right tuple. The node positions are specified by the
   * pos pair. Dangling tuples are padded with "NULL" consume_(gdb, {&n});
   */
  query_builder &outerjoin_on_node(const std::pair<int, int> &left_right, query_builder &other);

  /**
   * Add a join operator for merging tuples of two 
   * queries if there exists a relationship defined by an object
   * (at a given position) in the left tuple as the source node 
   * and an object (at a given position) in the right tuple as 
   * the destination node 
   */
  query_builder &join_on_rship(std::pair<int, int> src_des, query_builder &other);

  /**
   * Add a left outerjoin operator for merging tuples of two 
   * queries if there exists a relationship defined by an object
   * (at a given position) in the left tuple as the source node 
   * and an object (at a given position) in the right tuple as 
   * the destination node 
   */
  query_builder &outerjoin_on_rship(std::pair<int, int> src_des, query_builder &other);

  /**
   * Add an operator to find the unweighted shortest path between the pair 
   * of nodes given their positions in the query tuple. Bidirectional 
   * search (i.e. via outgoing and incoming relationships) is optionally 
   * set using the flag, bidirectional.
   * rpred is a predicate for checking if a relationship is traversed.
   * The operator appends an array of IDs of the nodes along the shortest
   * path.
   * all_spaths specfies if all shortest path of equal distance are searched.
  */
  query_builder &algo_shortest_path(std::pair<std::size_t, std::size_t> start_stop,
        rship_predicate rpred, bool bidirectional = false, bool all_spaths = false);

  /**
   * Add an operator to find the weighted shortest path between the pair 
   * of nodes given their positions in the query tuple. Bidirectional 
   * search (i.e. via outgoing and incoming relationships) is optionally 
   * set using the flag, bidirectional.
   * rpred is a predicate for checking if a relationship is traversed.
   * weight is a function that computes the weight of a relationship.
   * The operator appends the total weight of the shortest path to the
   * query tuple.
   * all_spaths specfies if all shortest path of equal weight are searched.
  */
  query_builder &algo_weighted_shortest_path(std::pair<std::size_t, std::size_t> start_stop,
        rship_predicate rpred, rship_weight weight, bool bidirectional = false,
        bool all_spaths = false);

#ifdef USE_GUNROCK
  /**
   * Add an operator for Breadth-First Search algorithm leveraging Gunrock.
  */
  query_builder &gunrock_bfs(std::size_t start, bool bidirectional = false);

  /**
   * Add an operator for Single-Source Shortest Path algorithm leveraging Gunrock.
  */
  query_builder &gunrock_sssp(std::size_t start, rship_weight weight, bool bidirectional = false);

  /**
   * Add an operator for PageRank algorithm leveraging Gunrock.
  */
  query_builder &gunrock_pr(bool bidirectional = false);
#endif

  /**
   * Add an operator to find the top k weighted shortest path between the pair 
   * of nodes given their positions in the query tuple. Bidirectional 
   * search (i.e. via outgoing and incoming relationships) is optionally 
   * set using the flag, bidirectional.
   * rpred is a predicate for checking if a relationship is traversed.
   * weight is a function that computes the weight of a relationship.
   * The operator appends the total weight of the shortest path to the
   * query tuple.
  */
  query_builder &algo_k_weighted_shortest_path(std::pair<std::size_t, std::size_t> start_stop,
      std::size_t k, rship_predicate rpred, rship_weight weight, bool bidirectional = false);

  /**
   * Add an operator to get the ids of the neighbours of each node 
   * of the graph, for conversion to the CSR format. In addition, 
   * the weight of the relationship connecting each neighbouring 
   * node is computed from the weight function. 
   * The bidirectional flag specifies whether only outgoing relationships 
   * are considered (false) or both outgoing and incoming relationships 
   * are considered (true).
  */
  query_builder &csr(rship_weight weight, bool bidirectional = false,
             std::size_t pos = std::numeric_limits<std::size_t>::max());

  /**
   * Add an operator for invoking a LUA function as part of the query.
   */
  /* query_builder &call_lua(const std::string &proc_name,
                  const std::vector<std::size_t> &params);
   */

  /*-------------------------------------------------------------------*/

  /**
   * Add an operator for creating a node.
   */
  query_builder &create(const std::string &label, const properties_t &props);

  /**
   * Add an operator for creating a relationship that connects two nodes 
   * at any given positions in the result.
   */
  query_builder &create_rship(std::pair<int, int> src_des, const std::string &label,
                        const properties_t &props);

  /**
   * Add an operator for creating a relationship that connects two nodes 
   * from two different queries: One of the nodes (the source node by default) 
   * is at a given position in the left query and the other (the destination 
   * node by default) is at the last position in the right query. The default 
   * relationship direction can be changed via a flag..
   */
  query_builder &create_rship(query_builder &other, int l_node_pos, const std::string &label,
                        const properties_t &props, bool src_to_des = true);

  /**
   *  Add an operator for updating a node.
   */
  query_builder &update(std::size_t var, properties_t &props);

  /**
   * Add an operator for deleting the last node in a query tuple.
   * All relationship objects connected to the node are also deleted.
   * The optional pos specifies a node to be deleted at other
   * positions in the tuple.
   */
  query_builder &delete_detach(const std::size_t pos = std::numeric_limits<std::size_t>::max());

  /**
   * Add an operator for deleting the last node in a query tuple.
   * The optional pos specifies a node to be deleted at other
   * positions in the tuple.
   */
  query_builder &delete_node(const std::size_t pos = std::numeric_limits<std::size_t>::max());

  /**
   * Add an operator for deleting the last relationship in a query tuple.
   * The optional pos specifies a relationship to be deleted at other
   * positions in the tuple.
   */
  query_builder &delete_rship(const std::size_t pos = std::numeric_limits<std::size_t>::max());

  /*-------------------------------------------------------------------*/

  /**
   * Start the execution of the query.
   */
  void start(query_ctx& ctx);

  /**
   * Print the query plan.
   */
  void print_plan(std::ostream& os = std::cout);

  static void start(query_ctx& ctx, std::initializer_list<query_builder *> queries);
  static void print_plans(std::initializer_list<query_builder *> queries, std::ostream& os = std::cout);

  void extract_args();
  /**
   * Return the pointer to the graph database.
   */
  graph_db_ptr &get_graph_db() { return ctx_.gdb_; }

  qop_ptr &plan_head() { return plan_head_; }

private:
  query_builder &append_op(qop_ptr op, qop::consume_func cf, qop::finish_func ff);
  query_builder &append_op(qop_ptr op, qop::consume_func cf);

  qop_ptr plan_head_, plan_tail_;
  query_ctx& ctx_;
};


#endif