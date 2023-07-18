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

#include <algorithm>
#include <iostream>
#include <numeric>
#include <sstream>

#include <boost/algorithm/string.hpp>
#include <boost/dynamic_bitset.hpp>
#include <boost/hana.hpp>

#include <fmt/ostream.h>

#include "qop.hpp"
#include "profiling.hpp"

#include "expr_interpreter.hpp"

using namespace boost::posix_time;

result_set::sort_spec_list sort_spec_;

/* ------------------------------------------------------------------------ */

p_item get_property_value(query_ctx &ctx, const qr_tuple& v, std::size_t var, const std::string& prop) {
  auto qv = v[var];
  p_item res;
  switch (qv.which()) {
    case 0: // node *
      {
        auto nptr = boost::get<node *>(qv);
        res = ctx.gdb_->get_property_value(*nptr, prop);
      }
      break;
    case 1: // relationship *
      {
        auto rptr = boost::get<relationship *>(qv);
        res = ctx.gdb_->get_property_value(*rptr, prop);
      }
      break;
    default:
      // return null
      break;
  }
  return res;
}

template <>
int get_property_value<int>(query_ctx &ctx, const qr_tuple& v, std::size_t var, const std::string& prop) {
  int res = 0;
  auto qv = get_property_value(ctx, v, var, prop);
  switch (qv.typecode()) {
    case p_item::p_int:
      res = qv.get<int>();
      break;
    case p_item::p_double:
      res = (int)qv.get<double>();
      break;
    case p_item::p_uint64:
      res = (int)qv.get<uint64_t>();
      break;
    default:
      break;
  }
  return res;
}

template <>
uint64_t get_property_value<uint64_t>(query_ctx &ctx, const qr_tuple& v, std::size_t var, const std::string& prop) {
  uint64_t res = 0;
  auto qv = get_property_value(ctx, v, var, prop);
  switch (qv.typecode()) {
    case p_item::p_int:
      res = qv.get<int>();
      break;
    case p_item::p_double:
      res = (int)qv.get<double>();
      break;
    case p_item::p_uint64:
      res = (int)qv.get<uint64_t>();
      break;
    default:
      break;
  }
  return res;
}

template <>
double get_property_value<double>(query_ctx &ctx, const qr_tuple& v, std::size_t var, const std::string& prop) {
  double res = 0;
  auto qv = get_property_value(ctx, v, var, prop);
  switch (qv.typecode()) {
    case p_item::p_int:
      res = (double)qv.get<int>();
      break;
    case p_item::p_double:
      res = qv.get<double>();
      break;
    case p_item::p_uint64:
      res = (double)qv.get<uint64_t>();
      break;
    default:
      break;
  }
  return res;
}

template <>
std::string get_property_value<std::string>(query_ctx &ctx, const qr_tuple& v, std::size_t var, const std::string& prop) {
  std::string res;
  auto qv = get_property_value(ctx, v, var, prop);
  switch (qv.typecode()) {
    case p_item::p_dcode:
      res = ctx.gdb_->get_string(qv.get<dcode_t>());
      break;
    case p_item::p_int:
      res = std::to_string(qv.get<int>());
      break;
    case p_item::p_double:
      res = std::to_string(qv.get<double>());
      break;
    case p_item::p_uint64:
      res = std::to_string(qv.get<uint64_t>());
      break;
    case p_item::p_ptime:
    case p_item::p_unused:
    // TODO
      break;
  }
  return res;
}

/* ------------------------------------------------------------------------ */

void scan_nodes::start(query_ctx &ctx) {
  if (label.empty() && labels.empty())
#ifdef QOP_RECOVERY
    if(!ranged) {
#endif
      ctx.parallel_nodes([&](node &n) { consume_(ctx, {&n}); });
#ifdef QOP_RECOVERY
    } else {
      ctx.parallel_nodes([&](node &n) { consume_(ctx, {&n}); }, ranges);
    }
#endif
  else if (!label.empty())
    ctx.nodes_by_label(label, [&](node &n) { PROF_PRE; consume_(ctx, {&n}); PROF_POST(1); });
  // TODO: in case of calling parallel_nodes we should handle this differently
  else
    ctx.nodes_by_label(labels, [&](node &n) { PROF_PRE; consume_(ctx, {&n}); PROF_POST(1); });
  qop::default_finish(ctx);
}

void scan_nodes::dump(std::ostream &os) const {
  os << "scan_nodes([" << label << "]) - " << PROF_DUMP;
}

#ifdef QOP_RECOVERY
/* ------------------------------------------------------------------------ */

void continue_scan_nodes::start(query_ctx &ctx) {
#ifdef USE_PMDK
  ctx.continue_parallel_nodes(check_points, [&](node &n) { consume_(ctx, {&n}); });
#endif
  qop::default_finish(ctx);
}

void continue_scan_nodes::dump(std::ostream &os) const {
  os << "continue_scan_nodes([" << label << "])=>";
  if (subscriber_)
    subscriber_->dump(os);
}

/* ------------------------------------------------------------------------ */
#endif

void index_scan::start(query_ctx &ctx) {
  if (idxs.empty())
    ctx.gdb_->index_lookup(idx, key, [&](node &n) { consume_(ctx, {&n}); });
  else
    ctx.gdb_->index_lookup(idxs, key, [&](node &n) { consume_(ctx, {&n}); });
  
  qop::default_finish(ctx);
}

void index_scan::dump(std::ostream &os) const {
  os << "index_scan([" << key << "]) - " << PROF_DUMP;
}

/* ------------------------------------------------------------------------ */

void foreach_from_relationship::process(query_ctx &ctx, const qr_tuple &v) {
  PROF_PRE;
  node *n = nullptr;
  if (npos == std::numeric_limits<int>::max())
    n = boost::get<node *>(v.back());
  else
    n = boost::get<node *>(v[npos]);

  if (lcode == 0)
    lcode = ctx.gdb_->get_code(label);

  uint64_t num = 0;
  ctx.foreach_from_relationship_of_node(*n, lcode, [&](relationship &r) {
    auto v2 = append(v, query_result(&r));
    consume_(ctx, v2);
    num++;
  });
  PROF_POST(num);
}

void foreach_from_relationship::dump(std::ostream &os) const {
  os << "foreach_from_relationship([" << label << "]) - " << PROF_DUMP;
}

/* ------------------------------------------------------------------------ */

void foreach_variable_from_relationship::process(query_ctx &ctx,
                                                 const qr_tuple &v) {
  PROF_PRE;
  node *n = nullptr;
  if (npos == std::numeric_limits<int>::max())
    n = boost::get<node *>(v.back());
  else
    n = boost::get<node *>(v[npos]);

  if (lcode == 0)
    lcode = ctx.gdb_->get_code(label);

  uint64_t num = 0;
  ctx.foreach_variable_from_relationship_of_node(
      *n, lcode, min_range, max_range, [&](relationship &r) {
        auto v2 = append(v, query_result(&r));
        consume_(ctx, v2);
        num++;
      });
  PROF_POST(num);
}

void foreach_variable_from_relationship::dump(std::ostream &os) const {
  os << "foreach_variable_from_relationship([" << label << ", (" << min_range
     << "," << max_range << ")]) - " << PROF_DUMP;
}

/* ------------------------------------------------------------------------ */

void foreach_all_relationship::process(query_ctx &ctx, const qr_tuple &v) {
  PROF_PRE;
  node *n = npos == std::numeric_limits<int>::max() ?
            boost::get<node *>(v.back()) : boost::get<node *>(v[npos]);

  if (lcode == 0)
    lcode = ctx.gdb_->get_code(label);

  uint64_t num = 0;
  ctx.foreach_from_relationship_of_node(*n, lcode, [&](relationship &r) {
    auto v2 = append(v, query_result(&r));
    v2 = append(v2, query_result(&(ctx.gdb_->node_by_id(r.dest_node))));
    consume_(ctx, v2);
    num++;
  });
  ctx.foreach_to_relationship_of_node(*n, lcode, [&](relationship &r) {
    auto v2 = append(v, query_result(&r));
    v2 = append(v2, query_result(&(ctx.gdb_->node_by_id(r.src_node))));
    consume_(ctx, v2);
    num++;
  });
  PROF_POST(num);
}

void foreach_all_relationship::dump(std::ostream &os) const {
  os << "foreach_all_relationship([" << label << "]) - " << PROF_DUMP;
}

/* ------------------------------------------------------------------------ */

void foreach_variable_all_relationship::process(query_ctx &ctx,
                                                 const qr_tuple &v) {
  PROF_PRE;
  node *n = npos == std::numeric_limits<int>::max() ?
            boost::get<node *>(v.back()) : boost::get<node *>(v[npos]);

  if (lcode == 0)
    lcode = ctx.gdb_->get_code(label);

  uint64_t num = 0;
  ctx.foreach_variable_from_relationship_of_node(
      *n, lcode, min_range, max_range, [&](relationship &r) {
        auto v2 = append(v, query_result(&r));
        v2 = append(v2, query_result(&(ctx.gdb_->node_by_id(r.dest_node))));
        consume_(ctx, v2);
        num++;
      });
  ctx.foreach_variable_to_relationship_of_node(
      *n, lcode, min_range, max_range, [&](relationship &r) {
        auto v2 = append(v, query_result(&r));
        v2 = append(v2, query_result(&(ctx.gdb_->node_by_id(r.src_node))));
        consume_(ctx, v2);
        num++;
      });
  PROF_POST(num);
}

void foreach_variable_all_relationship::dump(std::ostream &os) const {
  os << "foreach_variable_all_relationship([" << label << ", (" << min_range
     << "," << max_range << ")]) - " << PROF_DUMP;
}

/* ------------------------------------------------------------------------ */

void foreach_to_relationship::process(query_ctx &ctx, const qr_tuple &v) {
  PROF_PRE;
  node *n = nullptr;
  if (npos == std::numeric_limits<int>::max())
    n = boost::get<node *>(v.back());
  else
    n = boost::get<node *>(v[npos]);

  if (lcode == 0)
    lcode = ctx.gdb_->get_code(label);

  uint64_t num = 0;
  ctx.foreach_to_relationship_of_node(*n, lcode, [&](relationship &r) {
    auto v2 = append(v, query_result(&r));
    consume_(ctx, v2);
    num++;
  });
  PROF_POST(num);
}

void foreach_to_relationship::dump(std::ostream &os) const {
  os << "foreach_to_relationship([" << label << "]) - " << PROF_DUMP;
}

/* ------------------------------------------------------------------------ */
void foreach_variable_to_relationship::process(query_ctx &ctx,
                                               const qr_tuple &v) {
  PROF_PRE;
  node *n = nullptr;
  if (npos == std::numeric_limits<int>::max())
    n = boost::get<node *>(v.back());
  else
    n = boost::get<node *>(v[npos]);

  if (lcode == 0)
    lcode = ctx.gdb_->get_code(label);

  uint64_t num = 0;
  ctx.foreach_variable_to_relationship_of_node(
      *n, lcode, min_range, max_range, [&](relationship &r) {
        auto v2 = append(v, query_result(&r));
        consume_(ctx, v2);
        num++;
      });
  PROF_POST(num);
}

void foreach_variable_to_relationship::dump(std::ostream &os) const {
  os << "foreach_variable_to_relationship([" << label << ", (" << min_range
     << "," << max_range << ")]) - " << PROF_DUMP;
}

/* ------------------------------------------------------------------------ */

void is_property::dump(std::ostream &os) const {
  os << "is_property([" << property << "]) - " << PROF_DUMP;
}

void is_property::process(query_ctx &ctx, const qr_tuple &v) {
  PROF_PRE;
  bool success = false;
  auto n = v.back();
  if (pcode == 0)
    pcode = ctx.gdb_->get_code(property);

  if (n.type() == typeid(node *)) {
    if (ctx.is_node_property(*(boost::get<node *>(n)), pcode, predicate)) {
      consume_(ctx, v);
      success = true;
    }
  } else if (n.type() == typeid(relationship *)) {
    if (ctx.is_relationship_property(*(boost::get<relationship *>(n)), pcode,
                                      predicate)) {
      consume_(ctx, v);
      success = true;
    }
  }
  PROF_POST(success ? 1 : 0);
}

/* ------------------------------------------------------------------------ */

void node_has_label::dump(std::ostream &os) const {
  os << "node_has_label([" << label << "]) - " << PROF_DUMP;
}

void node_has_label::process(query_ctx &ctx, const qr_tuple &v) {
  PROF_PRE;
  bool success = false;
  auto n = boost::get<node *>(v.back());
  if (labels.empty()) {
    if (lcode == 0)
      lcode = ctx.gdb_->get_code(label);
    if (n->node_label == lcode) {
      consume_(ctx, v);
      success = true;
    }
  }
  else {
    for (auto &label : labels) {
      lcode = ctx.gdb_->get_code(label);
      if (n->node_label == lcode) {
        consume_(ctx, v);
        success = true;
        break;
      }
    }
  }
  PROF_POST(success ? 1 : 0);
}

/* ------------------------------------------------------------------------ */

void get_from_node::process(query_ctx &ctx, const qr_tuple &v) {
  PROF_PRE;
  auto rship = boost::get<relationship *>(v.back());
  auto v2 = append(v, query_result(&(ctx.gdb_->node_by_id(rship->src_node))));
  consume_(ctx, v2);
  PROF_POST(1);
}

void get_from_node::dump(std::ostream &os) const {
  os << "get_from_node() - " << PROF_DUMP;

}

/* ------------------------------------------------------------------------ */

void get_to_node::process(query_ctx &ctx, const qr_tuple &v) {
  PROF_PRE;
  auto rship = boost::get<relationship *>(v.back());
  auto v2 = append(v, query_result(&(ctx.gdb_->node_by_id(rship->dest_node))));
  consume_(ctx, v2);
  PROF_POST(1);
}

void get_to_node::dump(std::ostream &os) const {
  os << "get_to_node()" << PROF_DUMP;
}

/* ------------------------------------------------------------------------ */

template <> struct fmt::formatter<ptime> : ostream_formatter {};
template <> struct fmt::formatter<node_description> : ostream_formatter {};
template <> struct fmt::formatter<rship_description> : ostream_formatter {};

void printer::dump(std::ostream &os) const { os << "printer()"; }

void printer::process(query_ctx &ctx, const qr_tuple &v) {
  if (ntuples_ == 0) {
    std::cout << "+";
    for (auto i = 0u; i < v.size(); i++)
      std::cout << fmt::format("{0:-^{1}}+", "", 20);
    std::cout << "\n";
    output_width_ = 21 * v.size() + 1;
  }
  ntuples_++;
  auto my_visitor = boost::hana::overload(
      [&](const node_description& n) { std::cout << fmt::format(" {:<18} |", n); },
      [&](const rship_description& r) { std::cout << fmt::format(" {:<18} |", r); },
      [&](node *n) { std::cout << fmt::format(" {:<18} |", ctx.gdb_->get_node_description(n->id())); },
      [&](relationship *r) { std::cout << fmt::format(" {:<18} |", ctx.gdb_->get_relationship_label(*r)); },
      [&](int i) { std::cout << fmt::format(" {:>18} |", i); }, 
      [&](double d) { std::cout << fmt::format(" {:>18f} |", d); },
      [&](const std::string &s) { std::cout << fmt::format(" {:<18.18} |", s); },
      [&](uint64_t ll) { std::cout << fmt::format(" {:>18} |", ll); },
      [&](null_t n) { std::cout << fmt::format(" {:>18} |", "NULL"); },
      [&](array_t arr) {
        std::cout << "[ ";
        for (auto elem : arr.elems)
          std::cout << elem << " ";
        std::cout << " ]"; },
      [&](ptime dt) { std::cout << fmt::format(" {:<18.18} |", dt); });
  std::cout << "|";
  for (auto &ge : v) {
    boost::apply_visitor(my_visitor, ge);
  }
  std::cout << "\n";
}

void printer::finish(query_ctx &ctx) {
  auto s = fmt::format("{} tuple(s) returned. ", ntuples_);
  std::cout << "+-- " << s;
  for (int i = output_width_ - s.length() - 5; i > 0; i--) 
    std::cout << "-";
  std::cout << "+\n";
}

/* ------------------------------------------------------------------------ */

void limit_result::dump(std::ostream &os) const {
  os << "limit([" << num_ << "]) - " << PROF_DUMP;
}

void limit_result::process(query_ctx &ctx, const qr_tuple &v) {
  bool success = false;
  PROF_PRE;
  if (processed_ < num_) {
    consume_(ctx, v);
    processed_++;
    success = true;
  }
  PROF_POST(success ? 1 : 0);
}

#ifdef QOP_RECOVERY
void crash_at::dump(std::ostream &os) const {
  os << "limit([" << num_ << "]) - " << PROF_DUMP;
}

#include <stdexcept>
void crash_at::process(query_ctx &ctx, const qr_tuple &v) {
  if (processed_ < num_) {
    consume_(gdb, v);
    processed_++;
  } else {
    throw std::invalid_argument( "query failure :(" );
  }
}
#endif
/* ------------------------------------------------------------------------ */

void nodes_connected::dump(std::ostream &os) const {
  os << "nodes_connected([" "]) - " << PROF_DUMP;
}

void nodes_connected::process(query_ctx &ctx, const qr_tuple &v) {
  auto src = boost::get<node *>(v[src_des_nodes_.first]);
  auto des = boost::get<node *>(v[src_des_nodes_.second]);
  bool found = false;

  ctx.foreach_from_relationship_of_node((*src), [&](auto &r) {
      if (r.to_node_id() == des->id()){
        found = true;
        auto res = append(v, query_result(&r));
        consume_(ctx, res);
      }
  });

  if (!found && append_null_){
    auto res = append(v, query_result(null_val));
    consume_(ctx, res);
  }
}

/* ------------------------------------------------------------------------ */

std::function<bool(const qr_tuple &, const qr_tuple &)> order_by::cmp_func_ = 0;

void order_by::dump(std::ostream &os) const {
  os << "order_by([";
  if (! sort_spec_.empty()) {
    for (auto& sspec : sort_spec_) {
      os << " " << sspec.vidx << ":" << sspec.s_order;
    }
  }
  os << " ]) - " << PROF_DUMP;
}

void order_by::process(query_ctx &ctx, const qr_tuple &v) {
  PROF_PRE;
  results_.append(v);
  PROF_POST(1);
}

void order_by::finish(query_ctx &ctx) {
  PROF_PRE0;
  if (cmp_func_ != nullptr)
    results_.sort(ctx, cmp_func_);
  else
    results_.sort(ctx, sort_spec_);
  for (auto &v : results_.data) {
    consume_(ctx, v);
  }
  finish_(ctx);
  PROF_POST(0);
}

/* ------------------------------------------------------------------------ */
#if 0
group_by::group_by(const std::vector<std::size_t> &pos) :
    grpkey_cnt_(0), grpkey_pos_(pos) {}

group_by::group_by(const std::vector<std::size_t> &pos,
  const std::vector<std::pair<std::string, std::size_t>> &aggrs) :
    grpkey_cnt_(0), grpkey_pos_(pos), aggrs_(aggrs) {}

void group_by::dump(std::ostream &os) const {
  os << "group_by([" << grpkey_pos_.size() << "],[" << aggrs_.size() << "]) - " << PROF_DUMP;
}

group_by::group_by(std::list<qr_tuple> &grps, const std::vector<std::size_t> &pos,
    const std::vector<std::pair<std::string, std::size_t>> &aggrs) : grpkey_cnt_(0), grpkey_pos_(pos), aggrs_(aggrs) {
  for(auto & g : grps) {
    std::string grpkeys = "";
    for (auto pos : grpkey_pos_) {
      if (g[pos].type() == typeid(std::string)) {
        grpkeys += boost::get<std::string>(g[pos]);
      } else if (g[pos].type() == typeid(int)) {
        grpkeys += std::to_string(boost::get<int>(g[pos]));
      } else if (g[pos].type() == typeid(double)) {
        grpkeys += std::to_string(boost::get<double>(g[pos]));
      } else if (g[pos].type() == typeid(uint64_t)) {
        grpkeys += std::to_string(boost::get<uint64_t>(g[pos]));
      } else if (g[pos].type() == typeid(ptime)) {
        grpkeys += to_iso_extended_string(boost::get<ptime>(g[pos]));
      } else if (g[pos].type() == typeid(node *)) {
        grpkeys += std::to_string(boost::get<node *>(g[pos])->id());
      }
    }
    std::size_t gpos;
    gpos = grpkey_cnt_;
    grpkey_set_.push_back(grpkeys);
    grp_size_map_.emplace(gpos, 1);
    grpkey_map_.emplace(grpkeys, gpos);

    grp_tpl_map_.emplace(gpos, g);
    grpkey_cnt_++;
  }
}

void group_by::process(query_ctx &ctx, const qr_tuple &v) {
  std::lock_guard<std::mutex> lck(grp_mutex_);
  std::string grpkeys = "";
  for (auto pos : grpkey_pos_) {
    if (v[pos].type() == typeid(std::string)) {
      grpkeys += boost::get<std::string>(v[pos]);
    } else if (v[pos].type() == typeid(int)) {
      grpkeys += std::to_string(boost::get<int>(v[pos]));
    } else if (v[pos].type() == typeid(double)) {
      grpkeys += std::to_string(boost::get<double>(v[pos]));
    } else if (v[pos].type() == typeid(uint64_t)) {
      grpkeys += std::to_string(boost::get<uint64_t>(v[pos]));
    } else if (v[pos].type() == typeid(ptime)) {
      grpkeys += to_iso_extended_string(boost::get<ptime>(v[pos]));
    } else if (v[pos].type() == typeid(node *)) {
      grpkeys += std::to_string(boost::get<node *>(v[pos])->id());
    }
  }

  std::lock_guard<std::mutex> lock(m_);
  std::size_t gpos;
  const auto gitr = grpkey_map_.find(grpkeys);
  if (gitr != grpkey_map_.end()) {
    gpos = gitr->second;
    grp_size_map_[gpos]++;
  }
  else {
    gpos = grpkey_cnt_;
    grpkey_set_.push_back(grpkeys);
    grp_size_map_.emplace(gpos, 1);
    grpkey_map_.emplace(grpkeys, gpos);

    qr_tuple gtpl;
    for (auto pos : grpkey_pos_)
      gtpl.push_back(v[pos]);
    for (auto &aggr : aggrs_) {
      if (aggr.first == "count")
        gtpl.push_back(query_result((uint64_t)0));
      else if (aggr.first == "sum") {
        if (v[aggr.second].type() == typeid(uint64_t))
          gtpl.push_back(query_result((uint64_t)0));
        else if (v[aggr.second].type() == typeid(int))
          gtpl.push_back(query_result(0));
        else if (v[aggr.second].type() == typeid(double))
          gtpl.push_back(query_result(0.0));
      }
      else if (aggr.first == "avg" || aggr.first == "pcount")
        gtpl.push_back(query_result(0.0));
      else if (aggr.first == "min") {
        if (v[aggr.second].type() == typeid(uint64_t))
          gtpl.push_back(query_result(std::numeric_limits<uint64_t>::max()));
        else if (v[aggr.second].type() == typeid(int))
          gtpl.push_back(query_result(std::numeric_limits<int>::max()));
        else if (v[aggr.second].type() == typeid(double))
          gtpl.push_back(query_result(std::numeric_limits<double>::max()));
      }
      else if (aggr.first == "max") {
        if (v[aggr.second].type() == typeid(uint64_t))
          gtpl.push_back(query_result((uint64_t)0));
        else if (v[aggr.second].type() == typeid(int))
          gtpl.push_back(query_result(0));
        else if (v[aggr.second].type() == typeid(double))
          gtpl.push_back(query_result(0.0));
      }
    }

    grp_tpl_map_.emplace(gpos, gtpl);
    grpkey_cnt_++;
  }

  auto &aggr_tpl = grp_tpl_map_[gpos];
  auto aggr_pos = grpkey_pos_.size();

  for (auto &aggr : aggrs_) {
    if (aggr.first == "count") {
      boost::get<uint64_t>(aggr_tpl[aggr_pos])++;
    }
    else if (aggr.first == "sum") {
      if (v[aggr.second].type() == typeid(uint64_t)) {
        uint64_t &gsum = boost::get<uint64_t>(aggr_tpl[aggr_pos]);
        gsum += boost::get<uint64_t>(v[aggr.second]);
      }
      else if (v[aggr.second].type() == typeid(int)) {
        int &gsum = boost::get<int>(aggr_tpl[aggr_pos]);
        gsum += boost::get<int>(v[aggr.second]);
      }
      else if (v[aggr.second].type() == typeid(double)) {
        double &gsum = boost::get<double>(aggr_tpl[aggr_pos]);
        gsum += boost::get<double>(v[aggr.second]);
      }
    }
    else if (aggr.first == "avg") {
      double &gavg = boost::get<double>(aggr_tpl[aggr_pos]);
      auto gsize = grp_size_map_[gpos];
      auto val = (v[aggr.second].type() == typeid(uint64_t)) ?
                    boost::get<uint64_t>(v[aggr.second]) :
                  (v[aggr.second].type() == typeid(int)) ?
                    boost::get<int>(v[aggr.second]) :
                  (v[aggr.second].type() == typeid(double)) ?
                    boost::get<double>(v[aggr.second]) : 0;
      gavg = (gsize == 1) ? (double)val :
              (gavg * (gsize-1) + (double)val) / (double)gsize;
    }
    else if (aggr.first == "min") {
      if (v[aggr.second].type() == typeid(uint64_t)) {
        uint64_t &gmin = boost::get<uint64_t>(aggr_tpl[aggr_pos]);
        if (boost::get<uint64_t>(v[aggr.second]) < gmin)
          gmin = boost::get<uint64_t>(v[aggr.second]);
      }
      else if (v[aggr.second].type() == typeid(int)) {
        int &gmin = boost::get<int>(aggr_tpl[aggr_pos]);
        if (boost::get<int>(v[aggr.second]) < gmin)
          gmin = boost::get<int>(v[aggr.second]);
      }
      else if (v[aggr.second].type() == typeid(double)) {
        double &gmin = boost::get<double>(aggr_tpl[aggr_pos]);
        if (boost::get<double>(v[aggr.second]) < gmin)
          gmin = boost::get<double>(v[aggr.second]);
      }
    }
    else if (aggr.first == "max") {
      if (v[aggr.second].type() == typeid(uint64_t)) {
        uint64_t &gmax = boost::get<uint64_t>(aggr_tpl[aggr_pos]);
        if (boost::get<uint64_t>(v[aggr.second]) > gmax)
          gmax = boost::get<uint64_t>(v[aggr.second]);
      }
      else if (v[aggr.second].type() == typeid(int)) {
        int &gmax = boost::get<int>(aggr_tpl[aggr_pos]);
        if (boost::get<int>(v[aggr.second]) > gmax)
          gmax = boost::get<int>(v[aggr.second]);
      }
      else if (v[aggr.second].type() == typeid(double)) {
        double &gmax = boost::get<double>(aggr_tpl[aggr_pos]);
        if (boost::get<double>(v[aggr.second]) > gmax)
          gmax = boost::get<double>(v[aggr.second]);
      }
    } // process pcount (percentage count) in group_by::finish
    aggr_pos++;
  }

}

void group_by::finish(query_ctx &ctx) {
  auto aggr_pos = grpkey_pos_.size();
  for (auto &aggr : aggrs_) {
    if (aggr.first == "pcount"){
      std::size_t tsize = 0;
      for (auto &elem : grp_size_map_)
        tsize += elem.second;
      for (auto &grp : grpkey_set_) {
        auto gpos = grpkey_map_[grp];
        auto gsize = grp_size_map_[gpos];
        auto &aggr_tpl = grp_tpl_map_[gpos];
        double &gpcount = boost::get<double>(aggr_tpl[aggr_pos]);
        gpcount = (gsize / (double)tsize) * 100;
      }
    }
    aggr_pos++;
  }

  for (auto &grp : grpkey_set_) {
    auto gpos = grpkey_map_[grp];
    auto &gtpl = grp_tpl_map_[gpos];
    consume_(ctx, gtpl);
  }
  finish_(ctx);
}
#endif
#ifdef QOP_RECOVERY
/* ------------------------------------------------------------------------ */
persistent_group_by::persistent_group_by(const std::vector<std::size_t> &pos) :
    grpkey_cnt_(0), grpkey_pos_(pos) {}

persistent_group_by::persistent_group_by(const std::vector<std::size_t> &pos,
  const std::vector<std::pair<std::string, std::size_t>> &aggrs) :
    grpkey_cnt_(0), grpkey_pos_(pos), aggrs_(aggrs) {}

void persistent_group_by::dump(std::ostream &os) const {
  os << "persistent_group_by([]) - " << PROF_DUMP;
}

std::mutex grp_mutex2;
void persistent_group_by::process(query_ctx &ctx, const qr_tuple &v) {
  std::lock_guard<std::mutex> lck(grp_mutex2);
  std::string grpkeys = "";
  for (auto pos : grpkey_pos_) {
    if (v[pos].type() == typeid(std::string)) {
      grpkeys += boost::get<std::string>(v[pos]);
    } else if (v[pos].type() == typeid(int)) {
      grpkeys += std::to_string(boost::get<int>(v[pos]));
    } else if (v[pos].type() == typeid(double)) {
      grpkeys += std::to_string(boost::get<double>(v[pos]));
    } else if (v[pos].type() == typeid(uint64_t)) {
      grpkeys += std::to_string(boost::get<uint64_t>(v[pos]));
    } else if (v[pos].type() == typeid(ptime)) {
      grpkeys += to_iso_extended_string(boost::get<ptime>(v[pos]));
    } else if (v[pos].type() == typeid(node *)) {
      grpkeys += std::to_string(boost::get<node *>(v[pos])->id());
    }
  }

  std::size_t gpos;
  const auto gitr = grpkey_map_.find(grpkeys);
  if (gitr != grpkey_map_.end()) {
    gpos = gitr->second;
    grp_size_map_[gpos]++;
  }
  else {
    gpos = grpkey_cnt_;
    grpkey_set_.push_back(grpkeys);
    grp_size_map_.emplace(gpos, 1);
    grpkey_map_.emplace(grpkeys, gpos);

    qr_tuple gtpl;
    for (auto pos : grpkey_pos_)
      gtpl.push_back(v[pos]);
    for (auto &aggr : aggrs_) {
      if (aggr.first == "count")
        gtpl.push_back(query_result((uint64_t)0));
      else if (aggr.first == "sum") {
        if (v[aggr.second].type() == typeid(uint64_t))
          gtpl.push_back(query_result((uint64_t)0));
        else if (v[aggr.second].type() == typeid(int))
          gtpl.push_back(query_result(0));
        else if (v[aggr.second].type() == typeid(double))
          gtpl.push_back(query_result(0.0));
      }
      else if (aggr.first == "avg" || aggr.first == "pcount")
        gtpl.push_back(query_result(0.0));
    }

    grp_tpl_map_.emplace(gpos, gtpl);
    grpkey_cnt_++;

    std::vector<std::size_t> pgrp_pos = gdb->store_query_result(gtpl, 0);
    pgrp_tpl_pos_[gpos].swap(pgrp_pos);
  }
// here pls
  auto &aggr_tpl = pgrp_tpl_pos_[gpos];
  auto aggr_pos = grpkey_pos_.size();

  for (auto &aggr : aggrs_) {
    if (aggr.first == "count") {
      auto & ir = gdb->ir_by_id(aggr_tpl[aggr_pos]);
      ir.res_++;
      //boost::get<uint64_t>(aggr_tpl[aggr_pos])++;
    }
    else if (aggr.first == "sum") {
      if (v[aggr.second].type() == typeid(uint64_t)) {
        auto &ir = gdb->ir_by_id(aggr_tpl[aggr_pos]);
        uint64_t &gsum = ir.res_;
        gsum += boost::get<uint64_t>(v[aggr.second]);
      }
      else if (v[aggr.second].type() == typeid(int)) {
        auto &ir = gdb->ir_by_id(aggr_tpl[aggr_pos]);
        int &gsum = (int&)(ir.res_);
        gsum += boost::get<int>(v[aggr.second]);
      }
      else if (v[aggr.second].type() == typeid(double)) {
        auto &ir = gdb->ir_by_id(aggr_tpl[aggr_pos]);
        offset_t &gsum = ir.res_;
        double d;
        std::memcpy(&d, &gsum, sizeof(d));
        d += boost::get<double>(v[aggr.second]);
        std::memcpy(&gsum, &d, sizeof(d));
      }
    }
    else if (aggr.first == "avg") {
      auto &ir = gdb->ir_by_id(aggr_tpl[aggr_pos]);
      offset_t &gavg = ir.res_;
      double d;
      std::memcpy(&d, &gavg, sizeof(d));

      auto gsize = grp_size_map_[gpos];
      auto val = (v[aggr.second].type() == typeid(uint64_t)) ?
                    boost::get<uint64_t>(v[aggr.second]) :
                  (v[aggr.second].type() == typeid(int)) ?
                    boost::get<int>(v[aggr.second]) :
                  (v[aggr.second].type() == typeid(double)) ?
                    boost::get<double>(v[aggr.second]) : 0;
      d = (gsize == 1) ? (double)val :
              (d * (gsize-1) + (double)val) / (double)gsize;
      std::memcpy(&gavg, &d, sizeof(d));
    } // process pcount (percentage count) in group_by::finish
    aggr_pos++;
  }
}

void persistent_group_by::finish(query_ctx &ctx) {
  auto aggr_pos = grpkey_pos_.size();
  for (auto &aggr : aggrs_) {
    if (aggr.first == "pcount"){
      std::size_t tsize = 0;
      for (auto &elem : grp_size_map_)
        tsize += elem.second;
      for (auto &grp : grpkey_set_) {
        auto gpos = grpkey_map_[grp];
        auto gsize = grp_size_map_[gpos];
        //auto &aggr_tpl = grp_tpl_map_[gpos];
        auto ids = pgrp_tpl_pos_[gpos];
        auto aggr_id = ids[aggr_pos];

        auto &ir = gdb->ir_by_id(aggr_id);
        offset_t &pc = ir.res_;
        double gpcount;
        std::memcpy(&gpcount, &pc, sizeof(pc));
        gpcount = (gsize / (double)tsize) * 100;
        std::memcpy(&pc, &gpcount, sizeof(pc));
      }
    }
    aggr_pos++;
  }

  for (auto &grp : grpkey_set_) {
    auto gpos = grpkey_map_[grp];
    //auto &gtpl = grp_tpl_map_[gpos];
    qr_tuple gtpl;
    auto ids = pgrp_tpl_pos_[gpos];
    gdb->tuple_by_ids(ids, gtpl);
    consume_(ctx, gtpl);
  }
  finish_(ctx);
}
#endif
/* ------------------------------------------------------------------------ */

void distinct_tuples::dump(std::ostream &os) const {
  os << "distinct_tuples() - " << PROF_DUMP;
}

void distinct_tuples::process(query_ctx &ctx, const qr_tuple &v) {
  std::string key = "";
  for (const auto& qres : v) {
    if (qres.type() == typeid(std::string)) {
      key += boost::get<std::string>(qres);
    } else if (qres.type() == typeid(uint64_t)) {
      key += std::to_string(boost::get<uint64_t>(qres));
    } else if (qres.type() == typeid(ptime)) {
      key += to_iso_extended_string(boost::get<ptime>(qres));
    } else if (qres.type() == typeid(node *)) {
      key += std::to_string(boost::get<node *>(qres)->id());
    } else if (qres.type() == typeid(relationship *)) {
      key += std::to_string(boost::get<relationship *>(qres)->id());
    } else if (qres.type() == typeid(int)) {
      key += std::to_string(boost::get<int>(qres));
    } else if (qres.type() == typeid(double)) {
      key += std::to_string(boost::get<double>(qres));
    } else if (qres.type() == typeid(null_val)) {
      key += std::string("NULL");
    } else if (qres.type() == typeid(array_t)) {
      auto arr = boost::get<array_t>(qres).elems;
      for (auto a : arr)
        key += std::to_string(a);
    }
  }

  std::lock_guard<std::mutex> lock(m_);
  if (keys_.find(key) == keys_.end()) {
    keys_.insert(key); // TODO optimize with integer value representation
    consume_(ctx, v);
  }
}

/* ------------------------------------------------------------------------ */

void filter_tuple::dump(std::ostream &os) const {
  os << "filter_tuple([";
  if (ex_) 
    os << ex_->dump();
  os << "]) - " << PROF_DUMP;
}

void filter_tuple::process(query_ctx &ctx, const qr_tuple &v) {
  PROF_PRE;
  bool tp = ex_ ? interpret_expression(ctx, ex_, v) : pred_func1_(v);
  if (tp) {
    consume_(ctx, v);
    PROF_POST(1);
  }
  else PROF_POST(0);
}

/* ------------------------------------------------------------------------ */

void qr_tuple_append::dump(std::ostream &os) const {
  os << "qr_tuple_append([]) - " << PROF_DUMP;
}

void qr_tuple_append::process(query_ctx &ctx, const qr_tuple &v) {
  PROF_PRE;
  auto a = func_(v);
  auto res = append(v, a);
  consume_(ctx, res);
  PROF_POST(1);
}

/* ------------------------------------------------------------------------ */

void union_all_qres::dump(std::ostream &os) const { // TODO
  os << "union_all_qres() - " << PROF_DUMP;
}

void union_all_qres::process_left(query_ctx &ctx, const qr_tuple &v) {
  PROF_PRE;
  if (init) {
    for (auto &r : res_)
      consume_(ctx, r);
    init = false;
  }
  consume_(ctx, v);
  PROF_POST(1);
}

void union_all_qres::process_right(query_ctx &ctx, const qr_tuple &v) {
  PROF_PRE;
  res_.push_back(v);
  // consume_(gdb, v);
  PROF_POST(1);
}

void union_all_qres::r_finish(query_ctx &ctx) { }
void union_all_qres::finish(query_ctx &ctx) { qop::default_finish(ctx); }

/* ------------------------------------------------------------------------ */

void count_result::dump(std::ostream &os) const {
  os << "count_result() - " << PROF_DUMP;
}

void count_result::process(query_ctx &ctx, const qr_tuple &v) {
  count_++;
}

void count_result::finish(query_ctx &ctx) {
  consume_(ctx, {query_result(count_)});
  finish_(ctx);
}

/* ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------ */

void collect_result::dump(std::ostream &os) const {
  os << "collect_result() - " << PROF_DUMP;
}


void collect_result::process(query_ctx &ctx, const qr_tuple &v) {
  std::lock_guard<std::mutex> lock(collect_mtx);
  PROF_PRE;
  // we transform node and relationship into their string representations ...
  qr_tuple res(v.size());
  auto my_visitor = boost::hana::overload(
      [&](const node_description& n) { return n.to_string(); },
      [&](const rship_description& r) { return r.to_string(); },
      [&](node *n) { return ctx.gdb_->get_node_description(n->id()).to_string(); },
      [&](relationship *r) {
        return ctx.gdb_->get_rship_description(r->id()).to_string();
      },
      [&](int i) { return std::to_string(i); },
      [&](double d) { return std::to_string(d); },
      [&](const std::string &s) { return s; },
      [&](uint64_t ll) { return std::to_string(ll); },
      [&](null_t n) { return std::string("NULL"); },
      [&](array_t arr) {
        auto astr = std::string("[ ");
        for (auto elem : arr.elems)
          astr += (std::to_string(elem) + std::string(" "));
        astr += std::string("]");
        return astr; },
      [&](ptime dt) { return to_iso_extended_string(dt); });
  for (std::size_t i = 0; i < v.size(); i++) {
    res[i] = boost::apply_visitor(my_visitor, v[i]);
  }

  results_.data.push_back(res);
  PROF_POST(1);
}

void collect_result::finish(query_ctx &ctx) { 
  results_.notify(); 
}

/* ------------------------------------------------------------------------ */

void end_pipeline::dump(std::ostream &os) const { os << "end_pipeline()"; }

void end_pipeline::process() { return; }

#ifdef QOP_RECOVERY
/* ------------------------------------------------------------------------ */

void persist_result::dump(std::ostream &os) const { os << "persist()"; }

void persist_result::process(query_ctx &ctx, const qr_tuple &v) { 
  qr_tuple t = v;
#ifdef USE_PMDK
  ctx.gdb_->store_query_result(t, 0);
#endif
  consume_(ctx, v);
}


void recover_scan::dump(std::ostream &os) const { os << "recover_scan()"; }

void recover_scan::start(query_ctx &ctx) {
  std::mutex mtx;
  
  ctx.gdb_->recover_scan_parallel([&](const qr_tuple &qr, int tuple_id) {
    std::lock_guard<std::mutex> lck(mtx);
    tuple_map_[tuple_id].push_back(qr.front());
    //f(tuple_map_[tuple_id].size() == 2)
     // consume_(gdb, tuple_map_[tuple_id]);
  });
  finish(ctx);  
}

void recover_scan::finish(query_ctx &ctx) {
  for(auto & t : tuple_map_) {
      consume_(ctx, t.second);
  }
  qop::default_finish(ctx);
}
#endif

/* ------------------------------------------------------------------------ */

projection::projection(const expr_list &exprs, std::vector<projection_expr>& prexpr) : exprs_(exprs), prexpr_(prexpr) {
  type_ = qop_type::project;
  init_expr_vars();
}

projection::projection(const expr_list &exprs) : exprs_(exprs) {
  type_ = qop_type::project;
  init_expr_vars();
}

void projection::init_expr_vars() {
  if (exprs_.empty())
    return;
  // we build a mapping table where for each expression variable refering to a
  // property a new index is created
  auto it =
    std::max_element(exprs_.begin(), exprs_.end(),
                       [](expr &e1, expr &e2) { return e1.vidx < e2.vidx; });

  nvars_ = it->vidx + 1;
  npvars_ = 0;
  var_map_.resize(nvars_);
  for (auto &ex : exprs_) {
    if (ex.func != nullptr) {
      var_map_[ex.vidx] = nvars_ + ex.vidx;
      accessed_vars_.insert(ex.vidx);
      npvars_++;
    } else
      var_map_[ex.vidx] = 0;
  }
  /*
  std::ostringstream os;
  os << "var_map_[";
  for (auto v : var_map_)
    os << " " << v;
  os << " ]";
  spdlog::info("{}, accessed_vars_={}", os.str(), accessed_vars_.size());
  */
}

void projection::dump(std::ostream &os) const {
  os << "project([";
  for (auto &ex : exprs_) {
    os << " $" << ex.vidx;
    if (ex.func != nullptr)
      os << ".func";
  }
  os << " ]) - " << PROF_DUMP;
}

void projection::process(query_ctx &ctx, const qr_tuple &v) {
  // First, we build a list of all node_/rship_description objects which appear
  // in the query result. This list is used as a cache for property functions.
  PROF_PRE;
  auto i = 0;
  auto num_accessed_vars = accessed_vars_.size();
  std::vector<query_result> pv(num_accessed_vars * 2);
  for (auto index : accessed_vars_) {
    pv[i] = v[index];
    if (var_map_[index] == 0)
      continue;
    if (v[index].type() == typeid(node *)) {
      auto n = boost::get<node *>(v[index]);
      pv[num_accessed_vars + i] = ctx.gdb_->get_node_description(n->id());
    } else if (v[index].type() == typeid(relationship *)) {
      auto r = boost::get<relationship *>(v[index]);
      pv[num_accessed_vars + i] = ctx.gdb_->get_rship_description(r->id());
    }
    else {
      pv[num_accessed_vars + i]  = null_val;
    }
    var_map_[index] = num_accessed_vars + i; // we update mapping table
    i++;
  }

  // Then, we process all projection functions...
  qr_tuple res(exprs_.size());
  for (auto i = 0u; i < exprs_.size(); i++) {
    auto &ex = exprs_[i];
    // spdlog::info("projection::process: pv={}, i={}, vidx={} --> {}", pv.size(), i, ex.vidx, var_map_[ex.vidx]);
    try {
      if (ex.func != nullptr)
        res[i] = ex.func(ctx, pv[var_map_[ex.vidx]]);
      else {
        query_result fwd = v[ex.vidx];
        res[i] = builtin::forward(fwd);
      }
    } catch (unknown_property& exc) { }
  }

  consume_(ctx, res);
  PROF_POST(1);
}

/* --------------------------------------------------------------------- */

namespace builtin {

query_result forward(query_result &pv) {
  if (pv.type() == typeid(node *)) {
    return boost::get<node *>(pv);
  } else if (pv.type() == typeid(relationship *)) {
    return boost::get<relationship *>(pv);
  } else if (pv.type() == typeid(int)) {
    return boost::get<int>(pv);
  } else if (pv.type() == typeid(double)) {
    return boost::get<double>(pv);
  } else if (pv.type() == typeid(std::string)) {
    return boost::get<std::string &>(pv);
  } else if (pv.type() == typeid(uint64_t)) {
    return boost::get<uint64_t>(pv);
  } else if (pv.type() == typeid(node_description)) {
    auto &nd = boost::get<node_description>(pv);
    return nd.to_string();
  } else if (pv.type() == typeid(ptime)) {
    return boost::get<ptime>(pv);
  } else if (pv.type() == typeid(array_t)) {
    return boost::get<array_t>(pv);
  } else if (pv.type() == typeid(null_t)) {
      return null_val;
  }
  spdlog::info("builtin::forward: unexpected type: {}", pv.type().name());
  return null_val;
}


bool has_property(query_result &pv, const std::string &key) {
  if (pv.type() == typeid(node_description &)) {
    auto nd = boost::get<node_description &>(pv);
    return nd.has_property(key);
  } else if (pv.type() == typeid(rship_description &)) {
    auto rd = boost::get<rship_description &>(pv);
    return rd.has_property(key);
  }
  return false;
}

bool has_label(query_result &pv, const std::string &l) {
  if (pv.type() == typeid(node_description &)) {
    auto nd = boost::get<node_description &>(pv);
    return nd.label == l;
  } else if (pv.type() == typeid(rship_description &)) {
    auto rd = boost::get<rship_description &>(pv);
    return rd.label == l;
  }
  return false;
}

query_result int_property(const query_result &pv, const std::string &key) {
  if (pv.type() == typeid(node_description &)) {
    auto nd = boost::get<const node_description &>(pv);
    auto o = get_property<int>(nd.properties, key);
    return o.has_value() ? query_result(o.value()) : query_result(null_val);
  } else if (pv.type() == typeid(rship_description &)) {
    auto rd = boost::get<const rship_description &>(pv);
    auto o = get_property<int>(rd.properties, key);
    return o.has_value() ? query_result(o.value()) : query_result(null_val);
  }
  return null_val;
}

query_result double_property(const query_result &pv, const std::string &key) {
  if (pv.type() == typeid(node_description &)) {
    auto nd = boost::get<const node_description &>(pv);
    auto o = get_property<double>(nd.properties, key);
    return o.has_value() ? query_result(o.value()) : query_result(null_val);
  } else if (pv.type() == typeid(rship_description &)) {
    auto rd = boost::get<const rship_description &>(pv);
    auto o = get_property<double>(rd.properties, key);
    return o.has_value() ? query_result(o.value()) : query_result(null_val);
  }
  return null_val;
}

query_result string_property(const query_result &pv, const std::string &key) {
  if (pv.type() == typeid(node_description &)) {
    auto nd = boost::get<const node_description &>(pv);
    auto o = get_property<std::string>(nd.properties, key);
    return o.has_value() ? query_result(o.value()) : query_result(null_val);
  } else if (pv.type() == typeid(rship_description &)) {
    auto rd = boost::get<const rship_description &>(pv);
    auto o = get_property<std::string>(rd.properties, key);
    return o.has_value() ? query_result(o.value()) : query_result(null_val);
  }

  return null_val;
}

query_result uint64_property(const query_result &pv, const std::string &key) {
  if (pv.type() == typeid(node_description &)) {
    auto nd = boost::get<const node_description &>(pv);
    auto o = get_property<uint64_t>(nd.properties, key);
    return o.has_value() ? query_result(o.value()) : query_result(null_val);
  } else if (pv.type() == typeid(rship_description &)) {
    auto rd = boost::get<const rship_description &>(pv);
    auto o = get_property<uint64_t>(rd.properties, key);
    return o.has_value() ? query_result(o.value()) : query_result(null_val);
  }
  return null_val;
}

query_result  ptime_property(const query_result &pv, const std::string &key) {
  if (pv.type() == typeid(node_description &)) {
    auto nd = boost::get<const node_description &>(pv);
    auto o = get_property<ptime>(nd.properties, key);
    return o.has_value() ? query_result(o.value()) : query_result(null_val);
  } else if (pv.type() == typeid(rship_description &)) {
    auto rd = boost::get<const rship_description &>(pv);
    auto o = get_property<ptime>(rd.properties, key);
    return o.has_value() ? query_result(o.value()) : query_result(null_val);
  }
  return null_val;
}

query_result pr_date(const query_result &pv, const std::string &key) {
  if (pv.type() == typeid(node_description &)) {
    auto nd = boost::get<const node_description &>(pv);
    if (nd.has_property(key)) {
      auto o = get_property<ptime>(nd.properties, key);
      return o.has_value() ? query_result(to_iso_extended_string(o.value().date()))
        : query_result(null_val);
    }
  } else if (pv.type() == typeid(rship_description &)) {
    auto rd = boost::get<const rship_description &>(pv);
    if (rd.has_property(key)) {
      auto o = get_property<ptime>(rd.properties, key);
      return o.has_value() ? query_result(to_iso_extended_string(o.value().date()))
        : query_result(null_val);
    }
  }
  return null_val;
}

query_result pr_year(const query_result &pv, const std::string &key) {
  if (pv.type() == typeid(node_description &)) {
    auto nd = boost::get<const node_description &>(pv);
    if (nd.has_property(key)) {
      auto o = get_property<ptime>(nd.properties, key);
      if (o.has_value()) {
        auto dt = to_iso_extended_string(o.value());
        auto yr = dt.substr(0, dt.find("-"));
        return query_result(std::stoi(yr));
      }
      return query_result(null_val);
    }
  } else if (pv.type() == typeid(rship_description &)) {
    auto rd = boost::get<const rship_description &>(pv);
    if (rd.has_property(key)) {
      auto o = get_property<ptime>(rd.properties, key);
      if (o.has_value()) {
        auto dt = to_iso_extended_string(o.value());
        auto yr = dt.substr(0, dt.find("-"));
        return query_result(std::stoi(yr));
      }
      return query_result(null_val);
    }
  }
  return null_val;
}

query_result pr_month(const query_result &pv, const std::string &key) {
  if (pv.type() == typeid(node_description &)) {
    auto nd = boost::get<const node_description &>(pv);
    if (nd.has_property(key)) {
      auto o = get_property<ptime>(nd.properties, key);
      if (o.has_value()) {
        auto dt = to_iso_extended_string(o.value());
        auto mo = dt.substr(5, 2);
        return query_result(std::stoi(mo));
      }
      return query_result(null_val);
    }
  } else if (pv.type() == typeid(rship_description &)) {
    auto rd = boost::get<const rship_description &>(pv);
    if (rd.has_property(key)) {
      auto o = get_property<ptime>(rd.properties, key);
      if (o.has_value()) {
        auto dt = to_iso_extended_string(o.value());
        auto mo = dt.substr(5, 2);
        return query_result(std::stoi(mo));
      }
      return query_result(null_val);
    }
  }
  return null_val;
}

std::string string_rep(query_result &res) {
  auto my_visitor =
      boost::hana::overload([&](node_description &n) { return n.to_string(); },
                            [&](rship_description &r) { return r.to_string(); },
                            [&](node *n) { return std::string(""); },
                            [&](relationship *r) { return std::string(""); },
                            [&](int i) { return std::to_string(i); },
                            [&](double d) { return std::to_string(d); },
                            [&](null_t n) { return std::string("NULL"); },
                            [&](array_t arr) {
                              auto astr = std::string("[ ");
                              for (auto elem : arr.elems)
                                astr += std::to_string(elem);
                              astr += std::string(" ]");
                              return astr; },
                            [&](const std::string &s) { return s; },
                            [&](uint64_t ll) { return std::to_string(ll); },
                            [&](ptime dt) { return to_iso_extended_string(dt); } );
  return boost::apply_visitor(my_visitor, res);
}

int to_int(const std::string &s) { return std::stoi(s); }

static boost::gregorian::date_facet *df =
    new boost::gregorian::date_facet{"%Y-%m-%d"};

static boost::posix_time::time_facet *dtf =
    new boost::posix_time::time_facet{"%Y-%m-%d %H:%M:%S"};

std::string int_to_datestring(int v) {
  auto d = boost::posix_time::from_time_t(v).date();
  std::ostringstream os;
  os.imbue(std::locale{std::cout.getloc(), df});
  os << d;
  return os.str();
}

std::string int_to_datestring(const query_result& v) {
  assert(v.type() == typeid(int));
 return int_to_datestring(boost::get<int>(v));
}

int datestring_to_int(const std::string &d) {
  boost::gregorian::date dt = boost::gregorian::from_simple_string(d);
  static ptime epoch(boost::gregorian::date(1970, 1, 1));
  time_duration::sec_type secs =
      (ptime(dt, seconds(0)) - epoch).total_seconds();
  return time_t(secs);
}

std::string int_to_dtimestring(int v) {
  auto d = boost::posix_time::from_time_t(v);
  std::ostringstream os;
  // os.imbue(std::locale{std::cout.getloc(), dtf});
  os << d;
  return os.str();
}

std::string int_to_dtimestring(const query_result& v) {
  assert(v.type() == typeid(int));
 return int_to_dtimestring(boost::get<int>(v));
}

int dtimestring_to_int(const std::string &dt) {
  ptime pdt = time_from_string(dt);
  static ptime epoch(boost::gregorian::date(1970, 1, 1));
  time_duration::sec_type secs = (pdt - epoch).total_seconds();
  return time_t(secs);
}

bool is_null(const query_result& pv) { return pv.type() == typeid(null_t); }

} // namespace builtin