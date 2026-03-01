#ifndef qop_temporal_topk_hpp_
#define qop_temporal_topk_hpp_

#include "qop.hpp"
#include <mutex>

namespace poseidon {
    class qop_temporal_topk : public enable_shared<qop, qop_temporal_topk> {
    public:
        qop_temporal_topk(size_t limit, std::function<bool(const qr_tuple &, const qr_tuple &)> cmp)
            : limit_(limit), cmp_func_(std::move(cmp)) {
            type_ = qop_type::sort;
        }

        void process(query_ctx &ctx, const qr_tuple &v);
        void finish(query_ctx &ctx);
        
        void start(query_ctx &ctx) override {
            if (has_subscriber()) subscriber_->start(ctx);
        }
        
        void accept(qop_visitor &vis) override {
            if (has_subscriber()) subscriber_->accept(vis);
        }
        
        void dump(std::ostream &os) const override { 
            os << "temporal_topk(limit: " << limit_ << ")"; 
        }

    private:
        size_t limit_;
        std::function<bool(const qr_tuple &, const qr_tuple &)> cmp_func_;
        std::vector<qr_tuple> buffer_;
        mutable std::mutex mtx_;
    };
}
#endif