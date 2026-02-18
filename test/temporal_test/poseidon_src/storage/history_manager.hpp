#ifndef HISTORY_MANAGER_HPP
#define HISTORY_MANAGER_HPP
#include "defs.hpp"
#include "nodes.hpp"
#include "relationships.hpp"
#include "vec.hpp"
#include "history_storage.hpp"
#include "history_types.hpp"
#include "graph_snapshot.hpp"
#include "temporal_index.hpp"
#include "dict.hpp"
#include "properties.hpp"
#include "thread_pool.hpp" 
#include <list>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <mutex>

class graph_db; // Forward declaration

// Tagged IDs: distinguish between hot IDs and archived IDs
constexpr uint64_t HISTORY_FLAG = 1ULL << 63;
inline bool is_history_id(offset_t id)    { return (id & HISTORY_FLAG) != 0; } // check if highest bit is set
inline offset_t encode_history_id(offset_t id){ return id | HISTORY_FLAG; } // set highest bit
inline offset_t decode_history_id(offset_t id){ return id & ~HISTORY_FLAG; } // clear highest bit

template <template <typename> typename VectorType>
class HistoryManager {
public:

    // Lightweight checkpoint index
    struct CheckpointIndex {
        std::unordered_map<uint64_t, std::map<uint64_t, node_checkpoint>>  node_checkpoints;   // lid -> {ts -> checkpoint}
        std::unordered_map<uint64_t, std::map<uint64_t, rship_checkpoint>> rship_checkpoints;  // lid -> {ts -> checkpoint}
    };

    // access to checkpoint index
    const CheckpointIndex& get_checkpoint_index() const { return checkpoint_index_; }

    using NodeProps = property_list<VectorType>; // property list handler for nodes
    using RshipProps = property_list<VectorType>; // property list handler for relationships
    using Storage    = HistoryStorage<VectorType>; // history storage handler

    // Build a graph snapshot at valid-time t for specified nodes and relationships
    GraphSnapshot get_graph_as_of(uint64_t t,  const std::unordered_set<uint64_t>& node_lids, const std::unordered_set<uint64_t>& rship_lids);


    // Constructor: storage, property lists, dictionary
    HistoryManager(std::shared_ptr<Storage>   storage, std::shared_ptr<NodeProps> node_props,
                   std::shared_ptr<RshipProps> rship_props, dict_ptr dict, thread_pool& pool, graph_db* gdb);

    // Create lists of changed properties (p_items) compared to a base property set
    std::list<p_item> build_node_delta(property_set::id_t base_pid, const properties_t& updates);
    std::list<p_item> build_rship_delta(property_set::id_t base_pid, const properties_t& updates);

    // Archive delta records and update temporal index
    offset_t archive_node_delta(const node& old_node, const properties_t& updates, uint64_t new_vt_start, dcode_t label);

    // Archive delta records and update temporal index
    offset_t archive_rship_delta(const relationship& old_r, const properties_t& updates, uint64_t new_vt_start, dcode_t label,
                                 uint64_t src_lid, uint64_t dest_lid);


    properties_t resolve_node_overlay(offset_t target_id);
    properties_t resolve_rship_overlay(offset_t target_id);

    // Retrieve delta records by encoded history id
	//Return by value because HistoryStorage constructs these on-the-fly
    node_history_delta  get_node_delta(offset_t encoded_hist_id)  const;
    rship_history_delta get_rship_delta(offset_t encoded_hist_id) const;

    // Indexed property change detection (optional)
    bool indexed_props_changed(dcode_t label,
                               const std::list<p_item>& old_props,
                               const std::list<p_item>& new_props,
                               const std::unordered_set<dcode_t>& indexed_keys) const;

    // to return the nodes and relations which doesnt have a delta
    // Pass encoded history ID, index it as HIST at vt_start.
    void index_base_node(uint64_t lid, uint64_t vt_start, uint64_t vt_end, uint64_t hist_id);
    void index_base_rship(uint64_t lid, uint64_t vt_start, uint64_t vt_end, uint64_t hist_id);

    void restore_state(); // to scan the temporal_index_(B-Tree) and fills the g_last_node_delta_offset map
                            //with the latest offsets found- resolving segmentation fault issue on restart

    void runtime_initialize();

    TemporalIndex<temporal_btree_ptr_t>& get_temporal_index() {
        return temporal_index_;
    }

    std::shared_ptr<Storage> get_storage() const {
        return storage_;
    }

    //  access to checkpoint index for updates
    CheckpointIndex& get_checkpoint_index() {
        return checkpoint_index_;
    }

    offset_t get_last_node_delta(uint64_t lid) const {
        std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(index_mtx_));
        auto it = last_node_delta_map_.find(lid);
        return (it != last_node_delta_map_.end()) ? it->second : UNKNOWN;
    }

    offset_t get_last_rship_delta(uint64_t lid) const {
        std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(index_mtx_));
        auto it = last_rship_delta_map_.find(lid);
        return (it != last_rship_delta_map_.end()) ? it->second : UNKNOWN;
    }

    void update_last_node_delta(uint64_t lid, offset_t hist_id) {
        std::lock_guard<std::mutex> lock(index_mtx_);
        last_node_delta_map_[lid] = hist_id;
    }

    void update_last_rship_delta(uint64_t lid, offset_t hist_id) {
        std::lock_guard<std::mutex> lock(index_mtx_);
        last_rship_delta_map_[lid] = hist_id;
    }

    // Indexing for Nodes
    void index_hot_node(uint64_t lid, uint64_t vt, offset_t phys_id) {
        std::lock_guard<std::mutex> lock(index_mtx_);
        temporal_index_.insert_version(lid, vt, TI_PtrType::HOT, phys_id, false);
    }

    void index_hist_node(uint64_t lid, uint64_t vt, offset_t hist_id) {
        std::lock_guard<std::mutex> lock(index_mtx_);
        temporal_index_.insert_version(lid, vt, TI_PtrType::HIST, hist_id, false);
    }

    //  Indexing for Relationships
    void index_hot_rship(uint64_t lid, uint64_t vt, offset_t phys_id) {
        std::lock_guard<std::mutex> lock(index_mtx_);
        temporal_index_.insert_version(lid, vt, TI_PtrType::HOT, phys_id, true);
    }

    void index_hist_rship(uint64_t lid, uint64_t vt, offset_t hist_id) {
        std::lock_guard<std::mutex> lock(index_mtx_);
        temporal_index_.insert_version(lid, vt, TI_PtrType::HIST, hist_id, true);
    }

    thread_pool& get_pool() { return pool_; } // access to thread pool

// For Nodes
    offset_t archive_node_delta_raw( // Pass logical ID, label, valid-time range, property changes, and new valid-time start for the delta record
        uint64_t lid, dcode_t label, uint64_t vt_start, uint64_t vt_end,
        property_set::id_t pid, offset_t from_rship, offset_t to_rship,
        const properties_t &updates, uint64_t new_vt_start
    );

    // For Relationships
    offset_t archive_rship_delta_raw( // Pass logical ID, label, source/destination lids, valid-time range, property changes, and new valid-time start for the delta record
        uint64_t lid, dcode_t label, uint64_t src_lid, uint64_t dest_lid,
        uint64_t vt_start, uint64_t vt_end, property_set::id_t pid,
        const properties_t &updates, uint64_t new_vt_start
    );

    static inline uint64_t isqrt_u64(uint64_t x) { // sqrt implementation for adaptive interval calculation
        uint64_t r = 0;
        uint64_t bit = 1ULL << 62;
        while (bit > x) bit >>= 2;
        while (bit) {
            if (x >= r + bit) {
                x -= r + bit;
                r = (r >> 1) + bit;
        } 
        else {
            r >>= 1;
        }
        bit >>= 2;
    }
    return r;
}

static inline uint64_t snap_pow2(uint64_t x) { // snap to nearest power of 2 for adaptive interval calculation
    uint64_t p = 1;
    while (p < x) p <<= 1;
    uint64_t lower = p >> 1;
    if (lower < 1) return 1;
    return (x - lower <= p - x) ? lower : p;
}

private:
    thread_pool& pool_;
    // References
    std::shared_ptr<Storage>    storage_; // history storage
    std::shared_ptr<NodeProps>  node_props_; // node property list handler
    std::shared_ptr<RshipProps> rship_props_; // relationship property list handler
    dict_ptr                    dict_; // dictionary for string-ID mapping


    std::unordered_map<uint64_t, uint32_t> update_counts_;
    std::unordered_map<uint64_t, offset_t> last_node_delta_map_; // map of last node delta offsets per logical ID
    std::unordered_map<uint64_t, offset_t> last_rship_delta_map_; // map of last relationship delta offsets per logical ID
    // Checkpoint index
    CheckpointIndex checkpoint_index_;
    // Temporal index for as-of lookups
    TemporalIndex<temporal_btree_ptr_t> temporal_index_; // temporal index (B-Tree) for version lookups
    
    static inline uint64_t adaptive_interval(uint64_t freq) {
        // LOWER value = More snapshots (Anchors), which means HIGHER memory usage.
        constexpr double COST_RATIO_WEIGHT = 8.0; 
        constexpr uint64_t MIN_U = 64;  
        constexpr uint64_t MAX_U = 1024; 
        if (freq < 64) return MIN_U;
        uint64_t u = static_cast<uint64_t>(isqrt_u64(freq) * COST_RATIO_WEIGHT);
        u = std::clamp<uint64_t>(u, MIN_U, MAX_U);
        return snap_pow2(u);
    }

    graph_db* gdb_; 
    std::mutex index_mtx_;

};
#endif // HISTORY_MANAGER_HPP