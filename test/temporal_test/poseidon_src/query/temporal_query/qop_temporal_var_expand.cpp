#include "qop_temporal_var_expand.hpp"
#include "history_manager.hpp"
#include "graph_db.hpp"
#include <queue>

namespace poseidon {
    void qop_temporal_var_expand::process(query_ctx &ctx, const qr_tuple &v) {
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
        std::queue<std::pair<node::logical_id_t, size_t>> bfs_queue;
        std::unordered_set<node::logical_id_t> visited;

        bfs_queue.push({src_node->logical_id, 0});
        visited.insert(src_node->logical_id);

        while (!bfs_queue.empty()) {
            auto [current_lid, hops] = bfs_queue.front();
            bfs_queue.pop();

            auto curr_nid = ctx.gdb_->try_get_node_at_vt(current_lid, target_time_);
            if (!curr_nid.has_value()) continue;
            auto curr_desc = ctx.gdb_->get_node_description(*curr_nid);

            if (hops >= min_hops_ && hops <= max_hops_) {
                qr_tuple output = v;
                if (hops > 0) output.push_back(curr_desc);
                consume(ctx, output);
            }

            if (hops >= max_hops_) continue;

            offset_t r_head = UNKNOWN;
            
            // THE TOPOLOGICAL JUMP
            const auto& cp_index = hm.get_checkpoint_index();
            auto it_node_map = cp_index.node_checkpoints.find(current_lid);
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
                if (is_history_id(*curr_nid)) {
                    const auto& nd = hm.get_node_delta(*curr_nid);
                    r_head = is_out_ ? nd.from_rship_list : nd.to_rship_list;
                } else {
                    auto& n = ctx.gdb_->get_nodes()->get(*curr_nid);
                    r_head = is_out_ ? n.from_rship_list : n.to_rship_list;
                }
            }

            offset_t curr_rid = r_head;
            while (curr_rid != UNKNOWN && curr_rid != static_cast<offset_t>(-1) && curr_rid != 0) {
                relationship::logical_id_t r_lid = UNKNOWN;
                if (is_history_id(curr_rid)) {
                    r_lid = hm.get_rship_delta(curr_rid).lid;
                } else {
                    r_lid = ctx.gdb_->get_relationships()->get(curr_rid).logical_id();
                }

                auto valid_rid = ctx.gdb_->try_get_rship_at_vt(r_lid, target_time_);
                if (valid_rid.has_value()) {
                    auto r_desc = ctx.gdb_->get_rship_description(*valid_rid);
                    if (label_filter_.empty() || r_desc.label == label_filter_) {
                        node::logical_id_t neighbor_lid = is_out_ ? r_desc.to_id : r_desc.from_id;
                        
                        if (visited.find(neighbor_lid) == visited.end()) {
                            visited.insert(neighbor_lid);
                            bfs_queue.push({neighbor_lid, hops + 1});
                        }
                    }
                }
                
                if (is_history_id(curr_rid)) {
                    curr_rid = is_out_ ? hm.get_rship_delta(curr_rid).next_src : hm.get_rship_delta(curr_rid).next_dest;
                } else {
                    auto& rel = ctx.gdb_->get_relationships()->get(curr_rid);
                    curr_rid = is_out_ ? rel.next_src_rship : rel.next_dest_rship;
                }
            }
        }
    }
}