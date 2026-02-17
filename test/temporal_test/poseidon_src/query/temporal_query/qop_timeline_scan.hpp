#ifndef qop_timeline_scan_hpp_
#define qop_timeline_scan_hpp_
#include "qop.hpp"

namespace poseidon {
    class qop_timeline_scan : public enable_shared<qop, qop_timeline_scan> {
    public:
        // Constructor with temporal window and type
        qop_timeline_scan(uint64_t start, uint64_t end, bool is_node = true, size_t lid_index = 0)
            : start_time_(start), end_time_(end), is_node_(is_node), lid_index_(lid_index) {
            type_ = qop_type::scan; // Set operator type
        }
        // Generic processing methods
        void process(query_ctx &ctx, const qr_tuple &v);
        // Default finish behavior
        void finish(query_ctx &ctx) { qop::default_finish(ctx); }
        // Override lifecycle methods
        void start(query_ctx &ctx) override {
            if (has_subscriber()) subscriber_->start(ctx);
        }
        // Accept visitor
        void accept(qop_visitor &vis) override {
            if (has_subscriber()) subscriber_->accept(vis);
        }
        // Dump operator info
        void dump(std::ostream &os) const override {
            os << (is_node_ ? "node" : "rship") << "_generic_timeline";
        }
    private:
        uint64_t start_time_, end_time_;
        bool is_node_;
        size_t lid_index_;
    };
}
#endif