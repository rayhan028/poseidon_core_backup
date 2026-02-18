#ifndef qop_temporal_filter_hpp_
#define qop_temporal_filter_hpp_

#include "qop.hpp"
#include <functional>

namespace poseidon {
    /**
     * Filters the temporal result stream 
     */
    class qop_temporal_filter : public enable_shared<qop, qop_temporal_filter> {
    public:
        using predicate_t = std::function<bool(const qr_tuple &)>;

        qop_temporal_filter(predicate_t pred) : pred_(std::move(pred)) {
            type_ = qop_type::filter;
        }

        void process(query_ctx &ctx, const qr_tuple &v);
        
       void finish(query_ctx &ctx) { qop::default_finish(ctx); }
        
        void start(query_ctx &ctx) override { 
            if (has_subscriber()) subscriber_->start(ctx); 
        }
        
        void accept(qop_visitor &vis) override { 
            if (has_subscriber()) subscriber_->accept(vis); 
        }
        
        void dump(std::ostream &os) const override { os << "qop_temporal_filter"; }

    private:
        predicate_t pred_;
    };
}

#endif