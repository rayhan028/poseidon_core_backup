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

#ifndef graph_db_hpp_
#define graph_db_hpp_

#include <boost/any.hpp>
#include <map>
#include <mutex>
#include <string>

#include "dict.hpp"
#include "exceptions.hpp"
#include "nodes.hpp"
#include "properties.hpp"
#include "relationships.hpp"
#ifdef QOP_RECOVERY
#include <libpmemobj++/container/concurrent_hash_map.hpp>
#include "recovery.hpp"
#endif
#include "transaction.hpp"
#include "btree.hpp"
#include "index_map.hpp"
#include "pmlog.hpp"
#include "gc.hpp"
#include "robin_hood.h"
#if defined CSR_DELTA && defined USE_TX
#include "csr_delta.hpp"
#endif

/**
 * graph_db represents a graph consisting of nodes and relationships with
 * properties stored in a database. The class provides methods for constructing
 * and querying the graph.
 */
class graph_db {
public:
  /**
   * mapping_t is used during importing data from CSV files to map node names to
   * internal ids which are required for creating relationships.
   */
  // using mapping_t = std::unordered_map<std::string, node::id_t>;
  using mapping_t = robin_hood::unordered_map<std::string, node::id_t>;

  using node_consumer_func = std::function<void(node &)>;
  using rship_consumer_func = std::function<void(relationship &)>;

#ifdef QOP_RECOVERY
  using tuple_consumer_func = std::function<void(const qr_tuple &, int)>;

  using rec_map_t = pmem::obj::concurrent_hash_map<p<int>, p<int>>;
#endif

  static void destroy(p_ptr<graph_db> gp);

  /**
   * Constructor for a new empty graph database.
   */
  graph_db(const std::string &db_name = "");

  /**
   * Destructor.
   */
  ~graph_db();

  /* -------------- transaction management -------------- */

  /**
   * Starts a new transaction. This transaction is associated with the 
   * current thread and stored there as thread_local property.
   */
  void begin_transaction();

  /**
   * Commits the currently active transaction associated with this thread.
   */
  bool commit_transaction();

  /**
   * Aborts the currently active transaction associated with this thread.
   */
  bool abort_transaction();

  /**
   * Encapsulated code for execution a transaction. If body returns true then
   * the transaction is committed, otherwise the transaction is aborted.
   */
  bool run_transaction(std::function<bool()> body);

  /* ---------------- graph construction ---------------- */

  /**
   * Add a new node to the graph with the given label (type) and the
   * set of properties (key-value pairs). The method returns the node identifier
   * which can be used to create a relationship from or to this node.
   */
  node::id_t add_node(const std::string &label, const properties_t &props,
                      bool append_only = false);

  /**
   * Add a new node to the graph with the given label (type) and the
   * set of properties (key-value pairs) without transactional support.
   * The method returns the node identifier which can be used to create a
   * relationship from or to this node.
   */
  node::id_t import_node(const std::string &label, const properties_t &props);

  node::id_t import_typed_node(dcode_t label, const std::vector<dcode_t> &keys,
                              const std::vector<p_item::p_typecode>& typelist, 
                              const std::vector<boost::any>& values);

  node::id_t import_typed_node(dcode_t label, const std::vector<dcode_t> &keys,
                              const std::vector<p_item::p_typecode>& typelist,
							  const std::vector<std::string>& values,dict_ptr &dict);

  /**
   * Add a new relationship to the graph that connects from_node and to_node.
   * This relationship has initialized with the given label and properties.
   */
  relationship::id_t add_relationship(node::id_t from_node, node::id_t to_node,
                                      const std::string &label,
                                      const properties_t &props,
                                      bool append_only = false);

  /**
   * Add a new relationship to the graph that connects from_node and to_node
   * without transactional support. This relationship has initialized with the
   * given label and properties.
   */
  relationship::id_t import_relationship(node::id_t from_node,
                                         node::id_t to_node,
                                         const std::string &label,
                                         const properties_t &props);

  relationship::id_t import_typed_relationship(node::id_t from_node,
                                         node::id_t to_node,
                                         dcode_t label, 
                                         const std::vector<dcode_t> &keys,
                                         const std::vector<p_item::p_typecode>& typelist, 
					  const std::vector<std::string>& values,dict_ptr &dict);


  relationship::id_t import_typed_relationship(node::id_t from_node,
                                         node::id_t to_node,
                                         dcode_t label,
                                         const std::vector<dcode_t> &keys,
                                         const std::vector<p_item::p_typecode>& typelist,
                                         const std::vector<boost::any>& values);

  /* --------------- node/relationship information --------------- */

  /**
   * Returns a description of the node, i.e. with all decoded labels and
   * properties.
   */
  node_description get_node_description(node::id_t nid);

  /**
   * Returns a description of the relationship, i.e. with all decoded labels and
   * properties.
   */
  rship_description get_rship_description(relationship::id_t rid);

  /**
   * Returns the decoded label of the relationship.
   */
  const char *get_relationship_label(const relationship &r);

  /**
   * Returns the node identified by the given id.
   */
  node &node_by_id(node::id_t id);

  /**
   * Returns the relationship identified by the given id.
   */
  relationship &rship_by_id(relationship::id_t id);

  /* --------------- graph updates --------------- */
 
  /**
   * Updates the given node by changing the given
   * properties and replacing the label with the given one.
   */
  void update_node(node &n, const properties_t &props,
                   const std::string &label = "");

  /**
   * Updates the given relationship by changing the given
   * properties and replacing the label with the given one.
   */
  void update_relationship(relationship &r, const properties_t &props,
                           const std::string &label = "");


  /**
   * Deletes the node and its properties identified by the given id.
   */
  void delete_node(node::id_t id);

  /**
   * Delets the node identified by the given id, its properties and all the relationships of this node.
   */
  void detach_delete_node(node::id_t id);
  
  /**
   * Deletes the relationship and its properties identified by the given id.
   */
  void delete_relationship(relationship::id_t id);

  /**
   * Deletes the relationship and its properties identified by the given 
   * source and destination node ids.
   */
  void delete_relationship(node::id_t src, node::id_t dest);

  /* ---------------- CSV data import ---------------- */

  /**
   * Read the list of nodes from the given CSV file. The file is in ldbc
   * format with the given delimiter.
   */
  std::size_t import_nodes_from_csv(const std::string &label,
                                    const std::string &filename, char delim,
                                    mapping_t &m, std::mutex *mtx = nullptr);

  std::size_t import_typed_nodes_from_csv(const std::string &label,
                                    const std::string &filename, char delim,
                                    mapping_t &m, std::mutex *mtx = nullptr);
  std::size_t import_typed_n4j_nodes_from_csv(const std::string &label,
                                    const std::string &filename, char delim,
                                    mapping_t &m);

  /**
   * Read the list of relationships from the given CSV file. The file is in
   * ldbc format with the given delimiter.
   */
  std::size_t import_relationships_from_csv(const std::string &filename,
                                            char delim, const mapping_t &m, std::mutex *mtx = nullptr);

  std::size_t import_typed_relationships_from_csv(const std::string &filename,
                                            char delim, const mapping_t &m, std::mutex *mtx = nullptr);

   std::size_t import_typed_n4j_relationships_from_csv(const std::string &filename,
                                            char delim, const mapping_t &m, const std::string& rship_type = "");

  /* ---------------- helper ---------------- */

  /**
   * Performs initialization steps after starting the database.
   */
  void runtime_initialize();

  /**
   * Returns a reference to the dictionary of string codes.
   */
  p_ptr<dict> &get_dictionary() { return dict_; }

  /**
   * Returns a reference to the node property list of this graph.
   */
  const p_ptr<property_list>& get_node_properties() { return node_properties_; }

  /**
   * Returns a reference to the relationship property list of this graph.
   */
  const p_ptr<property_list>& get_rship_properties() { return rship_properties_; }

  /**
   * Returns a reference to the node list of this graph.
   */  

  const p_ptr<node_list>& get_nodes() { return nodes_; }

  /**
   * Returns a reference to the relationship list of this graph.
   */
  const p_ptr<relationship_list>& get_relationships() { return rships_; }

  /**
   * Returns the string value encoded with the given dictionary code.
   */
  const char *get_string(dcode_t c);

  /**
   * Returns the dictionary code for the given string.
   */
  dcode_t get_code(const std::string &s);

  /**
   * Prints the graph (nodes, relationships) to standard output.
   */
  void dump();

  /**
   * Generate a DOT file with the given name representing the entire graph.
   */
  void dump_dot(const std::string& fname);

  /**
   * Print some stats about memory usage.
   */
  void print_stats();

  /**
   * Perform recovery using the undo log.
   */ 
  void apply_undo_log();

  /* ---------------- index management ---------------- */
  
  /**
   * Create an index on the nodes table for all nodes with the given label and
   * the property. The resulting index allows lookup and range scans on values 
   * of this property.
   */
  index_id create_index(const std::string& node_label, const std::string& prop_name);

  /**
   * Return the id of the index for the given label/property combination. Raises an
   * exception of no corresponding index exists.
   */
  index_id get_index(const std::string& node_label, const std::string& prop_name);

  /**
   * Delete the given index.
   */
  void drop_index(const std::string& node_label, const std::string& prop_name);

  /**
   * Perform an index lookup on the given index for the given property value key. 
   * For each matching node the consumer function is called.
   */
  void index_lookup(index_id idx, uint64_t key, node_consumer_func consumer);

  void index_lookup(std::list<index_id> &idxs, uint64_t key, node_consumer_func consumer);

  /* ---------------- query support ---------------- */

  /**
   * Scans all nodes of the graph with the given label and invokes for each of
   * these nodes the consumer function.
   */
  void nodes_by_label(const std::string &label, node_consumer_func consumer);

  /**
   * Scans all nodes of the graph with any of the given labels and invokes for each of
   * these nodes the consumer function. This is for entity objects belonging to the same
   * abstract entity (e.g. Post and Comment are sub-classes of Message)
   */
  void nodes_by_label(const std::vector<std::string> &labels, node_consumer_func consumer);

  /**
   * Scans all nodes of the graph and invokes for each nodes the given consumer
   * function.
   */
  void nodes(node_consumer_func consumer);

  /**
   * Scans all nodes of the graph and invokes for each nodes the given consumer
   * function. The scan is performed in parallel by multiple threads via a
   * thread pool.
   */
  void parallel_nodes(node_consumer_func consumer);

#ifdef QOP_RECOVERY
  /**
   * Continues the scan from the given positions (checkpoints) and invokes the given consumer function for each node.
   */
  void parallel_nodes(node_consumer_func consumer, std::map<std::size_t, std::vector<std::size_t>> &range_map);
  void continue_parallel_nodes(std::map<std::size_t, std::size_t> &check_points, node_consumer_func consumer);
#endif
  /**
   * Scans all nodes which satisfy the given predicate on the property with
   * label pkey and invokes for each of these nodes the consumer function.
   */
  void nodes_where(const std::string &pkey, p_item::predicate_func pred,
                   node_consumer_func consumer);

  /**
   * Scans all relationships of the graph with the given label and invokes for
   * each of these relationship the consumer function.
   */
  void relationships_by_label(const std::string &label,
                              rship_consumer_func consumer);

  /**
   * Scans all FROM relationships recursivley starting from the the given node
   * and invokes for each of these relationships the consumer function. The
   * parameters min and max determine the minimum and maximum number of hops.
   */
  void foreach_variable_from_relationship_of_node(const node &n,
                                                  std::size_t min,
                                                  std::size_t max,
                                                  rship_consumer_func consumer);

  /**
   * Scans all FROM relationships with the given label code recursivley starting
   * from the the given node and invokes for each of these relationships the
   * consumer function. The parameters min and max determine the minimum and
   * maximum number of hops.
   */
  void foreach_variable_from_relationship_of_node(const node &n, dcode_t lcode,
                                                  std::size_t min,
                                                  std::size_t max,
                                                  rship_consumer_func consumer);

  /**
   * Scans all FROM relationships of the the given node and invokes for each of
   * these relationships the consumer function.
   */
  void foreach_from_relationship_of_node(const node &n,
                                         rship_consumer_func consumer);

  /**
   * Scans all TO relationships of the the given node and invokes for each of
   * these relationships the consumer function.
   */
  void foreach_to_relationship_of_node(const node &n,
                                       rship_consumer_func consumer);

  /**
   * Iterates over all FROM relationships of node n with the given label
   * and invokes for each of these relationships the consumer function.
   */
  void foreach_from_relationship_of_node(const node &n,
                                         const std::string &label,
                                         rship_consumer_func consumer);

  /**
   * Iterates over all FROM relationships of node n with the given label code
   * and invokes for each of these relationships the consumer function.
   */
  void foreach_from_relationship_of_node(const node &n, dcode_t lcode,
                                         rship_consumer_func consumer);

  /**
   * Iterates over all TO relationships of node n with the given label
   * and invokes for each of these relationships the consumer function.
   */
  void foreach_to_relationship_of_node(const node &n, const std::string &label,
                                       rship_consumer_func consumer);

  /**
   * Scans all TO relationships recursivley ending at the the given node
   * and invokes for each of these relationships the consumer function. The
   * parameters min and max determine the minimum and maximum number of hops.
   */
  void foreach_variable_to_relationship_of_node(const node &n, std::size_t min,
                                                std::size_t max,
                                                rship_consumer_func consumer);

  /**
   * Scans all TO relationships with the given label code recursivley ending at
   * the the given node and invokes for each of these relationships the consumer
   * function. The parameters min and max determine the minimum and maximum
   * number of hops.
   */
  void foreach_variable_to_relationship_of_node(const node &n, dcode_t lcode,
                                                std::size_t min,
                                                std::size_t max,
                                                rship_consumer_func consumer);

  /**
   * Iterates over all TO relationships of node n with the given label code
   * and invokes for each of these relationships the consumer function.
   */
  void foreach_to_relationship_of_node(const node &n, dcode_t lcode,
                                       rship_consumer_func consumer);

  /**
   * Checks whether the property with name pkey of the given node satisfies the
   * predicate.
   */
  bool is_node_property(const node &n, const std::string &pkey,
                        p_item::predicate_func pred);

  /**
   * Checks whether the property with encoded name pcode of the given node
   * satisfies the predicate.
   */
  bool is_node_property(const node &n, dcode_t pcode,
                        p_item::predicate_func pred);

  /**
   * Checks whether the property with name pkey of the given relationship
   * satisfies the predicate.
   */
  bool is_relationship_property(const relationship &r, const std::string &pkey,
                                p_item::predicate_func pred);

  /**
   * Checks whether the property with the encoded name of the given relationship
   * satisfies the predicate.
   */
  bool is_relationship_property(const relationship &r, dcode_t pcode,
                                p_item::predicate_func pred);

  /**
   * Return the node version from the dirty list that is valid for the
   * transaction identified by xid.
   */
  node &get_valid_node_version(node &n, xid_t xid);

#ifdef QOP_RECOVERY
  /**
   * Return the checkpoints for each touched chunked from the last query
   */
  const p_ptr<rec_map_t>& get_query_checkpoints() { return recovery_res_; }

  /**
   * Stores the tuple of a query into intermediate storage
   */
  std::vector<std::size_t> store_query_result(qr_tuple &qr, std::size_t chunk);

  /**
   * Stores the checkpoint of a chunk into intermediate storage
   */
  void store_iter(std::pair<std::size_t, std::size_t> iter_pos);

  /**
   * Recovers the stored intermediate results into a given list
   */
  void restore_results(std::list<qr_tuple> &result_list);

  /**
   * Returns the checkpoint positions to continue a failed query
   */
  std::map<std::size_t, std::size_t> restore_positions();

  const p_ptr<recovery_list>& get_recovery_results() { return recovery_results_; } 

  intermediate_result &ir_by_id(offset_t id);
  void tuple_by_ids(std::vector<offset_t> ids, qr_tuple &fwd_tpl);
  int get_stored_results();

  void clear_result_storage();
  void recover_scan_parallel(tuple_consumer_func consumer);
  const p_ptr<recovery_list>& get_rec_list() { return recovery_results_; }
#endif

/* ---------------- Analytics support ---------------- */

  /**
   * Converts the graph data to a CSR representation. If the CSR delta store is enabled 
   * and a CSR exists already for the graph, it updates the CSR so that it reflects the 
   * latest snapshot of the graph data. Otherwise, it builds the CSR by scanning the entire 
   * graph data.
   * The weight of a traversed relationship is calculated from the weight function. 
   * The bidirectional flag determines whether only outgoing relationships are considered 
   * (bidirectional = false) or both outgoing and incoming relationships (bidirectional = true).
   */
  void poseidon_to_csr(csr_arrays &csr, rship_weight weight_func, bool bidirectional = false);

  /**
   * Converts the graph data to a CSR representation by scanning the entire graph data.
   */
  void csr_build(csr_arrays &csr, rship_weight weight_func, bool bidirectional = false);

  /**
   * Converts the graph data to a CSR representation from a parallel scan of the entire graph data.
   */
  void parallel_csr_build(csr_arrays &csr, rship_weight weight_func, bool bidirectional = false);

#if defined CSR_DELTA && defined USE_TX
  /**
   * Updates the existing CSR using the appropriate deltas in the CSR delta store, such that it 
   * represents the latest snapshot of the graph data.
   */
  void csr_update_with_delta(csr_arrays &csr);

  /**
   * Returns a reference to the CSR delta store.
   */
  const p_ptr<delta_store>& csr_delta_store() { return delta_store_; }
#endif

  /**
   * Converts the graph data to a COO representation.
   * The weight of a traversed relationship is calculated from the weight function. 
   * The bidirectional flag determines whether only outgoing relationships are considered 
   * (bidirectional = false) or both outgoing and incoming relationships (bidirectional = true).
   */
  void poseidon_to_coo(edge_coords* edge_coordinates, float* edge_values, rship_weight weight_func, bool bidirectional = false);

private:
  friend struct scan_task;
  friend struct recover_scan;

  /**
   * Update the given node as the FROM node of the relationship. The relationship was already
   * created in the same transaction.
   */
  void update_from_node(transaction_ptr tx, node &n, relationship& r);

  /**
   * Update the given node as the TO node of the relationship. The relationship was already
   * created in the same transaction.
   */
  void update_to_node(transaction_ptr tx, node &n, relationship& r);

  /**
   * Handle the commit of a node from the dirty list.
   */
  void commit_dirty_node(transaction_ptr tx, node::id_t node_id);

  /**
   * Handle the commit of a relationship from the dirty list.
   */
  void commit_dirty_relationship(transaction_ptr tx, relationship::id_t rship_id);

  /**
   * Return the relationship version from the dirty list that is valid for the
   * transaction identified by xid.
   */
  relationship &get_valid_rship_version(relationship &r, xid_t xid);

  /**
   * Copy the properties from the dirty node to the nodes_ and properties_
   * tables.
   */
  void copy_properties(node &n, const dirty_node_ptr& dn);

  /**
   * Copy the properties from the dirty relationship to the rships_ and
   * properties_ tables.
   */
  void copy_properties(relationship &r, const dirty_rship_ptr& dr);

  /**
   * Check if the node still has valid FROM relationships.
   */
  bool has_valid_from_rships(node &n, xid_t tx);

  /**
   * Check if the node still has valid TO relationships.
   */  
  bool has_valid_to_rships(node &n, xid_t tx);

   /**
    * Perform garbage collection.
    */
  void vacuum(xid_t tx);

  std::string database_name_;
  p_ptr<node_list> nodes_; // the list of all nodes of the graph
  p_ptr<relationship_list>
      rships_; // the list of all relationships of the graph
  p_ptr<property_list>
      node_properties_;   // the list of all properties of nodes 
  p_ptr<property_list>
      rship_properties_;   // the list of all properties of relationships
  p_ptr<dict> dict_; // the dictionary used for string compression

  p_ptr<index_map> index_map_; // the list of all exisiting indexes
  p_ptr<pmlog> ulog_; // the undo log 

#ifdef QOP_RECOVERY
  p_ptr<recovery_list> recovery_results_; // stored intermediate tuples of a query
  p_ptr<rec_map_t> recovery_res_; // stored checkpoints of the chunks 
#endif
#if defined CSR_DELTA && defined USE_TX
  p_ptr<delta_store> delta_store_; // the CSR delta store
#endif
  /**
   * These member variables are volatile and have to be reinitialized
   * during startup.
   */
  std::map<xid_t, transaction_ptr>
      *active_tx_;   // the list of all active transactions
  std::mutex *m_;    // mutex for accessing active_tx_
  xid_t oldest_xid_; // timestamp of the oldest transaction
  std::mutex *gcm_;
  gc_list *garbage_;
};

using graph_db_ptr = p_ptr<graph_db>;

struct scan_task {
  using range = std::pair<std::size_t, std::size_t>;
  scan_task(graph_db *gdb, node_list &n, std::size_t first, std::size_t last,
	    graph_db::node_consumer_func c, transaction_ptr tp = nullptr, std::size_t start_pos = 0);

  void operator()();

  static void scan(transaction_ptr tx, graph_db *gdb, std::size_t first, std::size_t last, graph_db::node_consumer_func consumer);

  static std::function<void(transaction_ptr tx, graph_db *gdb, std::size_t first, std::size_t last, graph_db::node_consumer_func consumer)> callee_;

  graph_db *graph_db_;
  node_list &nodes_;
  range range_;
  graph_db::node_consumer_func consumer_;
  transaction_ptr tx_;
  std::size_t start_pos_;
};


#endif