
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

#include "qop_scans.hpp"

void node_scan::start(query_ctx &ctx) {
  if (label.empty() && labels.empty())
      ctx.parallel_nodes([&](node &n) { PROF_PRE; consume_(ctx, {&n}); PROF_POST(1); });
  else if (!label.empty())
    // ctx.nodes_by_label(label, [&](node &n) { PROF_PRE; consume_(ctx, {&n}); PROF_POST(1); });
    ctx.parallel_nodes(label, [&](node &n) { PROF_PRE; consume_(ctx, {&n}); PROF_POST(1); });
  else
    ctx.nodes_by_label(labels, [&](node &n) { PROF_PRE; consume_(ctx, {&n}); PROF_POST(1); });

  qop::default_finish(ctx);
}

void node_scan::dump(std::ostream &os) const {
  if (labels.size() > 0) {
    os << "node_scan(["; 
    for (auto& l : labels) 
      os << " " << l;
    os << "]) - " << PROF_DUMP;
  }
  else
    os << "node_scan([" << label << "]) - " << PROF_DUMP;
}

void relationship_scan::start(query_ctx &ctx) {
  if (label.empty() && labels.empty())
      ctx.parallel_relationships([&](relationship &r) { PROF_PRE; consume_(ctx, {&r}); PROF_POST(1); });
  else if (!label.empty())
    // ctx.nodes_by_label(label, [&](node &n) { PROF_PRE; consume_(ctx, {&n}); PROF_POST(1); });
    ctx.parallel_relationships(label, [&](relationship &r) { PROF_PRE; consume_(ctx, {&r}); PROF_POST(1); });
  else
    ctx.relationships_by_label(labels, [&](relationship &r) { PROF_PRE; consume_(ctx, {&r}); PROF_POST(1); });

  qop::default_finish(ctx);
}

void relationship_scan::dump(std::ostream &os) const {
  if (labels.size() > 0) {
    os << "relationship_scan(["; 
    for (auto& l : labels) 
      os << " " << l;
    os << "]) - " << PROF_DUMP;
  }
  else
    os << "relationship_scan([" << label << "]) - " << PROF_DUMP;
}

void index_scan::start(query_ctx &ctx) {
  if (idxs.empty())
    ctx.gdb_->index_lookup(idx, key, [&](node &n) { PROF_PRE; consume_(ctx, {&n}); PROF_POST(1); });
  else
    ctx.gdb_->index_lookup(idxs, key, [&](node &n) { PROF_PRE; consume_(ctx, {&n}); PROF_POST(1); });

  qop::default_finish(ctx);
}

void index_scan::dump(std::ostream &os) const {
  os << "index_scan([" << key << "]) - " << PROF_DUMP;
}

void start_pipeline::dump(std::ostream &os) const {
  os << "start_pipeline()"; 
}

void start_pipeline::start(query_ctx &ctx, const qr_tuple &v) {
  consume_(ctx, v);  
}

void node_by_id::dump(std::ostream &os) const {
  os << "node_by_id([" << id_ << "]) - " << PROF_DUMP;
}

void node_by_id::start(query_ctx &ctx) {
  auto& n = ctx.gdb_->node_by_id(id_);  
  PROF_PRE; 
  consume_(ctx, {&n}); 
  PROF_POST(1); 
  qop::default_finish(ctx);
}
