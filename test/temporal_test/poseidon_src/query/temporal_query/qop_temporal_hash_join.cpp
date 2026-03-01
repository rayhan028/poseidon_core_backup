#include "qop_temporal_hash_join.hpp"
#include "qresult.hpp"

namespace poseidon {

    uint64_t qop_temporal_hash_join::extract_id(const query_result& item) const {
        if (item.which() == node_descr_type) return boost::get<node_description>(item).logical_id;
        if (item.which() == rship_descr_type) return boost::get<rship_description>(item).logical_id;
        if (item.which() == uint64_type) return boost::get<uint64_t>(item);
        return UNKNOWN;
    }

    // Build phase: build the hash table on the LHS tuples
    void qop_temporal_hash_join::build_phase(query_ctx &ctx, const qr_tuple &v) { 
        if (v.size() > lhs_idx_) {
            uint64_t join_key = extract_id(v[lhs_idx_]);
            if (join_key != UNKNOWN) {
                std::unique_lock lock(m_);
                htable_[join_key].push_back(v);
            }
        }
    }
    // Probe phase: for each RHS tuple, probe the hash table and produce joined tuples
    void qop_temporal_hash_join::probe_phase(query_ctx &ctx, const qr_tuple &v) {
        if (v.size() > rhs_idx_) {
            uint64_t join_key = extract_id(v[rhs_idx_]);
            if (join_key != UNKNOWN) {
                std::shared_lock lock(m_);
                auto it = htable_.find(join_key);
                if (it != htable_.end()) {
                    for (const auto& lhs_tuple : it->second) {
                        qr_tuple merged = concat(lhs_tuple, v);
                        consume(ctx, merged);
                    }
                }
            }
        }
    }

    // Clear hash table after processing
    void qop_temporal_hash_join::finish(query_ctx &ctx) {
        std::unique_lock lock(m_);
        phases_++;
        if (phases_ == 2) {
            qop::default_finish(ctx);
        }
    }
}