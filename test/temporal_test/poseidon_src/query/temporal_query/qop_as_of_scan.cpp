#include "qop_as_of_scan.hpp"
#include "query_ctx.hpp"
#include "qresult.hpp"

namespace poseidon {
    // Process incoming tuple
    void qop_as_of_scan::process(query_ctx &ctx, const qr_tuple &v) {
        if (v.empty() || lid_index_ >= v.size()) return; // Safety check

        uint64_t lid = UNKNOWN;

        // EXTRACTION: Peek at types instead of indices
        if (auto* id_ptr = boost::get<uint64_t>(&v[lid_index_])) { // Direct LID
            lid = *id_ptr; // Extract LID
        } else if (auto* n_ptr = boost::get<node_description>(&v[lid_index_])) { // Node description
            lid = n_ptr->logical_id; // Extract LID
        } else if (auto* r_ptr = boost::get<rship_description>(&v[lid_index_])) { // Rship description
            lid = r_ptr->logical_id; // Extract LID
        }

        if (lid == UNKNOWN) return;

        if (is_node_) { // Node scan
            auto nid_opt = ctx.gdb_->try_get_node_at_vt(lid, target_time_); // Try get node ID at VT
            if (nid_opt.has_value()) { // If found
                node_description desc = ctx.gdb_->get_node_description(nid_opt.value()); // Get description
                consume(ctx, {desc}); // Emit result
            }
        } else { // Rship scan
            auto rid_opt = ctx.gdb_->try_get_rship_at_vt(lid, target_time_); // Try get rship ID at VT
            if (rid_opt.has_value()) { // If found
                rship_description desc = ctx.gdb_->get_rship_description(rid_opt.value()); // Get description
                consume(ctx, {desc}); // Emit result
            }
        }
    }
}