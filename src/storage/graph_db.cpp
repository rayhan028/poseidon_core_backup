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

#include <boost/algorithm/string.hpp>

#include "graph_db.hpp"
#include "chunked_vec.hpp"
#include "parser.hpp"
#include "spdlog/spdlog.h"
#include <iostream>

#ifdef USE_PMDK
namespace nvm = pmem::obj;
#endif

graph_db::graph_db(const std::string &db_name) {
  nodes_ = p_make_ptr<node_list>();
  rships_ = p_make_ptr<relationship_list>();
  properties_ = p_make_ptr<property_list>();
  dict_ = p_make_ptr<dict>();
  index_map_ = p_make_ptr<index_map>();
  ulog_ = p_make_ptr<pmlog>();
  active_tx_ = new std::map<xid_t, transaction_ptr>();
  m_ = new std::mutex();
}

graph_db::~graph_db() {
  // we don't delete persistent objects here!
  for (auto tp : *active_tx_) {
    // TODO: abort all active transactions
  }
  delete active_tx_;
  delete m_;
#ifdef USE_TX
  current_transaction_.reset();
#endif
}

void graph_db::runtime_initialize() {
  nodes_->runtime_initialize();
  rships_->runtime_initialize();
  // make sure the dictionary is initialized
  dict_->initialize();
  // TODO: perform recovery using the undo log!!

  // recreate volatile objects: active_tx_ table and mutex
  active_tx_ = new std::map<xid_t, transaction_ptr>();
  oldest_xid_ = 0;
  m_ = new std::mutex();
}

transaction_ptr graph_db::begin_transaction() {
  if (current_transaction_)
    throw invalid_nested_transaction();
  auto tx = std::make_shared<transaction>();
  current_transaction_ = tx;
#ifdef USE_LOGGING
  tx->set_logid(ulog_->transaction_begin(tx->xid()));
#endif
#ifdef USE_TX
  std::lock_guard<std::mutex> guard(*m_);
  active_tx_->insert({tx->xid(), tx});
  // spdlog::info("begin transaction {}", tx->xid());
#endif
  return tx;
}

void graph_db::commit_dirty_node(transaction_ptr tx, node::id_t node_id) {
  auto xid = tx->xid();
 	   auto &n = nodes_->get(node_id);
	  /* A dirty object was just inserted, when add_node() or update_node() was executed.
	   * So there must be atleast one dirty version.
	   */

	  if (!n.has_dirty_versions()) {
      // TODO: in case of inserts perform undo using the log
		  throw transaction_abort();
	  }
	  // get the version of dirty object.
	  // Note: Dirty version are always put in front of the list.
	  // If that order is changed, then same order must be used during access.
	  if(const auto& dn {n.dirty_list->front()}; !dn->updated()) {
		  // CASE #1 = INSERT: we have added a new node to node_list, but its properties
		  // are stored in a dirty_node object in this case, we simply copy the
		  // properties to property_list and release the lock
		  // spdlog::info("commit INSERT transaction {}: copy properties", xid);
		  copy_properties(n, dn);
		  // set bts/cts
		  n.set_timestamps(xid, INF);
		  // we can already delete the object from the dirty version list
		  n.dirty_list->pop_front();
	  } else {
		  // case #2: we have updated a node, thus copy both properties
		  // and node version to the main tables
		  // spdlog::info("commit UPDATE transaction {}: copy properties", xid);
		  // update node (label)
#ifdef USE_LOGGING
      auto log_id = current_transaction_->logid(); 
#endif 

      if (dn->elem_.bts == dn->elem_.cts) {
        // CASE #2 = DELETE
        // spdlog::info("COMMIT DELETE: [{},{}]", short_ts(dn->elem_.bts), short_ts(dn->elem_.cts));
#ifdef USE_LOGGING
        log_del_record rec { pmlog::log_delete, pmlog::log_node, node_id };
        ulog_->append(log_id, static_cast<void *>(&rec), sizeof(log_del_record));       
        // TODO: log property delete
#endif
        // Because there might be an active transaction which still needs the object
        // we cannot delete the node, yet. However, we set the bts and cts accordingly.
		    n.set_cts(xid);
        // spdlog::info("===> COMMIT DELETE: #{}: [{},{}]", n.id(), short_ts(n.bts),short_ts(n.cts));
		    // we can already delete the object from the dirty version list
		    n.dirty_list->pop_front();
		    // release the lock of the old version.
		    n.dirty_list->front()->elem_.unlock();
        // we can delete properties because the old values are still in the dirty list
        properties_->remove_properties(n.property_list);
      }
      else {
        // CASE #3 = UPDATE
#ifdef USE_LOGGING
        // create and append a log_upd_node_record BEFORE we copy the properties and override the label
        log_upd_node_record rec{ pmlog::log_update, pmlog::log_node, node_id, 
          n.node_label, n.from_rship_list, n.to_rship_list, n.property_list};
        ulog_->append(log_id, static_cast<void *>(&rec), sizeof(log_upd_node_record));
#endif
		    n.node_label = dn->elem_.node_label;
		    copy_properties(n, dn);
		    // spdlog::info("COMMIT UPDATE: set new={},{} @{}", xid, INF, n.id());
		    n.set_timestamps(xid, INF);
		    /// spdlog::info("COMMIT UPDATE: set old.cts={} @{}", xid,
		    ///             (unsigned long)&(dn->node_));
		    // we can already delete the object from the dirty version list
		    n.dirty_list->pop_front();
		    // release the lock of the old version.
		    n.dirty_list->front()->elem_.unlock();
	    }
    }
	  // finally, release the lock of the persistent object and initiate the
	  // garbage collection
	  n.unlock();
	  n.gc(oldest_xid_);
  }

void graph_db::commit_dirty_relationship(transaction_ptr tx, relationship::id_t rel_id) {
  auto xid = tx->xid();
	auto& r = rships_->get(rel_id);
	  /* A dirty object was just inserted, when add_relation() or update_relation() was executed.
	   * So there must be at least one dirty version.
	   */
	  if (!r.has_dirty_versions()) {
      // TODO: in case of inserts perform undo using the log
		  throw transaction_abort();
	  }
	  // get the version of dirty object.
      // Note: Dirty versions are always put in front of the list.
      // If that order is changed, then same order must be used during access.
	  if(const auto& dr {r.dirty_list->front()}; !dr->updated()) {
		  // CASE #1 = INSERT: we have added a new relationship to relationship_list, but
		  // its properties are stored in a dirty_rship object in this case, we
		  // simply copy the properties to property_list and release the lock
		  copy_properties(r, dr);
		  // set bts/cts
		  r.set_timestamps(xid, INF);
		  // we can already delete the object from the dirty version list
		  r.dirty_list->pop_front();
	  } else {
		  // case #2: we have updated a relationship, thus copy both properties
		  // and relationship version to the main tables
		  // update relationship (label)
#ifdef USE_LOGGING
      auto log_id = current_transaction_->logid(); 
#endif 
      if (dr->elem_.bts == dr->elem_.cts) {
        // CASE #2 = DELETE
        // spdlog::info("COMMIT DELETE: {}, {}", dr->elem_.bts, dr->elem_.cts);
#ifdef USE_LOGGING
        log_del_record rec { pmlog::log_delete, pmlog::log_rship, rel_id };
        ulog_->append(log_id, static_cast<void *>(&rec), sizeof(log_del_record));       
        // TODO: log property delete
#endif
        // Because there might be an active transaction which still needs the object
        // we cannot delete the relationship, yet. However, we set the cts accordingly.
		    r.set_cts(xid);
        // TODO: how to handle corresponding node entry, i.e. the from/to_rship_list ????

		    // we can already delete the object from the dirty version list
		    r.dirty_list->pop_front();
		    // release the lock of the old version.
		    r.dirty_list->front()->elem_.unlock();
        properties_->remove_properties(r.property_list);
      }
      else {
        // CASE #3 = UPDATE
#ifdef USE_LOGGING
      // create and append a log_upd_rship_record BEFORE we copy the properties and override the label
      auto log_id = current_transaction_->logid(); 
 
      log_upd_rship_record rec{ pmlog::log_update, pmlog::log_rship, rel_id, 
          r.rship_label, r.src_node, r.dest_node, r.next_src_rship, r.next_dest_rship };

      ulog_->append(log_id, static_cast<void *>(&rec), sizeof(log_upd_rship_record));
#endif
		  r.rship_label = dr->elem_.rship_label;
		  copy_properties(r, dr);
		  r.set_timestamps(xid, INF);
		  // we can already delete the dirty object from the dirty version list
		  r.dirty_list->pop_front();
		  // release the lock of the older version.
		  r.dirty_list->front()->elem_.unlock();
	  }
  }
  // finally, release the lock of the persistent object and initiate the
	// garbage collection
	r.unlock();
	r.gc(oldest_xid_);
}

bool graph_db::commit_transaction() {
#ifdef USE_TX
  check_tx_context();
  auto tx = current_transaction();
  auto xid = tx->xid();

  {
    // remove transaction from the active transaction set
    std::lock_guard<std::mutex> guard(*m_);
    active_tx_->erase(xid);
    oldest_xid_ = !active_tx_->empty() ? active_tx_->begin()->first : xid;
  }

  // process dirty_nodes list
  for  (auto node_id : tx->dirty_nodes()) {
    commit_dirty_node(tx, node_id);
#if 0
 	   auto &n = nodes_->get(node_id);
	  /* A dirty object was just inserted, when add_node() or update_node() was executed.
	   * So there must be atleast one dirty version.
	   */

	  if (!n.has_dirty_versions()) {
      // TODO: in case of inserts perform undo using the log
		  throw transaction_abort();
	  }
	  // get the version of dirty object.
	  // Note: Dirty version are always put in front of the list.
	  // If that order is changed, then same order must be used during access.
	  if(const auto& dn {n.dirty_list->front()}; !dn->updated()) {
		  // case #1: we have added a new node to node_list, but its properties
		  // are stored in a dirty_node object in this case, we simply copy the
		  // properties to property_list and release the lock
		  // spdlog::info("commit INSERT transaction {}: copy properties", xid);
		  copy_properties(n, dn);
		  // set bts/cts
		  n.set_timestamps(xid, INF);
		  // we can already delete the object from the dirty version list
		  n.dirty_list->pop_front();
	  } else {
		  // case #2: we have updated a node, thus copy both properties
		  // and node version to the main tables
		  /// spdlog::info("commit UPDATE transaction {}: copy properties", xid);
		  // update node (label)
#ifdef USE_LOGGING
      auto log_id = current_transaction_->logid(); 
#endif 

      if (dn->elem_.bts == dn->elem_.cts) {
        // spdlog::info("COMMIT DELETE: {}, {}", dn->elem_.bts, dn->elem_.cts);
#ifdef USE_LOGGING
        log_del_record rec { pmlog::log_delete, pmlog::log_node, node_id };
        ulog_->append(log_id, static_cast<void *>(&rec), sizeof(log_del_record));       
        // TODO: log property delete
#endif
        // Because there might be an active transaction which still needs the object
        // we cannot delete the node, yet. However, we set the cts accordingly.
		    n.set_cts(xid);
		    // we can already delete the object from the dirty version list
		    n.dirty_list->pop_front();
		    // release the lock of the old version.
		    n.dirty_list->front()->elem_.unlock();
      }
      else {
#ifdef USE_LOGGING
        // create and append a log_upd_node_record BEFORE we copy the properties and override the label
        log_upd_node_record rec{ pmlog::log_update, pmlog::log_node, node_id, 
          n.node_label, n.from_rship_list, n.to_rship_list, n.property_list};
        ulog_->append(log_id, static_cast<void *>(&rec), sizeof(log_upd_node_record));
#endif
		    n.node_label = dn->elem_.node_label;
		    copy_properties(n, dn);
		    /// spdlog::info("COMMIT UPDATE: set new={},{} @{}", xid, INF,
		    ///             (unsigned long)nptr);
		    n.set_timestamps(xid, INF);
		    /// spdlog::info("COMMIT UPDATE: set old.cts={} @{}", xid,
		    ///             (unsigned long)&(dn->node_));
		    // we can already delete the object from the dirty version list
		    n.dirty_list->pop_front();
		    // release the lock of the old version.
		    n.dirty_list->front()->elem_.unlock();
	    }
    }
	  // finally, release the lock of the persistent object and initiate the
	  // garbage collection
	  n.unlock();
	  n.gc(oldest_xid_);
  #endif 
  }
  // process dirty_rships list
 for  (auto rel_id : tx->dirty_relationships())  {
    commit_dirty_relationship(tx, rel_id);
#if 0
	auto& r = rships_->get(rel_id);
	  /* A dirty object was just inserted, when add_relation() or update_relation() was executed.
	   * So there must be at least one dirty version.
	   */
	  if (!r.has_dirty_versions()) {
      // TODO: in case of inserts perform undo using the log
		  throw transaction_abort();
	  }
	  // get the version of dirty object.
      // Note: Dirty versions are always put in front of the list.
      // If that order is changed, then same order must be used during access.
	  if(const auto& dr {r.dirty_list->front()}; !dr->updated()) {
		  // case #1: we have added a new relationship to relationship_list, but
		  // its properties are stored in a dirty_rship object in this case, we
		  // simply copy the properties to property_list and release the lock
		  copy_properties(r, dr);
		  // set bts/cts
		  r.set_timestamps(xid, INF);
		  // we can already delete the object from the dirty version list
		  r.dirty_list->pop_front();
	  } else {
		  // case #2: we have updated a relationship, thus copy both properties
		  // and relationship version to the main tables
		  // update relationship (label)
#ifdef USE_LOGGING
      // create and append a log_upd_rship_record BEFORE we copy the properties and override the label
      auto log_id = current_transaction_->logid(); 
 
      log_upd_rship_record rec{ pmlog::log_update, pmlog::log_rship, rel_id, 
          r.rship_label, r.src_node, r.dest_node, r.next_src_rship, r.next_dest_rship };

      ulog_->append(log_id, static_cast<void *>(&rec), sizeof(log_upd_rship_record));
#endif
		  r.rship_label = dr->elem_.rship_label;
		  copy_properties(r, dr);
		  r.set_timestamps(xid, INF);
		  // we can already delete the dirty object from the dirty version list
		  r.dirty_list->pop_front();
		  // release the lock of the older version.
		  r.dirty_list->front()->elem_.unlock();
	  }
	  // finally, release the lock of the persistent object and initiate the
	  // garbage collection
	  r.unlock();
	  r.gc(oldest_xid_);
#endif
  }

#ifdef USE_LOGGING
  ulog_->transaction_end(current_transaction_->logid());
#endif
  current_transaction_.reset();
#endif
  return true;
}

bool graph_db::abort_transaction() {
#ifdef USE_TX
  check_tx_context();
  auto tx = current_transaction();
  auto xid = tx->xid();

  {
    // remove transaction from the active transaction set
    std::lock_guard<std::mutex> guard(*m_);
    active_tx_->erase(xid);
    oldest_xid_ = !active_tx_->empty() ? active_tx_->begin()->first : xid;
  }
  // if we have added a node or relationship then
  // we have to delete it again from the nodes_ or rships_ tables.
   for (auto node_id  : tx->dirty_nodes()) {
    auto &n = nodes_->get(node_id);
    bool was_updated = n.updated_in_version(xid);
    n.remove_dirty_version(xid);
    n.unlock();
    // remove the node only if it was added but not updated!
    if (!was_updated) {
      // spdlog::info("remove dirty node for tx {} and node #{}", xid, node_id);
      nodes_->remove(node_id);
    }
  }

  for (auto rel_id  : tx->dirty_relationships()) {
    /// spdlog::info("remove dirty relationship for tx {}", xid);
    auto &r = rships_->get(rel_id );
    bool was_updated = r.updated_in_version(xid);
    r.remove_dirty_version(xid);
    r.unlock();
    // remove the relationship only if it was added but not updated!
    if (!was_updated) {
      rships_->remove(rel_id);
    }
  }
#endif
#ifdef USE_LOGGING
  ulog_->transaction_end(current_transaction_->logid());
#endif
  current_transaction_.reset();
  return true;
}

void graph_db::print_mem_usage() {
  std::size_t nchunks =
      nodes_->num_chunks() + rships_->num_chunks() + properties_->num_chunks();
  std::cout << "Memory usage: " << nchunks * DEFAULT_CHUNK_SIZE / 1024 << " KiB"
            << std::endl;
}

node::id_t graph_db::add_node(const std::string &label,
                              const properties_t &props, bool append_only) {
#ifdef USE_TX
  check_tx_context();
  xid_t txid = current_transaction()->xid();
#else
  xid_t txid = 0;
#endif
  auto type_code = dict_->insert(label);
#ifdef USE_LOGGING
  // TODO: create and append a log_ins_record BEFORE the node table is modified
  auto log_id = current_transaction_->logid(); 
  auto cb = [log_id, this](offset_t n_id) {
    log_ins_record rec{ pmlog::log_insert, pmlog::log_node, n_id};
    ulog_->append(log_id, static_cast<void *>(&rec), sizeof(log_ins_record));
  };
  auto node_id = append_only ? nodes_->append(node(type_code), txid, cb)
                             : nodes_->insert(node(type_code), txid, cb);
#else
  auto node_id = append_only ? nodes_->append(node(type_code), txid)
                             : nodes_->insert(node(type_code), txid);
#endif
  // we need the node object not only the id
  auto &n = nodes_->get(node_id);

#ifdef USE_TX
  // handle properties
  const auto dirty_list = properties_->build_dirty_property_list(props, dict_);
  const auto &dv = n.add_dirty_version(
      std::make_unique<dirty_node>(n, dirty_list, false /* insert */));
  dv->elem_.set_dirty();

  current_transaction()->add_dirty_node(node_id);
#else
  // save properties
  if (!props.empty()) {
    property_set::id_t pid =
        append_only ? properties_->append_node_properties(node_id, props, dict_)
                    : properties_->add_node_properties(node_id, props, dict_);
    n.property_list = pid;
  }
#endif

  return node_id;
}

relationship::id_t graph_db::add_relationship(node::id_t from_id,
                                              node::id_t to_id,
                                              const std::string &label,
                                              const properties_t &props,
                                              bool append_only) {
#ifdef USE_TX
  check_tx_context();
  xid_t txid = current_transaction()->xid();
#else
  xid_t txid = 0;
#endif
  auto &from_node = nodes_->get(from_id);
  auto &to_node = nodes_->get(to_id);
  auto type_code = dict_->insert(label);
#ifdef USE_LOGGING
  // create and append a log_ins_record BEFORE the rship table is modified
  auto log_id = current_transaction_->logid(); 
  auto cb = [log_id, this](offset_t r_id) {
    log_ins_record rec{ pmlog::log_insert, pmlog::log_rship, r_id};
    ulog_->append(log_id, static_cast<void *>(&rec), sizeof(log_ins_record));
  };
  auto rid =
      append_only
          ? rships_->append(relationship(type_code, from_id, to_id), txid, cb)
          : rships_->insert(relationship(type_code, from_id, to_id), txid, cb);
#else
  auto rid =
      append_only
          ? rships_->append(relationship(type_code, from_id, to_id), txid)
          : rships_->insert(relationship(type_code, from_id, to_id), txid);
#endif
  auto &r = rships_->get(rid);

#ifdef USE_TX
  const auto dirty_list = properties_->build_dirty_property_list(props, dict_);
  const auto &rv = r.add_dirty_version(
      std::make_unique<dirty_rship>(r, dirty_list, false /* insert */));
  rv->elem_.set_dirty();

  current_transaction()->add_dirty_relationship(rid);

#else
  // save properties
  if (!props.empty()) {
    property_set::id_t pid =
        append_only
            ? properties_->append_relationship_properties(rid, props, dict_)
            : properties_->add_relationship_properties(rid, props, dict_);
    r.property_list = pid;
  }
#endif
  // update the list of relationships for each of both nodes
  if (from_node.from_rship_list == UNKNOWN)
    from_node.from_rship_list = rid;
  else {
    r.next_src_rship = from_node.from_rship_list;
    from_node.from_rship_list = rid;
  }

  if (to_node.to_rship_list == UNKNOWN)
    to_node.to_rship_list = rid;
  else {
    r.next_dest_rship = to_node.to_rship_list;
    to_node.to_rship_list = rid;
  }
  return rid;
}

node &graph_db::get_valid_node_version(node &n, xid_t xid) {
  if (n.is_locked_by(xid)) {
    // spdlog::info("[tx {}] node #{} is locked by {}", short_ts(xid), n.id(), short_ts(n.txn_id));
    // because the node is locked we know that it was already updated by us
    // and we should look for the dirty object containing the new values
    assert(n.has_dirty_versions());
    // TODO: check if we have deleted it 
    return n.find_valid_version(xid)->elem_;
  }
  // or (2) is not locked and xid is in [bts,cts]
  if (!n.is_locked()) {
    // spdlog::info("node_by_id: node #{} is unlocked: [{}, {}] <=> {}", n.id(),
    //             n.bts, n.cts, xid);
    return n.is_valid(xid) ? n : n.find_valid_version(xid)->elem_;
  }

  // or (3) node is locked by another transaction
  else {
    // spdlog::info("node #{} is locked by another tx in {}", n.id(), xid);
    // dump();
    // try to find a valid version which is not locked
    auto &nv = n.find_valid_version(xid)->elem_;
    if (!nv.is_locked() || nv.is_locked_by(xid))
      return nv;
    throw transaction_abort();
  }
  throw unknown_id();
}

relationship &graph_db::get_valid_rship_version(relationship &r, xid_t xid) {
  // we can read relationship r if
  // (1) we own the lock
  if (r.is_locked_by(xid)) {
    // because the relationship is locked we know that it was already updated
    // by us and we should look for the dirty object containing the new values
    assert(r.has_dirty_versions());
    return r.find_valid_version(xid)->elem_;
  }
  // or (2) is unlocked and xid is in [bts,cts]
  if (!r.is_locked()) {
    return r.is_valid(xid) ? r : r.find_valid_version(xid)->elem_;
  }

  // relationship is locked by another transaction -> abort!!
  if (r.is_locked())  {
    // try to find a valid version which is not locked
    auto &rv = r.find_valid_version(xid)->elem_;
    if (!rv.is_locked() || rv.is_locked_by(xid))
      return rv;
    // spdlog::info("relationship #{} is locked by another tx", n.id());
    throw transaction_abort();
  }
  throw unknown_id();
}

node &graph_db::node_by_id(node::id_t id) {
#ifdef USE_TX
  check_tx_context();
  auto xid = current_transaction()->xid();
  /// spdlog::info("[{}] try to fetch node #{}", xid, id);
  auto &n = nodes_->get(id);
  n.set_rts(xid);
  return get_valid_node_version(n, xid);
#else
  return nodes_->get(id);
#endif
}

relationship &graph_db::rship_by_id(relationship::id_t id) {
#ifdef USE_TX
  check_tx_context();
  auto xid = current_transaction()->xid();
  auto &r = rships_->get(id);
  r.set_rts(xid);
  return get_valid_rship_version(r, xid);
#else
  return rships_->get(id);
#endif
}

node_description graph_db::get_node_description(const node &n) {
  std::string label; 
  properties_t props;
#ifdef USE_TX
  check_tx_context();
  auto xid = current_transaction()->xid();
  // spdlog::info("get_node_description @{}", (unsigned long)&n);
  if (!n.has_dirty_versions()) {
    // the simple case: no concurrent transactions are active and
    // we can get the properties from the properties_ table
     // spdlog::info("get_node_description - not dirty");
    props = properties_->all_properties(n.property_list, dict_);
    label = dict_->lookup_code(n.node_label);
  }
  else {
    //spdlog::info("tx #{}: get_node_description - is_dirty={} - is_valid={}", 
    //  xid, n.is_dirty(), n.is_valid(xid));
    // dump();
    // otherwise there are two options:
    // (1) we still can get the data from the properties_ table
    if (!n.is_dirty() && n.is_valid(xid)) {
      props = properties_->all_properties(n.property_list, dict_);
      label = dict_->lookup_code(n.node_label);
    }
    else {
      // (2) we get the property values directly from the p_item list
      const auto& dn = n.find_valid_version(xid);
      // spdlog::info("got dirty version: @{}", (unsigned long)&(dn->elem_));
      // dump();
      props = properties_->build_properties_from_pitems(dn->properties_, dict_);
      label = dict_->lookup_code(dn->elem_.node_label);
    }
  }
#else
  props = properties_->all_properties(n.property_list, dict_);
  label = dict_->lookup_code(n.node_label);
#endif
  return node_description{n.id(), label, props};
}

rship_description graph_db::get_rship_description(const relationship &r) {
  std::string label;
  properties_t props;
#ifdef USE_TX
  check_tx_context();
  auto xid = current_transaction()->xid();
  if (!r.has_dirty_versions()) {
   // the simple case: no concurrent transactions are active and
    // we can get the properties from the properties_ table
     // spdlog::info("get_node_description - not dirty");
    props = properties_->all_properties(r.property_list, dict_);
    label = dict_->lookup_code(r.rship_label);
  }
  else {
    // otherwise there are two options:
    // (1) we still can get the data from the properties_ table
    if (!r.is_dirty() && r.is_valid(xid)) {
      props = properties_->all_properties(r.property_list, dict_);
      label = dict_->lookup_code(r.rship_label);
    }
    else {
      // (2) we get the property values directly from the p_item list
      const auto& dr = r.find_valid_version(xid);
      // spdlog::info("got dirty version: @{}", (unsigned long)&(dn->elem_));
      // dump();
      props = properties_->build_properties_from_pitems(dr->properties_, dict_);
      label = dict_->lookup_code(dr->elem_.rship_label);
    }
  }
#else
  props = properties_->all_properties(r.property_list, dict_);
  label = dict_->lookup_code(r.rship_label);
#endif
  return rship_description{r.id(), r.from_node_id(), r.to_node_id(),
                           label, props};
}

const char *graph_db::get_relationship_label(const relationship &r) {
  return dict_->lookup_code(r.rship_label);
}

void graph_db::update_node(node &n, const properties_t &props,
                           const std::string &label) {
  auto lc = label.empty() ? 0 : dict_->insert(label);
#ifdef USE_TX
  // acquire lock and create a dirty object
  check_tx_context();
  xid_t txid = current_transaction()->xid();

  // if we don't own the lock and cannot acquire a lock, we have to abort
  if (!n.is_locked_by(txid) && !n.try_lock(txid))
    throw transaction_abort();

  // make sure we don't overwrite an object that was read by 
  // a more recent transaction
  if (n.rts > txid)
   throw transaction_abort();

  bool first_update = true;
  if (n.has_dirty_versions()) {
    // let's look for a version which we already have created in this transaction
    try {
      auto& dn = n.get_dirty_version(txid);
      // apply update to dn
      properties_->apply_updates(dn->properties_, props, dict_);
      if (lc > 0)
        dn->elem_.node_label = lc;
      first_update = false;
    } catch (unknown_id& exc) { /* do nothing */ }
  }

  if (first_update) {
  // first, we make a copy of the original node which is stored in
  // the dirty list
  std::list<p_item> pitems =
      properties_->build_dirty_property_list( /* n.id(),*/ n.property_list);
  // cts is set to txid
  const auto& oldv = n.add_dirty_version(std::make_unique<dirty_node>(n, pitems));
  oldv->elem_.set_timestamps(n.bts, txid);
  oldv->elem_.set_dirty();
  // but unlock it for readers
  oldv->elem_.unlock();

  // ... and create another copy as the new version
  pitems = properties_->apply_updates(pitems, props, dict_);
  const auto &newv = n.add_dirty_version(std::make_unique<dirty_node>(n, pitems));
  newv->elem_.set_timestamps(txid, INF);
  newv->elem_.set_dirty();
  if (lc > 0)
    newv->elem_.node_label = lc;

  current_transaction()->add_dirty_node(n.id());
  }
#else
  if (lc > 0)
    n.node_label = lc;
  auto rid =
      properties_->update_properties(n.id(), n.property_list, props, dict_);
  if (rid != n.property_list) {
    auto &n2 = nodes_->get(n.id());
    n2.property_list = rid;
  }
#endif
}

void graph_db::update_relationship(relationship &r, const properties_t &props,
                                   const std::string &label) {
  auto lc = label.empty() ? 0 : dict_->insert(label);
#ifdef USE_TX
  // acquire lock and create a dirty object
  check_tx_context();
  xid_t txid = current_transaction()->xid();

  // if we don't own the lock and cannot acquire a lock, we have to abort
  if (!r.is_locked_by(txid) && !r.try_lock(txid))
    throw transaction_abort();

  // make sure we don't overwrite an object that was read by 
  // a more recent transaction
  if (r.rts > txid)
   throw transaction_abort();

  bool first_update = true;
  if (r.has_dirty_versions()) {
    // let's look for a version which we already have created in this transaction
    try {
      auto& dr = r.get_dirty_version(txid);
      // apply update to dn
      properties_->apply_updates(dr->properties_, props, dict_);
      if (lc > 0)
        dr->elem_.rship_label = lc;
      first_update = false;
    } catch (unknown_id& exc) { /* do nothing */ }
  }

  if (first_update) {
  // first, we make a copy of the original node which is stored in
  // the dirty list
  std::list<p_item> pitems =
      properties_->build_dirty_property_list(/*r.id(),*/ r.property_list);
  // cts is set to txid
  const auto& oldv = r.add_dirty_version(std::make_unique<dirty_rship>(r, pitems));
  oldv->elem_.set_timestamps(r.bts, txid);
  oldv->elem_.set_dirty();
  // but unlock it for readers
  oldv->elem_.unlock();

  // ... and create another copy as the new version
  pitems = properties_->apply_updates(pitems, props, dict_);
  const auto& newv = r.add_dirty_version(std::make_unique<dirty_rship>(r, pitems));
  newv->elem_.set_timestamps(txid, INF);
  newv->elem_.set_dirty();
  if (lc > 0)
    newv->elem_.rship_label = lc;

  current_transaction()->add_dirty_relationship(r.id());
  }
#else
  if (lc > 0)
    r.rship_label = lc;
  auto rid =
      properties_->update_properties(r.id(), r.property_list, props, dict_);
  if (rid != r.property_list) {
    auto &r2 = rships_->get(r.id());
    r2.property_list = rid;
  }
#endif
}

void graph_db::delete_node(node::id_t id) {
 #ifdef USE_TX
  // acquire lock and create a dirty object
  check_tx_context();
  xid_t txid = current_transaction()->xid();

  auto &n = this->node_by_id(id);

  // if we don't own the lock and cannot acquire a lock, we have to abort
  if (!n.is_locked_by(txid) && !n.try_lock(txid))
    throw transaction_abort();

  // make sure we don't overwrite an object that was read by 
  // a more recent transaction
  if (n.rts > txid)
   throw transaction_abort();

  // first, we make a copy of the original node which is stored in
  // the dirty list
  std::list<p_item> pitems =
      properties_->build_dirty_property_list( /* n.id(),*/ n.property_list);
  // cts is set to txid
  const auto& oldv = n.add_dirty_version(std::make_unique<dirty_node>(n, pitems));
  oldv->elem_.set_timestamps(n.bts, txid);
  oldv->elem_.set_dirty();
  // but unlock it for readers
  oldv->elem_.unlock();

  // ... and create another copy as the new deleted version
  const auto &newv = n.add_dirty_version(std::make_unique<dirty_node>(n, std::list<p_item>(), true /* updated */));
  newv->elem_.set_timestamps(txid, txid);
  newv->elem_.set_dirty();

  current_transaction()->add_dirty_node(n.id());
  spdlog::info("delete_node: {}", n.id());
#else 
  auto &n = this->node_by_id(id);
  // delete the node properties
  properties_->remove_properties(n.property_list);
  // delete the node object
  nodes_->remove(id);
#endif
}

void graph_db::detach_delete_node(node::id_t id) {
  // TODO
}

void graph_db::delete_relationship(relationship::id_t id) {
  auto &r = this->rship_by_id(id);

  // delete the relationship properties
  properties_->remove_properties(r.property_list);

  // delete the relationship object
  rships_->remove(id);
}

const char *graph_db::get_string(dcode_t c) { return dict_->lookup_code(c); }

dcode_t graph_db::get_code(const std::string &s) {
  return dict_->lookup_string(s);
}

void graph_db::dump() {
  nodes_->dump();
  rships_->dump();
}

void graph_db::copy_properties(node &n, const dirty_node_ptr& dn) {
  if (dn->properties_.empty())
    return;

  property_set::id_t pid;
  if (dn->updated()) {
#ifdef USE_LOGGING
    // create and append a log_upd_property_record
    auto log_id = current_transaction_->logid();
    auto node_id = n.id(); 
    auto cb = [log_id, node_id, this](offset_t oid, property_set::p_item_list& items, offset_t next) {
      log_upd_property_record rec{ pmlog::log_update, pmlog::log_property,
            oid, 0, items, next, node_id };
      ulog_->append(log_id, static_cast<void *>(&rec), sizeof(log_upd_property_record));
    };
    properties_->foreach_property_set(n.id(), cb);
#endif
    // we have to update the properties
    pid = properties_->update_pitems(n.id(), n.property_list, dn->properties_,
                                     dict_, true /* Node */);
  } else {
    // the node was newly added - we have to add the properties
    // to the properties_ table
    // But we should log this. Otherwise, the slot might get be lost in
    // case of system failure.
    auto log_id = current_transaction_->logid(); 
    auto cb = [log_id, this](offset_t p_id) {
      log_ins_record rec{ pmlog::log_insert, pmlog::log_property, p_id};
      ulog_->append(log_id, static_cast<void *>(&rec), sizeof(log_ins_record));
    };
    pid = properties_->add_pitems(n.id(), dn->properties_, dict_,
                                  true /* Node */, cb);
  }
  n.property_list = pid;
}

void graph_db::copy_properties(relationship &r, const dirty_rship_ptr& dr) {
  if (dr->properties_.empty())
    return;

  property_set::id_t pid;
  if (dr->updated()) {
#ifdef USE_LOGGING
    // create and append a log_upd_property_record
    auto log_id = current_transaction_->logid();
    auto rship_id = r.id(); 
    auto cb = [log_id, rship_id, this](offset_t oid, property_set::p_item_list& items, offset_t next) {
      log_upd_property_record rec{ pmlog::log_update, pmlog::log_property,
            oid, 0, items, next, rship_id };
      ulog_->append(log_id, static_cast<void *>(&rec), sizeof(log_upd_property_record));
    };
    properties_->foreach_property_set(r.id(), cb);
#endif
    // we have to update the properties
    pid = properties_->update_pitems(r.id(), r.property_list, dr->properties_,
                                     dict_, false /* Relationship */);
    /// spdlog::info("update node -> set properties to {}", pid);
  } else {
    // the relationship was newly added - we have to add the properties
    // to the properties_ table
    // But we should log this. Otherwise, the slot might get be lost in
    // case of system failure.
    auto log_id = current_transaction_->logid(); 
    auto cb = [log_id, this](offset_t p_id) {
      log_ins_record rec{ pmlog::log_insert, pmlog::log_property, p_id};
      ulog_->append(log_id, static_cast<void *>(&rec), sizeof(log_ins_record));
    };
    pid = properties_->add_pitems(r.id(), dr->properties_, dict_,
                                  false /* Relationship */, cb);
  }
  r.property_list = pid;
}
