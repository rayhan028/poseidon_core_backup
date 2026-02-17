/** Performs a Point-in-Time snapshot lookup.
* Uses the Temporal Index to retrieve the specific physical version of a Node
 * or Relationship that was valid at exactly 'target_time_'.
 * Snapshot Scan: Recovers the state of a specific node or relationship as it existed at point-in-time T.
 */

#ifndef qop_as_of_scan_hpp_
#define qop_as_of_scan_hpp_

#include "qop.hpp"

namespace poseidon {
    class qop_as_of_scan : public enable_shared<qop, qop_as_of_scan> {
    public:
        // Generic constructor with takes and Type
        qop_as_of_scan(uint64_t as_of_time, bool is_node = true, size_t lid_index = 0)
            : target_time_(as_of_time), is_node_(is_node), lid_index_(lid_index) {
            type_ = qop_type::scan;
        }
        // Generic processing methods
        void process(query_ctx &ctx, const qr_tuple &v);
        // Default finish
        void finish(query_ctx &ctx) { qop::default_finish(ctx); }

            // Overrides
        void start(query_ctx &ctx) override { // Start operator
            if (has_subscriber()) subscriber_->start(ctx); // Propagate start
        }
        // End operator
        void accept(qop_visitor &vis) override {
            if (has_subscriber()) subscriber_->accept(vis); // Propagate accept
        }
        // Dump operator info
        void dump(std::ostream &os) const override {
            os << (is_node_ ? "node" : "rship") << "_generic_as_of(T=" << target_time_ << ")";
        }

    private:
        uint64_t target_time_;
        bool is_node_;
        size_t lid_index_;
    };
}
#endif