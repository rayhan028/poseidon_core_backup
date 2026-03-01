#ifndef qop_temporal_left_join_hpp_
#define qop_temporal_left_join_hpp_

#include "qop.hpp"
#include <unordered_map>
#include <shared_mutex>

namespace poseidon {
    class qop_temporal_left_join : public enable_shared<qop, qop_temporal_left_join> {
    public:
        qop_temporal_left_join(size_t lhs_idx, size_t rhs_idx, size_t rhs_expected_size) 
            : lhs_idx_(lhs_idx), rhs_idx_(rhs_idx), rhs_pad_size_(rhs_expected_size), phases_(0) {
            type_ = qop_type::left_join;
        }

        void build_right_phase(query_ctx &ctx, const qr_tuple &v);
        void probe_left_phase(query_ctx &ctx, const qr_tuple &v);
        
        void finish(query_ctx &ctx);
        void start(query_ctx &ctx) override { if (has_subscriber()) subscriber_->start(ctx); }
        void accept(qop_visitor &vis) override {
            if (has_subscriber()) subscriber_->accept(vis);
        }
        void dump(std::ostream &os) const override { os << "temporal_left_join"; }
        bool is_binary() const override { return true; }

    private:
        uint64_t extract_id(const query_result& item) const;

        size_t lhs_idx_, rhs_idx_, rhs_pad_size_;
        std::size_t phases_;
        std::unordered_map<uint64_t, std::vector<qr_tuple>> right_table_;
        mutable std::shared_mutex m_;
    };
}
#endif