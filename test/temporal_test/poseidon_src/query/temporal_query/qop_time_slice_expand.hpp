
// Time-slice expansion (overlap semantics)
// emit (neighbor, relationship) if the relationship overlaps [start_vt_, end_vt_)
// and the neighbor exists at some time within the refined overlap window.

#pragma once
#include "qop.hpp"
#include <string>
#include <cstdint>

namespace poseidon {

// Time-slice expansion (overlap semantics)
// emit (neighbor, relationship) if the relationship overlaps [start_vt_, end_vt_)
// and the neighbor exists at some time within the refined overlap window.
class qop_time_slice_expand : public enable_shared<qop, qop_time_slice_expand> {
public:
    qop_time_slice_expand(uint64_t start_vt, uint64_t end_vt, bool is_out = true, std::string label = "")
        : start_vt_(start_vt), end_vt_(end_vt), is_out_(is_out), label_filter_(std::move(label)) {
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
        os << "timeslice_expand(T=[" << start_vt_ << "," << end_vt_ << "], "
           << (is_out_ ? "OUT" : "IN") << ")";
    }

private:
    uint64_t start_vt_;
    uint64_t end_vt_;
    bool is_out_;
    std::string label_filter_;
};

} // namespace poseidon
