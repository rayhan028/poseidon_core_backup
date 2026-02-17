#include "qop_temporal_expand.hpp"
#include "query_ctx.hpp"
#include "qresult.hpp"
#include "graph_db.hpp"

namespace poseidon {
    void qop_temporal_expand::process(query_ctx &ctx, const qr_tuple &v) {
        if (v.empty()) return;
        // Extract source node from the input tuple
        const node_description* src_node = nullptr;
        for (const auto& res : v) {
            if (res.which() == node_descr_type) {
                src_node = &boost::get<node_description>(res);
                break;
            }
        }
        if (!src_node) return;

        auto& hm = *ctx.gdb_->get_history_manager();
        offset_t r_head = UNKNOWN;

        // THE TOPOLOGICAL JUMP 
        // Access the CheckpointIndex in history_manager.cpp
        const auto& cp_index = hm.get_checkpoint_index();
        auto it_node_map = cp_index.node_checkpoints.find(src_node->logical_id);
        if (it_node_map != cp_index.node_checkpoints.end()) {
            const auto& cp_map = it_node_map->second;
            // Find the latest checkpoint valid at or before target_time_
            auto it_cp = cp_map.upper_bound(target_time_);
            if (it_cp != cp_map.begin()) {
                --it_cp;
                const node_checkpoint& cp = it_cp->second;
                // Jump directly to the adjacency list head at this checkpoint
                r_head = is_out_ ? cp.from_rship_list : cp.to_rship_list;
            }
        }

        // FALLBACK RESOLUTION 
        // If no checkpoint exists (e.g., node has < 8 updates), use standard version resolution
        if (r_head == UNKNOWN || r_head == static_cast<offset_t>(-1)) {
            auto nid_opt = ctx.gdb_->try_get_node_at_vt(src_node->logical_id, target_time_);
            if (!nid_opt.has_value()) return;

            if (is_history_id(*nid_opt)) {
                const auto& nd = hm.get_node_delta(*nid_opt);
                r_head = is_out_ ? nd.from_rship_list : nd.to_rship_list;
            } else {
                auto& n = ctx.gdb_->get_nodes()->get(*nid_opt);
                r_head = is_out_ ? n.from_rship_list : n.to_rship_list;
            }
        }

        if (r_head == UNKNOWN || r_head == static_cast<offset_t>(-1)) return;

        // STEP 4: BOUNDED ADJACENCY TRAVERSAL ---
        offset_t curr_rid = r_head;
        while (curr_rid != UNKNOWN) {
            relationship::logical_id_t r_lid = UNKNOWN;
            if (is_history_id(curr_rid)) {
                r_lid = hm.get_rship_delta(curr_rid).lid;
            } else {
                r_lid = ctx.gdb_->get_relationships()->get(curr_rid).logical_id();
            }
            // Resolve relationship version at target_time_
            auto valid_rid = ctx.gdb_->try_get_rship_at_vt(r_lid, target_time_);
            if (valid_rid.has_value()) {
                auto r_desc = ctx.gdb_->get_rship_description(*valid_rid);              
                // Optional Label Filter
                if (!label_filter_.empty() && r_desc.label != label_filter_) goto advance;
                // Resolve neighbor node at target_time_
                node::logical_id_t neighbor_lid = is_out_ ? r_desc.to_id : r_desc.from_id;
                auto neighbor_nid = ctx.gdb_->try_get_node_at_vt(neighbor_lid, target_time_);
                if (neighbor_nid.has_value()) {
                    auto neighbor_desc = ctx.gdb_->get_node_description(*neighbor_nid);
                    qr_tuple output;
                    output.push_back(neighbor_desc);
                    output.push_back(r_desc);
                    consume(ctx, output);
                }
            }

        advance:
            // Advance through the list using prev_version pointers in RAM
            if (is_history_id(curr_rid)) {
                const auto& rd = hm.get_rship_delta(curr_rid);
                curr_rid = is_out_ ? rd.next_src : rd.next_dest;
            } else {
                auto& rel = ctx.gdb_->get_relationships()->get(curr_rid);
                curr_rid = is_out_ ? rel.next_src_rship : rel.next_dest_rship;
            }
        }
    }
}