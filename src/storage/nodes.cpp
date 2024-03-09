/*
 * Copyright (C) 2019-2020 DBIS Group - TU Ilmenau, All Rights Reserved.
 *
 * This file is part of the Poseidon package.
 *
 * Poseidon is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Poseidon is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Poseidon. If not, see <http://www.gnu.org/licenses/>.
 */
#include "nodes.hpp"
#include "thread_pool.hpp"
#include <iostream>
#include <sstream>

#include "spdlog/spdlog.h"

#define PARALLEL_INIT

template <class T>
bool output_any(std::ostream &os, const std::any &any_value) {
  try {
    T v = std::any_cast<T>(any_value);
    os << v;
    return true;
  } catch (std::bad_any_cast &e) {
    return false;
  }
}

template <>
bool output_any<std::string>(std::ostream &os, const std::any &any_value) {
  try {
    auto v = std::any_cast<std::string>(any_value);
    os << '"' << v << '"';
    return true;
  } catch (std::bad_any_cast &e) {
    return false;
  }
}

template <>
bool output_any<const char *>(std::ostream &os, const std::any &any_value) {
  try {
    auto v = std::any_cast<const char *>(any_value);
    os << '"' << v << '"';
    return true;
  } catch (std::bad_any_cast &e) {
    return false;
  }
}
std::ostream &operator<<(std::ostream &os, const std::any &any_value) {
  // list all types you want to try
  if (!output_any<int>(os, any_value))
    if (!output_any<double>(os, any_value))
      if (!output_any<bool>(os, any_value))
        if (!output_any<std::string>(os, any_value))
          if (!output_any<const char *>(os, any_value))
            if (!output_any<uint64_t>(os, any_value))
              if (!output_any<boost::posix_time::ptime>(os, any_value))
                os << "{unknown}"; // all cast are failed, we have a unknown type of any
  return os;
}

std::ostream &operator<<(std::ostream &os, const node_description &ndescr) {
  //  os << "(#" << ndescr.id << ":" << ndescr.label << ", {";
  os << ndescr.label << "[" << ndescr.id << "]{"; // OpenCypher 9.0
  bool first = true;
  for (auto &p : ndescr.properties) {
    if (!first)
      os << ", ";
    os << p.first << ": " << p.second;
    first = false;
  }
  //  os << "})";
  os << "}";
  return os;
}

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
  return id == other.id && label == other.label/* && properties == other.properties*/;
}

/* ------------------------------------------------------------------------ */
