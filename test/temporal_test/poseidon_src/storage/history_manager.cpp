#include "history_manager.hpp"
#include "history_types.hpp"
#include "graph_snapshot.hpp"
#include "vec.hpp"
#include "properties.hpp"
#include "transaction.hpp"
#include <algorithm>
#include <unordered_map>
#include "graph_db.hpp"  

// HELPER: compare two p_items for equality
static inline bool same_value(const p_item& a, const p_item& b) {
  if (a.key() != b.key() || a.typecode() != b.typecode()) return false;
  switch (a.typecode()) {
    case p_item::p_int:     return a.get<int>()      == b.get<int>();
    case p_item::p_double:  return a.get<double>()   == b.get<double>();
    case p_item::p_uint64:  return a.get<uint64_t>() == b.get<uint64_t>();
    case p_item::p_dcode:   return a.get<dcode_t>()  == b.get<dcode_t>();
    case p_item::p_ptime:   return a.get<boost::posix_time::ptime>() == b.get<boost::posix_time::ptime>();
    case p_item::p_unused:  return true;
  }
  return false;
}

template <template <typename> typename VectorType>
HistoryManager<VectorType>::HistoryManager(std::shared_ptr<Storage> storage,std::shared_ptr<NodeProps> node_props,
                                           std::shared_ptr<RshipProps> rship_props, dict_ptr dict, thread_pool& pool, graph_db* gdb)
    : storage_(std::move(storage)),
      node_props_(std::move(node_props)),
      rship_props_(std::move(rship_props)),
      dict_(std::move(dict)),
      temporal_index_(storage_->get_btree()),
      pool_(pool) , gdb_(gdb) 
{}


template <template <typename> typename VectorType>
offset_t HistoryManager<VectorType>::archive_node_delta(const node& old_node, const properties_t& updates, uint64_t new_vt_start, dcode_t label) {
  const uint64_t lid = old_node.logical_id();
  uint32_t current_count = ++update_counts_[lid];
  // Get the previous historical version to find the existing anchor
  offset_t prev_hist_id = last_node_delta_map_.count(lid) ? last_node_delta_map_[lid] : UNKNOWN;
  property_set::id_t base_pid = UNKNOWN;
  property_set::id_t delta_pid = 0;

  // Determine whether to create a new anchor or a delta based on the adaptive interval
   uint64_t u = adaptive_interval(current_count);
   if (current_count % u == 0 || prev_hist_id == UNKNOWN) {
    // ANCHOR MODE: Store ALL current properties as a new base
    std::list<p_item> base_items = node_props_->build_dirty_property_list(old_node.property_list);
    base_pid = node_props_->add_pitems(old_node.id(), base_items, dict_);
    delta_pid = 0; // Anchors don't need an overlay
  }
  else {
    // DELTA MODE: Reuse the base pointer from the previous record
    auto prev_record = get_node_delta(prev_hist_id);
    base_pid = prev_record.base_property_list; 
    // Store ONLY the changed properties provided in the 'updates' map
    std::list<p_item> delta_items = node_props_->build_dirty_property_list(updates, dict_);
    delta_pid = node_props_->add_pitems(old_node.id(), delta_items, dict_);
  }

  // Create the optimized historical record
  node_history_delta nd { lid, label, old_node.vt_start, new_vt_start, base_pid, delta_pid, prev_hist_id, 
                          current_transaction()->xid(), UINT64_MAX, old_node.from_rship_list, old_node.to_rship_list};

  offset_t raw_id  = storage_->append_node_delta(nd); 
  offset_t hist_id = encode_history_id(raw_id);
  last_node_delta_map_[lid] = hist_id;

  // Update Indexing
  temporal_index_.remove_version(lid, old_node.vt_start, false); 
  temporal_index_.insert_version(lid, old_node.vt_start, TI_PtrType::HIST, hist_id, false);

  if (current_count % u == 0) {
      node_checkpoint cp{ lid, old_node.vt_start, base_pid, 0, old_node.from_rship_list, old_node.to_rship_list };
      checkpoint_index_.node_checkpoints[lid][old_node.vt_start] = cp;
  }

  return hist_id; 
}

template <template <typename> typename VectorType>
offset_t HistoryManager<VectorType>::archive_rship_delta(const relationship& old_r, const properties_t& updates, uint64_t new_vt_start, dcode_t label, uint64_t src_lid, uint64_t dest_lid) {
  const uint64_t lid = old_r.logical_id();
  uint32_t current_count = ++update_counts_[lid]; 

  offset_t prev_hist_id = last_rship_delta_map_.count(lid) ? last_rship_delta_map_[lid] : UNKNOWN;
  property_set::id_t base_pid = UNKNOWN;
  property_set::id_t delta_pid = 0;

  uint64_t u = adaptive_interval(current_count);
  if (current_count % u == 0 || prev_hist_id == UNKNOWN) {
    // ANCHOR MODE: Store full map
    properties_t full_map = rship_props_->all_properties(old_r.property_list, dict_);
    std::list<p_item> final_items;
    for (auto const& [key_str, val] : full_map) final_items.push_back(p_item(dict_->lookup_string(key_str), val, dict_));
    base_pid = rship_props_->add_pitems(old_r.id(), final_items, dict_);
    delta_pid = 0;
  }
  else {
    // DELTA MODE: Reuse base, store only the specific 'updates'
    auto prev_record = get_rship_delta(prev_hist_id);
    base_pid = prev_record.base_property_list;
    
    std::list<p_item> delta_items = rship_props_->build_dirty_property_list(updates, dict_);
    delta_pid = rship_props_->add_pitems(old_r.id(), delta_items, dict_);
  }

  rship_history_delta rd { lid, label, src_lid, dest_lid, old_r.vt_start, new_vt_start, base_pid, delta_pid, prev_hist_id,
                           current_transaction()->xid(), UINT64_MAX, old_r.next_src_rship, old_r.next_dest_rship};

  offset_t raw_id  = storage_->append_rship_delta(rd);
  offset_t hist_id = encode_history_id(raw_id);
  last_rship_delta_map_[lid] = hist_id;

  temporal_index_.remove_version(lid, old_r.vt_start, true);
  temporal_index_.insert_version(lid, old_r.vt_start, TI_PtrType::HIST, hist_id, true);
  return hist_id;
}

// Reconstruct a graph snapshot as of valid time t for given node and relationship logical IDs with thread pool
template <template <typename> typename VectorType>
GraphSnapshot HistoryManager<VectorType>::get_graph_as_of(uint64_t t,  const std::unordered_set<uint64_t>& node_lids,
                                 const std::unordered_set<uint64_t>& rship_lids) {
  GraphSnapshot snapshot; 
  std::vector<std::future<void>> futures; 
  auto tx = current_transaction();

  //  NODE RECONSTRUCTION 
  for (uint64_t lid : node_lids) {
    futures.push_back(pool_.submit([this, lid, t, tx, &snapshot]() {
      ::current_transaction_ = tx;
      try {
        // CALL THE DATABASE (gdb_)
        uint64_t nid = gdb_->get_node_at_vt(lid, t);
        auto desc = gdb_->get_node_description(nid); // get full description        
        std::lock_guard<std::mutex> lock(snapshot.snap_mtx);
        snapshot.nodes.push_back(desc);
      } 
      catch (...) { 
      }
    }));
  }
  // RELATIONSHIP RECONSTRUCTION 
  for (uint64_t lid : rship_lids) {
    futures.push_back(pool_.submit([this, lid, t, tx, &snapshot]() {
      ::current_transaction_ = tx;
      try {
        // CALL THE DATABASE (gdb_)
        uint64_t rid = gdb_->get_rship_at_vt(lid, t);
        auto desc = gdb_->get_rship_description(rid);        
        std::lock_guard<std::mutex> lock(snapshot.snap_mtx);
        snapshot.relationships.push_back(desc);
      } 
      catch (...) { 
      }
    }));
  }

  for (auto& f : futures) f.get();
  return snapshot;
}


template <template <typename> typename VectorType>
properties_t HistoryManager<VectorType>::resolve_node_overlay(offset_t target_id) {
    properties_t final_props;
    offset_t current_id = target_id;
    std::vector<property_set::id_t> delta_chain;

    // TRAVERSAL: Follow prev_version pointers back to the Anchor
    while (current_id != UNKNOWN && is_history_id(current_id)) {
        auto record = get_node_delta(current_id);
        // If it's a delta, save it and keep moving back
        if (record.delta_pid != 0 && record.delta_pid != UNKNOWN) {
            delta_chain.push_back(record.delta_pid);
            current_id = record.prev_version;
        } 
        // If we hit the Anchor (delta_pid == 0), fetch base and stop
        else {
            final_props = node_props_->all_properties(record.base_property_list, dict_);
            current_id = UNKNOWN; 
        }
    }
    // RECONSTRUCTION: Apply deltas in chronological order (reverse of traversal)
    for (auto it = delta_chain.rbegin(); it != delta_chain.rend(); ++it) {
        properties_t dp = node_props_->all_properties(*it, dict_);
        for (const auto& kv : dp) final_props[kv.first] = kv.second;
    }

    return final_props;
}

template <template <typename> typename VectorType>
properties_t HistoryManager<VectorType>::resolve_rship_overlay(offset_t target_id) {
    properties_t final_props;
    offset_t current_id = target_id;
    std::vector<property_set::id_t> delta_chain;

    while (current_id != UNKNOWN && is_history_id(current_id)) {
        auto record = get_rship_delta(current_id);
        
        if (record.delta_pid != 0 && record.delta_pid != UNKNOWN) {
            delta_chain.push_back(record.delta_pid);
            current_id = record.prev_version;
        } else {
            final_props = rship_props_->all_properties(record.base_property_list, dict_);
            current_id = UNKNOWN;
        }
    }

    for (auto it = delta_chain.rbegin(); it != delta_chain.rend(); ++it) {
        properties_t dp = rship_props_->all_properties(*it, dict_);
        for (const auto& kv : dp) final_props[kv.first] = kv.second;
    }

    return final_props;
}

template <template <typename> typename VectorType>
node_history_delta HistoryManager<VectorType>::get_node_delta(offset_t id) const {
  return storage_->get_node_delta(decode_history_id(id)); 
}

template <template <typename> typename VectorType>
rship_history_delta HistoryManager<VectorType>::get_rship_delta(offset_t id) const {
  return storage_->get_rship_delta(decode_history_id(id)); 
}

template <template <typename> typename VectorType>
void HistoryManager<VectorType>::index_base_node(uint64_t lid, uint64_t vt, uint64_t, uint64_t hid) {
  std::lock_guard<std::mutex> lock(index_mtx_);
  temporal_index_.insert_version(lid, vt, TI_PtrType::HIST, hid, false); 
}

template <template <typename> typename VectorType>
void HistoryManager<VectorType>::index_base_rship(uint64_t lid, uint64_t vt, uint64_t, uint64_t hid) {
  std::lock_guard<std::mutex> lock(index_mtx_);
  temporal_index_.insert_version(lid, vt, TI_PtrType::HIST, hid, true); 
}

template <template <typename> typename VectorType>
void HistoryManager<VectorType>::restore_state() { 
  // Populate Class Member Maps 
  last_node_delta_map_.clear(); 
  last_rship_delta_map_.clear(); 
  auto bt_ptr = temporal_index_.get_btree(); if (!bt_ptr) return; 
  bt_ptr->scan([&](const uint64_t key, const uint64_t val) { 
    if (ti_decode_type(val) == TI_PtrType::HIST) { 
      uint32_t lid = ti_decode_lid(key); offset_t payload = ti_decode_payload(val); 
      if ((key >> 63) != 0) last_rship_delta_map_[lid] = payload; 
      else last_node_delta_map_[lid] = payload; 
    }
  });
}

//
template <template <typename> typename VectorType>
offset_t HistoryManager<VectorType>::archive_node_delta_raw(
    uint64_t lid, dcode_t label, uint64_t vt_start, uint64_t vt_end,
    property_set::id_t pid, offset_t from_rship, offset_t to_rship,
    const properties_t &updates, uint64_t new_vt_start) 
{
    // THREAD-SAFE METADATA RESOLUTION 
    uint32_t current_count;
    offset_t prev_hist_id;
    
    {
        std::lock_guard<std::mutex> lock(index_mtx_); 
        current_count = ++update_counts_[lid];
        prev_hist_id = last_node_delta_map_.count(lid) ? last_node_delta_map_[lid] : UNKNOWN;
    }

    property_set::id_t base_pid = UNKNOWN;
    property_set::id_t delta_pid = 0;

    // DELTA: Decide between Anchor (full) vs Delta (partial) based on update count
    uint64_t u = adaptive_interval(current_count);
    if (current_count % u == 0 || prev_hist_id == UNKNOWN) {
        std::list<p_item> base_items = node_props_->build_dirty_property_list(pid);
        base_pid = node_props_->add_pitems(0, base_items, dict_);
        delta_pid = 0;
    }
    else {
        auto prev_record = get_node_delta(prev_hist_id);
        base_pid = prev_record.base_property_list;       
        std::list<p_item> delta_items = node_props_->build_dirty_property_list(updates, dict_);
        delta_pid = node_props_->add_pitems(0, delta_items, dict_);
    }

    node_history_delta nd { 
        lid, label, vt_start, new_vt_start, base_pid, delta_pid, prev_hist_id, 
        ::current_transaction_->xid(), UINT64_MAX, from_rship, to_rship
    };

    // THREAD-SAFE INDEXING & PERSISTENCE 
    offset_t raw_id  = storage_->append_node_delta(nd); 
    offset_t hist_id = encode_history_id(raw_id);

    {
        std::lock_guard<std::mutex> lock(index_mtx_);
        // Update both the chain map and the B+Tree under one lock
        last_node_delta_map_[lid] = hist_id;
        temporal_index_.remove_version(lid, vt_start, false); 
        temporal_index_.insert_version(lid, vt_start, TI_PtrType::HIST, hist_id, false);
        // Handle checkpointing under same lock to prevent race conditions
        if (current_count % u == 0) {
            node_checkpoint cp{ lid, vt_start, base_pid, 0, from_rship, to_rship };
            checkpoint_index_.node_checkpoints[lid][vt_start] = cp;
        }
    }

    return hist_id; 
}

template <template <typename> typename VectorType>
offset_t HistoryManager<VectorType>::archive_rship_delta_raw(
    uint64_t lid, dcode_t label, uint64_t src_lid, uint64_t dest_lid,
    uint64_t vt_start, uint64_t vt_end, property_set::id_t pid,
    const properties_t &updates, uint64_t new_vt_start) 
{
    // THREAD-SAFE METADATA RESOLUTION
    uint32_t current_count;
    offset_t prev_hist_id;
    
    {
        std::lock_guard<std::mutex> lock(index_mtx_);
        current_count = ++update_counts_[lid];
        prev_hist_id = last_rship_delta_map_.count(lid) ? last_rship_delta_map_[lid] : UNKNOWN;
    }

    property_set::id_t base_pid = UNKNOWN;
    property_set::id_t delta_pid = 0;

    // DELTA: Decide between Anchor (full) vs Delta (partial) based on update count
    uint64_t u = adaptive_interval(current_count);
    if (current_count % u == 0 || prev_hist_id == UNKNOWN) {
        std::list<p_item> base_items = rship_props_->build_dirty_property_list(pid);
        base_pid = rship_props_->add_pitems(0, base_items, dict_);
        delta_pid = 0;
    }
    else {
        auto prev_record = get_rship_delta(prev_hist_id);
        base_pid = prev_record.base_property_list;
        std::list<p_item> delta_items = rship_props_->build_dirty_property_list(updates, dict_);
        delta_pid = rship_props_->add_pitems(0, delta_items, dict_);
    }

    rship_history_delta rd { 
        lid, label, src_lid, dest_lid, vt_start, new_vt_start, base_pid, delta_pid, prev_hist_id,
        ::current_transaction_->xid(), UINT64_MAX, UNKNOWN, UNKNOWN
    };

    // THREAD-SAFE INDEXING & PERSISTENCE 
    offset_t raw_id  = storage_->append_rship_delta(rd);
    offset_t hist_id = encode_history_id(raw_id);

    {
        std::lock_guard<std::mutex> lock(index_mtx_);
        last_rship_delta_map_[lid] = hist_id;
        temporal_index_.remove_version(lid, vt_start, true);
        temporal_index_.insert_version(lid, vt_start, TI_PtrType::HIST, hist_id, true);
        
        // Handle checkpointing under same lock to prevent race conditions
        if (current_count % u == 0) {
            rship_checkpoint cp{ lid, vt_start, base_pid, 0 };
            checkpoint_index_.rship_checkpoints[lid][vt_start] = cp;
        }
    }

    return hist_id;
}

#ifdef USE_PFILES
template class HistoryManager<buffered_vec>;
#endif
#ifdef USE_PMDK
template class HistoryManager<nvm_chunked_vec>;
#endif
#ifdef USE_IN_MEMORY
template class HistoryManager<mem_chunked_vec>;
#endif