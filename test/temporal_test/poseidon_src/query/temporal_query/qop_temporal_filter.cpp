#include "qop_temporal_filter.hpp"
#include "query_ctx.hpp"
#include "qresult.hpp"

namespace poseidon {
    void qop_temporal_filter::process(query_ctx &ctx, const qr_tuple &v) {
        // Forward the tuple only if the predicate condition is satisfied
        if (pred_(v)) {
            consume(ctx, v);
        }
    }
}