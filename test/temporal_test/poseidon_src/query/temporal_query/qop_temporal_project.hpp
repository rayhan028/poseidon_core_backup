#ifndef qop_temporal_project_hpp_
#define qop_temporal_project_hpp_

#include "qop.hpp"
#include <string>
#include <vector>

namespace poseidon {
    /**
     * Extracts specific properties from a node or relationship snapshot description.
     * Projects the internal std::any property values into the query_result variant.
     */
    class qop_temporal_project : public enable_shared<qop, qop_temporal_project> {
    public:
        // pnames: List of property keys (e.g., "firstName", "creationDate")
        // input_idx: Position in the incoming tuple containing the description object
        qop_temporal_project(const std::vector<std::string> &pnames, size_t input_idx = 0)
            : pnames_(pnames), input_idx_(input_idx) {
            type_ = qop_type::project;
        }

       void process(query_ctx &ctx, const qr_tuple &v);
        
        void finish(query_ctx &ctx) { qop::default_finish(ctx); }
        
        void start(query_ctx &ctx) override { 
            if (has_subscriber()) subscriber_->start(ctx); 
        }
        
        void accept(qop_visitor &vis) override { 
            if (has_subscriber()) subscriber_->accept(vis); 
        }
        
        void dump(std::ostream &os) const override { os << "qop_temporal_project"; }

    private:
        std::vector<std::string> pnames_;
        size_t input_idx_;
    };
}

#endif