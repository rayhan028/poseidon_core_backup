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

#include "spdlog/spdlog.h"
#include <boost/hana.hpp>
#include <iostream>
#include <regex>

#include "properties.hpp"

static std::regex float_expr("-?[0-9]+\\.[0-9]+");

static std::regex int_expr("(-?[1-9][0-9]*)|0");

static std::regex date_expr("^((19[7-9][0-9])|(20[0-9][0-9]))-" // YYYY-MM-DD
                            "(0[1-9]|1[012])-"
                            "(0[1-9]|[12][0-9]|3[01])$");

static std::regex dtime_expr("^((19[7-9][0-9])|(20[0-9][0-9]))-" // YYYY-MM-DDTHH:MM:SS:ZZZZ
                            "(0[1-9]|1[012])-"
                            "(0[1-9]|[12][0-9]|3[01])T"
                            "([01][0-9]|2[0-3]):"
                            "([0-5][0-9]):"
                            "([0-5][0-9])."
                            "[0-9]{3}[+]"
                            "(([0-9]{4})|([0-9]{2}:[0-9]{2}))$");

bool is_quoted_string(const std::string &s) {
  return (s[0] == '"' && s[s.length() - 1] == '"') ||
         (s[0] == '\'' && s[s.length() - 1] == '\'');
}

bool is_float(const std::string &s) { return std::regex_match(s, float_expr); }

bool is_int(const std::string &s) { return std::regex_match(s, int_expr); }

bool is_uint64(const std::string &s) { 
  if (!is_int(s)) return false;
  try {
    std::stoi(s);
    return false;
  }
  catch (std::out_of_range& ex1) {
    try {
      std::stoull(s);
      return true;
    }
    catch(std::exception& ex2) {}
  }
  return false;
}

bool is_date(const std::string &s) { return std::regex_match(s, date_expr); }

bool is_dtime(const std::string &s) { return std::regex_match(s, dtime_expr); }

/* --------------------------------------------------------------------- */
using namespace boost::posix_time;

template <> double p_item::get<double>() const {
  assert(P_DOUBLE_VAL(flags_));
  return *(reinterpret_cast<const double *>(value_));
}

template <> int p_item::get<int>() const {
  assert(P_INT_VAL(flags_));
  return *(reinterpret_cast<const int *>(value_));
}

template <> dcode_t p_item::get<dcode_t>() const {
  assert(P_DICT_VAL(flags_));
  return *(reinterpret_cast<const dcode_t *>(value_));
}

template <> uint64_t p_item::get<uint64_t>() const {
  assert(P_UINT64_VAL(flags_));
  return *(reinterpret_cast<const uint64_t *>(value_));
}

template <> ptime p_item::get<ptime>() const {
  assert(P_PTIME_VAL(flags_));
  return *(reinterpret_cast<const ptime *>(value_));
}

template <> void p_item::set<double>(double v) {
  P_SET_VAL(flags_, p_double);
  memcpy(&value_, &v, sizeof(double));
}

template <> void p_item::set<int>(int v) {
  P_SET_VAL(flags_, p_int);
  memset(&value_, 0, 8);
  memcpy(&value_, &v, sizeof(int));
}

template <> void p_item::set<dcode_t>(dcode_t v) {
  P_SET_VAL(flags_, p_dcode);
  memset(&value_, 0, 8);
  memcpy(&value_, &v, sizeof(dcode_t));
}

template <> void p_item::set<uint64_t>(uint64_t v) {
  P_SET_VAL(flags_, p_uint64);
  memset(&value_, 0, 8);
  memcpy(&value_, &v, sizeof(uint64_t));
}

template <> void p_item::set<ptime>(ptime v) {
  P_SET_VAL(flags_, p_ptime);
  memset(&value_, 0, 8);
  memcpy(&value_, &v, sizeof(ptime));
}

p_item::p_item(dcode_t k, p_item::p_typecode tc, const std::any &v) : key_(k), flags_(0) {
  switch(tc) {
    case p_int: set<int>(std::any_cast<int>(v)); break;
    case p_double: set<double>(std::any_cast<double>(v)); break;
    case p_dcode: set<dcode_t>(std::any_cast<dcode_t>(v)); break;
    case p_uint64: set<uint64_t>(std::any_cast<uint64_t>(v)); break;
    case p_ptime: 
    case p_date: set<ptime>(std::any_cast<ptime>(v)); break;
    default: break;
  }  
}


p_item::p_item(dcode_t k, p_item::p_typecode tc, const std::string& v,dict_ptr &dict) : key_(k), flags_(0) {

	  switch(tc) {
	    case p_int    : set<int>(std::stoi(v)); 	    break;
	    case p_double : set<double>(std::stod(v));   	break;
	    case p_dcode  : set<dcode_t>(dict->insert(v));  break;
	    case p_uint64 : set<uint64_t>(std::stoull(v));  break;
	    case p_ptime  : set<ptime>(boost::posix_time::time_from_string([&](){std::string s=v; s[s.find("T")] = ' '; 
	                    return s.substr(0, s.find("+"));}())); break;
	    case p_date   : set<ptime>(boost::posix_time::ptime([&](){ return boost::gregorian::from_simple_string(v);}(),
	    		     boost::posix_time::seconds(0)));break;
	    default: break;
	  }
}


p_item::p_item(dcode_t k, double v) : key_(k), flags_(0) { set<double>(v); }
p_item::p_item(dcode_t k, int v) : key_(k), flags_(0) { set<int>(v); }
p_item::p_item(dcode_t k, dcode_t v) : key_(k), flags_(0) { set<dcode_t>(v); }
p_item::p_item(dcode_t k, uint64_t v) : key_(k), flags_(0) { set<uint64_t>(v); }
p_item::p_item(dcode_t k, boost::posix_time::ptime v) : key_(k), flags_(0) { set<ptime>(v); }

p_item::p_item(const std::string &k, const std::any &v, dict_ptr &dct)
    : p_item(v, dct) {
  key_ = dct->insert(k);
}

p_item::p_item(dcode_t k, const std::any &v, dict_ptr &dct) : p_item(v, dct) {
  key_ = k;
}

p_item::p_item(const std::any &v, dict_ptr &dct) : key_(0), flags_(0) {
  P_SET_VAL(flags_, p_unused);

  if (v.type() == typeid(uint64_t)){
    set<uint64_t>(std::any_cast<uint64_t>(v));
    return;
  }
  try {
    std::string s = std::any_cast<std::string>(v);
    if (is_quoted_string(s))
      set<dcode_t>(dct->insert(s));
    else if (is_int(s))
      set<int>((int)std::stoi(s));
    else if (is_float(s))
      set<double>((double)std::stod(s));
    else if (is_date(s)){
      boost::gregorian::date dt = boost::gregorian::from_simple_string(s);
      set<ptime>(ptime(dt, seconds(0)));
    }
    else if (is_dtime(s)){
      s[s.find("T")] = ' ';
      auto dts = s.substr(0, s.find("+"));
      set<ptime>(time_from_string(dts));
    }
    else 
      set<dcode_t>(dct->insert(s));
    return;
  } catch (std::bad_any_cast &e) {
    // do nothing, just continue
  }
  try {
    ptime dt = std::any_cast<ptime>(v);
    set<ptime>(dt);
    return;
  } catch (std::bad_any_cast &e) {
    // do nothing, just continue
  }
  try {
    double d = std::any_cast<double>(v);
    set<double>(d);
    return;
  } catch (std::bad_any_cast &e) {
    // do nothing, just continue
  }
  try {
    set<int>(std::any_cast<int>(v));
  } catch (std::bad_any_cast &e) {
    spdlog::info("ERROR: Cannot get or set int value.");
  }
}

p_item &p_item::operator=(const p_item &p) {
  flags_ = p.flags_;
  key_ = p.key_;
  memcpy(value_, p.value_, 8);
  return *this;
}

bool p_item::equal(int i) const {
  if (P_INT_VAL(flags_))
    return get<int>() == i;
  else if (P_DOUBLE_VAL(flags_))
    return get<double>() == i;
  throw invalid_typecast();
}

bool p_item::equal(double d) const {
  if (P_INT_VAL(flags_))
    return get<int>() == d;
  else if (P_DOUBLE_VAL(flags_))
    return get<double>() == d;
  throw invalid_typecast();
}

bool p_item::equal(dcode_t c) const {
  if (P_DICT_VAL(flags_))
    return get<dcode_t>() == c;
  throw invalid_typecast();
}

bool p_item::equal(uint64_t ll) const {
  if (P_UINT64_VAL(flags_))
    return get<uint64_t>() == ll;
  throw invalid_typecast();
}

bool p_item::equal(boost::posix_time::ptime dt) const {
  if (P_PTIME_VAL(flags_))
    return get<ptime>() == dt;
  throw invalid_typecast();
}

uint64_t p_item::get_raw() const {
  return *(reinterpret_cast<const uint64_t *>(value_));
}
  
std::ostream& operator<< (std::ostream& os, const p_item& pi) {
  os << "(" << static_cast<unsigned int>(pi.flags_) << "|" << pi.key_ << "|" << pi.get_raw() << ")";
  return os;
}
