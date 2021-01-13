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

#include <queue>

#include "shortest_path.hpp"

#include <boost/dynamic_bitset.hpp>

bool unweighted_shortest_path(graph_db_ptr gdb, node::id_t start, node::id_t stop,
        bool unidirectional, rship_predicate rpred, path_visitor visit, path_item &spath) {
    bool found = false;
    std::queue<path> frontier;
    boost::dynamic_bitset<> visited(gdb->get_nodes()->as_vec().capacity());
    std::vector<std::size_t> distance(gdb->get_nodes()->as_vec().capacity(),
                                        std::numeric_limits<uint64_t>::max());

    distance[start] = 0;
    visited.set(start);
    frontier.push({start});

    while (!frontier.empty()) {
        auto u = frontier.front();
        auto uid = u.back();    
        frontier.pop();

        auto& n = gdb->node_by_id(uid);
        visit(n, u);
       
        gdb->foreach_from_relationship_of_node(n, [&](auto &r) {
            auto vid = r.to_node_id();
            if (rpred(r) && !visited[vid]) {
                visited.set(vid);
                distance[vid] = distance[uid] + 1;
                path u2(u);
                u2.push_back(vid);
                frontier.push(u2);

                if (vid == stop) {
                    found = true;
                    spath.path_ = u2;
                    spath.hops_ = distance[vid];
                }
            }
        });

        if (unidirectional) {
            gdb->foreach_to_relationship_of_node(n, [&](auto &r) {
                auto vid = r.from_node_id();
                if (rpred(r) && !visited[vid]) {
                    visited.set(vid);
                    distance[vid] = distance[uid] + 1;
                    path u2(u);
                    u2.push_back(vid);
                    frontier.push(u2);

                    if (vid == stop) {
                        found = true;
                        spath.path_ = u2;
                        spath.hops_ = distance[vid];
                    }
                }
            });
        }

        if (found)
            return true;
    }
    return false;

}