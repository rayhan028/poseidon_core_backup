/**
* HistoryStorage
 * Append-only archive for outdated committed versions.
 */
#ifndef HISTORY_STORAGE_HPP
#define HISTORY_STORAGE_HPP

#include "nodes.hpp"
#include "relationships.hpp"
#include "vec.hpp"
#include "defs.hpp"
#include "history_types.hpp"
#include "btree.hpp"
#include <memory>
#include <vector>
#include <cstddef>
#include <unordered_map>
#include <algorithm>
#include <mutex>

#ifdef USE_PMDK
using temporal_btree_ptr_t = nvm_btree_ptr;
#elif defined(USE_IN_MEMORY)
using temporal_btree_ptr_t = im_btree_ptr;
#else
using temporal_btree_ptr_t = pf_btree_ptr;
#endif

template <template <typename> typename VectorType>
class HistoryStorage {
public:
    using NodeHistoryVec  = node_list<VectorType>; // node_list is defined in history_types
    using RshipHistoryVec = relationship_list<VectorType>; // relationship_list is defined in history_types

    std::shared_ptr<NodeHistoryVec>  nodes_; // Shared pointer to node history vector
    std::shared_ptr<RshipHistoryVec> rships_; // Shared pointer to relationship history vector

    // Use PACKED records for storage (48 Bytes)
    std::vector<node_history_record>  node_deltas_; // Packed storage for node history deltas
    std::vector<rship_history_record> rship_deltas_; // Packed storage for relationship history deltas

    // Eviction flags (do not erase vectors to keep offsets stable)
    std::vector<uint8_t> node_delta_evicted_; // Eviction flags for node deltas
    std::vector<uint8_t> rship_delta_evicted_; // Eviction flags for relationship deltas

    std::unordered_map<uint64_t, std::vector<offset_t>> node_delta_index_; // Index mapping node logical IDs to their delta offsets
    std::unordered_map<uint64_t, std::vector<offset_t>> rship_delta_index_; // Index mapping relationship logical IDs to their delta offsets

    // Config
    void set_max_delta_chain_length(std::size_t n) { max_delta_chain_length_ = n; } // 0 = unlimited
    void set_max_full_object_versions(std::size_t n) { max_full_object_versions_ = n; } // 0 = unlimited
    // Reserve space for node deltas
    void reserve_node_deltas(std::size_t n)  { node_deltas_.reserve(n); node_delta_evicted_.reserve(n); }
    // Reserve space for relationship deltas
    void reserve_rship_deltas(std::size_t n) { rship_deltas_.reserve(n); rship_delta_evicted_.reserve(n); }

    std::size_t node_delta_count()  const { return node_deltas_.size(); } // Current count of node deltas
    std::size_t rship_delta_count() const { return rship_deltas_.size(); } // Current count of relationship deltas

    offset_t append_node_delta(const node_history_delta& d) { // Pack on Write
        std::lock_guard<std::mutex> lock(storage_mtx_);
        // Full API struct -> Packed Storage struct
        node_history_record record;
        record.lid = d.lid;
        record.label = d.label;
        record.vt_start = d.vt_start;
        record.vt_end   = d.vt_end;
        record.base_property_list = d.base_property_list;
        record.delta_pid = d.delta_pid;
        record.prev_version = d.prev_version;
        record.tt_start = d.tt_start;
        record.tt_end   = d.tt_end;
        record.from_rship_list = d.from_rship_list;
        record.to_rship_list   = d.to_rship_list;
        // Push Packed Record
        node_deltas_.push_back(record);
        // Update Metadata (Indices/Eviction)
        node_delta_evicted_.push_back(0);
        const offset_t id = static_cast<offset_t>(node_deltas_.size() - 1); // New Delta ID is last index

        auto& chain = node_delta_index_[d.lid]; // Get or create chain for this logical ID
        chain.push_back(id); // Append new delta ID to chain
        enforce_node_chain_cap(chain); // Enforce chain length cap

        return id;
    }

    // Unpack on Read
    node_history_delta get_node_delta(offset_t id) const {
        // Get Packed Record
        const auto& record = node_deltas_.at(id);
        // Inflate to Full API struct
        node_history_delta d;
        d.lid = record.lid;
        d.label = record.label;
        d.vt_start = record.vt_start;
        d.vt_end   = record.vt_end;
        d.base_property_list = record.base_property_list;
        d.delta_pid = record.delta_pid;
        d.prev_version = record.prev_version;
        d.tt_start = record.tt_start;
        d.tt_end   = record.tt_end;
        d.from_rship_list = record.from_rship_list; //
        d.to_rship_list   = record.to_rship_list;   //

        return d;
    }

    bool is_node_delta_evicted(offset_t id) const { // Check eviction status for node delta
        return node_delta_evicted_.at(id) != 0;
    }

    // Pack on Write (Relationships)
    offset_t append_rship_delta(const rship_history_delta& d) {
        std::lock_guard<std::mutex> lock(storage_mtx_);
        rship_history_record record;
        record.lid = d.lid;
        record.label = d.label;
        record.src_lid = d.src_lid;
        record.dest_lid = d.dest_lid;
        record.vt_start = d.vt_start;
        record.vt_end   = d.vt_end;
        record.base_property_list = d.base_property_list;
        record.delta_pid = d.delta_pid;
        record.prev_version = d.prev_version;
        record.tt_start = d.tt_start;
        record.tt_end   = d.tt_end;
        record.next_src  = d.next_src;  //
        record.next_dest = d.next_dest; //
        rship_deltas_.push_back(record);
        rship_delta_evicted_.push_back(0);
        const offset_t id = static_cast<offset_t>(rship_deltas_.size() - 1);

        auto& chain = rship_delta_index_[d.lid];
        chain.push_back(id);
        enforce_rship_chain_cap(chain);

        return id;
    }

    // Unpack on Read (Relationships)
    rship_history_delta get_rship_delta(offset_t id) const {
        const auto& record = rship_deltas_.at(id);

        rship_history_delta d;
        d.lid = record.lid;
        d.label = record.label;
        d.src_lid = record.src_lid;
        d.dest_lid = record.dest_lid;
        d.vt_start = record.vt_start;
        d.vt_end   = record.vt_end;
        d.base_property_list = record.base_property_list;
        d.delta_pid = record.delta_pid;
        d.prev_version = record.prev_version;
        d.tt_start = record.tt_start;
        d.tt_end   = record.tt_end;
        d.next_src  = record.next_src;  //
        d.next_dest = record.next_dest; //

        return d;
    }

    bool is_rship_delta_evicted(offset_t id) const {
        return rship_delta_evicted_.at(id) != 0; // Check eviction status for relationship delta
    }

#ifdef USE_PFILES
    HistoryStorage(bufferpool &bp, offset_t node_file_id, offset_t rship_file_id) {
        nodes_  = std::make_shared<NodeHistoryVec>(bp, node_file_id);
        rships_ = std::make_shared<RshipHistoryVec>(bp, rship_file_id);
    }
#else
    HistoryStorage() {
        nodes_  = std::make_shared<NodeHistoryVec>();
        rships_ = std::make_shared<RshipHistoryVec>();
    }
#endif

    // Full-object archiving, Pruning, Initialization, B-Tree setters
    offset_t append_node_history(node&& n) { // Append full node version to history
        // VT filtering (if enabled)
        if (vt_filter_enabled_ && !vt_filter_node(n)) return static_cast<offset_t>(-1);
        const offset_t hid = nodes_->append(std::move(n), 0); // Append node to history
        //prune_node_history(n.logical_id(), 2); // Prune old versions by logical ID
        return hid; // Return history ID
    }

    node& get_node(offset_t history_id) { // Retrieve node from history by ID
        return nodes_->get(history_id); // Get node from history vector
    }

    offset_t append_rship_history(relationship&& r) { // Append full relationship version to history
        if (vt_filter_enabled_ && !vt_filter_rship(r)) return static_cast<offset_t>(-1);
        const offset_t hid = rships_->append(std::move(r), 0);
        //prune_rship_history(r.logical_id(), 2);
        return hid;
    }

    relationship& get_rship(offset_t history_id) { // Retrieve relationship from history by ID
        return rships_->get(history_id); // Get relationship from history vector
    }

    void runtime_initialize() { // Initialize runtime structures
        nodes_->runtime_initialize();
        rships_->runtime_initialize();
    }

    void set_vt_filter_enabled(bool enabled) { vt_filter_enabled_ = enabled; } // Enable/disable VT filtering

    bool vt_filter_node(const node& n) const { return (n.vt_end > n.vt_start); } // VT filtering for nodes
    bool vt_filter_rship(const relationship& r) const { return (r.vt_end > r.vt_start); } // VT filtering for relationships

    // PRUNING BY LOGICAL ID FOR NODES
    void prune_node_history(node::logical_id_t lid, std::size_t max_versions = 5) { // Prune old node versions by logical ID
        std::vector<offset_t> ids; // Collect all history IDs for the given logical ID
        collect_node_ids_for_lid(lid, ids); // Helper function to collect IDs
        if (ids.size() > max_versions) { // If more than max_versions exist
            const std::size_t to_remove = ids.size() - max_versions; // Calculate how many to remove
            for (std::size_t i = 0; i < to_remove; ++i) nodes_->remove(ids[i]); // Remove oldest versions
        }
    }

    // PRUNING BY LOGICAL ID FOR RELATIONSHIPS
    void prune_rship_history(relationship::logical_id_t lid, std::size_t max_versions = 5) {
        std::vector<offset_t> ids; // Collect all history IDs for the given logical ID
        collect_rship_ids_for_lid(lid, ids); // Helper function to collect IDs
        if (ids.size() > max_versions) { // If more than max_versions exist
            const std::size_t to_remove = ids.size() - max_versions; // Calculate how many to remove
            for (std::size_t i = 0; i < to_remove; ++i) rships_->remove(ids[i]); // Remove oldest versions
        }
    }

    std::size_t node_chunk_count() const { return nodes_->num_chunks(); } // Get number of node chunks
    std::size_t rship_chunk_count() const { return rships_->num_chunks(); } // Get number of relationship chunks

    void set_btree(temporal_btree_ptr_t ptr) { btree_ = std::move(ptr); } // Set B-Tree pointer for temporal indexing
    temporal_btree_ptr_t get_btree() const { return btree_; } // Get B-Tree pointer for temporal indexing

private:
    bool vt_filter_enabled_{false}; // VT filtering flag
    //std::size_t max_delta_chain_length_{64}; // 0 = unlimited
    std::size_t max_delta_chain_length_{0};
    std::size_t max_full_object_versions_{2}; // 0 = unlimited
    std::mutex storage_mtx_;

    temporal_btree_ptr_t btree_{}; // B-Tree pointer for temporal indexing

    void enforce_node_chain_cap(std::vector<offset_t>& chain) { // Enforce maximum chain length for node deltas
        if (max_delta_chain_length_ == 0) return; // 0 means unlimited
        if (chain.size() <= max_delta_chain_length_) return; // No action needed
        const std::size_t excess = chain.size() - max_delta_chain_length_; // Calculate excess length
        for (std::size_t i = 0; i < excess; ++i) { // Mark excess deltas as evicted
            const offset_t old_id = chain[i]; // Get old delta ID
            node_delta_evicted_[old_id] = 1; // Mark as evicted
        }
        // Remove excess from the front of the chain
        chain.erase(chain.begin(), chain.begin() + static_cast<std::ptrdiff_t>(excess));
    }

    // Similar function for relationship deltas
    void enforce_rship_chain_cap(std::vector<offset_t>& chain) { // Enforce maximum chain length for relationship deltas
        if (max_delta_chain_length_ == 0) return;
        if (chain.size() <= max_delta_chain_length_) return;
        const std::size_t excess = chain.size() - max_delta_chain_length_;
        for (std::size_t i = 0; i < excess; ++i) {
            const offset_t old_id = chain[i];
            rship_delta_evicted_[old_id] = 1;
        }
        chain.erase(chain.begin(), chain.begin() + static_cast<std::ptrdiff_t>(excess));
    }

    // retention & Helper collectors
    void enforce_node_full_retention(offset_t /*last_hid*/) {} // Placeholder for node full retention enforcement
    void enforce_rship_full_retention(offset_t /*last_hid*/) {} // Placeholder for relationship full retention enforcement

    // Helper for Nodes
    void collect_all_node_ids_grouped(std::unordered_map<node::logical_id_t, std::vector<offset_t>>& out) {
        const std::size_t nchunks = nodes_->num_chunks(); // Get number of node chunks
        for (std::size_t first = 0; first < nchunks; ++first) { // Iterate over chunks
            const std::size_t last = first; // Single chunk range
            auto it = nodes_->range(first, last, 0); // Get iterator for the chunk
            while (it) { // Iterate through nodes in the chunk
                node& n = *it; // Get current node
                out[n.logical_id()].push_back(n.id()); // Group by logical ID
                ++it; // Move to next node
            }
        }
        // Sort the ID lists for each logical ID
        for (auto& kv : out) std::sort(kv.second.begin(), kv.second.end());
    }
    // Helper for Relationships
    void collect_all_rship_ids_grouped(std::unordered_map<relationship::logical_id_t, std::vector<offset_t>>& out) {
        const std::size_t nchunks = rships_->num_chunks(); // Get number of relationship chunks
        for (std::size_t first = 0; first < nchunks; ++first) { // Iterate over chunks
            const std::size_t last = first; // Single chunk range
            auto it = rships_->range(first, last); // Get iterator for the chunk
            while (it) { // Iterate through relationships in the chunk
                relationship& r = *it; // Get current relationship
                out[r.logical_id()].push_back(r.id());// Group by logical ID
                ++it; // Move to next relationship
            }
        }
        // Sort the ID lists for each logical ID
        for (auto& kv : out) std::sort(kv.second.begin(), kv.second.end());
    }

    // Helper for Nodes
    void collect_node_ids_for_lid(node::logical_id_t lid, std::vector<offset_t>& out) {
        const std::size_t nchunks = nodes_->num_chunks(); // Get number of node chunks
        for (std::size_t first = 0; first < nchunks; ++first) { // Iterate over chunks
            const std::size_t last = first; // Single chunk range
            auto it = nodes_->range(first, last, 0); // Get iterator for the chunk
            while (it) { // Iterate through nodes in the chunk
                node& n = *it; // Get current node
                if (n.logical_id() == lid) out.push_back(n.id()); // If logical ID matches, add to output
                ++it;  // Move to next node
            }
        }
        std::sort(out.begin(), out.end()); // Sort the collected IDs
    }
    // Helper for Relationships
    void collect_rship_ids_for_lid(relationship::logical_id_t lid, std::vector<offset_t>& out) {
        const std::size_t nchunks = rships_->num_chunks(); // Get number of relationship chunks
        for (std::size_t first = 0; first < nchunks; ++first) { // Iterate over chunks
            const std::size_t last = first; // Single chunk range
            auto it = rships_->range(first, last); // Get iterator for the chunk
            while (it) { // Iterate through relationships in the chunk
                relationship& r = *it; // Get current relationship
                if (r.logical_id() == lid) out.push_back(r.id()); // If logical ID matches, add to output
                ++it; // Move to next relationship
            }
        }
        // Sort the collected IDs
        std::sort(out.begin(), out.end());
    }
};

#endif // HISTORY_STORAGE_HPP