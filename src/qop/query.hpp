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

#ifndef query_hpp_
#define query_hpp_

#include <initializer_list>

#include "graph_db.hpp"
#include "qop.hpp"

struct base_op;

/**
 * The query class allows to construct a plan for executing a graph query in dot
 * notation. The individual methods represent plan operators which can be
 * combined into a query. Example: auto q = query(graph) .all_nodes()
 *            .from_relationships()
 *            .to_node()
 *            .project({ PExpr_(0, builtin::int_property(res, "id")),
 *                       PExpr_(2, builtin::int_property(res, "id"))})
 *            .print();
 *   q.start();
 */
class query {
  friend class query_set;
public:
  /**
   * Constructor for a query on the given graph database.
   */
  query(graph_db_ptr gdb) : graph_db_(gdb) {}

  query(graph_db_ptr gdb, qop_ptr qop);

  /**
   * Default destructor.
   */
  ~query() = default;

  query &operator=(const query &) = default;

  /**
   * Add a scan over all nodes (optionally with the given label = type).
   */
  query &all_nodes(const std::string &label = "");
  query &all_nodes(std::map<std::size_t, std::vector<std::size_t>> &range_map, const std::string &label = "");

  /**
   * Add a scan over all nodes with the label which satisfy the given predicate
   * on the property with the given key.
   */
  query &nodes_where(const std::string &label, const std::string &key,
                     std::function<bool(const p_item &)> pred);
  
  /**
   * Add a scan over all nodes with any of the given labels which satisfy the given predicate
   * on the property with the given key. This is for entity objects belonging to the same
   * abstract entity (e.g. Post and Comment are sub-classes of Message)
   */
  query &nodes_where(const std::vector<std::string> &labels, const std::string &key,
                     std::function<bool(const p_item &)> pred);

  /**
   * Add an index scan over nodes where the key is equal to the given value. 
   */
  query &nodes_where_indexed(const std::string &label, const std::string &prop, uint64_t val);

  query &nodes_where_indexed(const std::vector<std::string> &labels,
                              const std::string &prop, uint64_t val);

  /**
   * Add an operator that scans all incoming relationships of the last node in
   * the query result. Optionally, 1) the given label of the relationship is
   * checked, too. 2) Nodes that were already explored, i.e. other than the frontier,
   * can also be re-explored, given their position.
   */
  query &to_relationships(const std::string &label = "",
                          int pos = std::numeric_limits<int>::max());
  query &to_relationships(std::pair<int, int> range,
                          const std::string &label = "",
                          int pos = std::numeric_limits<int>::max());

  /**
   * Add an operator that scans all outgoing relationships of the last node in
   * the query result. Optionally, 1) the given label of the relationship is
   * checked, too. 2) Nodes that were already explored, i.e. other than the frontier,
   * can also be re-explored, given their position.
   */
  query &from_relationships(const std::string &label = "",
                            int pos = std::numeric_limits<int>::max());

  query &from_relationships(std::pair<int, int> range,
                            const std::string &label = "",
                            int pos = std::numeric_limits<int>::max());

  /**
   * Add an operator that scans all outgoing and incoming relationships of the last node in
   * the query result. Optionally, 1) the given label of the relationship is
   * checked, too. 2) Nodes that were already explored, i.e. other than the frontier,
   * can also be re-explored, given their position.
   */
  query &all_relationships(const std::string &label = "",
                            int pos = std::numeric_limits<int>::max());

  query &all_relationships(std::pair<int, int> range,
                            const std::string &label = "",
                            int pos = std::numeric_limits<int>::max());

  /**
   * Add a filter operator for checking that the property with the given key
   * satisfies the predicate. This predicate is applied to the last
   * node/relationship that was processed, i.e. in a pattern like
   * (n1:Node)-[r:Relationship]->(n2:Node) it refers to the node n2.
   */
  query &property(const std::string &key,
                  std::function<bool(const p_item &)> pred);

  /**
   * Add an operator the retrieves the node at the destination side of the
   * currently processed relationship with an optional filter for label(s).
   */
  query &to_node(const std::string &label = "");

  query &to_node(const std::vector<std::string> &labels);

  /**
   * Add an operator the retrieves the node at the source side of the
   * currently processed relationship with an optional filter for label(s).
   */
  query &from_node(const std::string &label = "");

  query &from_node(const std::vector<std::string> &labels);

  /**
   * Add a filter operator that checks whether the last node/relationship
   * in the result has the given label.
   */
  query &has_label(const std::string &label);

  /**
   * Add a filter operator that checks whether the last node/relationship
   * in the result has one of the given labels.
   */
  query &has_label(const std::vector<std::string> &labels);

  /**
   * Add a limit operator that produces only the first n result elements.
   */
  query &limit(std::size_t n);

  /**
   * Add an operator that appends the relationship object between a source and a
   * destination node, whose positions in the query tuple are given by the
   * src_des pair.
   * When no relationship exist between them, the boolean b sets whether a
   * null_t is appended instead (true) or not (false)
   */
  query &rship_exists(std::pair<int, int> src_des, bool append_null = true);

  /**
   * Add a projection operator that applies the given list of projection
   * functions to the query result.
   */
  query &project(const projection::expr_list &exprs);

  /**
   * Add an operator for sorting the results.
   */
  query &orderby(std::function<bool(const qr_tuple &, const qr_tuple &)> cmp);

  /**
   * Add an operator for grouping and optional aggregation. The positions of the 
   * grouping keys in the query result tuple are specified by the positions in 
   * the vector pos. The aggregation function name(s) and the position(s) of the 
   * attribute(s) (in the tuple) to be aggregated are given as the vector of 
   * string-int pairs aggrs.
   */
  query &groupby(const std::vector<std::size_t> &pos);
  query &groupby(const std::vector<std::size_t> &pos,
    const std::vector<std::pair<std::string, std::size_t>> &aggrs);
  query &groupby(std::list<qr_tuple> &grps, const std::vector<std::size_t> &pos,
    const std::vector<std::pair<std::string, std::size_t>> &aggrs);
  

  /**
   * Add an operator for eliminating duplicates in result tuples.
   * Resulting tuples are distinct tuples.
   */
  query &distinct();

  /**
   * Add an operator to filter projected result tuples based on the pred function.
   */
  query &where_qr_tuple(std::function<bool(const qr_tuple &)> pred);

  /**
   * Add an operator that applies a function on multiple query results in the 
   * same query tuple and appends the result to the tuple.
   */
  query &append_to_qr_tuple(std::function<query_result(const qr_tuple &)> func);

  /**
   * Add an operator to unions all the query tuples of the left query 
   * pipeline and the right query pipeline(s).
   */
  query &union_all(query &other);

  query &union_all(std::initializer_list<query *> queries);

  /**
   * Add an operator to count the number of tuples in the pipeline.
   */
  query &count();

  /**
   * Add a print operator for outputting the query results to cout.
   */
  query &print();

  /**
   * Add an operator for collecting the query results in the given result_set
   * object.
   */
  query &collect(result_set &rs);

  /**
   * Ends a query pipeline.
   */
  query &finish();

#ifdef QOP_RECOVERY
  /**
   * Add an operator for recovering stored intermediate results 
   * and pushing them to the query pipeline.
   */
  query &recover_results();

  /**
   * Perists intermediate tuple results
   */
  query &persist();

  /**
   * Add an operator to continue a sequential scan starting from given checkpoint positions
   */
  query &continue_scan(std::map<std::size_t, std::size_t> &cp, const std::string &label = "");

  /**
   * Add an operator to group query tuples where the intermediate results stored in PMem
   */
  query &pgroupby(const std::vector<std::size_t> &pos,
    const std::vector<std::pair<std::string, std::size_t>> &aggrs);

  /**
   * Add an operator to intentionally crash a query after a certain tuple count
   */
  query &crash(std::size_t n);
#endif 
  /**
   * Add an operator for constructing the cartesian product of the query tuples 
   * of the left and right query pipelines.
   */
  query &crossjoin(query &other);

  /**
   * Add a nested loop join operator for merging tuples of two
   * query pipelines if the node at a given position in the left tuple
   * is the same as the node at another given position in the right tuple.
   * The node positions are specified by the pos pair. 
   */
  query &join_on_node(std::pair<int, int> left_right, query &other);

  /**
   * Add a hash join operator for merging tuples of two
   * query pipelines if the node at a given position in the left tuple
   * is the same as the node at another given position in the right tuple.
   * The node positions are specified by the pos pair. 
   */
  query &hashjoin_on_node(std::pair<int, int> left_right, query &other);

  /**
   * Add a left outerjoin operator for merging tuples of two queries based 
   * on the given join condition. Dangling tuples are padded with "null_val" 
   */
  query &outerjoin(query &other, std::function<bool(const qr_tuple &, const qr_tuple &)> pred);

  /**
   * Add a left outerjoin operator for merging tuples of two queries if the node
   * at a given position in the left tuple is the same as the node at another
   * given position in the right tuple. The node positions are specified by the
   * pos pair. Dangling tuples are padded with "NULL" consume_(gdb, {&n});
   */
  query &outerjoin_on_node(const std::pair<int, int> &left_right, query &other);

  /**
   * Add a join operator for merging tuples of two 
   * queries if there exists a relationship defined by an object
   * (at a given position) in the left tuple as the source node 
   * and an object (at a given position) in the right tuple as 
   * the destination node 
   */
  query &join_on_rship(std::pair<int, int> src_des, query &other);

  /**
   * Add a left outerjoin operator for merging tuples of two 
   * queries if there exists a relationship defined by an object
   * (at a given position) in the left tuple as the source node 
   * and an object (at a given position) in the right tuple as 
   * the destination node 
   */
  query &outerjoin_on_rship(std::pair<int, int> src_des, query &other);

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
  query &algo_shortest_path(std::pair<std::size_t, std::size_t> start_stop,
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
  query &algo_weighted_shortest_path(std::pair<std::size_t, std::size_t> start_stop,
        rship_predicate rpred, rship_weight weight, bool bidirectional = false,
        bool all_spaths = false);

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
  query &algo_k_weighted_shortest_path(std::pair<std::size_t, std::size_t> start_stop,
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
  query &csr(rship_weight weight, bool bidirectional = false,
             std::size_t pos = std::numeric_limits<std::size_t>::max());

  /**
   * Add an operator for invoking a LUA function as part of the query.
   */
  /* query &call_lua(const std::string &proc_name,
                  const std::vector<std::size_t> &params);
   */

  /*-------------------------------------------------------------------*/

  /**
   * Add an operator for creating a node.
   */
  query &create(const std::string &label, const properties_t &props);

  /**
   * Add an operator for creating a relationship that connects two nodes 
   * at any given positions in the result.
   */
  query &create_rship(std::pair<int, int> src_des, const std::string &label,
                        const properties_t &props);

  /**
   * Add an operator for creating a relationship that connects two nodes 
   * from two different queries: One of the nodes (the source node by default) 
   * is at a given position in the left query and the other (the destination 
   * node by default) is at the last position in the right query. The default 
   * relationship direction can be changed via a flag..
   */
  query &create_rship(query &other, int l_node_pos, const std::string &label,
                        const properties_t &props, bool src_to_des = true);

  /**
   *  Add an operator for updating a node.
   */
  query &update(std::size_t var, properties_t &props);

  /**
   * Add an operator for deleting the last node in a query tuple.
   * All relationship objects connected to the node are also deleted.
   * The optional pos specifies a node to be deleted at other
   * positions in the tuple.
   */
  query &delete_detach(const std::size_t pos = std::numeric_limits<std::size_t>::max());

  /**
   * Add an operator for deleting the last node in a query tuple.
   * The optional pos specifies a node to be deleted at other
   * positions in the tuple.
   */
  query &delete_node(const std::size_t pos = std::numeric_limits<std::size_t>::max());

  /**
   * Add an operator for deleting the last relationship in a query tuple.
   * The optional pos specifies a relationship to be deleted at other
   * positions in the tuple.
   */
  query &delete_rship(const std::size_t pos = std::numeric_limits<std::size_t>::max());

  /*-------------------------------------------------------------------*/

  /**
   * Start the execution of the query.
   */
  void start();

  /**
   * Print the query plan.
   */
  void print_plan(std::ostream& os = std::cout);

  static void start(std::initializer_list<query *> queries);
  static void print_plans(std::initializer_list<query *> queries, std::ostream& os = std::cout);

  void extract_args();
  /**
   * Return the pointer to the graph database.
   */
  graph_db_ptr &get_graph_db() { return graph_db_; }

  std::shared_ptr<base_op> get_algebra_plan() { return algebra_head; }


private:
  query &append_op(qop_ptr op, qop::consume_func cf, qop::finish_func ff);
  query &append_op(qop_ptr op, qop::consume_func cf);

  qop_ptr plan_head_, plan_tail_;
  graph_db_ptr graph_db_;

  std::shared_ptr<base_op> algebra_head;
  std::shared_ptr<base_op> algebra_plan;
};


#endif