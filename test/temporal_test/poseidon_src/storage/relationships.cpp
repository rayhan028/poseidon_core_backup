#include "relationships.hpp"
#include "thread_pool.hpp"
#include <iostream>
#include <sstream>

#define PARALLEL_INIT

/* ------------------------------------------------------------------------ */
/* Print a relationship description aligned with temporal node design       */
/* ------------------------------------------------------------------------ */

std::ostream &operator<<(std::ostream &os, const rship_description &rdescr) {
    // OpenCypher-like with logical/physical IDs, endpoints, properties, and VT/TT
    os << ":" << rdescr.label
       << "[" << rdescr.logical_id << ":" << rdescr.id << "]"
       << "(" << rdescr.from_id << "->" << rdescr.to_id << ")"
       << "{";

    bool first = true;
    for (const auto &p : rdescr.properties) {
        if (!first) os << ", ";
        os << p.first << ": " << p.second; // relies on operator<<(std::any)
        first = false;
    }
    os << "}";

    os << " VT=[" << short_ts(rdescr.vt_start) << "," << short_ts(rdescr.vt_end) << "]"
       << " TT=[" << short_ts(rdescr.tt_start) << "," << short_ts(rdescr.tt_end) << "]";

    return os;
}

/* ------------------------------------------------------------------------ */
/* rship_description methods                                                */
/* ------------------------------------------------------------------------ */

std::string rship_description::to_string() const {
    std::ostringstream os;
    os << *this;
    return os.str();
}

bool rship_description::has_property(const std::string& pname) const {
    return properties.find(pname) != properties.end();
}

bool rship_description::operator==(const rship_description& other) const {
    return logical_id == other.logical_id &&
           id == other.id &&
           from_id == other.from_id &&
           to_id == other.to_id &&
           label == other.label &&
           vt_start == other.vt_start &&
           vt_end == other.vt_end &&
           tt_start == other.tt_start &&
           tt_end == other.tt_end;
}