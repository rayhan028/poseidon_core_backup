#include "qop_temporal_optional_expand.hpp"
#include "query_ctx.hpp"
#include "qresult.hpp"
#include "graph_db.hpp"

namespace poseidon {

    void qop_temporal_optional_expand::process(query_ctx &ctx, const qr_tuple &v) {
        if (v.empty()) return;
        
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
        bool emitted = false;

        // TOPOLOGICAL JUMP - Check checkpoints and deltas to find the correct relationship list head for the source node at the target time
        const auto& cp_index = hm.get_checkpoint_index();
        auto it_node_map = cp_index.node_checkpoints.find(src_node->logical_id);
        if (it_node_map != cp_index.node_checkpoints.end()) {
            const auto& cp_map = it_node_map->second;
            auto it_cp = cp_map.upper_bound(target_time_);
            if (it_cp != cp_map.begin()) {
                --it_cp;
                const node_checkpoint& cp = it_cp->second;
                r_head = is_out_ ? cp.from_rship_list : cp.to_rship_list;
            }
        }

        // FALLBACK RESOLUTION
        if (r_head == UNKNOWN || r_head == static_cast<offset_t>(-1)) {
            auto nid_opt = ctx.gdb_->try_get_node_at_vt(src_node->logical_id, target_time_);
            if (nid_opt.has_value()) {
                if (is_history_id(*nid_opt)) {
                    const auto& nd = hm.get_node_delta(*nid_opt);
                    r_head = is_out_ ? nd.from_rship_list : nd.to_rship_list;
                } else {
                    auto& n = ctx.gdb_->get_nodes()->get(*nid_opt);
                    r_head = is_out_ ? n.from_rship_list : n.to_rship_list;
                }
            }
        }

        offset_t curr_rid = r_head;

        // TRAVERSAL
        while (curr_rid != UNKNOWN && curr_rid != static_cast<offset_t>(-1) && curr_rid != 0) {
            relationship::logical_id_t r_lid = UNKNOWN; // Get logical ID of the relationship, handling history IDs if necessary
            if (is_history_id(curr_rid)) { // If it's a history ID, get the logical ID from the delta record
                r_lid = hm.get_rship_delta(curr_rid).lid;
            } 
            else {
                r_lid = ctx.gdb_->get_relationships()->get(curr_rid).logical_id(); // Otherwise, get it directly from the relationship record
            }

            auto valid_rid = ctx.gdb_->try_get_rship_at_vt(r_lid, target_time_);
            if (valid_rid.has_value()) { // If there's a valid relationship version at the target time, get its description and check label filter
                auto r_desc = ctx.gdb_->get_rship_description(*valid_rid); 
                if (label_filter_.empty() || r_desc.label == label_filter_) { // If label filter matches, get the neighbor node and emit the result
                    node::logical_id_t neighbor_lid = is_out_ ? r_desc.to_id : r_desc.from_id;
                    auto neighbor_nid = ctx.gdb_->try_get_node_at_vt(neighbor_lid, target_time_);
                    if (neighbor_nid.has_value()) {
                        auto neighbor_desc = ctx.gdb_->get_node_description(*neighbor_nid);
                        qr_tuple output = v;
                        output.push_back(neighbor_desc);
                        output.push_back(r_desc);
                        consume(ctx, output);
                        emitted = true;
                    }
                }
            }
            
            if (is_history_id(curr_rid)) { // Move to the next relationship in the adjacency list, handling history IDs if necessary
                const auto& rd = hm.get_rship_delta(curr_rid);
                curr_rid = is_out_ ? rd.next_src : rd.next_dest;
            } 
            else {
                auto& rel = ctx.gdb_->get_relationships()->get(curr_rid);
                curr_rid = is_out_ ? rel.next_src_rship : rel.next_dest_rship;
            }
        }

        if (!emitted) { // If no relationships were emitted, emit the original tuple with NULLs for the neighbor and relationship
            qr_tuple output = v;
            output.push_back(null_val);
            output.push_back(null_val);
            consume(ctx, output);
        }
    }
}