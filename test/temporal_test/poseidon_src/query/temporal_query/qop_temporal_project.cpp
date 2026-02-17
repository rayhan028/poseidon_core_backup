#include "qop_temporal_project.hpp"
#include "query_ctx.hpp"
#include "qresult.hpp"
#include "nodes.hpp"
#include "relationships.hpp"

namespace poseidon {
    // Helper to map std::any back to the boost::variant used in qr_tuple
    static query_result any_to_qv_variant(const std::any& a) {
        if (!a.has_value()) return null_val;
        if (a.type() == typeid(int)) return std::any_cast<int>(a);
        if (a.type() == typeid(double)) return std::any_cast<double>(a);
        if (a.type() == typeid(std::string)) return std::any_cast<std::string>(a);
        if (a.type() == typeid(uint64_t)) return std::any_cast<uint64_t>(a);
        if (a.type() == typeid(boost::posix_time::ptime)) return std::any_cast<boost::posix_time::ptime>(a);
        return null_val;
    }

    void qop_temporal_project::process(query_ctx &ctx, const qr_tuple &v) {
        if (v.empty() || input_idx_ >= v.size()) return;

        const properties_t* props = nullptr;
        const auto& item = v[input_idx_];

        // Access the properties map from the description variant
        if (item.which() == node_descr_type) {
            props = &boost::get<node_description>(item).properties;
        } else if (item.which() == rship_descr_type) {
            props = &boost::get<rship_description>(item).properties;
        }

        if (!props) return;

        qr_tuple projected_tuple;
        projected_tuple.reserve(pnames_.size());

        for (const auto& name : pnames_) {
            auto it = props->find(name);
            if (it != props->end()) {
                projected_tuple.push_back(any_to_qv_variant(it->second));
            } else {
                projected_tuple.push_back(null_val);
            }
        }
        consume(ctx, projected_tuple);
    }
}