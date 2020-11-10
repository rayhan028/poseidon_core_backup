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

#include "qop.hpp"

using namespace boost::posix_time;

/* ------------------------------------------------------------------------ */

void scan_nodes::start(graph_db_ptr &gdb) {
  if (label.empty() && labels.empty())
    gdb->parallel_nodes([&](node &n) { consume_(gdb, {&n}); });
  else if (!label.empty())
    gdb->nodes_by_label(label, [&](node &n) { consume_(gdb, {&n}); });
  // TODO: in case of calling parallel_nodes we should handle this differently
  else
    gdb->nodes_by_label(labels, [&](node &n) { consume_(gdb, {&n}); });
  qop::default_finish(gdb);
}

void scan_nodes::dump(std::ostream &os) const {
  os << "scan_nodes([" << label << "])=>";
  if (subscriber_)
    subscriber_->dump(os);
}

/* ------------------------------------------------------------------------ */

void index_scan::start(graph_db_ptr &gdb) {
  gdb->index_lookup(idx, key, [&](node &n) { consume_(gdb, {&n}); });
  qop::default_finish(gdb);
}

void index_scan::dump(std::ostream &os) const {
  os << "index_scan([" << key << "])=>";
  if (subscriber_)
    subscriber_->dump(os);
}

/* ------------------------------------------------------------------------ */

void foreach_from_relationship::process(graph_db_ptr &gdb, const qr_tuple &v) {
  node *n = nullptr;
  if (npos == std::numeric_limits<int>::max())
    n = boost::get<node *>(v.back());
  else
    n = boost::get<node *>(v[npos]);
  
  if (lcode == 0)
    lcode = gdb->get_code(label);

  gdb->foreach_from_relationship_of_node(*n, lcode, [&](relationship &r) {
    auto v2 = append(v, query_result(&r));
    consume_(gdb, v2);
  });
}

void foreach_from_relationship::dump(std::ostream &os) const {
  os << "foreach_from_relationship([" << label << "])=>";
  if (subscriber_)
    subscriber_->dump(os);
}

/* ------------------------------------------------------------------------ */

void foreach_variable_from_relationship::process(graph_db_ptr &gdb,
                                                 const qr_tuple &v) {
  node *n = nullptr;
  if (npos == std::numeric_limits<int>::max())
    n = boost::get<node *>(v.back());
  else
    n = boost::get<node *>(v[npos]);

  if (lcode == 0)
    lcode = gdb->get_code(label);

  gdb->foreach_variable_from_relationship_of_node(
      *n, lcode, min_range, max_range, [&](relationship &r) {
        auto v2 = append(v, query_result(&r));
        consume_(gdb, v2);
      });
}

void foreach_variable_from_relationship::dump(std::ostream &os) const {
  os << "foreach_variable_from_relationship([" << label << ", (" << min_range
     << "," << max_range << ")])=>";
  if (subscriber_)
    subscriber_->dump(os);
}

/* ------------------------------------------------------------------------ */

void foreach_to_relationship::process(graph_db_ptr &gdb, const qr_tuple &v) {
  auto n = boost::get<node *>(v.back());
  if (lcode == 0)
    lcode = gdb->get_code(label);
  gdb->foreach_to_relationship_of_node(*n, lcode, [&](relationship &r) {
    auto v2 = append(v, query_result(&r));
    consume_(gdb, v2);
  });
}

void foreach_to_relationship::dump(std::ostream &os) const {
  os << "foreach_to_relationship([" << label << "])=>";
  if (subscriber_)
    subscriber_->dump(os);
}

/* ------------------------------------------------------------------------ */
void foreach_variable_to_relationship::process(graph_db_ptr &gdb,
                                               const qr_tuple &v) {
  auto n = boost::get<node *>(v.back());
  if (lcode == 0)
    lcode = gdb->get_code(label);

  gdb->foreach_variable_to_relationship_of_node(
      *n, lcode, min_range, max_range, [&](relationship &r) {
        auto v2 = append(v, query_result(&r));
        consume_(gdb, v2);
      });
}

void foreach_variable_to_relationship::dump(std::ostream &os) const {
  os << "foreach_variable_to_relationship([" << label << ", (" << min_range
     << "," << max_range << ")])=>";
  if (subscriber_)
    subscriber_->dump(os);
}

/* ------------------------------------------------------------------------ */

void is_property::dump(std::ostream &os) const {
  os << "is_property([" << property << "])=>";
  if (subscriber_)
    subscriber_->dump(os);
}

void is_property::process(graph_db_ptr &gdb, const qr_tuple &v) {
  auto n = v.back();
  if (pcode == 0)
    pcode = gdb->get_code(property);

  if (n.type() == typeid(node *)) {
    if (gdb->is_node_property(*(boost::get<node *>(n)), pcode, predicate))
      consume_(gdb, v);
  } else if (n.type() == typeid(relationship *)) {
    if (gdb->is_relationship_property(*(boost::get<relationship *>(n)), pcode,
                                      predicate))
      consume_(gdb, v);
  }
}

/* ------------------------------------------------------------------------ */

void node_has_label::dump(std::ostream &os) const {
  os << "node_has_label([" << label << "])=>";
  if (subscriber_)
    subscriber_->dump(os);
}

void node_has_label::process(graph_db_ptr &gdb, const qr_tuple &v) {
  auto n = boost::get<node *>(v.back());
  if (labels.empty()) {
    if (lcode == 0)
      lcode = gdb->get_code(label);
    if (n->node_label == lcode) {
      consume_(gdb, v);
    }
  }
  else {
    for (auto &label : labels) {
      lcode = gdb->get_code(label);
      if (n->node_label == lcode) {
        consume_(gdb, v);
        break;
      }
    }
  }
}

/* ------------------------------------------------------------------------ */

void get_from_node::process(graph_db_ptr &gdb, const qr_tuple &v) {
  auto rship = boost::get<relationship *>(v.back());
  auto v2 = append(v, query_result(&(gdb->node_by_id(rship->src_node))));
  consume_(gdb, v2);
}

void get_from_node::dump(std::ostream &os) const {
  os << "get_from_node()=>";
  if (subscriber_)
    subscriber_->dump(os);
}

/* ------------------------------------------------------------------------ */

void get_to_node::process(graph_db_ptr &gdb, const qr_tuple &v) {
  auto rship = boost::get<relationship *>(v.back());
  auto v2 = append(v, query_result(&(gdb->node_by_id(rship->dest_node))));
  consume_(gdb, v2);
}

void get_to_node::dump(std::ostream &os) const {
  os << "get_to_node()=>";
  if (subscriber_)
    subscriber_->dump(os);
}

/* ------------------------------------------------------------------------ */

void printer::dump(std::ostream &os) const { os << "printer()"; }

void printer::process(graph_db_ptr &gdb, const qr_tuple &v) {
  auto my_visitor = boost::hana::overload(
      [&](node *n) { std::cout << gdb->get_node_description(n->id()); },
      [&](relationship *r) { std::cout << gdb->get_relationship_label(*r); },
      [&](int i) { std::cout << i; }, [&](double d) { std::cout << d; },
      [&](const std::string &s) { std::cout << s; },
      [&](uint64_t ll) { std::cout << ll; },
      [&](null_t n) { std::cout << "NULL"; },
      [&](ptime dt) { std::cout << dt; });
  for (auto &ge : v) {
    boost::apply_visitor(my_visitor, ge);
    std::cout << " ";
  }
  std::cout << "\n";
}

/* ------------------------------------------------------------------------ */

void limit_result::dump(std::ostream &os) const {
  os << "limit([" << num_ << "])";
  if (subscriber_)
    subscriber_->dump(os);
}

void limit_result::process(graph_db_ptr &gdb, const qr_tuple &v) {
  if (processed_ < num_) {
    consume_(gdb, v);
    processed_++;
  }
}

/* ------------------------------------------------------------------------ */

void nodes_connected::dump(std::ostream &os) const {
  os << "nodes_connected([" "])=>";
  if (subscriber_)
    subscriber_->dump(os);
}

void nodes_connected::process(graph_db_ptr &gdb, const qr_tuple &v) {
  auto src = boost::get<node *>(v[src_des_nodes_.first]);
  auto des = boost::get<node *>(v[src_des_nodes_.second]);
  bool flag = true;

  gdb->foreach_from_relationship_of_node((*src), [&](auto &r) {
      if (r.to_node_id() == des->id()){
        flag = false;
        auto res = append(v, query_result(&r));
        consume_(gdb, res); //TODO: fix for potential result tuple size mismatch
      }
  });

  if (flag){
    auto res = append(v, query_result(std::string("[0]{}")));
    consume_(gdb, res);
  }
}

/* ------------------------------------------------------------------------ */

void order_by::dump(std::ostream &os) const {
  os << "order_by([])=>";
  if (subscriber_)
    subscriber_->dump(os);
}

void order_by::process(graph_db_ptr &gdb, const qr_tuple &v) {
  results_.append(v);
}

void order_by::finish(graph_db_ptr &gdb) {
  if (cmp_func_ != nullptr)
    results_.sort(cmp_func_);
  else
    results_.sort(sort_spec_);
  for (auto &v : results_.data) {
    consume_(gdb, v);
  }
  finish_(gdb);
}

/* ------------------------------------------------------------------------ */

void group_by::dump(std::ostream &os) const {
  os << "group_by([])=>";
  if (subscriber_)
    subscriber_->dump(os);
}

void group_by::process(graph_db_ptr &gdb, const qr_tuple &v) {
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

  // building a map of grouping keys and position in result_set vector
  const auto itr = grpkey_map_.find(grpkeys);
  if (itr != grpkey_map_.end())
    res_set_vec_[itr->second].append(v);
  else {
    grpkey_map_.emplace(grpkeys, grpkey_cnt_);
    grpkey_set_.push_back(grpkeys);
    res_set_vec_.emplace_back();
    res_set_vec_[grpkey_cnt_++].append(v);
  }
}

void group_by::finish(graph_db_ptr &gdb) {
  for (auto &grp : grpkey_set_) {
    qr_tuple res;
    auto gpos = grpkey_map_[grp];
    auto tpl = res_set_vec_[gpos].data.front();
    for (auto pos : grpkey_pos_)
      res.push_back(tpl[pos]);
    consume_(gdb, res);
  }
  finish_(gdb);
}

/* ------------------------------------------------------------------------ */

aggr_ops::aggr_ops(const std::vector<result_set> &grps,
                    const std::vector<std::pair<std::string, int>> &aggrs) :
                    grpkey_cnt_(0), total_(false), total_cnt_(0), aggrs_(aggrs), res_set_vec_(grps) {}

void aggr_ops::dump(std::ostream &os) const {
  os << "aggr_ops([])=>";
  if (subscriber_)
    subscriber_->dump(os);
}

void aggr_ops::process(graph_db_ptr &gdb, const qr_tuple &v) {
  auto res = v;
  auto &grp_data = res_set_vec_[grpkey_cnt_++].data;

  for (auto &aggr : aggrs_) {
    if (aggr.first.compare("count") == 0) {
      uint64_t gcnt = grp_data.size();
      res = append(res, query_result(gcnt));
    }
    else if (aggr.first.compare("pcount") == 0) {
      if (!total_) {
        for (auto &res : res_set_vec_)
          total_cnt_ += res.data.size();
        total_ = true;
      }
      uint64_t gcnt = grp_data.size();
      double p_gcnt = (gcnt / (double)total_cnt_) * 100;
      res = append(res, query_result(p_gcnt));
    }
    else if (aggr.first.compare("sum") == 0) {
      auto key = grp_data.front()[aggr.second];
      if (key.type() == typeid(int)) {
        int gsum = 0;
        for (auto &tpl : grp_data)
          gsum += boost::get<int>(tpl[aggr.second]);
        res = append(res, query_result(gsum));
      }
      else if (key.type() == typeid(uint64_t)) {
        uint64_t gsum = 0;
        for (auto &tpl : grp_data)
          gsum += boost::get<uint64_t>(tpl[aggr.second]);
        res = append(res, query_result(gsum));
      }
      else if (key.type() == typeid(double)) {
        double gsum = 0.0;
        for (auto &tpl : grp_data)
          gsum += boost::get<double>(tpl[aggr.second]);
        res = append(res, query_result(gsum));
      }
    }
    else if (aggr.first.compare("avg") == 0) {
      uint64_t gcnt = grp_data.size();
      auto key = grp_data.front()[aggr.second];
      if (key.type() == typeid(int)) {
        int gsum = 0;
        for (auto &tpl : grp_data)
          gsum += boost::get<int>(tpl[aggr.second]);
        double g_avg = gsum / gcnt;
        res = append(res, query_result(g_avg));
      }
      else if (key.type() == typeid(uint64_t)) {
        uint64_t gsum = 0;
        for (auto &tpl : grp_data)
          gsum += boost::get<uint64_t>(tpl[aggr.second]);
        double g_avg = gsum / gcnt;
        res = append(res, query_result(g_avg));
      }
      else if (key.type() == typeid(double)) {
        double gsum = 0.0;
        for (auto &tpl : grp_data)
          gsum += boost::get<double>(tpl[aggr.second]);
        double g_avg = gsum / gcnt;
        res = append(res, query_result(g_avg));
      }
    }
  }

  consume_(gdb, res); 
}

/* ------------------------------------------------------------------------ */

void count_aggr::dump(std::ostream &os) const {
  os << "count_aggr()=>";
  if (subscriber_)
    subscriber_->dump(os);
}

void count_aggr::process(graph_db_ptr &gdb, const qr_tuple &v) {
  uint64_t gcnt = res_set_vec_[grpkey_cnt_++].data.size();
  auto v2 = append(v, query_result(gcnt));
  if (percentage) {
    if (!total) {
      for (auto &res : res_set_vec_)
        total_cnt += res.data.size();
      total = true;
    }
    double p_gcnt = (gcnt / (double)total_cnt) * 100;
    auto v3 = append(v2, query_result(p_gcnt));
    consume_(gdb, v3);
    return;
  }

  consume_(gdb, v2); 
}

/* ------------------------------------------------------------------------ */
  

void sum_aggr::dump(std::ostream &os) const {
  os << "sum_aggr([])=>";
  if (subscriber_)
    subscriber_->dump(os);
}

void sum_aggr::process(graph_db_ptr &gdb, const qr_tuple &v) {
  auto v2 = v;
  for (auto pos : grpkey_pos_) {
    auto &grp_data = res_set_vec_[grpkey_cnt_++].data;
    auto key = grp_data.front()[pos];
    if (key.type() == typeid(int)) {
      int gsum = 0;
      for (auto &tpl : grp_data)
        gsum += boost::get<int>(tpl[pos]);
      v2 = append(v2, query_result(gsum));
    }
    else if (key.type() == typeid(uint64_t)) {
      uint64_t gsum = 0;
      for (auto &tpl : grp_data)
        gsum += boost::get<uint64_t>(tpl[pos]);
      v2 = append(v2, query_result(gsum));
    }
    else if (key.type() == typeid(double)) {
      double gsum = 0.0;
      for (auto &tpl : grp_data)
        gsum += boost::get<double>(tpl[pos]);
      v2 = append(v2, query_result(gsum));
    }
  }
  consume_(gdb, v2);
}

/* ------------------------------------------------------------------------ */

void avg_aggr::dump(std::ostream &os) const {
  os << "avg_aggr([])=>";
  if (subscriber_)
    subscriber_->dump(os);
}

void avg_aggr::process(graph_db_ptr &gdb, const qr_tuple &v) { // TODO reuse count and sum
  auto v2 = v;
  for (auto pos : grpkey_pos_) {
    auto &grp_data = res_set_vec_[grpkey_cnt_++].data;
    uint64_t gcnt = grp_data.size();
    auto key = grp_data.front()[pos];
    if (key.type() == typeid(int)) {
      int gsum = 0;
      for (auto &tpl : grp_data)
        gsum += boost::get<int>(tpl[pos]);
      double g_avg = gsum / gcnt;
      v2 = append(v2, query_result(g_avg));
    }
    else if (key.type() == typeid(uint64_t)) {
      uint64_t gsum = 0;
      for (auto &tpl : grp_data)
        gsum += boost::get<uint64_t>(tpl[pos]);
      double g_avg = gsum / gcnt;
      v2 = append(v2, query_result(g_avg));
    }
    else if (key.type() == typeid(double)) {
      double gsum = 0.0;
      for (auto &tpl : grp_data)
        gsum += boost::get<double>(tpl[pos]);
      double g_avg = gsum / gcnt;
      v2 = append(v2, query_result(g_avg));
    }
  }
  consume_(gdb, v2);
}

/* ------------------------------------------------------------------------ */

void filter_tuple::dump(std::ostream &os) const {
  os << "filter_tuple([])=>";
  if (subscriber_)
    subscriber_->dump(os);
}

void filter_tuple::process(graph_db_ptr &gdb, const qr_tuple &v) {
  bool tp = pred_func_(v);
  if (tp)
    consume_(gdb, v);
}

/* ------------------------------------------------------------------------ */

void qr_tuple_append::dump(std::ostream &os) const {
  os << "qr_tuple_append([])=>";
  if (subscriber_)
    subscriber_->dump(os);
}

void qr_tuple_append::process(graph_db_ptr &gdb, const qr_tuple &v) {
  auto v1 = v;
  auto res = func_(v1);
  auto v2 = append(v1, res);
  consume_(gdb, v2);
}

/* ------------------------------------------------------------------------ */

void result_set::wait() {
  std::unique_lock<std::mutex> lock(m);
  cond_var.wait(lock, [&] { return ready.load(); });
}

void result_set::notify() {
  std::lock_guard<std::mutex> lock(m);
  ready = true;
  cond_var.notify_one();
}

bool result_set::operator==(const result_set &other) const {
  return data == other.data;
}

bool result_set::qr_compare(const qr_tuple &qr1, const qr_tuple &qr2,
                            const sort_spec &spec) {
  // TODO
  return true;
}

void result_set::sort(const sort_spec &spec) {
  data.sort([&](const qr_tuple &v1, const qr_tuple &v2) {
    return qr_compare(v1, v2, spec);
  });
}

void result_set::sort(
    std::function<bool(const qr_tuple &, const qr_tuple &)> cmp) {
  data.sort(cmp);
}

std::ostream &operator<<(std::ostream &os, const result_set &rs) {
  auto my_visitor = boost::hana::overload(
      [&](node *n) { /*os << gdb->get_node_description(*n); */ },
      [&](relationship *r) { /* os << gdb->get_relationship_label(*r); */ },
      [&](int i) { os << i; }, [&](double d) { os << d; },
      [&](const std::string &s) { os << s; }, [&](uint64_t ll) { os << ll; },
      [&](null_t n) { os << "NULL"; },
      [&](ptime dt) { os << dt; }); 

  for (const qr_tuple &qv : rs.data) {
    os << "{ ";

    auto i = 0u;
    for (const auto &qr : qv) {
      boost::apply_visitor(my_visitor, qr);
      if (++i < qv.size())
        os << ", ";
    }
    os << " }" << std::endl;
  }
  return os;
}

/* ------------------------------------------------------------------------ */

void collect_result::dump(std::ostream &os) const {
  os << "collect_result()";
  if (subscriber_)
    subscriber_->dump(os);
}

void collect_result::process(graph_db_ptr &gdb, const qr_tuple &v) {
  // we transform node and relationship into their string representations ...
  qr_tuple res(v.size());

  auto my_visitor = boost::hana::overload(
      [&](node *n) { return gdb->get_node_description(n->id()).to_string(); },
      [&](relationship *r) {
        return gdb->get_rship_description(r->id()).to_string();
      },
      [&](int i) { return std::to_string(i); },
      [&](double d) { return std::to_string(d); },
      [&](const std::string &s) { return s; }, 
      [&](uint64_t ll) { return std::to_string(ll); },
      [&](null_t n) { return std::string("NULL"); },
      [&](ptime dt) { return to_iso_extended_string(dt); }); 
  for (std::size_t i = 0; i < v.size(); i++) {
    res[i] = boost::apply_visitor(my_visitor, v[i]);
  }

  results_.data.push_back(res);
}

void collect_result::finish(graph_db_ptr &gdb) { results_.notify(); }

/* ------------------------------------------------------------------------ */

void end_pipeline::dump(std::ostream &os) const { os << "end_pipeline()"; }

void end_pipeline::process() { return; }

/* ------------------------------------------------------------------------ */

projection::projection(const expr_list &exprs) : exprs_(exprs) {
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
    os << " " << ex.vidx;
    if (ex.func != nullptr)
      os << "+";
  }
  os << " ])=>";
  if (subscriber_)
    subscriber_->dump(os);
}

void projection::process(graph_db_ptr &gdb, const qr_tuple &v) {
  // First, we build a list of all node_/rship_description objects which appear
  // in the query result. This list is used as a cache for property functions.

  auto i = 0;
  auto num_accessed_vars = accessed_vars_.size();
  std::vector<projection::pr_result> pv(num_accessed_vars * 2);
  for (auto index : accessed_vars_) {
    pv[i] = v[index];
    if (var_map_[index] == 0)
      continue;
    if (v[index].type() == typeid(node *)) {
      auto n = boost::get<node *>(v[index]);
      pv[num_accessed_vars + i] = gdb->get_node_description(n->id());
    } else if (v[index].type() == typeid(relationship *)) {
      auto r = boost::get<relationship *>(v[index]);
      pv[num_accessed_vars + i] = gdb->get_rship_description(r->id());
    }
    var_map_[index] = num_accessed_vars + i; // we update mapping table
    i++;
  }

  // Then, we process all projection functions...
  qr_tuple res(exprs_.size());
  for (auto i = 0u; i < exprs_.size(); i++) {
    // spdlog::info("projection::process: pv={}, i={}", pv.size(), i);
    auto &ex = exprs_[i];
    try {
      if (ex.func != nullptr)
        res[i] = ex.func(pv[var_map_[ex.vidx]]);
      else{
        pr_result fwd = v[ex.vidx];
        res[i] = builtin::forward(fwd);
      }
    } catch (unknown_property& exc) { }
  }

  consume_(gdb, res);
}

/* --------------------------------------------------------------------- */

namespace builtin {

query_result forward(projection::pr_result &pv) {
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
  } 
  spdlog::info("builtin::forward: unexpected type: {}", pv.type().name());
  return null_val;
}


bool has_property(projection::pr_result &pv, const std::string &key) {
  if (pv.type() == typeid(node_description &)) {
    auto nd = boost::get<node_description &>(pv);
    return nd.has_property(key);
  } else if (pv.type() == typeid(rship_description &)) {
    auto rd = boost::get<rship_description &>(pv);
    return rd.has_property(key);
  }
  return false; 
}
	
query_result int_property(projection::pr_result &pv, const std::string &key) {
  if (pv.type() == typeid(node_description &)) {
    auto nd = boost::get<node_description &>(pv);
    auto o = get_property<int>(nd.properties, key);
    return o.has_value() ? query_result(o.value()) : query_result(null_val);
  } else if (pv.type() == typeid(rship_description &)) {
    auto rd = boost::get<rship_description &>(pv);
    auto o = get_property<int>(rd.properties, key);
    return o.has_value() ? query_result(o.value()) : query_result(null_val);
  }
  return null_val;
}

query_result double_property(projection::pr_result &pv, const std::string &key) {
  if (pv.type() == typeid(node_description &)) {
    auto nd = boost::get<node_description &>(pv);
    auto o = get_property<double>(nd.properties, key);
    return o.has_value() ? query_result(o.value()) : query_result(null_val);
  } else if (pv.type() == typeid(rship_description &)) {
    auto rd = boost::get<rship_description &>(pv);
    auto o = get_property<double>(rd.properties, key);
    return o.has_value() ? query_result(o.value()) : query_result(null_val);
  }
  return null_val;
}

query_result string_property(projection::pr_result &pv, const std::string &key) {
  if (pv.type() == typeid(node_description &)) {
    auto nd = boost::get<node_description &>(pv);
    auto o = get_property<std::string>(nd.properties, key);
    return o.has_value() ? query_result(o.value()) : query_result(null_val);
  } else if (pv.type() == typeid(rship_description &)) {
    auto rd = boost::get<rship_description &>(pv);
    auto o = get_property<std::string>(rd.properties, key);
    return o.has_value() ? query_result(o.value()) : query_result(null_val);
  }

  return null_val;
}

query_result uint64_property(projection::pr_result &pv, const std::string &key) {
  if (pv.type() == typeid(node_description &)) {
    auto nd = boost::get<node_description &>(pv);
    auto o = get_property<uint64_t>(nd.properties, key);
    return o.has_value() ? query_result(o.value()) : query_result(null_val);
  } else if (pv.type() == typeid(rship_description &)) {
    auto rd = boost::get<rship_description &>(pv);
    auto o = get_property<uint64_t>(rd.properties, key);
    return o.has_value() ? query_result(o.value()) : query_result(null_val);
  }
  return null_val;
}

query_result  ptime_property(projection::pr_result &pv, const std::string &key) {
  if (pv.type() == typeid(node_description &)) {
    auto nd = boost::get<node_description &>(pv);
    auto o = get_property<ptime>(nd.properties, key);
    return o.has_value() ? query_result(o.value()) : query_result(null_val);
  } else if (pv.type() == typeid(rship_description &)) {
    auto rd = boost::get<rship_description &>(pv);
    auto o = get_property<ptime>(rd.properties, key);
    return o.has_value() ? query_result(o.value()) : query_result(null_val);
  }
  return null_val;
}

query_result pr_date(projection::pr_result &pv, const std::string &key) {
  if (pv.type() == typeid(node_description &)) {
    auto nd = boost::get<node_description &>(pv);
    if (nd.has_property(key)) {
      auto o = get_property<ptime>(nd.properties, key);
      return o.has_value() ? query_result(to_iso_extended_string(o.value().date())) 
        : query_result(null_val);
    }
  } else if (pv.type() == typeid(rship_description &)) {
    auto rd = boost::get<rship_description &>(pv);
    if (rd.has_property(key)) {
      auto o = get_property<ptime>(rd.properties, key);
      return o.has_value() ? query_result(to_iso_extended_string(o.value().date())) 
        : query_result(null_val);
    }
  }
  return null_val; 
}

query_result pr_year(projection::pr_result &pv, const std::string &key) {
  if (pv.type() == typeid(node_description &)) {
    auto nd = boost::get<node_description &>(pv);
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
    auto rd = boost::get<rship_description &>(pv);
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

query_result pr_month(projection::pr_result &pv, const std::string &key) {
  if (pv.type() == typeid(node_description &)) {
    auto nd = boost::get<node_description &>(pv);
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
    auto rd = boost::get<rship_description &>(pv);
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

std::string string_rep(projection::pr_result &res) {
  auto my_visitor =
      boost::hana::overload([&](node_description &n) { return n.to_string(); },
                            [&](rship_description &r) { return r.to_string(); },
                            [&](node *n) { return std::string(""); },
                            [&](relationship *r) { return std::string(""); },
                            [&](int i) { return std::to_string(i); },
                            [&](double d) { return std::to_string(d); },
                            [&](null_t n) { return std::string("NULL"); },
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

} // namespace builtin
