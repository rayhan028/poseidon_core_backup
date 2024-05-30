/*
 * Copyright (C) 2019-2023 DBIS Group - TU Ilmenau, All Rights Reserved.
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

#include "qop_updates.hpp"
#include "expr_interpreter.hpp"

/* ------------------------------------------------------------------------ */

bool check_for_expr_in_properties(const properties_t& props) {
  for (auto& prop : props) {
        auto& any_val = prop.second;
        if (any_val.type() == typeid(expr)) {
          return true;
        }
  }
  return false;
}

properties_t eval_properties(query_ctx &ctx, const qr_tuple &v, const properties_t& props) {
  properties_t new_props;
    for (auto& prop : props) {
        auto& any_val = prop.second;
        if (any_val.type() == typeid(expr)) {
          auto ex = std::any_cast<expr>(any_val);
          auto val = interpret_expression(ctx, ex, v); 
          // boost::variant(val) -> value 
          new_props.insert({prop.first, qv_to_any(val)});
        }
        else
          new_props.insert({prop.first, prop.second});
    }
    return new_props;
}  

/* ------------------------------------------------------------------------ */

create_node::create_node(const std::string &l, const properties_t &p) : label(l), props(p) {
  type_ = qop_type::create;
  expr_in_properties_= check_for_expr_in_properties(props);
}

void create_node::dump(std::ostream &os) const {
  os << "create_node([" << label << "]";
  if (!props.empty()) {
    os << ", {";
    bool first = true;
    for (auto &p : props) {
      if (!first)
        os << ", ";
      os << p.first << ": " << p.second;
      first = false;
    }
    os << "}";
  }
  os << ")";
}

void create_node::start(query_ctx &ctx) { 
  // evaluate expressions in property
  if (expr_in_properties_) {
    qr_tuple dummy;
    auto new_props = eval_properties(ctx, dummy, props);
    auto &n = ctx.gdb_->node_by_id(ctx.gdb_->add_node(label, new_props, true));
    consume_(ctx, {&n});
  }
  else {
    auto &n = ctx.gdb_->node_by_id(ctx.gdb_->add_node(label, props, true));
    consume_(ctx, {&n});
  }
}

void create_node::process(query_ctx &ctx, const qr_tuple &v) {
  // evaluate expressions in property
  if (expr_in_properties_) {
      auto new_props = eval_properties(ctx, v, props);
      auto &n = ctx.gdb_->node_by_id(ctx.gdb_->add_node(label, props, true));
      auto v2 = append(v, query_result(&n));
      consume_(ctx, v2);
  }
  else {
    auto &n = ctx.gdb_->node_by_id(ctx.gdb_->add_node(label, props, true));
    auto v2 = append(v, query_result(&n));
    consume_(ctx, v2);
  }
}

/*
bool create_node::check_for_expr_in_properties() {
  for (auto& prop : props) {
        auto& any_val = prop.second;
        if (any_val.type() == typeid(expr)) {
          return true;
        }
  }
  return false;
}

properties_t create_node::eval_properties(query_ctx &ctx, const qr_tuple &v) {
  properties_t new_props;
    for (auto& prop : props) {
        auto& any_val = prop.second;
        if (any_val.type() == typeid(expr)) {
          auto ex = std::any_cast<expr>(any_val);
          auto val = interpret_expression(ctx, ex, v); 
          // boost::variant(val) -> value 
          new_props.insert({prop.first, qv_to_any(val)});
        }
        else
          new_props.insert({prop.first, prop.second});
    }
    return new_props;
}
*/
/* ------------------------------------------------------------------------ */

void create_relationship::dump(std::ostream &os) const {
  os << "create_relationship(" << src_des_nodes_.first;
  os << "->"; 
  os << src_des_nodes_.second << ", [" << label << "]";
  if (!props.empty()) {
    os << ", {";
    bool first = true;
    for (auto &p : props) {
      if (!first)
        os << ", ";
      os << p.first << ": " << p.second;
      first = false;
    }
    os << "}";
  }
  os << ")";
}

void create_relationship::process(query_ctx &ctx, const qr_tuple &v) {
  if (expr_in_properties_ == 0)
    expr_in_properties_ = check_for_expr_in_properties(props) ? 1 : -1;

  assert(v[src_des_nodes_.first].which() == node_ptr_type);
  auto n1 = qv_get_node(v[src_des_nodes_.first]);
  assert(v[src_des_nodes_.second].which() == node_ptr_type);
  auto n2 = qv_get_node(v[src_des_nodes_.second]);

  relationship::id_t rid = 0;

  if (expr_in_properties_ > 0) {
      auto new_props = eval_properties(ctx, v, props);
      rid = ctx.gdb_->add_relationship(n1->id(), n2->id(), label, new_props, true);
  }
  else
    rid = ctx.gdb_->add_relationship(n1->id(), n2->id(), label, props, true);

  auto& r = ctx.gdb_->rship_by_id(rid);
  auto v2 = append(v, query_result(&r));
  
  consume_(ctx, v2);
}

/* ------------------------------------------------------------------------ */

void update_node::dump(std::ostream &os) const {
  os << "update_node([ " << var_no_ << " ]";
  if (!props.empty()) {
    os << ", {";
    bool first = true;
    for (auto &p : props) {
      if (!first)
        os << ", ";
      os << p.first << ": " << p.second;
      first = false;
    }
    os << "}";
  }
  os << ")=>";
  if (subscriber_)
    subscriber_->dump(os);
}

void update_node::process(query_ctx &ctx, const qr_tuple &v) {
  // fetch the node to be updated
  auto &ge = v[var_no_];
  assert(ge.which() == node_ptr_type); // TODO: raise exception
  auto n = boost::get<node *>(ge);
  // update the node
  ctx.gdb_->update_node(*n, props);
  consume_(ctx, v);
}

/* ------------------------------------------------------------------------ */

void detach_node::dump(std::ostream &os) const {
  os << "detach_node([" "])=>";
  if (subscriber_)
    subscriber_->dump(os);
}

void detach_node::process(query_ctx &ctx, const qr_tuple &v) {
  qr_tuple res = v;
  node * n = nullptr;
  if (pos_ == std::numeric_limits<std::size_t>::max()) {
    n = boost::get<node *>(v.back());
    res[(v.size() - 1)] = query_result(null_t(-1));
  }
  else {
    n = boost::get<node *>(v[pos_]);
    res[pos_] = query_result(null_t(-1));
  }

  std::list<relationship::id_t> rships;
  ctx.foreach_from_relationship_of_node(*n, [&](relationship &r) {
    rships.push_back(r.id()); });
  ctx.foreach_to_relationship_of_node(*n, [&](relationship &r) {
    rships.push_back(r.id()); });

  for (auto rid : rships)
    ctx.gdb_->delete_relationship(rid);

  // for(auto rel : rels_)
  //   res[rel] = query_result(null_t(-1));

  ctx.gdb_->delete_node(n->id());

  consume_(ctx, res);
}

/* ------------------------------------------------------------------------ */

void remove_node::dump(std::ostream &os) const {
  os << "remove_node([" "])=>";
  if (subscriber_)
    subscriber_->dump(os);
}

void remove_node::process(query_ctx &ctx, const qr_tuple &v) {
  qr_tuple res = v;
  node * n = nullptr;
  if (pos_ == std::numeric_limits<std::size_t>::max()) {
    n = boost::get<node *>(v.back());
    res[(v.size() - 1)] = query_result(null_t(-1));
  }
  else {
    n = boost::get<node *>(v[pos_]);
    res[pos_] = query_result(null_t(-1));
  }

  ctx.gdb_->delete_node(n->id());
  consume_(ctx, res);
}

/* ------------------------------------------------------------------------ */

void remove_relationship::dump(std::ostream &os) const {
  os << "remove_relationship([" "])=>";
  if (subscriber_)
    subscriber_->dump(os);
}

void remove_relationship::process(query_ctx &ctx, const qr_tuple &v) {
  qr_tuple res = v;
  relationship::id_t rid = 0;
  if (pos_ == std::numeric_limits<std::size_t>::max()) {
    std::cout << "remove: " << v.back() << std::endl;
    if (v.back().which() == rship_ptr_type) {
      auto r = qv_get_relationship(v.back());
      rid = r->id();
    }
    else {
      auto r = qv_get_rship_descr(v.back());
      rid = r.id;
    }
    res[(v.size() - 1)] = query_result(null_t(-1));
  }
  else {
    if (v[pos_].which() == rship_ptr_type) {
      auto r = qv_get_relationship(v[pos_]);
      rid = r->id();
    }
    else {
      auto r = qv_get_rship_descr(v[pos_]);
      rid = r.id;
    }
    res[pos_] = query_result(null_t(-1));
  }
  spdlog::info("delete rship #{}", rid);
  ctx.gdb_->delete_relationship(rid);
  consume_(ctx, res);
}