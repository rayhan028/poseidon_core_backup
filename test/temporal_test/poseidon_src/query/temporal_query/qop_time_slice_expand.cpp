#include "qop_time_slice_expand.hpp"
#include "query_ctx.hpp"
#include "qresult.hpp"
#include "graph_db.hpp"
#include "history_manager.hpp"

namespace poseidon {

void qop_time_slice_expand::process(query_ctx &ctx, const qr_tuple &v) {
    const node_description* src_node = nullptr;
    for (const auto& res : v) {
        if (res.which() == node_descr_type) {
            src_node = &boost::get<node_description>(res);
            break;
        }
    }
    if (!src_node) return;
    auto& gdb = *ctx.gdb_;
    auto& hm = *gdb.get_history_manager();
    node::logical_id_t src_lid = src_node->logical_id;
    // Find the first source version in the window via B-Tree JUMP
    auto start_src_opt = gdb.try_get_node_at_vt(src_lid, end_vt_ - 1);
    if (!start_src_opt.has_value()) return;
    offset_t curr_src_nid = *start_src_opt;
    while (curr_src_nid != UNKNOWN && curr_src_nid != 0) {
        // Fetch delta to get temporal boundaries and the prev_version pointer
        node_history_delta sn_delta = hm.get_node_delta(is_history_id(curr_src_nid) ? curr_src_nid : encode_history_id(curr_src_nid));
        // Window Overlap Check for Source Node
        const uint64_t S1 = std::max(sn_delta.vt_start, start_vt_);
        const uint64_t S2 = std::min(sn_delta.vt_end, end_vt_);

        if (S1 < S2) {
            // Use the ADJACENCY ANCHOR saved in the node delta
            // This 'jumps' over all relationships that weren't active for this node version
            offset_t curr_rid = is_out_ ? sn_delta.from_rship_list : sn_delta.to_rship_list;
            while (curr_rid != UNKNOWN && curr_rid != 0) {
                auto r_desc = gdb.get_rship_description(curr_rid);               
                // Check if relationship overlaps the active window of the source version
                if (r_desc.vt_start < S2 && r_desc.vt_end > S1) {
                    if (label_filter_.empty() || r_desc.label == label_filter_) {
                        const uint64_t E1 = std::max(S1, r_desc.vt_start);                       
                        // NEIGHBOR RESOLUTION via B-TREE JUMP
                        // Instead of walking neighbor history, jump to the neighbor version at time E1
                        node::logical_id_t neighbor_lid = is_out_ ? r_desc.to_id : r_desc.from_id;
                        auto nbr_opt = gdb.try_get_node_at_vt(neighbor_lid, E1);
                        
                        if (nbr_opt.has_value()) {
                            consume(ctx, {gdb.get_node_description(*nbr_opt), r_desc});
                        }
                    }
                }
                // Advance through the adjacency list
                if (is_history_id(curr_rid)) {
                    const auto& rd = hm.get_rship_delta(curr_rid);
                    curr_rid = is_out_ ? rd.next_src : rd.next_dest;
                } else {
                    auto& rel = gdb.get_relationships()->get(curr_rid);
                    curr_rid = is_out_ ? rel.next_src_rship : rel.next_dest_rship;
                }
            }
        }
        // BOUNDED EXIT: Stop walking back if version starts before our window
        if (sn_delta.vt_start <= start_vt_) break;
        curr_src_nid = sn_delta.prev_version;
    }
}
}