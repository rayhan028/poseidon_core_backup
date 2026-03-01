#ifndef qop_temporal_optional_expand_hpp_
#define qop_temporal_optional_expand_hpp_

#include "qop.hpp"

namespace poseidon {
    class qop_temporal_optional_expand : public enable_shared<qop, qop_temporal_optional_expand> {
    public:
        qop_temporal_optional_expand(uint64_t target_time, bool is_out = true, std::string label = "")
            : target_time_(target_time), is_out_(is_out), label_filter_(std::move(label)) {
            type_ = qop_type::expand;
        }

        void process(query_ctx &ctx, const qr_tuple &v);
        
        void finish(query_ctx &ctx) { qop::default_finish(ctx); }
        
        void start(query_ctx &ctx) override {
            if (has_subscriber()) subscriber_->start(ctx);
        }
        
        void accept(qop_visitor &vis) override {
            if (has_subscriber()) subscriber_->accept(vis);
        }
        void dump(std::ostream &os) const override { 
            os << "temporal_optional_expand(" << target_time_ << ")"; 
        }

    private:
        uint64_t target_time_;
        bool is_out_;
        std::string label_filter_;
    };
}
#endif