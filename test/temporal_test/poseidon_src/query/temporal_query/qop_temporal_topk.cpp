#include "qop_temporal_topk.hpp"
#include <algorithm>

namespace poseidon {

    void qop_temporal_topk::process(query_ctx &ctx, const qr_tuple &v) { // Buffer incoming tuples
        std::unique_lock lock(mtx_);
        buffer_.push_back(v);
    }

    void qop_temporal_topk::finish(query_ctx &ctx) { // Sort buffered tuples and emit top-k
        std::sort(buffer_.begin(), buffer_.end(), cmp_func_);
        
        size_t emit_count = std::min(limit_, buffer_.size()); // Emit top-k tuples
        for (size_t i = 0; i < emit_count; ++i) { // Emit tuple to subscriber
             if (consume_)
            consume(ctx, buffer_[i]);
        }
        
        qop::default_finish(ctx);
    }
}