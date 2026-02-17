#include "qop_timeline_scan.hpp"
#include "query_ctx.hpp"
#include "qresult.hpp"
#include "graph_db.hpp"
#include "history_manager.hpp"

namespace poseidon {

    void qop_timeline_scan::process(query_ctx &ctx, const qr_tuple &v) {
        if (v.empty()) return;
        uint64_t lid = UNKNOWN;
        if (v[0].which() == node_descr_type) {
            lid = boost::get<node_description>(v[0]).logical_id;
        } else {
            try { lid = boost::get<uint64_t>(v[0]); } catch(...) { return; }
        }
        if (lid == UNKNOWN) return;

        auto& hm = *ctx.gdb_->get_history_manager();

        // THE TOPOLOGICAL JUMP: Teleport to the end of the query window using the B-Tree
        auto start_id_opt = is_node_ ? ctx.gdb_->try_get_node_at_vt(lid, end_time_ - 1) 
                                     : ctx.gdb_->try_get_rship_at_vt(lid, end_time_ - 1);
        
        if (!start_id_opt.has_value()) return;
        offset_t curr_id = *start_id_opt;

        while (curr_id != UNKNOWN && curr_id != 0) {
            offset_t phys_id = is_history_id(curr_id) ? curr_id : encode_history_id(curr_id);            
            if (is_node_) {
                node_history_delta delta = hm.get_node_delta(phys_id);
                if (delta.vt_start < end_time_ && delta.vt_end > start_time_) {
                    consume(ctx, {ctx.gdb_->get_node_description(phys_id)});
                }
                // BOUNDED EXIT: Stop walking immediately once we are older than start_time_
                if (delta.vt_start <= start_time_) break; 
                curr_id = delta.prev_version;
            } 
            else {
                rship_history_delta delta = hm.get_rship_delta(phys_id);
                if (delta.vt_start < end_time_ && delta.vt_end > start_time_) {
                    consume(ctx, {ctx.gdb_->get_rship_description(phys_id)});
                }
                if (delta.vt_start <= start_time_) break;
                curr_id = delta.prev_version;
            }
        }
    }
}