/**  Executes a time-consistent adjacency traversal.
* Navigates from a source node to its neighbors at 'target_time_',
 * dynamically switching between 'Hot' storage and 'Historical' deltas
 * to ensure all traversed edges and neighbors are valid at that specific moment.
 * Traverses edges and discovers neighbors while strictly maintaining temporal consistency at time T.
 */

#ifndef qop_temporal_expand_hpp_
#define qop_temporal_expand_hpp_

#include "qop.hpp"

namespace poseidon {
    class qop_temporal_expand : public enable_shared<qop, qop_temporal_expand> {
    public:
        // direction: true = OUT, false = IN
        qop_temporal_expand(uint64_t target_time, bool is_out = true, std::string label = "")
            : target_time_(target_time), is_out_(is_out), label_filter_(label) {
            type_ = qop_type::expand;
        }
        // Process input tuple to expand relationships at target_time_
        void process(query_ctx &ctx, const qr_tuple &v);
        // Default finish
        void finish(query_ctx &ctx) { qop::default_finish(ctx); }

            // Override virtual methods
        void start(query_ctx &ctx) override {
            if (has_subscriber()) subscriber_->start(ctx);
        }
        // Accept visitor
        void accept(qop_visitor &vis) override {
            if (has_subscriber()) subscriber_->accept(vis);
        }
        // Dump operator info
        void dump(std::ostream &os) const override {
            os << "temporal_expand(" << (is_out_ ? "OUT" : "IN") << ", T=" << target_time_ << ")";
        }

    private:
        uint64_t target_time_;
        bool is_out_;
        std::string label_filter_;
    };
}
#endif