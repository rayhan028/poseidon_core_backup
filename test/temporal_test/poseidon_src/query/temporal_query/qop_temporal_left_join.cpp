#include "qop_temporal_left_join.hpp"
#include "qresult.hpp"

namespace poseidon {

    uint64_t qop_temporal_left_join::extract_id(const query_result& item) const {
        if (item.which() == node_descr_type) return boost::get<node_description>(item).logical_id;
        if (item.which() == rship_descr_type) return boost::get<rship_description>(item).logical_id;
        if (item.which() == uint64_type) return boost::get<uint64_t>(item);
        return UNKNOWN;
    }

    // Build phase: Consume right input and build hash table
    void qop_temporal_left_join::build_right_phase(query_ctx &ctx, const qr_tuple &v) {
        if (v.size() > rhs_idx_) {
            uint64_t join_key = extract_id(v[rhs_idx_]);
            if (join_key != UNKNOWN) {
                std::unique_lock lock(m_);
                right_table_[join_key].push_back(v);
            }
        }
    }
    // Probe phase: Consume left input, probe hash table, and emit results
    void qop_temporal_left_join::probe_left_phase(query_ctx &ctx, const qr_tuple &v) {
        if (v.size() > lhs_idx_) {
            uint64_t join_key = extract_id(v[lhs_idx_]);
            if (join_key != UNKNOWN) {
                std::shared_lock lock(m_);
                auto it = right_table_.find(join_key);
                
                if (it != right_table_.end()) {
                    // Match found: Emit merged tuples
                    for (const auto& rhs_tuple : it->second) {
                        qr_tuple merged = concat(v, rhs_tuple);
                        consume(ctx, merged);
                    }
                } else {
                    //No match found on the right side
                    // Pad the tuple with NULLs and let it pass through.
                    qr_tuple padded = v;
                    for (size_t i = 0; i < rhs_pad_size_; ++i) {
                        padded.push_back(null_val);
                    }
                    consume(ctx, padded);
                }
            }
        }
    }

    void qop_temporal_left_join::finish(query_ctx &ctx) {
        std::unique_lock lock(m_);
        phases_++;
        if (phases_ == 2) {
            qop::default_finish(ctx);
        }
    }
}