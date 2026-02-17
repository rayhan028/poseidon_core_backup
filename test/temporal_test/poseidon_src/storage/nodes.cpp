#include "nodes.hpp"
#include "thread_pool.hpp"
#include <iostream>
#include <sstream>
#include "spdlog/spdlog.h"

#define PARALLEL_INIT

/* ------------------------------------------------------------------------ */
/* Utility: print std::any values in properties                             */
/* ------------------------------------------------------------------------ */

template <class T>
bool output_any(std::ostream &os, const std::any &any_value) {
  try {
    T v = std::any_cast<T>(any_value);
    os << v;
    return true;
  } catch (std::bad_any_cast &) {
    return false;
  }
}

template <>
bool output_any<std::string>(std::ostream &os, const std::any &any_value) {
  try {
    auto v = std::any_cast<std::string>(any_value);
    os << '"' << v << '"';
    return true;
  } catch (std::bad_any_cast &) {
    return false;
  }
}

template <>
bool output_any<const char *>(std::ostream &os, const std::any &any_value) {
  try {
    auto v = std::any_cast<const char *>(any_value);
    os << '"' << v << '"';
    return true;
  } catch (std::bad_any_cast &) {
    return false;
  }
}

std::ostream &operator<<(std::ostream &os, const std::any &any_value) {
  // Try all supported types
  if (!output_any<int>(os, any_value))
    if (!output_any<double>(os, any_value))
      if (!output_any<bool>(os, any_value))
        if (!output_any<std::string>(os, any_value))
          if (!output_any<const char *>(os, any_value))
            if (!output_any<uint64_t>(os, any_value))
              if (!output_any<boost::posix_time::ptime>(os, any_value))
                os << "{unknown}";
  return os;
}

/* ------------------------------------------------------------------------ */
/* Node description printing                                                */
/* ------------------------------------------------------------------------ */

std::ostream &operator<<(std::ostream &os, const node_description &ndescr) {
  // OpenCypher-like: Label[logical_id:id]{props} with VT/TT ranges
  os << ndescr.label << "[" << ndescr.logical_id << ":" << ndescr.id << "]"
     << "{";

  bool first = true;
  for (auto &p : ndescr.properties) {
    if (!first) os << ", ";
    os << p.first << ": " << p.second;
    first = false;
  }
  os << "}";

  // Print temporal metadata
  os << " VT=[" << short_ts(ndescr.vt_start) << "," << short_ts(ndescr.vt_end) << "]"
     << " TT=[" << short_ts(ndescr.tt_start) << "," << short_ts(ndescr.tt_end) << "]";

  return os;
}

/* ------------------------------------------------------------------------ */
/* Node description methods                                                 */
/* ------------------------------------------------------------------------ */

std::string node_description::to_string() const {
  std::ostringstream os;
  os << *this;
  return os.str();
}

bool node_description::has_property(const std::string& pname) const {
  return properties.find(pname) != properties.end();
}

bool node_description::operator==(const node_description& other) const {
  return logical_id == other.logical_id &&
         id == other.id &&
         label == other.label &&
         vt_start == other.vt_start &&
         vt_end == other.vt_end &&
         tt_start == other.tt_start &&
         tt_end == other.tt_end;
}