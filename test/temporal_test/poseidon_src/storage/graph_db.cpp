#include <boost/algorithm/string.hpp>
#include <filesystem>
#include "graph_db.hpp"
#include "vec.hpp"
#include "spdlog/spdlog.h"
#include <iostream>
#include <stdio.h>
#include <set>
#include <variant>
#include "history_types.hpp"
#ifdef USE_PMDK
namespace nvm = pmem::obj;

#define UNDO_CB cb
#else
#define UNDO_CB nullptr
#endif

void graph_db::destroy(graph_db_ptr gp) {
#ifdef USE_PFILES
  std::filesystem::path path_obj(gp->database_name_);
  if (std::filesystem::exists(path_obj))
    std::filesystem::remove_all(path_obj);
#endif
}

void graph_db::prepare_files(const std::string &pool_path, const std::string &pfx) {
#ifdef USE_PFILES
  spdlog::debug("graph_db: prepare files {} / {}", pool_path, pfx);
  std::filesystem::path path_obj(pool_path);
  path_obj /= pfx;
  if (!std::filesystem::exists(path_obj))
    std::filesystem::create_directory(path_obj);

  std::string prefix = path_obj.string() + "/";

  spdlog::debug("graph_db: prepare files in '{}'", prefix);

  node_file_ = std::make_shared<paged_file>();
  node_file_->open(prefix + "nodes.db", NODE_FILE_ID);
  bpool_.register_file(NODE_FILE_ID, node_file_);

  rship_file_ = std::make_shared<paged_file>();
  rship_file_->open(prefix + "rships.db", RSHIP_FILE_ID);
  bpool_.register_file(RSHIP_FILE_ID, rship_file_);

  nprops_file_ = std::make_shared<paged_file>();
  nprops_file_->open(prefix + "nprops.db", NPROPS_FILE_ID);
  bpool_.register_file(NPROPS_FILE_ID, nprops_file_);

  rprops_file_ = std::make_shared<paged_file>();
  rprops_file_->open(prefix + "rprops.db", RPROPS_FILE_ID);
  bpool_.register_file(RPROPS_FILE_ID, rprops_file_);

  // ADDITION FOR HISTORY STORAGE (IDs 10 & 11)
   auto h_node_file = std::make_shared<paged_file>();
   h_node_file->open(prefix + "history_nodes.db", HISTORY_NODE_FILE_ID);
   bpool_.register_file(HISTORY_NODE_FILE_ID, h_node_file);
   index_files_.push_back(h_node_file);

   auto h_rship_file = std::make_shared<paged_file>();
   h_rship_file->open(prefix + "history_rships.db", HISTORY_RSHIP_FILE_ID);
   bpool_.register_file(HISTORY_RSHIP_FILE_ID, h_rship_file);
   index_files_.push_back(h_rship_file);
    // NEW BLOCK <<<
   auto h_index_file = std::make_shared<paged_file>();
   h_index_file->open(prefix + "history_index.db", HISTORY_INDEX_FILE_ID);
   bpool_.register_file(HISTORY_INDEX_FILE_ID, h_index_file);
   //index_files_.push_back(h_index_file);

  dict_ = p_make_ptr<dict>(bpool_, prefix);

#endif
}

// constructor
graph_db::graph_db(const std::string &db_name, const std::string& pool_path, std::size_t bpool_size)
  : database_name_(db_name),
#ifndef USE_PFILES
    bpool_(0)
#else
    bpool_(bpool_size == 0 ? DEFAULT_BUFFER_SIZE : bpool_size)
#endif
{
#ifdef USE_PMDK
  nodes_ = p_make_ptr<node_list<nvm_chunked_vec> >();
  rships_ = p_make_ptr<relationship_list<nvm_chunked_vec> >();
  node_properties_ = p_make_ptr<property_list<nvm_chunked_vec> >();
  rship_properties_ = p_make_ptr<property_list<nvm_chunked_vec> >();
  dict_ = p_make_ptr<dict>();
  index_map_ = p_make_ptr<index_map>();
  ulog_ = p_make_ptr<pm_ulog>();

  // --- RESERVE ID 0 ---
  if (nodes_->num_chunks() == 0) {
    node dummy;
    nodes_->append(std::move(dummy), 0);
  }
  if (rships_->num_chunks() == 0) {
    relationship dummy;
    rships_->append(std::move(dummy), 0);
  }

  history_ = p_make_ptr<HistoryStorage<nvm_chunked_vec>>(); // <--- Initialize
    // Construct the backend B+tree pointer
  nvm_btree_ptr backend_btree = std::make_shared<pbtree::PBPTree<uint64_t, uint64_t, 256, 64>>();
  // Register it with HistoryStorage
  history_->set_btree(backend_btree);
  //history_mgr_ = std::make_shared<HistoryManager<nvm_chunked_vec>>(history_, node_properties_, rship_properties_, dict_, pool_);// demo delta storage
  history_mgr_ = std::make_shared<HistoryManager<nvm_chunked_vec>>(history_, node_properties_, rship_properties_, dict_, pool_, this);

#elif defined(USE_IN_MEMORY)
  nodes_ = p_make_ptr<node_list<mem_chunked_vec> >();
  rships_ = p_make_ptr<relationship_list<mem_chunked_vec> >();
  node_properties_ = p_make_ptr<property_list<mem_chunked_vec> >();
  rship_properties_ = p_make_ptr<property_list<mem_chunked_vec> >();
  dict_ = p_make_ptr<dict>();
  index_map_ = p_make_ptr<index_map>();

  // --- RESERVE ID 0 ---
  if (nodes_->num_chunks() == 0) {
    node dummy;
    nodes_->append(std::move(dummy), 0);
  }
  if (rships_->num_chunks() == 0) {
    relationship dummy;
    rships_->append(std::move(dummy), 0);
  }
  history_ = p_make_ptr<HistoryStorage<mem_chunked_vec>>(); // <--- Initialize

  im_btree_ptr backend_btree = std::make_shared<imbtree::BPTree<uint64_t, uint64_t, 256, 64>>();
  history_->set_btree(backend_btree);
  //history_mgr_ = std::make_shared<HistoryManager<mem_chunked_vec>>(history_, node_properties_, rship_properties_, dict_, pool_); // demo delta storage
  history_mgr_ = std::make_shared<HistoryManager<mem_chunked_vec>>(history_, node_properties_, rship_properties_, dict_, pool_, this);

#else
  pool_path_ = pool_path;
  prepare_files(pool_path, db_name);
  nodes_ = p_make_ptr<node_list<buffered_vec> >(bpool_, NODE_FILE_ID);
  rships_ = p_make_ptr<relationship_list<buffered_vec> >(bpool_, RSHIP_FILE_ID);
  node_properties_ = p_make_ptr<property_list<buffered_vec> >(bpool_, NPROPS_FILE_ID);
  rship_properties_ = p_make_ptr<property_list<buffered_vec> >(bpool_, RPROPS_FILE_ID);
  index_map_ = p_make_ptr<index_map>();
  restore_indexes(pool_path, db_name);

  //  RESERVE ID 0 ---
  if (nodes_->num_chunks() == 0) {
    node dummy;
    nodes_->append(std::move(dummy), 0);
  }
  if (rships_->num_chunks() == 0) {
    relationship dummy;
    rships_->append(std::move(dummy), 0);
  }

  	 // Pass the Buffer Pool and the new File IDs
  history_ = p_make_ptr<HistoryStorage<buffered_vec>>(bpool_, HISTORY_NODE_FILE_ID,  HISTORY_RSHIP_FILE_ID);// <--- Initialize

  //pf_btree_ptr backend_btree = make_pf_btree(bpool_, HISTORY_NODE_FILE_ID);
  pf_btree_ptr backend_btree = make_pf_btree(bpool_, HISTORY_INDEX_FILE_ID);
  history_->set_btree(backend_btree);
  //history_mgr_ = std::make_shared<HistoryManager<buffered_vec>>(history_, node_properties_, rship_properties_, dict_, pool_); // demo delta storage
  history_mgr_ = std::make_shared<HistoryManager<buffered_vec>>(history_, node_properties_, rship_properties_, dict_, pool_, this);
  //std::cout << "DEBUG: HistoryStorage pointer created." << std::endl;
#endif
// call restore_state to populate last delta offsets
  if (history_mgr_) {
      history_mgr_->restore_state();
  }
#if defined CSR_DELTA
  delta_store_ = p_make_ptr<delta_store>();
#endif
  active_tx_ = new std::map<xid_t, transaction_ptr>();
  runtime_initialize();
}

graph_db::~graph_db() {
  for (auto tp : *active_tx_) {
    // TODO: abort all active transactions
  }
  delete active_tx_;
  delete m_;
  delete garbage_;
  delete gcm_;
  current_transaction_.reset();
  close_files();
}

// Helper function definitions for logical ID lookup - nodes
node::id_t graph_db::find_current_node(node::logical_id_t lid) const {
  auto it = node_logical_index_.find(lid);
  return (it != node_logical_index_.end()) ? it->second : UNKNOWN;
}
// Helper function definitions for logical ID lookup - relationships
relationship::id_t graph_db::find_current_rship(relationship::logical_id_t lid) const {
  auto it = rship_logical_index_.find(lid);
  return (it != rship_logical_index_.end()) ? it->second : UNKNOWN;
}

void graph_db::flush() {
  bpool_.flush_all();
}

void graph_db::flush(const std::set<offset_t>& dirty_chunks) {
  for (const auto& dc : dirty_chunks) {
    bpool_.flush_page(dc, false);
  }
}

void graph_db::close_files() {
#ifdef USE_PFILES
  if (dict_) dict_->close_file();
  if (node_file_) node_file_->close();
  if (rship_file_) rship_file_->close();
  if (nprops_file_) nprops_file_->close();
  if (rprops_file_) rprops_file_->close();
  index_map_->clear();
  for (auto pf : index_files_) {
    pf->close();
  }
  if (walog_) {
    walog_->close(true);
  }
#endif
}

void graph_db::runtime_initialize() {
  spdlog::debug("graph_db::runtime_initialize()");
#ifdef USE_PMDK
  nodes_->runtime_initialize();
  rships_->runtime_initialize();
  //history_->runtime_initialize();
#endif
	// Initialize History for ALL modes (PMDK, DRAM, and DISK) ---
  	// check if the pointer exists first.
  	if (history_) {
      	history_->runtime_initialize();
  	}
  apply_log();

#ifdef USE_PFILES
  std::string prefix = pool_path_ + "/" + database_name_;
  walog_ = p_make_ptr<wa_log>(prefix + "/poseidon.wal");
#endif

#if defined CSR_DELTA
  delta_store_->initialize();
#endif

  active_tx_ = new std::map<xid_t, transaction_ptr>();
  oldest_xid_ = 0;
  m_ = new std::mutex();
  gcm_ = new std::mutex();
  garbage_ = new gc_list();
}

bool graph_db::run_transaction(std::function<bool()> body) {
  begin_transaction();
  return body() ? commit_transaction() : abort_transaction();
}

void graph_db::begin_transaction() {
  if (current_transaction_)
    throw invalid_nested_transaction();
  auto tx = std::make_shared<transaction>();
  current_transaction_ = tx;
#ifdef USE_PMDK
  tx->set_logid(ulog_->transaction_begin(tx->xid()));
#elif defined(USE_PFILES)
  walog_->transaction_begin(tx->xid());
#endif
  std::unique_lock guard(*m_);
  active_tx_->insert({tx->xid(), tx});
}

/**
 adjacency unlinking on DELETE.
 *
 * Previously, deleted nodes and relationships remained linked in adjacency lists,
 * leaving objeacts visible during traversal. This caused incorrect results and wasted cycles when walking from_rship_list / to_rship_list chains.
 *
 * Now, commit_dirty_node() and commit_dirty_relationship() explicitly splice
 * deleted entries out of adjacency lists and reset their pointers to UNKNOWN.
 * - Nodes: incident relationships are isolated and adjacency heads cleared.
 * - Relationships: removed from source/destination chains and adjacency pointers reset.
 *
 * adjacency traversal only sees live edges/nodes, while version chains
 * still preserve historical state for snapshots, timelines, and diffs.
 */

void graph_db::commit_dirty_node(transaction_ptr tx, node::id_t node_id) {
  auto xid = tx->xid();
  auto &n = nodes_->get(node_id);

  // If node already deleted, drop stale dirty entry.
  if (n.cts() != INF) {
    n.remove_dirty_version(0);
    return;
  }
  if (!n.has_dirty_versions()) {
    return;
  }

  if (const auto& dn {n.dirty_list()->front()}; !dn->updated()) {
    // --- INSERT BRANCH ---
#ifdef USE_PFILES
    auto log_rec = wal::create_insert_node_record(dn);
    walog_->append(xid, log_rec);
#endif
    // Create hot version
    copy_properties(n, dn);
    n.node_label = dn->elem_.node_label;
    n.from_rship_list = dn->elem_.from_rship_list;
    n.to_rship_list = dn->elem_.to_rship_list;
    n.set_timestamps(xid, INF);
    // Metadata Handover for Inserts
    // If this node was created as a new version (e.g. during an update),
    // it already has a prev_version link in the dirty record. Transfer it.
    n.prev_version = dn->elem_.prev_version;
    n.update_count = dn->elem_.update_count;
    // Conditional Initial Archiving
    // Only archive the "base" version if this is a completely fresh node (pure insert).
    // If prev_version is NOT unknown, it means update_node_create_version already handled history.
    if (n.prev_version == UNKNOWN) {
        node_history_delta initial_nd {
            n.logical_id(),
            n.node_label,
            n.vt_start,
            n.vt_end,
            n.property_list,
            0,               // delta_pid
            UNKNOWN,         // prev_version
            xid,             // tt_start
            MAX_TIME ,       // tt_end
            n.from_rship_list,
            n.to_rship_list
        };
        offset_t raw_id  = history_->append_node_delta(initial_nd);
        offset_t hist_id = encode_history_id(raw_id);

        // Index and build the "Bridge" so Bob's first version is visible
        //history_mgr_->index_base_node(n.logical_id(), n.vt_start, n.vt_end, hist_id);
        history_mgr_->update_last_node_delta(n.logical_id(), hist_id);
    }

    auto idx = get_index(dn->elem_.node_label, dn->properties_);
    if (idx.first.which() > 0) {
      spdlog::debug("NODE INDEX UPDATE: insert");
      index_insert(idx, dn->elem_.id(), dn->properties_);
    }
    n.dirty_list()->pop_front();

  } else {
    // --- UPDATE or DELETE BRANCH ---
    if (dn->elem_.bts() == dn->elem_.cts()) {
      // DELETE
#ifdef USE_PMDK
      auto log_id = current_transaction_->logid();
      pmlog::log_node_record rec (log_delete, node_id,
            n.node_label, n.from_rship_list, n.to_rship_list, n.property_list);
      ulog_->append(log_id, rec);
#elif defined(USE_PFILES)
      auto log_rec = wal::create_delete_node_record(n);
      walog_->append(xid, log_rec);
#endif
      node_properties_->foreach_property_set(n.property_list, UNDO_CB);

      {
        std::unique_lock l(*gcm_);
        garbage_->push_back(gc_item { xid, node_id, gc_item::gc_node });
      }

      n.set_cts(xid);

      // Erase from logical index to allow historical fall-through
      node_logical_index_.erase(n.logical_id());
      uint64_t delete_vt = dn->elem_.vt_start;
      n.vt_end = delete_vt;
      properties_t noop;
      history_mgr_->archive_node_delta(n, noop, delete_vt, n.node_label);

      auto props = node_properties_->build_dirty_property_list(n.property_list);
      auto idx = get_index(dn->elem_.node_label, props);
      if (idx.first.which() > 0) {
        spdlog::debug("NODE INDEX UPDATE: delete");
        index_delete(idx, n.id(), props);
      }

      // UNLINK INCIDENT RELATIONSHIPS FROM ADJACENCY
      {
        offset_t curr = n.from_rship_list;
        n.from_rship_list = UNKNOWN; 
        while (curr != UNKNOWN) {
          	auto& rel = rships_->get(curr);
          	offset_t next = rel.next_src_rship;
          	rel.next_src_rship = UNKNOWN; 
          	curr = next;
        }
      }
      {
        offset_t curr = n.to_rship_list;
        n.to_rship_list = UNKNOWN; 
        while (curr != UNKNOWN) {
          	auto& rel = rships_->get(curr);
          	offset_t next = rel.next_dest_rship;
          	rel.next_dest_rship = UNKNOWN; 
          	curr = next;
        }
      }
      n.dirty_list()->pop_front();
      n.dirty_list()->front()->elem_.unlock();
      node_properties_->remove_properties(n.property_list);
      n.property_list = UNKNOWN;

    } else {
      // --- UPDATE ---
#ifdef USE_PMDK
      auto log_id = current_transaction_->logid();
      pmlog::log_node_record rec(log_update, node_id,
        n.node_label, n.from_rship_list, n.to_rship_list, n.property_list);
      ulog_->append(log_id, rec);
#elif defined(USE_PFILES)
      auto log_rec = wal::create_update_node_record(n, dn);
      walog_->append(xid, log_rec);
#endif
      n.node_label = dn->elem_.node_label;
      n.from_rship_list = dn->elem_.from_rship_list;
      n.to_rship_list = dn->elem_.to_rship_list;
      n.set_timestamps(xid, INF);
      // Copy updated properties over 
      auto old_property_id = n.property_list;
      copy_properties(n, dn);
      // Physically delete the old set from the property vector
      if (old_property_id != UNKNOWN) {
          node_properties_->remove_properties(old_property_id);
      }
      // THE METADATA HANDOVER
      // Transfer the chain link and version count to the committed node.
      n.prev_version = dn->elem_.prev_version;
      n.update_count = dn->elem_.update_count;

      assert(n.dirty_list()->size() > 1);
      auto it = n.dirty_list()->begin(); ++it;
      auto& old_props = (*it)->properties_;
      auto& new_props = dn->properties_;

      auto idx = get_index(dn->elem_.node_label, new_props);
      if (idx.first.which() > 0) {
        spdlog::debug("NODE INDEX UPDATE: update: {}", n.dirty_list()->size());
        index_update(idx, dn->elem_.id(), old_props, new_props);
      }
      n.dirty_list()->pop_front();
      n.dirty_list()->front()->elem_.unlock();
    }
  }
  n.unlock();
  n.gc(oldest_xid_);
}

void graph_db::commit_dirty_relationship(transaction_ptr tx, relationship::id_t rel_id) {
  auto xid = tx->xid();
  auto& r = rships_->get(rel_id);

  if (r.cts() != INF) {
    r.remove_dirty_version(0);
    return;
  }
  if (!r.has_dirty_versions()) {
    return;
  }

  if (const auto& dr { r.dirty_list()->front() }; !dr->updated()) {
    // --- INSERT BRANCH ---
#ifdef USE_PFILES
    auto log_rec = wal::create_insert_rship_record(dr);
    walog_->append(xid, log_rec);
#endif
    r.set_timestamps(xid, INF);
    r.rship_label = dr->elem_.rship_label;
    r.vt_end = MAX_TIME;
    copy_properties(r, dr);

    //  Metadata Handover for Inserts
    r.prev_version = dr->elem_.prev_version;
    r.update_count = dr->elem_.update_count;

    // Conditional Archiving for Birth Bridge
    if (r.prev_version == UNKNOWN) {
        auto full_items = rship_properties_->build_dirty_property_list(r.property_list);
        property_set::id_t full_delta_pid = rship_properties_->add_pitems(r.id(), full_items, dict_);

        auto lid1 = nodes_->get(r.src_node).logical_id(); 
        auto lid2 = nodes_->get(r.dest_node).logical_id();

        rship_history_delta initial_rd {
            r.logical_id(),
            r.rship_label,
            lid1,
            lid2,
            r.vt_start,
            r.vt_end,
            r.property_list,
            full_delta_pid,
            UNKNOWN,         // prev_version
            xid,             // tt_start
            MAX_TIME,        // tt_end
            r.next_src_rship,
            r.next_dest_rship
        };
        offset_t raw_id  = history_->append_rship_delta(initial_rd);
        offset_t hist_id = encode_history_id(raw_id);      
        //history_mgr_->index_base_rship(r.logical_id(), r.vt_start, r.vt_end, hist_id);
        history_mgr_->update_last_rship_delta(r.logical_id(), hist_id);
    }

    auto idx = get_index(dr->elem_.rship_label, dr->properties_);
    if (idx.first.which() > 0) {
      spdlog::info("RSHIP INDEX UPDATE: insert");
      index_insert(idx, dr->elem_.id(), dr->properties_);
    }
    r.dirty_list()->pop_front();

  } else {
    // --- UPDATE or DELETE BRANCH ---
    if (dr->elem_.bts() == dr->elem_.cts()) {
      // DELETE
#ifdef USE_PMDK
      auto log_id = current_transaction_->logid();
      pmlog::log_rship_record rec { log_delete, rel_id,
        r.rship_label, r.src_node, r.dest_node, r.next_src_rship, r.next_dest_rship };
      ulog_->append(log_id, rec);
#elif defined(USE_PFILES)
      auto log_rec = wal::create_delete_rship_record(r);
      walog_->append(xid, log_rec);
#endif
      auto lid1 = nodes_->get(r.src_node).logical_id();
      auto lid2 = nodes_->get(r.dest_node).logical_id();
      uint64_t delete_vt = dr->elem_.vt_start;
      r.vt_end = delete_vt;
      properties_t noop; 
      history_mgr_->archive_rship_delta(r, noop, delete_vt, r.rship_label, lid1, lid2);
      rship_properties_->foreach_property_set(r.property_list, UNDO_CB);

      {
        std::unique_lock l(*gcm_);
        garbage_->push_back(gc_item { xid, rel_id, gc_item::gc_rship });
      }
      r.set_cts(xid);

      rship_logical_index_.erase(r.logical_id());
      auto props = rship_properties_->build_dirty_property_list(r.property_list);
      auto idx = get_index(dr->elem_.rship_label, props);
      if (idx.first.which() > 0) {
        spdlog::info("RSHIP INDEX UPDATE: delete");
        index_delete(idx, r.id(), props);
      }

      // Adjacency Unlinking
      {
        auto src_id = r.src_node;
        if (src_id != UNKNOWN) {
          auto& src_node = nodes_->get(src_id);
          offset_t prev_adj = UNKNOWN;
          offset_t curr_adj = src_node.from_rship_list;
          while (curr_adj != UNKNOWN) {
            auto& cur_rel = rships_->get(curr_adj);
            if (curr_adj == rel_id) {
              if (prev_adj == UNKNOWN) src_node.from_rship_list = cur_rel.next_src_rship;
              else rships_->get(prev_adj).next_src_rship = cur_rel.next_src_rship;
              break;
            }
            prev_adj = curr_adj;
            curr_adj = cur_rel.next_src_rship;
          }
        }
      }
      {
        auto dest_id = r.dest_node;
        if (dest_id != UNKNOWN) {
          auto& dest_node = nodes_->get(dest_id);
          offset_t prev_adj = UNKNOWN;
          offset_t curr_adj = dest_node.to_rship_list;
          while (curr_adj != UNKNOWN) {
            auto& cur_rel = rships_->get(curr_adj);
            if (curr_adj == rel_id) {
              if (prev_adj == UNKNOWN) dest_node.to_rship_list = cur_rel.next_dest_rship;
              else rships_->get(prev_adj).next_dest_rship = cur_rel.next_dest_rship;
              break;
            }
            prev_adj = curr_adj;
            curr_adj = cur_rel.next_dest_rship;
          }
        }
      }
      r.next_src_rship = UNKNOWN;
      r.next_dest_rship = UNKNOWN;

      r.dirty_list()->pop_front();
      r.dirty_list()->front()->elem_.unlock();
      rship_properties_->remove_properties(r.property_list);

    } else {
      // --- UPDATE ---
#ifdef USE_PMDK
      auto log_id = current_transaction_->logid();
      pmlog::log_rship_record rec(log_update, rel_id,
          r.rship_label, r.src_node, r.dest_node, r.next_src_rship, r.next_dest_rship);
      ulog_->append(log_id, rec);
#elif defined(USE_PFILES)
      auto log_rec = wal::create_update_rship_record(r, dr);
      walog_->append(xid, log_rec);
#endif
      r.set_timestamps(xid, INF);
      r.rship_label = dr->elem_.rship_label;
      auto old_property_id = r.property_list; // Capture old property list ID for cleanup after handover
      copy_properties(r, dr);

      // Physically delete the old set from the property vector
      if (old_property_id != UNKNOWN) {
          rship_properties_->remove_properties(old_property_id);
      }
      //  THE METADATA HANDOVER
      r.prev_version = dr->elem_.prev_version;
      r.update_count = dr->elem_.update_count;

      assert(r.dirty_list()->size() > 1);
      auto it = r.dirty_list()->begin(); ++it;
      auto& old_props = (*it)->properties_;
      auto& new_props = dr->properties_;

      auto idx = get_index(dr->elem_.rship_label, new_props);
      if (idx.first.which() > 0) {
        spdlog::info("RSHIP INDEX UPDATE: update: {}", r.dirty_list()->size());
        index_update(idx, dr->elem_.id(), old_props, new_props);
      }
      r.dirty_list()->pop_front();
      r.dirty_list()->front()->elem_.unlock();
    }
  }
  r.unlock();
  r.gc(oldest_xid_);
}

bool graph_db::commit_transaction() {
  check_tx_context();
  auto tx = current_transaction();
  auto xid = tx->xid();

  {
    std::unique_lock guard(*m_);
    active_tx_->erase(xid);
    oldest_xid_ = !active_tx_->empty() ? active_tx_->begin()->first : xid;
  }

  for (auto rel_id : tx->dirty_relationships())  {
    commit_dirty_relationship(tx, rel_id);
  }
  for (auto node_id : tx->dirty_nodes()) {
    commit_dirty_node(tx, node_id);
  }

#ifdef CSR_DELTA
#ifdef DIFF_DELTA
  auto &txn_delta_ids = tx->csr_delta_ids();
  auto count = delta_store_->num_delta_recs_;
  count += txn_delta_ids.deleted_nodes_.size();
  count += txn_delta_ids.deleted_neighbours_.size();
  for (auto &[nid, inserts] : txn_delta_ids.inserted_neighbours_)
    if (!txn_delta_ids.deleted_neighbours_.contains(nid))
      count++;

  if (!delta_store_->delta_mode_) {
    ;
  }
  else if (count > delta_store_->max_delta_recs_) {
    delta_store_->delta_mode_ = false;
  }
  else {
    delta_store_->store_deltas(xid, txn_delta_ids);
  }
#elif defined ADJ_DELTA
  auto &updated_nodes = tx->csr_delta_ids().updated_nodes_;
  auto &deleted_nodes = tx->csr_delta_ids().deleted_nodes_;
  auto &deleted_rships = tx->csr_delta_ids().deleted_rships_;
  auto count = delta_store_->num_delta_recs_ + updated_nodes.size();

  if (!delta_store_->delta_mode_) {
    ;
  }
  else if (count > delta_store_->max_delta_recs_) {
    delta_store_->delta_mode_ = false;
  }
  else {
    std::vector<uint64_t> neigbour_node_ids;
    std::vector<double> rship_weights;

    for (auto node_id : updated_nodes) {
      auto &n = nodes_->get(node_id);
      if (deleted_nodes.find(node_id) == deleted_nodes.end()) {
        auto rid = n.from_rship_list;
        while (rid != UNKNOWN) {
          auto &r = rships_->get(rid);
          if (deleted_rships.find(rid) == deleted_rships.end()) {
            neigbour_node_ids.push_back(r.to_node_id());
            rship_weights.push_back(delta_store_->weight_func_(r));
          }
          rid = r.next_src_rship;
        }

        if (delta_store_->bidirectional_) {
          rid = n.to_rship_list;
          while (rid != UNKNOWN) {
            auto &r = rships_->get(rid);
            if (deleted_rships.find(rid) == deleted_rships.end()) {
              neigbour_node_ids.push_back(r.from_node_id());
              rship_weights.push_back(delta_store_->weight_func_(r));
            }
            rid = r.next_dest_rship;
          }
        }
      }
      delta_store_->store_delta(xid, node_id, neigbour_node_ids, rship_weights);
      neigbour_node_ids.clear();
      rship_weights.clear();
    }
  }
#endif
#endif

#ifdef USE_PMDK
  ulog_->transaction_end(current_transaction_->logid());
#elif defined(USE_PFILES)
  walog_->transaction_commit(xid);
#endif

  current_transaction_.reset();
  vacuum(xid);
  //spdlog::info("transaction committed.");
  return true;
}

bool graph_db::abort_transaction() {
    if (!current_transaction_) return false;
    check_tx_context();
    auto tx  = current_transaction();
    auto xid = tx->xid();
    {
        std::unique_lock guard(*m_);
        active_tx_->erase(xid);
        oldest_xid_ = !active_tx_->empty() ? active_tx_->begin()->first : xid;
    }

    // ROLLBACK NODES
    std::unordered_set<node::id_t> processed_nodes;
    for (auto node_id : tx->dirty_nodes()) {
        if (processed_nodes.count(node_id)) continue;
        processed_nodes.insert(node_id);

        try {
            auto &n = nodes_->get(node_id);
            uint64_t lid = n.logical_id();
            // Use bts to distinguish new garbage from modified predecessors
            // bts == xid means the record was physically created in this transaction
            bool is_new_garbage = (n.bts() == xid);

            if (is_new_garbage) {
                // This version is garbage (e.g., Alice Age 999).
                // Remove from Temporal Index
                if (lid != UNKNOWN) {
                    history_mgr_->get_temporal_index().remove_version(lid, n.vt_start, false);
                }

                bool is_pure_insert = (n.prev_version == UNKNOWN);
                nodes_->remove(node_id);

                // If it was a pure insert, remove the logical ID mapping entirely
                if (is_pure_insert && lid != UNKNOWN) {
                    node_logical_index_.erase(lid);
                }
            } else {
                // This is a predecessor that was modified/closed (e.g., Senior Engineer Alice).
                // RE-OPEN the version:
                n.vt_end = MAX_TIME;
                n.next_version = UNKNOWN;
                n.set_cts(INF);
                n.unlock(); 

                if (lid != UNKNOWN) {
                    node_logical_index_[lid] = node_id;
                    // Restore index to point back to this HOT record in the Hot Store
                    history_mgr_->get_temporal_index().remove_version(lid, n.vt_start, false);
                    //history_mgr_->get_temporal_index().insert_version(lid, n.vt_start, TI_PtrType::HOT, node_id, false);
                    history_mgr_->index_hot_node(lid, n.vt_start, node_id);
                }
            }
        } catch (...) { continue; }
    }

    // ROLLBACK RELATIONSHIPS
    std::unordered_set<relationship::id_t> processed_rships;
    for (auto rel_id : tx->dirty_relationships()) {
        if (processed_rships.count(rel_id)) continue;
        processed_rships.insert(rel_id);

        try {
            auto &r = rships_->get(rel_id);
            uint64_t lid = r.logical_id();
            bool is_new_garbage = (r.bts() == xid);

            if (is_new_garbage) {
                if (lid != UNKNOWN) {
                    history_mgr_->get_temporal_index().remove_version(lid, r.vt_start, true);
                }
                bool is_pure_insert = (r.prev_version == UNKNOWN);
                rships_->remove(rel_id);
                if (is_pure_insert && lid != UNKNOWN) {
                    rship_logical_index_.erase(lid);
                }
            } else {
                r.vt_end = MAX_TIME;
                r.next_version = UNKNOWN;
                r.set_cts(INF);
                r.unlock(); 

                if (lid != UNKNOWN) {
                    rship_logical_index_[lid] = rel_id;
                    history_mgr_->get_temporal_index().remove_version(lid, r.vt_start, true);
                    //history_mgr_->get_temporal_index().insert_version(lid, r.vt_start, TI_PtrType::HOT, rel_id, true);
                    history_mgr_->index_hot_rship(lid, r.vt_start, rel_id);
                }
            }
        } catch (...) { continue; }
    }

    vacuum(xid);

#ifdef USE_PMDK
    ulog_->transaction_end(current_transaction_->logid());
#elif defined(USE_PFILES)
    walog_->transaction_abort(xid);
#endif

    current_transaction_.reset();
    return true;
}

void graph_db::print_stats() {
  std::cout << "nodes: " << nodes_->num_chunks() << " chunks, "
            << "chunk_size = " << nodes_->as_vec().real_chunk_size() << " Bytes, "
            << "elems_per_chunk = " << nodes_->as_vec().elements_per_chunk() << ", "
            << "node_size = " << sizeof(node) << " Bytes, "
            << "capacity = " << nodes_->as_vec().capacity() << std::endl;
  std::cout << "relationships: " << rships_->num_chunks() << " chunks, "
            << "chunk_size = " << rships_->as_vec().real_chunk_size() << " Bytes, "
            << "rship_size = " << sizeof(relationship) << " Bytes, "
            << "capacity = " << rships_->as_vec().capacity() << std::endl;
  std::cout << "node properties: " << node_properties_->num_chunks() << " chunks, "
            << "chunk_size = " << node_properties_->as_vec().real_chunk_size() << " Bytes, "
            << "property_set_size = " << sizeof(property_set) << " Bytes, "
            << "capacity = " << node_properties_->as_vec().capacity() << std::endl;
  std::cout << "rship properties: " << rship_properties_->num_chunks() << " chunks, "
            << "chunk_size = " << rship_properties_->as_vec().real_chunk_size() << " Bytes, "
            << "property_set_size = " << sizeof(property_set) << " Bytes, "
            << "capacity = " << rship_properties_->as_vec().capacity() << std::endl;

  uint64_t mem = 0;
  mem += nodes_->num_chunks() * nodes_->as_vec().real_chunk_size();
  mem += rships_->num_chunks() * rships_->as_vec().real_chunk_size();
  std::cout << mem / 1024;
  mem += node_properties_->num_chunks() * node_properties_->as_vec().real_chunk_size();
  mem += rship_properties_->num_chunks() * rship_properties_->as_vec().real_chunk_size();
  std::cout << " (" << mem / 1024 << ") KiB total" << std::endl;

  uint64_t nprops = 0;
  node_properties_->foreach_property([&nprops](auto pi) { nprops++; });
  std::cout << nprops << " node properties total." << std::endl;
  uint64_t rprops = 0;
  rship_properties_->foreach_property([&rprops](auto pi) { rprops++; });
  std::cout << rprops << " relationship properties total." << std::endl;

  std::cout << dict_->size() << " strings in dictionary." << std::endl;

#ifdef USE_PFILES
  std::cout << "bufferpool hit ratio: " << bpool_.hit_ratio() << std::endl;
#endif
}

/* ---------------- graph construction ---------------- */

node::id_t graph_db::add_node(const std::string &label,
                              const properties_t &props,
                              uint64_t vt_start,
                              uint64_t vt_end,
                              node::logical_id_t logical_id,
                              bool append_only) {
  check_tx_context();
  xid_t txid = current_transaction()->xid();

  auto type_code = dict_->lookup_string(label);
  if (type_code == 0) {
    type_code = dict_->insert(label);
#ifdef USE_PFILES
    wal::log_dict_record log_rec(type_code, label);
    walog_->append(txid, log_rec);
#endif
  }

#ifdef USE_PMDK
  auto log_id = current_transaction()->logid();
  auto cb = [log_id, this](offset_t n_id) {
    pmlog::log_ins_record rec(log_insert, log_node, n_id);
    ulog_->append(log_id, rec);
  };
#endif

  auto node_id = append_only ? nodes_->append(node(type_code, logical_id), txid, UNDO_CB)
                             : nodes_->insert(node(type_code, logical_id), txid, UNDO_CB);
  auto &n = nodes_->get(node_id);

  // temporal init
  n.vt_start     = vt_start;
  n.vt_end       = vt_end;
  n.prev_version = UNKNOWN;
  n.next_version = UNKNOWN;

  // --- Version chain linking ---
  if (logical_id != UNKNOWN) {
    auto existing_id = find_current_node(logical_id);
    if (existing_id != UNKNOWN) {
      auto &old_node = nodes_->get(existing_id);
      old_node.vt_end = vt_start;           // close old VT
      old_node.next_version = node_id;      // forward link
      n.prev_version = existing_id;         // backward link
    }
    node_logical_index_[logical_id] = node_id; // update registry
    // Index new version into temporal index as HOT
    history_mgr_->index_hot_node(logical_id, vt_start, node_id);
    active_node_lids_.insert(logical_id);
  }

  // handle properties
  const auto dirty_list = node_properties_->build_dirty_property_list(props, dict_);
  const auto &dv = n.add_dirty_version(std::make_unique<dirty_node>(n, dirty_list, false));
  dv->elem_.set_dirty();

  current_transaction()->add_dirty_node(node_id);

#if defined CSR_DELTA
#ifdef DIFF_DELTA
  current_transaction()->add_inserted_node(node_id);
#elif defined ADJ_DELTA
  current_transaction()->add_updated_node(node_id);
#endif
#endif

  return node_id;
}

// store a new version of the node with updated properties and return the new node id
node::id_t graph_db::update_node_create_version(node::id_t node_id,
                                                const properties_t &props,
                                                uint64_t new_vt_start) {
  check_tx_context();
  auto xid = current_transaction()->xid();
  auto tx = current_transaction(); 

  // 1. Get the current active version
  auto &old_node = node_by_id(node_id);
  current_transaction()->add_dirty_node(old_node.id());

  // 2. SNAPSHOT PRIMITIVES: Capture only simple data, not the whole object
  auto old_lid    = old_node.logical_id();
  auto old_label  = old_node.node_label;
  auto old_pid    = old_node.property_list;
  auto old_vt_s   = old_node.vt_start;
  auto old_vt_e   = old_node.vt_end;
  auto old_from   = old_node.from_rship_list;
  auto old_to     = old_node.to_rship_list;
  properties_t updates_snapshot = props; // std::map can be copied

  // 3. FOREGROUND: Fast Hot Store Update (O(1))
  properties_t merged_props = node_properties_->all_properties(old_pid, dict_);
  for (const auto& [key, value] : props) { merged_props[key] = value; }

  auto new_id = add_node(get_string(old_label), merged_props, new_vt_start, MAX_TIME, old_lid);
  auto &new_node = node_by_id(new_id);
  new_node.update_count = old_node.update_count + 1;
  new_node.from_rship_list = old_from;
  new_node.to_rship_list   = old_to;

  // 4. ASYNCHRONOUS ARCHIVAL: Offload using the captured primitives
  pool_.submit([this, tx, old_lid, old_label, old_vt_s, old_vt_e, old_pid, 
                old_from, old_to, updates_snapshot, new_vt_start, new_id]() {
      // Set background thread-local context
      ::current_transaction_ = tx; 
      
      // Use the new RAW method that takes primitives
      auto encoded_hist_id = history_mgr_->archive_node_delta_raw(
          old_lid, old_label, old_vt_s, old_vt_e, old_pid, 
          old_from, old_to, updates_snapshot, new_vt_start
      );
      
      // Update the history link in the physical record head
      auto &new_node_ptr = nodes_->get(new_id);
      new_node_ptr.prev_version = encoded_hist_id;
  });

  // 5. Memory Reclamation (GC)
  old_node.set_cts(xid);
  old_node.vt_end = new_vt_start;
  {
      std::unique_lock l(*gcm_);
      garbage_->push_back(gc_item { xid, node_id, gc_item::gc_node });
  }

  node_logical_index_[old_lid] = new_id;
  return new_id;
}

//check the version chain of the node with logical id lid and return the node id valid at vt_time
node::id_t graph_db::get_node_at_vt(node::logical_id_t lid, uint64_t vt_time) {
  // Check the "Hot" head
  auto head_id = find_current_node(lid);
  if (head_id != UNKNOWN) {
    // Use raw access to bypass standard MVCC visibility filters
    auto& cur_hot = nodes_->get(head_id);
    cur_hot.prepare();

    // check if it was committed (bts != 0) and valid at vt_time
    if (cur_hot.bts() != 0 && cur_hot.vt_start <= vt_time && vt_time < cur_hot.vt_end) {
      return cur_hot.id();
    }
  }
  // Index Lookup setup
  TI_PtrType type;
  offset_t payload;
  uint32_t start_vt_hint = 0;

  auto& cp_map = history_mgr_->get_checkpoint_index().node_checkpoints[lid];
  auto it = cp_map.upper_bound(vt_time);
  if (it != cp_map.begin()) {
    start_vt_hint = static_cast<uint32_t>((--it)->first);
  }

  // Perform B+Tree Lookup
  if (history_mgr_->get_temporal_index().lookup_as_of(static_cast<uint32_t>(lid),
                                                     static_cast<uint32_t>(vt_time),
                                                     &type, &payload, start_vt_hint, false)) {
    if (type == TI_PtrType::HOT) {
      // raw access here to avoid standard visibility blocks
      auto& n = nodes_->get(payload);
      n.prepare();

      if (n.bts() == 0 || vt_time < n.vt_start || vt_time >= n.vt_end) {
        // Fallback for uncommitted or out-of-range HOT records
        if (history_mgr_->get_temporal_index().lookup_as_of(static_cast<uint32_t>(lid),
                                                           static_cast<uint32_t>(n.vt_start - 1),
                                                           &type, &payload, 0, false)) {
          if (type == TI_PtrType::HIST) {
            const auto& nd = history_mgr_->get_node_delta(payload);
            if (vt_time >= nd.vt_start && vt_time < nd.vt_end) return encode_history_id(payload);
          } else {
            //  Use raw access for fallback
            auto& fallback_n = nodes_->get(payload);
            fallback_n.prepare();
            if (vt_time >= fallback_n.vt_start && vt_time < fallback_n.vt_end && fallback_n.bts() != 0) return payload;
          }
        }
        throw unknown_id();
      }
      return payload;
    }

    if (type == TI_PtrType::HIST) {
      const auto& nd = history_mgr_->get_node_delta(payload);
      if (vt_time >= nd.vt_start && vt_time < nd.vt_end) {
        return encode_history_id(payload);
      }
    }
  }

  throw unknown_id();
}

node::id_t graph_db::import_node(const std::string &label,
                                 const properties_t &props,
                                 uint64_t vt_start,
                                 uint64_t vt_end,
                                 node::logical_id_t logical_id) {
  check_tx_context();
  xid_t txid = current_transaction()->xid();

  auto type_code = dict_->lookup_string(label);
  if (type_code == 0) {
    type_code = dict_->insert(label);
#ifdef USE_PFILES
    wal::log_dict_record log_rec(type_code, label);
    walog_->append(txid, log_rec);
#endif
  }

  auto node_id = nodes_->insert(node(type_code, logical_id), txid, UNDO_CB);
  auto &n = nodes_->get(node_id);

  // temporal init
  n.vt_start     = vt_start;
  n.vt_end       = vt_end;
  n.prev_version = UNKNOWN;
  n.next_version = UNKNOWN;

  // Version chain linking
  if (logical_id != UNKNOWN) {
    auto existing_id = find_current_node(logical_id);
    if (existing_id != UNKNOWN) {
      auto &old_node = nodes_->get(existing_id);
      old_node.vt_end = vt_start;
      old_node.next_version = node_id;
      n.prev_version = existing_id;
    }
    node_logical_index_[logical_id] = node_id;
    // Index new version into temporal index
    history_mgr_->index_hot_node(logical_id, vt_start, node_id);
    active_node_lids_.insert(logical_id);
  }

  const auto dirty_list = node_properties_->build_dirty_property_list(props, dict_);
  const auto &dv = n.add_dirty_version(std::make_unique<dirty_node>(n, dirty_list, false));
  dv->elem_.set_dirty();

  current_transaction()->add_dirty_node(node_id);
  return node_id;
}

node::id_t graph_db::import_typed_node(dcode_t label,
                                       const std::vector<dcode_t> &keys,
                                       const std::vector<p_item::p_typecode>& typelist,
                                       const std::vector<std::any>& values,
                                       uint64_t vt_start,
                                       uint64_t vt_end,
                                       node::logical_id_t logical_id) {
  check_tx_context();
  xid_t txid = current_transaction()->xid();

  auto node_id = nodes_->insert(node(label, logical_id), txid, UNDO_CB);
  auto &n = nodes_->get(node_id);

  // temporal init
  n.vt_start     = vt_start;
  n.vt_end       = vt_end;
  n.prev_version = UNKNOWN;
  n.next_version = UNKNOWN;

  if (logical_id != UNKNOWN) {
    auto existing_id = find_current_node(logical_id);
    if (existing_id != UNKNOWN) {
      auto &old_node = nodes_->get(existing_id);
      old_node.vt_end = vt_start;
      old_node.next_version = node_id;
      n.prev_version = existing_id;
    }
    node_logical_index_[logical_id] = node_id;
  // Index new version into temporal index
    history_mgr_->index_hot_node(logical_id, vt_start, node_id);
    active_node_lids_.insert(logical_id);
  }

  // Build dirty p_items explicitly (std::any path)
  std::list<p_item> dirty_list;
  for (size_t i = 0; i < keys.size(); ++i) {
    if (!values[i].has_value()) continue;
    dirty_list.emplace_back(p_item(keys[i], typelist[i], values[i]));
  }

  const auto &dv = n.add_dirty_version(std::make_unique<dirty_node>(n, dirty_list, false));
  dv->elem_.set_dirty();

  current_transaction()->add_dirty_node(node_id);
  return node_id;
}

node::id_t graph_db::import_typed_node(dcode_t label,
                                       const std::vector<dcode_t> &keys,
                                       const std::vector<p_item::p_typecode>& typelist,
                                       const std::vector<std::string>& values,
                                       dict_ptr &dict,
                                       uint64_t vt_start,
                                       uint64_t vt_end,
                                       node::logical_id_t logical_id) {
  check_tx_context();
  xid_t txid = current_transaction()->xid();

  auto node_id = nodes_->insert(node(label, logical_id), txid, UNDO_CB);
  auto &n = nodes_->get(node_id);

  // temporal init
  n.vt_start     = vt_start;
  n.vt_end       = vt_end;
  n.prev_version = UNKNOWN;
  n.next_version = UNKNOWN;

  if (logical_id != UNKNOWN) {
    auto existing_id = find_current_node(logical_id);
    if (existing_id != UNKNOWN) {
      auto &old_node = nodes_->get(existing_id);
      old_node.vt_end = vt_start;
      old_node.next_version = node_id;
      n.prev_version = existing_id;
    }
    node_logical_index_[logical_id] = node_id;
    // Index new version into temporal index
    history_mgr_->index_hot_node(logical_id, vt_start, node_id);
    active_node_lids_.insert(logical_id);
  }

  // Build dirty p_items (string + dict path)
  std::list<p_item> dirty_list;
  for (size_t i = 0; i < keys.size(); ++i) {
    if (values[i].empty()) continue;
    dirty_list.emplace_back(p_item(keys[i], typelist[i], values[i], dict));
  }

  const auto &dv = n.add_dirty_version(std::make_unique<dirty_node>(n, dirty_list, false));
  dv->elem_.set_dirty();

  current_transaction()->add_dirty_node(node_id);
  return node_id;
}

relationship::id_t graph_db::add_relationship(node::id_t from_id,
                                              node::id_t to_id,
                                              const std::string &label,
                                              const properties_t &props,
                                              uint64_t vt_start,
                                              uint64_t vt_end,
                                              relationship::logical_id_t logical_id,
                                              bool append_only) {
  check_tx_context();
  xid_t txid = current_transaction()->xid();
  auto &from_node = node_by_id(from_id);
  auto &to_node = node_by_id(to_id);

  auto type_code = dict_->lookup_string(label);
  if (type_code == 0) {
    type_code = dict_->insert(label);
#ifdef USE_PFILES
    wal::log_dict_record log_rec(type_code, label);
    walog_->append(txid, log_rec);
#endif
  }

#ifdef USE_PMDK
  auto log_id = current_transaction()->logid();
  auto cb = [log_id, this](offset_t r_id) {
    pmlog::log_ins_record rec(log_insert, log_rship, r_id);
    ulog_->append(log_id, rec);
  };
#endif

  auto rid = append_only
    ? rships_->append(relationship(type_code, from_id, to_id, logical_id), txid, UNDO_CB)
    : rships_->insert(relationship(type_code, from_id, to_id, logical_id), txid, UNDO_CB);
  auto &r = rships_->get(rid);

  // temporal init
  r.vt_start     = vt_start;
  r.vt_end       = vt_end;
  r.prev_version = UNKNOWN;
  r.next_version = UNKNOWN;

  // Version chain linking
  if (logical_id != UNKNOWN) {
    auto existing_id = find_current_rship(logical_id);
    if (existing_id != UNKNOWN) {
      auto &old_r = rships_->get(existing_id);
      old_r.vt_end = vt_start;          // close old VT
      old_r.next_version = rid;         // forward link
      r.prev_version = existing_id;     // backward link
    }
    rship_logical_index_[logical_id] = rid; // update registry
  // Index new version into temporal index
    history_mgr_->index_hot_rship(logical_id, vt_start, rid);
    active_rship_lids_.insert(logical_id);
  }

  const auto dirty_list = rship_properties_->build_dirty_property_list(props, dict_);
  const auto &rv = r.add_dirty_version(std::make_unique<dirty_rship>(r, dirty_list, false));
  rv->elem_.set_dirty();

  current_transaction()->add_dirty_relationship(rid);

  update_from_node(current_transaction(), from_node, r);
  update_to_node(current_transaction(), to_node, r);

#if defined CSR_DELTA
#ifdef DIFF_DELTA
  auto weight = delta_store_->weight_func_(rv->elem_);
  current_transaction()->add_inserted_neighbour(from_id, to_id, weight);
  if (delta_store_->bidirectional_) {
    current_transaction()->add_inserted_neighbour(to_id, from_id, weight);
  }
#elif defined ADJ_DELTA
  current_transaction()->add_updated_node(from_id);
  if (delta_store_->bidirectional_) {
    current_transaction()->add_updated_node(to_id);
  }
#endif
#endif

  return rid;
}

relationship::id_t graph_db::import_relationship(node::id_t from_id,
                                                 node::id_t to_id,
                                                 const std::string &label,
                                                 const properties_t &props,
                                                 uint64_t vt_start,
                                                 uint64_t vt_end,
                                                 relationship::logical_id_t logical_id) {
  check_tx_context();
  xid_t txid = current_transaction()->xid();

  auto type_code = dict_->lookup_string(label);
  if (type_code == 0) {
    type_code = dict_->insert(label);
#ifdef USE_PFILES
    wal::log_dict_record log_rec(type_code, label);
    walog_->append(txid, log_rec);
#endif
  }

  auto rid = rships_->insert(relationship(type_code, from_id, to_id, logical_id), txid, UNDO_CB);
  auto &r = rships_->get(rid);

  // temporal init
  r.vt_start     = vt_start;
  r.vt_end       = vt_end;
  r.prev_version = UNKNOWN;
  r.next_version = UNKNOWN;

  //  Version chain linking
  if (logical_id != UNKNOWN) {
    auto existing_id = find_current_rship(logical_id);
    if (existing_id != UNKNOWN) {
      auto &old_r = rships_->get(existing_id);
      old_r.vt_end = vt_start;          // close old VT
      old_r.next_version = rid;         // forward link
      r.prev_version = existing_id;     // backward link
    }
    rship_logical_index_[logical_id] = rid; // update registry
    // Index new version into temporal index
    history_mgr_->index_hot_rship(logical_id, vt_start, rid);
    active_rship_lids_.insert(logical_id);
  }

  const auto dirty_list = rship_properties_->build_dirty_property_list(props, dict_);
  const auto &rv = r.add_dirty_version(std::make_unique<dirty_rship>(r, dirty_list, false));
  rv->elem_.set_dirty();

  current_transaction()->add_dirty_relationship(rid);
  update_from_node(current_transaction(), node_by_id(from_id), r);
  update_to_node(current_transaction(), node_by_id(to_id), r);
  return rid;
}

relationship::id_t graph_db::import_typed_relationship(node::id_t from_id,
                                                       node::id_t to_id,
                                                       dcode_t label,
                                                       const std::vector<dcode_t> &keys,
                                                       const std::vector<p_item::p_typecode>& typelist,
                                                       const std::vector<std::string>& values,
                                                       dict_ptr &dict,
                                                       uint64_t vt_start,
                                                       uint64_t vt_end,
                                                       relationship::logical_id_t logical_id) {
  check_tx_context();
  xid_t txid = current_transaction()->xid();

  auto rid = rships_->insert(relationship(label, from_id, to_id, logical_id), txid, UNDO_CB);
  auto &r = rships_->get(rid);

  // temporal init
  r.vt_start     = vt_start;
  r.vt_end       = vt_end;
  r.prev_version = UNKNOWN;
  r.next_version = UNKNOWN;

  // --- Version chain linking ---
  if (logical_id != UNKNOWN) {
    auto existing_id = find_current_rship(logical_id);
    if (existing_id != UNKNOWN) {
      auto &old_r = rships_->get(existing_id);
      old_r.vt_end = vt_start;          // close old VT
      old_r.next_version = rid;         // forward link
      r.prev_version = existing_id;     // backward link
    }
    rship_logical_index_[logical_id] = rid; // update registry
    // Index new version into temporal index
    history_mgr_->index_hot_rship(logical_id, vt_start, rid);
    active_rship_lids_.insert(logical_id);
  }


  // Build dirty p_items (string + dict)
  std::list<p_item> dirty_list;
  for (size_t i = 0; i < keys.size(); ++i) {
    if (values[i].empty()) continue;
    dirty_list.emplace_back(p_item(keys[i], typelist[i], values[i], dict));
  }

  const auto &rv = r.add_dirty_version(std::make_unique<dirty_rship>(r, dirty_list, false));
  rv->elem_.set_dirty();

  current_transaction()->add_dirty_relationship(rid);
  update_from_node(current_transaction(), node_by_id(from_id), r);
  update_to_node(current_transaction(), node_by_id(to_id), r);
  return rid;
}

relationship::id_t graph_db::import_typed_relationship(node::id_t from_id,
                                                       node::id_t to_id,
                                                       dcode_t label,
                                                       const std::vector<dcode_t> &keys,
                                                       const std::vector<p_item::p_typecode>& typelist,
                                                       const std::vector<std::any>& values,
                                                       uint64_t vt_start,
                                                       uint64_t vt_end,
                                                       relationship::logical_id_t logical_id) {
  check_tx_context();
  xid_t txid = current_transaction()->xid();

  auto rid = rships_->insert(relationship(label, from_id, to_id, logical_id), txid, UNDO_CB);
  auto &r = rships_->get(rid);

  // temporal init
  r.vt_start     = vt_start;
  r.vt_end       = vt_end;
  r.prev_version = UNKNOWN;
  r.next_version = UNKNOWN;

  // --- Version chain linking ---
  if (logical_id != UNKNOWN) {
    auto existing_id = find_current_rship(logical_id);
    if (existing_id != UNKNOWN) {
      auto &old_r = rships_->get(existing_id);
      old_r.vt_end = vt_start;          // close old VT
      old_r.next_version = rid;         // forward link
      r.prev_version = existing_id;     // backward link
    }
    rship_logical_index_[logical_id] = rid; // update registry
    // Index new version into temporal index
    history_mgr_->index_hot_rship(logical_id, vt_start, rid);
    active_rship_lids_.insert(logical_id);
  }

  // Build dirty p_items  (std::any path)
  std::list<p_item> dirty_list;
  for (size_t i = 0; i < keys.size(); ++i) {
    if (!values[i].has_value()) continue;
    dirty_list.emplace_back(p_item(keys[i], typelist[i], values[i]));
  }

  const auto &rv = r.add_dirty_version(std::make_unique<dirty_rship>(r, dirty_list, false));
  rv->elem_.set_dirty();

  current_transaction()->add_dirty_relationship(rid);
  update_from_node(current_transaction(), node_by_id(from_id), r);
  update_to_node(current_transaction(), node_by_id(to_id), r);
  return rid;
}

// --- optional getters
std::optional<node::id_t> graph_db::try_get_node_at_vt(node::logical_id_t lid, uint64_t vt_time) {
    try {
        return get_node_at_vt(lid, vt_time);
    } catch (unknown_id&) {
        return std::nullopt;
    }
}

std::optional<relationship::id_t> graph_db::try_get_rship_at_vt(relationship::logical_id_t lid, uint64_t vt_time) {
    try {
        return get_rship_at_vt(lid, vt_time);
    } catch (unknown_id&) {
        return std::nullopt;
    }
}

// Interval queries (timeline retrieval)
std::vector<node::id_t> graph_db::get_node_versions_in_vt(node::logical_id_t lid,
                                                          uint64_t start, uint64_t end) {
  std::vector<node::id_t> results;
  auto& ti = history_mgr_->get_temporal_index();

  // Boundary lookup
  TI_PtrType type; offset_t payload;
  if (ti.lookup_as_of(static_cast<uint32_t>(lid), static_cast<uint32_t>(start), &type, &payload)) {
    if (type == TI_PtrType::HOT) {
      auto& n = nodes_->get(payload); // Raw access to avoid throwing
      n.prepare();
      // Only add if committed (bts != 0) and reaches 'start'
      if (n.bts() != 0 && n.bts() != INF && n.vt_end > start) results.push_back(payload);
    } else {
      const auto& nd = history_mgr_->get_node_delta(payload);
      if (nd.vt_end > start) results.push_back(encode_history_id(payload));
    }
  }
  //  Range Scan
  ti.scan_range(static_cast<uint32_t>(lid),
                static_cast<uint32_t>(start + 1),
                static_cast<uint32_t>(end - 1),
                false,
                [&](uint32_t vt_start, TI_PtrType type, offset_t payload) {

      if (type == TI_PtrType::HOT) {
          auto& n = nodes_->get(payload); // Raw access
          n.prepare();
          if (n.bts() != 0 && n.bts() != INF) results.push_back(payload);
      } else {
          results.push_back(encode_history_id(payload));
      }
  });

  std::reverse(results.begin(), results.end());
  return results;
}

std::vector<relationship::id_t> graph_db::get_rship_versions_in_vt(relationship::logical_id_t lid,
                                                                   uint64_t start, uint64_t end) {
  std::vector<relationship::id_t> results;
  auto& ti = history_mgr_->get_temporal_index();

  TI_PtrType type; offset_t payload;
  // Boundary lookup
  if (ti.lookup_as_of(static_cast<uint32_t>(lid), static_cast<uint32_t>(start), &type, &payload, 0, true)) {
    if (type == TI_PtrType::HOT) {
      auto& r = rships_->get(payload);
      r.prepare();
      if (r.bts() != 0 && r.bts() != INF && r.vt_end > start) results.push_back(payload);
    } else {
      const auto& rd = history_mgr_->get_rship_delta(payload);
      if (rd.vt_end > start) results.push_back(encode_history_id(payload));
    }
  }

  ti.scan_range(static_cast<uint32_t>(lid),
                static_cast<uint32_t>(start + 1),
                static_cast<uint32_t>(end - 1),
                true,
                [&](uint32_t vt_start, TI_PtrType type, offset_t payload) {

      if (type == TI_PtrType::HOT) {
          auto& r = rships_->get(payload);
          r.prepare();
          if (r.bts() != 0 && r.bts() != INF) results.push_back(payload);
      } else {
          results.push_back(encode_history_id(payload));
      }
  });

  std::reverse(results.begin(), results.end());
  return results;
}

// reconstruct snapshot at time T
GraphSnapshot graph_db::get_graph_as_of(uint64_t t) {
  // Pass the pre-tracked active LID sets to the history manager for O(Active LIDs) performance
  return history_mgr_->get_graph_as_of(t, active_node_lids_, active_rship_lids_);
}
/* ---------------- end graph construction ---------------- */

node &graph_db::get_valid_node_version(node &n, xid_t xid) {
  if (n.is_locked_by(xid)) {
    assert(n.has_dirty_versions());
    return n.find_valid_version(xid)->elem_;
  }
  else if (!n.is_locked()) {
    return n.is_valid_for(xid) ? n : n.find_valid_version(xid)->elem_;
  }
  else {
    auto &nv = n.find_valid_version(xid)->elem_;
    if (!nv.is_locked() || nv.is_locked_by(xid))
      return nv;
    throw transaction_abort();
  }
  throw unknown_id();
}

relationship &graph_db::get_valid_rship_version(relationship &r, xid_t xid) {
  if (r.is_locked_by(xid)) {
    assert(r.has_dirty_versions());
    return r.find_valid_version(xid)->elem_;
  }
  if (!r.is_locked()) {
    return r.is_valid_for(xid) ? r : r.find_valid_version(xid)->elem_;
  }
  if (r.is_locked())  {
    auto &rv = r.find_valid_version(xid)->elem_;
    if (!rv.is_locked() || rv.is_locked_by(xid))
      return rv;
    throw transaction_abort();
  }
  throw unknown_id();
}

node &graph_db::node_by_id(node::id_t id) {
  check_tx_context();
  auto xid = current_transaction()->xid();
  auto &n = nodes_->get(id);
  n.prepare();
  n.set_rts(xid);
  return get_valid_node_version(n, xid);
}

relationship &graph_db::rship_by_id(relationship::id_t id) {
  check_tx_context();
  auto xid = current_transaction()->xid();
  auto &r = rships_->get(id);
  r.prepare();
  r.set_rts(xid);
  return get_valid_rship_version(r, xid);
}

// Descriptions
node_description graph_db::get_node_description(node::id_t nid) {
  // History Path: from the Delta record
  if (is_history_id(nid)) {
    const auto& nd = history_mgr_->get_node_delta(nid);
    properties_t props = history_mgr_->resolve_node_overlay(nid);

    return node_description{
      nid, nd.lid,
      dict_->lookup_code(nd.label), // Use persistent label from delta
      props,
      nd.vt_start, nd.vt_end,
      nd.tt_start, nd.tt_end
    };
  }
  // Hot Store Path: Poseidon MVCC and Dirty-Version principles
  auto& n = node_by_id(nid);
  auto xid = current_transaction()->xid();
  std::string label;
  properties_t props;
  // Use properties from the chosen MVCC version (dirty head or committed record)
  if (!n.has_dirty_versions()) {
    props = node_properties_->all_properties(n.property_list, dict_);
    label = dict_->lookup_code(n.node_label);
  } else {
    const auto& dv = n.find_valid_version(xid);
    props = node_properties_->build_properties_from_pitems(dv->properties_, dict_);
    label = dict_->lookup_code(dv->elem_.node_label);
  }

  return node_description{
    n.id(), n.logical_id(), label, props,
    n.vt_start, n.vt_end, n.bts(), n.cts()
  };
}

rship_description graph_db::get_rship_description(relationship::id_t rid) {
  // History Path: from the Delta record
  if (is_history_id(rid)) {
    const auto& rd = history_mgr_->get_rship_delta(rid);
    // Resolve properties by overlaying deltas onto the base property set
    properties_t props = history_mgr_->resolve_rship_overlay(rid);
    // Use persistent metadata stored in the history record
    std::string label  = dict_->lookup_code(rd.label);
    node::id_t from_id = rd.src_lid; // Uses stored Logical Source ID
    node::id_t to_id   = rd.dest_lid; // Uses stored Logical Destination ID

    return rship_description{
      rid,           // encoded history ID
      rd.lid,        // logical ID
      from_id,
      to_id,
      label,
      props,
      rd.vt_start,
      rd.vt_end,
      rd.tt_start,   // transaction time captured at archival
      rd.tt_end
    };
  }

  // Hot Store Path: oseidon MVCC/Visibility principles
  auto& r = rship_by_id(rid);
  auto xid = current_transaction()->xid();

  std::string label;
  properties_t props;
  // Use the appropriate version based on current transaction context
  if (!r.has_dirty_versions()) {
    props = rship_properties_->all_properties(r.property_list, dict_);
    label = dict_->lookup_code(r.rship_label);
  } else {
    // fetch the valid version for this txn
    const auto& dr = r.find_valid_version(xid);
    props = rship_properties_->build_properties_from_pitems(dr->properties_, dict_);
    label = dict_->lookup_code(dr->elem_.rship_label);
  }

  return rship_description{
    r.id(),
    r.logical_id(),
    r.from_node_id(),
    r.to_node_id(),
    label,
    props,
    r.vt_start,
    r.vt_end,
    r.bts(),
    r.cts()
  };
}

const char *graph_db::get_relationship_label(const relationship &r) {
  return dict_->lookup_code(r.rship_label);
}

void graph_db::update_node(node &n, const properties_t &props,
                           const std::string &label) {
  auto lc = label.empty() ? 0 : dict_->insert(label);
  check_tx_context();
  xid_t txid = current_transaction()->xid();

  if (n.rts() > txid)
    throw transaction_abort();

  if (!n.is_locked_by(txid) && !n.try_lock(txid))
    throw transaction_abort();

  bool first_update = true;
  if (n.has_dirty_versions()) {
    try {
      auto& dn = n.get_dirty_version(txid);
      node_properties_->apply_updates(dn->properties_, props, dict_);
      if (lc > 0)
        dn->elem_.node_label = lc;
      first_update = false;
    } catch (unknown_id& exc) { /* ignore */ }
  }

  if (first_update) {
    std::list<p_item> pitems =
      node_properties_->build_dirty_property_list(n.property_list);

    const auto& oldv = n.add_dirty_version(std::make_unique<dirty_node>(n, pitems));
    oldv->elem_.set_timestamps(n.bts(), txid);
    oldv->elem_.set_dirty();
    oldv->elem_.unlock();

    pitems = node_properties_->apply_updates(pitems, props, dict_);
    const auto &newv = n.add_dirty_version(std::make_unique<dirty_node>(n, pitems));
    newv->elem_.set_timestamps(txid, INF);
    newv->elem_.set_dirty();
    if (lc > 0)
      newv->elem_.node_label = lc;

    current_transaction()->add_dirty_node(n.id());
  }
}

void graph_db::update_from_node(transaction_ptr tx, node &n, relationship& r) {
  xid_t txid = tx->xid();

  if (n.rts() > txid)
    throw transaction_abort();

  if (!n.is_locked_by(txid) && !n.try_lock(txid))
    throw transaction_abort();

  bool first_update = true;
  if (n.has_dirty_versions()) {
    try {
      auto& dn = n.get_dirty_version(txid)->elem_;
      if (dn.from_rship_list == UNKNOWN)
        dn.from_rship_list = r.id();
      else {
        r.next_src_rship = dn.from_rship_list;
        dn.from_rship_list = r.id();
      }
      first_update = false;
    } catch (unknown_id& exc) { /* ignore */ }
  }

  if (first_update) {
    std::list<p_item> pitems =
      node_properties_->build_dirty_property_list(n.property_list);

    const auto& oldv = n.add_dirty_version(std::make_unique<dirty_node>(n, pitems));
    oldv->elem_.set_timestamps(n.bts(), txid);
    oldv->elem_.set_dirty();
    oldv->elem_.unlock();

    const auto &newv = n.add_dirty_version(std::make_unique<dirty_node>(n, pitems));
    newv->elem_.set_timestamps(txid, INF);
    newv->elem_.set_dirty();

    if (newv->elem_.from_rship_list == UNKNOWN)
        newv->elem_.from_rship_list = r.id();
    else {
        r.next_src_rship = newv->elem_.from_rship_list;
        newv->elem_.from_rship_list = r.id();
    }
    current_transaction()->add_dirty_node(n.id());
  }
}

void graph_db::update_to_node(transaction_ptr tx, node &n, relationship& r) {
  xid_t txid = tx->xid();

  if (n.rts() > txid)
    throw transaction_abort();

  if (!n.is_locked_by(txid) && !n.try_lock(txid))
    throw transaction_abort();

  bool first_update = true;
  if (n.has_dirty_versions()) {
    try {
      auto& dn = n.get_dirty_version(txid)->elem_;
      if (dn.to_rship_list == UNKNOWN)
        dn.to_rship_list = r.id();
      else {
        r.next_dest_rship = dn.to_rship_list;
        dn.to_rship_list = r.id();
      }
      first_update = false;
    } catch (unknown_id& exc) { /* ignore */ }
  }

  if (first_update) {
    std::list<p_item> pitems =
      node_properties_->build_dirty_property_list(n.property_list);

    const auto& oldv = n.add_dirty_version(std::make_unique<dirty_node>(n, pitems));
    oldv->elem_.set_timestamps(n.bts(), txid);
    oldv->elem_.set_dirty();
    oldv->elem_.unlock();

    const auto &newv = n.add_dirty_version(std::make_unique<dirty_node>(n, pitems));
    newv->elem_.set_timestamps(txid, INF);
    newv->elem_.set_dirty();

    if (newv->elem_.to_rship_list == UNKNOWN)
        newv->elem_.to_rship_list = r.id();
    else {
        r.next_dest_rship = newv->elem_.to_rship_list;
        newv->elem_.to_rship_list = r.id();
    }
    current_transaction()->add_dirty_node(n.id());
  }
}

void graph_db::update_relationship(relationship &r, const properties_t &props,
                                   const std::string &label) {
  auto lc = label.empty() ? 0 : dict_->insert(label);
  check_tx_context();
  xid_t txid = current_transaction()->xid();

  if (r.rts() > txid)
    throw transaction_abort();

  if (!r.is_locked_by(txid) && !r.try_lock(txid))
    throw transaction_abort();

  bool first_update = true;
  if (r.has_dirty_versions()) {
    try {
      auto& dr = r.get_dirty_version(txid);
      rship_properties_->apply_updates(dr->properties_, props, dict_);
      if (lc > 0)
        dr->elem_.rship_label = lc;
      first_update = false;
    } catch (unknown_id& exc) { /* ignore */ }
  }

  if (first_update) {
    std::list<p_item> pitems =
        rship_properties_->build_dirty_property_list(r.property_list);

    const auto& oldv = r.add_dirty_version(std::make_unique<dirty_rship>(r, pitems));
    oldv->elem_.set_timestamps(r.bts(), txid);
    oldv->elem_.set_dirty();
    oldv->elem_.unlock();

    pitems = rship_properties_->apply_updates(pitems, props, dict_);
    const auto& newv = r.add_dirty_version(std::make_unique<dirty_rship>(r, pitems));
    newv->elem_.set_timestamps(txid, INF);
    newv->elem_.set_dirty();
    if (lc > 0)
      newv->elem_.rship_label = lc;

    current_transaction()->add_dirty_relationship(r.id());
  }
}

// Create a new version of the relationship with updated properties and a new valid-time start
relationship::id_t graph_db::update_relationship_create_version(relationship::id_t rship_id,
                                                                const properties_t &props,
                                                                uint64_t new_vt_start) {
  check_tx_context();
  auto xid = current_transaction()->xid();
  auto tx = current_transaction(); 

  // 1. Get current version
  auto &old_r = rship_by_id(rship_id);
  current_transaction()->add_dirty_relationship(old_r.id());

  // 2. SNAPSHOT PRIMITIVES: Capture data to bypass the "deleted copy constructor" error
  auto old_lid    = old_r.logical_id();
  auto old_label  = old_r.rship_label;
  auto old_pid    = old_r.property_list;
  auto old_vt_s   = old_r.vt_start;
  auto old_vt_e   = old_r.vt_end;
  
  // Get source and destination logical IDs (LIDs)
  uint64_t src_lid  = nodes_->get(old_r.src_node).logical_id();
  uint64_t dest_lid = nodes_->get(old_r.dest_node).logical_id();
  
  properties_t updates_snapshot = props; // Copyable property map

  // 3. ASYNCHRONOUS ARCHIVAL: Offload using primitives
  pool_.submit([this, tx, old_lid, old_label, src_lid, dest_lid, 
                old_vt_s, old_vt_e, old_pid, updates_snapshot, new_vt_start]() {
      // Set background thread context
      ::current_transaction_ = tx;
      
      // Call the RAW archival method we added to HistoryManager
      history_mgr_->archive_rship_delta_raw(
          old_lid, old_label, src_lid, dest_lid, 
          old_vt_s, old_vt_e, old_pid, 
          updates_snapshot, new_vt_start
      );
  });

  // 4. FOREGROUND: Fast Hot Store Update (O(1))
  auto latest_src_nid  = node_logical_index_[src_lid];
  auto latest_dest_nid = node_logical_index_[dest_lid];

  properties_t merged_props = rship_properties_->all_properties(old_pid, dict_);
  for (const auto& [key, value] : props) merged_props[key] = value;

  auto new_id = add_relationship(latest_src_nid, latest_dest_nid, get_string(old_label),
                                 merged_props, new_vt_start, MAX_TIME, old_lid);

  auto &new_r = rship_by_id(new_id);
  new_r.update_count = old_r.update_count + 1;
  rship_logical_index_[old_lid] = new_id;

  // 5. MEMORY RECLAMATION
  old_r.set_cts(xid);
  old_r.vt_end = new_vt_start;
  {
      std::unique_lock l(*gcm_);
      garbage_->push_back(gc_item { xid, rship_id, gc_item::gc_rship });
  }

  return new_id;
}



void graph_db::stream_rship_lids(uint64_t node_lid, bool out_direction, 
                                 const std::function<void(uint64_t)>& callback) {
    try {
        // This call throws unknown_id if the node was deleted
        uint64_t phys_id = get_node_at_vt(node_lid, 0xFFFFFFFFFFFFFFFFULL); 
        
        if (phys_id == 0 || phys_id == (uint64_t)-1) return;

        const auto& node_rec = history_->node_deltas_.at(phys_id);
        uint64_t current_r_phys = out_direction ? node_rec.from_rship_list : node_rec.to_rship_list;

        while (current_r_phys != 0 && current_r_phys != (uint64_t)-1) {
            const auto& r_rec = history_->rship_deltas_.at(current_r_phys);
            callback(r_rec.lid);
            current_r_phys = out_direction ? r_rec.next_src : r_rec.next_dest;
        }
    } catch (const unknown_id& e) {
        // Gracefully handle deleted nodes instead of crashing the process
        return; 
    }
}

// Point-in-time retrieval
relationship::id_t graph_db::get_rship_at_vt(relationship::logical_id_t lid, uint64_t vt_time) {
  // 1. Check the "Hot" head using RAW ACCESS to bypass modern MVCC filters
  auto head_rid = find_current_rship(lid);
  if (head_rid != UNKNOWN) {
    auto& cur_hot = rships_->get(head_rid); // RAW ACCESS
    cur_hot.prepare(); // Load from bufferpool
    // check validity for vt_time
    if (cur_hot.vt_start <= vt_time && vt_time < cur_hot.vt_end && cur_hot.bts() != 0) {
      return cur_hot.id();
    }
  }
  // Index Lookup setup
  TI_PtrType type;
  offset_t payload;
  uint32_t r_start_hint = 0;

  auto& r_cp_map = history_mgr_->get_checkpoint_index().rship_checkpoints[lid];
  auto r_it = r_cp_map.upper_bound(vt_time);
  if (r_it != r_cp_map.begin()) {
    r_start_hint = static_cast<uint32_t>((--r_it)->first);
  }

  // Perform partitioned B+Tree lookup (is_rel = true)
  if (history_mgr_->get_temporal_index().lookup_as_of(static_cast<uint32_t>(lid),
                                                     static_cast<uint32_t>(vt_time),
                                                     &type, &payload, r_start_hint, true)) {

    if (type == TI_PtrType::HOT) {
      auto& r = rships_->get(payload); // RAW ACCESS
      r.prepare();
      // If head is uncommitted or doesn't match range, try predecessor
      if (r.bts() == 0 || vt_time < r.vt_start || vt_time >= r.vt_end) {
        if (history_mgr_->get_temporal_index().lookup_as_of(static_cast<uint32_t>(lid),
                                                           static_cast<uint32_t>(r.vt_start - 1),
                                                           &type, &payload, 0, true)) {
          if (type == TI_PtrType::HIST) {
            const auto& rd = history_mgr_->get_rship_delta(payload);
            if (vt_time >= rd.vt_start && vt_time < rd.vt_end) return encode_history_id(payload);
          } else {
            auto& r_fallback = rships_->get(payload); // RAW ACCESS
            r_fallback.prepare();
            if (vt_time >= r_fallback.vt_start && vt_time < r_fallback.vt_end && r_fallback.bts() != 0) return payload;
          }
        }
        throw unknown_id();
      }
      return payload;
    }

    if (type == TI_PtrType::HIST) {
      const auto& rd = history_mgr_->get_rship_delta(payload);
      if (vt_time >= rd.vt_start && vt_time < rd.vt_end) {
        return encode_history_id(payload);
      }
    }
  }
  throw unknown_id();
}


void graph_db::delete_node(node::id_t id) {
  check_tx_context();
  xid_t txid = current_transaction()->xid();

  auto &n = this->node_by_id(id);

  if (n.rts() > txid)
    throw transaction_abort();

  if (!n.is_locked_by(txid) && !n.try_lock(txid))
    throw transaction_abort();

  if (n.from_rship_list != UNKNOWN || n.to_rship_list != UNKNOWN) {
    if (has_valid_from_rships(n, txid) || has_valid_to_rships(n, txid)) {
      spdlog::info("abort delete of node #{}", n.id());
      n.unlock();
      throw orphaned_relationship();
    }
  }

  std::list<p_item> pitems =
      node_properties_->build_dirty_property_list(n.property_list);

  const auto& oldv = n.add_dirty_version(std::make_unique<dirty_node>(n, pitems));
  oldv->elem_.set_timestamps(n.bts(), txid);
  oldv->elem_.set_dirty();
  oldv->elem_.unlock();

  const auto &newv = n.add_dirty_version(std::make_unique<dirty_node>(n, std::list<p_item>(), true));
  newv->elem_.set_timestamps(txid, txid);
  newv->elem_.set_dirty();

  //newv->elem_.vt_start = txid;
  current_transaction()->add_dirty_node(n.id());

#if defined CSR_DELTA
#ifdef DIFF_DELTA
  current_transaction()->add_deleted_node(id);
#elif defined ADJ_DELTA
  current_transaction()->add_updated_node(id);
  current_transaction()->add_deleted_node(id);
#endif
#endif
}

void graph_db::detach_delete_node(node::id_t id) {
  spdlog::info("try to detach_delete_node: {}", id);
  check_tx_context();
  xid_t txid = current_transaction()->xid();

  auto &n = this->node_by_id(id);

  if (n.rts() > txid)
    throw transaction_abort();

  if (!n.is_locked_by(txid) && !n.try_lock(txid))
    throw transaction_abort();

  std::list<relationship::id_t> rships;
  auto relship_id = n.from_rship_list;
  while (relship_id != UNKNOWN) {
    auto& relship = rships_->get(relship_id);
    if (relship.is_locked_by(txid)) {
      assert(relship.has_dirty_versions());
      if (relship.has_valid_version(txid))
        rships.push_back(relship_id);
    }
    else if (relship.is_valid_for(txid))
      rships.push_back(relship_id);

    relship_id = relship.next_src_rship;
  }
  relship_id = n.to_rship_list;
  while (relship_id != UNKNOWN) {
    auto& relship = rships_->get(relship_id);
    if (relship.is_locked_by(txid)) {
      assert(relship.has_dirty_versions());
      if (relship.has_valid_version(txid))
        rships.push_back(relship_id);
    }
    else if (relship.is_valid_for(txid))
      rships.push_back(relship_id);

    relship_id = relship.next_dest_rship;
  }

  for (auto& r : rships) {
    spdlog::info("detach_delete_node => delete rship: {}", r);
    delete_relationship(r);
  }

  bool first_update = !n.has_dirty_versions();

  if (first_update) {
    std::list<p_item> pitems =
      node_properties_->build_dirty_property_list(n.property_list);
    const auto& oldv = n.add_dirty_version(std::make_unique<dirty_node>(n, pitems));
    oldv->elem_.set_timestamps(n.bts(), txid);
    oldv->elem_.set_dirty();
    oldv->elem_.unlock();
  }

  const auto &newv = n.add_dirty_version(std::make_unique<dirty_node>(n, std::list<p_item>(), true));
  newv->elem_.set_timestamps(txid, txid);
  newv->elem_.set_dirty();

  current_transaction()->add_dirty_node(n.id());

#if defined CSR_DELTA
#ifdef DIFF_DELTA
  current_transaction()->add_deleted_node(id);
#elif defined ADJ_DELTA
  current_transaction()->add_updated_node(id);
  current_transaction()->add_deleted_node(id);
#endif
#endif
}

void graph_db::delete_relationship(node::id_t src, node::id_t dest) {
  check_tx_context();
  xid_t txid = current_transaction()->xid();

  auto &n = this->node_by_id(src);

  auto relship_id = n.from_rship_list;
  while (relship_id != UNKNOWN) {
    auto& relship = rships_->get(relship_id);
    if (relship.is_locked_by(txid)) {
      assert(relship.has_dirty_versions());
      if (relship.has_valid_version(txid) && (relship.to_node_id() == dest))
        return delete_relationship(relship_id);
    }
    else if (relship.is_valid_for(txid) && (relship.to_node_id() == dest))
      return delete_relationship(relship_id);

    relship_id = relship.next_src_rship;
  }
}

void graph_db::delete_relationship(relationship::id_t id) {
  check_tx_context();
  xid_t txid = current_transaction()->xid();

  auto &r = this->rship_by_id(id);

  if (r.rts() > txid)
    throw transaction_abort();

  if (!r.is_locked_by(txid) && !r.try_lock(txid))
    throw transaction_abort();

  bool first_update = !r.has_dirty_versions();

  if (first_update) {
    std::list<p_item> pitems =
      rship_properties_->build_dirty_property_list(r.property_list);
    const auto& oldv = r.add_dirty_version(std::make_unique<dirty_rship>(r, pitems));
    oldv->elem_.set_timestamps(r.bts(), txid);
    oldv->elem_.set_dirty();
    oldv->elem_.unlock();
  }

  const auto &newv = r.add_dirty_version(std::make_unique<dirty_rship>(r, std::list<p_item>(), true));
  newv->elem_.set_timestamps(txid, txid);
  newv->elem_.set_dirty();

  //newv->elem_.vt_start = txid;

  current_transaction()->add_dirty_relationship(r.id());

#if defined CSR_DELTA
#ifdef DIFF_DELTA
  auto from_id = newv->elem_.from_node_id();
  auto to_id = newv->elem_.to_node_id();
  current_transaction()->add_deleted_neighbour(from_id, to_id);
  if (delta_store_->bidirectional_) {
    current_transaction()->add_deleted_neighbour(to_id, from_id);
  }
#elif defined ADJ_DELTA
  auto from_id = newv->elem_.from_node_id();
  auto to_id = newv->elem_.to_node_id();
  current_transaction()->add_updated_node(from_id);
  if (delta_store_->bidirectional_) {
    current_transaction()->add_updated_node(to_id);
  }
  current_transaction()->add_deleted_rship(id);
#endif
#endif
}

const char *graph_db::get_string(dcode_t c) { return dict_->lookup_code(c); }

dcode_t graph_db::get_code(const std::string &s) {
  return dict_->lookup_string(s);
}

void graph_db::dump() {
  nodes_->dump();
  rships_->dump();
  node_properties_->dump();
  rship_properties_->dump();
}

void graph_db::dump_dot(const std::string& fname) {
  std::ofstream out;
  out.open(fname, std::ofstream::out | std::ofstream::trunc);
  out << "digraph poseidon_db {\n";
  for (auto& n : nodes_->as_vec()) {
    out << '\t' << "n" << n.id()
        << " [label = \"" << n.id() << ":"
        << get_string(n.node_label) << "\"];" << std::endl;
  }
  for (auto& r : rships_->as_vec()) {
    out << '\t' << "n" << r.src_node << " -> n" << r.dest_node
        << " [label = \"" << get_string(r.rship_label) << "\"];" << std::endl;
  }
  out << "}";
  out.close();
}

void graph_db::copy_properties(node &n, const dirty_node_ptr& dn) {
  if (dn->properties_.empty())
    return;

  property_set::id_t pid;
  if (dn->updated()) {
#ifdef USE_PMDK
    auto log_id = current_transaction_->logid();
    auto node_id = n.id();
    auto cb = [log_id, node_id, this](offset_t oid, property_set::p_item_list& items, offset_t next) {
      pmlog::log_property_record rec(log_update,
            oid, 0, items, next, node_id);
      ulog_->append(log_id, rec);
    };
#elif defined(USE_PFILES)
    // TODO: log property update
#endif
    node_properties_->foreach_property_set(n.property_list, UNDO_CB);
    pid = node_properties_->update_pitems(n.id(), n.property_list, dn->properties_, dict_);
  } else {
#ifdef USE_PMDK
    auto log_id = current_transaction_->logid();
    auto cb = [log_id, this](offset_t p_id) {
      pmlog::log_ins_record rec(log_insert, log_property, p_id);
      ulog_->append(log_id, rec);
    };
#elif defined(USE_PFILES)
    // TODO: log property add
#endif
    pid = node_properties_->add_pitems(n.id(), dn->properties_, dict_, UNDO_CB);
  }
  n.property_list = pid;
}

void graph_db::copy_properties(relationship &r, const dirty_rship_ptr& dr) {
  if (dr->properties_.empty())
    return;

  property_set::id_t pid;
  if (dr->updated()) {
#ifdef USE_PMDK
    auto log_id = current_transaction_->logid();
    auto rship_id = r.id();
    auto cb = [log_id, rship_id, this](offset_t oid, property_set::p_item_list& items, offset_t next) {
      pmlog::log_property_record rec(log_update,
            oid, 0, items, next, rship_id);
      ulog_->append(log_id, rec);
    };
#endif
    rship_properties_->foreach_property_set(r.property_list, UNDO_CB);
    pid = rship_properties_->update_pitems(r.id(), r.property_list, dr->properties_, dict_);
  } else {
#ifdef USE_PMDK
    auto log_id = current_transaction_->logid();
    auto cb = [log_id, this](offset_t p_id) {
      pmlog::log_ins_record rec(log_insert, log_property, p_id);
      ulog_->append(log_id, rec);
    };
#endif
    pid = rship_properties_->add_pitems(r.id(), dr->properties_, dict_, UNDO_CB);
  }
  r.property_list = pid;
}

bool graph_db::has_valid_from_rships(node &n, xid_t xid) {
  auto relship_id = n.from_rship_list;
  while (relship_id != UNKNOWN) {
    spdlog::debug("node #{}, try to get from rship {}", n.id(), relship_id);
    auto &relship = rships_->get(relship_id);
    if (relship.is_locked_by(xid)) {
      assert(relship.has_dirty_versions());
      if (relship.has_valid_version(xid))
        return true;
    }
    else {
      return relship.is_valid_for(xid);
    }
    relship_id = relship.next_src_rship;
  }
  return false;
}

bool graph_db::has_valid_to_rships(node &n, xid_t xid) {
  auto relship_id = n.to_rship_list;
  while (relship_id != UNKNOWN) {
    spdlog::debug("node #{}, try to get to rship {}", n.id(), relship_id);
    auto &relship = rships_->get(relship_id);
    if (relship.is_locked_by(xid)) {
      assert(relship.has_dirty_versions());
      if (relship.has_valid_version(xid))
        return true;
    }
    else {
      return relship.is_valid_for(xid);
    }
    relship_id = relship.next_dest_rship;
  }
  return false;
}

p_item graph_db::get_property_value(const node &n, const std::string& pkey) {
  auto pc = dict_->lookup_string(pkey);
  return get_property_value(n, pc);
}

p_item graph_db::get_property_value(const node &n, dcode_t pcode) {
  return node_properties_->property_value(n.property_list, pcode);
}

p_item graph_db::get_property_value(const relationship &r, const std::string& pkey) {
  auto pc = dict_->lookup_string(pkey);
  return get_property_value(r, pc);
}

p_item graph_db::get_property_value(const relationship &r, dcode_t pcode) {
  return rship_properties_->property_value(r.property_list, pcode);
}

void graph_db::close_node_at_vt(node::id_t node_id, uint64_t vt_end_time) {
  check_tx_context();
  auto &n = node_by_id(node_id);
  current_transaction()->add_dirty_node(n.id());

  // Close the valid-time interval (AeonG style)
  n.vt_end = vt_end_time;
  
  // Archive to history
  auto old_label = n.node_label;
  properties_t empty_props{};
  history_mgr_->archive_node_delta(n, empty_props, vt_end_time, old_label);

  // Update logical index to point to history (remove from hot index)
  node_logical_index_.erase(n.logical_id());
}

void graph_db::close_relationship_at_vt(relationship::id_t rship_id, uint64_t vt_end_time) {
  check_tx_context();
  // Use rship_by_id to ensure we follow MVCC visibility for the current transaction
  auto &r = rship_by_id(rship_id);
  current_transaction()->add_dirty_relationship(r.id());

  // Close the valid-time interval (AeonG style: updating st to [st, t1))
  r.vt_end = vt_end_time;

  // Archive to history: This moves the data to the history tier while preserving properties
  auto old_label = r.rship_label;
  uint64_t src_lid = nodes_->get(r.src_node).logical_id();
  uint64_t dest_lid = nodes_->get(r.dest_node).logical_id();
  properties_t empty_props{}; // AeonG logical delete doesn't change properties, just time
  
  history_mgr_->archive_rship_delta(r, empty_props, vt_end_time, old_label, src_lid, dest_lid);

  // Update logical index: Remove from the "Hot" index so future 'current' queries skip it
  rship_logical_index_.erase(r.logical_id());
}

void graph_db::temporal_detach_delete(node::logical_id_t lid, uint64_t vt) {
    check_tx_context();
    
    // 1. Identify all physical relationship IDs for this node
    std::vector<relationship::id_t> to_close;
    // Use the engine's internal streaming to fill a batch vector
    stream_rship_lids(lid, true, [&](uint64_t r_lid) {
        auto r_phys = find_current_rship(r_lid);
        if (r_phys != UNKNOWN) to_close.push_back(r_phys);
    });
    stream_rship_lids(lid, false, [&](uint64_t r_lid) {
        auto r_phys = find_current_rship(r_lid);
        if (r_phys != UNKNOWN) to_close.push_back(r_phys);
    });

    // 2. Execute the batch closure (Logical only - AeonG style)
    for (auto r_phys : to_close) {
        this->close_relationship_at_vt(r_phys, vt); //
    }

    // 3. Close the node itself
    auto n_phys = get_node_at_vt(lid, vt - 1);
    this->close_node_at_vt(n_phys, vt); //
}

node::id_t graph_db::get_node_id_for_property(dcode_t label, dcode_t prop, uint64_t val) {
  for (auto &n : nodes_->as_vec()) {
    if (n.is_valid() && n.node_label == label) {
      uint64_t result = 0;
      auto pi = get_property_value(n, prop);
      switch (pi.typecode()) {
        case p_item::p_int:
          result = pi.get<int>();
          break;
        case p_item::p_double:
          result = (uint64_t)pi.get<double>();
          break;
        case p_item::p_uint64:
          result = pi.get<uint64_t>();
          break;
       default:
          break;
      }
      if (val == result)
        return n.id();
    }
  }
  return UNKNOWN;
}