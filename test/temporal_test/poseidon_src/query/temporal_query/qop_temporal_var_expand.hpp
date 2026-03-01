#ifndef qop_temporal_var_expand_hpp_
#define qop_temporal_var_expand_hpp_

#include "qop.hpp"
#include <unordered_set>

namespace poseidon {
    class qop_temporal_var_expand : public enable_shared<qop, qop_temporal_var_expand> {
    public:
        // min_hops = 0 and max_hops = -1 (unlimited) for *0..
        qop_temporal_var_expand(uint64_t target_time, bool is_out = true, std::string label = "", 
                                size_t min_hops = 0, size_t max_hops = 100)
            : target_time_(target_time), is_out_(is_out), label_filter_(std::move(label)), 
              min_hops_(min_hops), max_hops_(max_hops) {
            type_ = qop_type::expand;
        }

        void process(query_ctx &ctx, const qr_tuple &v);
        void finish(query_ctx &ctx) { qop::default_finish(ctx); }
        void start(query_ctx &ctx) override { if (has_subscriber()) subscriber_->start(ctx); }
        void accept(qop_visitor &vis) override {
            if (has_subscriber()) subscriber_->accept(vis);
        }
        void dump(std::ostream &os) const override { 
            os << "temporal_var_expand(*" << min_hops_ << ".." << max_hops_ << ")"; 
        }

    private:
        uint64_t target_time_;
        bool is_out_;
        std::string label_filter_;
        size_t min_hops_;
        size_t max_hops_;
    };
}
#endif