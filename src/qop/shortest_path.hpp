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

#ifndef shortest_path_hpp_
#define shortest_path_hpp_

#include "graph_db.hpp"

/**
 * Typedef for a predicate to check that a relationship is followed via the search.
 */
using rship_predicate = std::function<bool(relationship&)>;

/**
 * Typedef for a node visitor callback.
 */
using node_visitor = std::function<void(node&)>;

/**
 * Typedef for a node visitor callback which receives the full path.
 */
using path = std::vector<offset_t>;

using path_visitor = std::function<void(node&, const path&)>;

struct path_item {
    path path_;
    uint64_t distance_;
};

/**
 * A sequential implementation of unweighted shortest path search on the given graph. The search starts at the
 * given start node and follows all relationships satisfying the predicate rpred. For each visited
 * node, the node_visitor callback is invoked. The unidirectional flag determines whether only outgoing 
 * relationships are considered (unidirectional = false) or both outgoing and incoming relationships 
 * (unidirectional = true).
 */
bool unweighted_shortest_path(graph_db_ptr gdb, node::id_t start, node::id_t stop,
            bool unidirectional, rship_predicate rpred, path_visitor visit, path_item &spath);

#endif