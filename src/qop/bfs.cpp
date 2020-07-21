#include <queue>
#include <boost/dynamic_bitset.hpp>

#include "bfs.hpp"

void bfs(graph_db_ptr gdb, node::id_t start, bool unidirectional, rship_predicate rpred, node_visitor visit) {
    std::queue<node::id_t> frontier;
    boost::dynamic_bitset<> visited(gdb->get_nodes()->as_vec().capacity());
    frontier.push(start);

    while (!frontier.empty()) {
        auto v = frontier.front();    
        frontier.pop();
        if (visited[v])
            continue;

        visited.set(v);
        auto& n = gdb->node_by_id(v);
        visit(n);
        gdb->foreach_from_relationship_of_node(n, [&](auto &r) {
            if (rpred(r)) {
                frontier.push(r.to_node_id());
            }
        });

        if (unidirectional) {
            gdb->foreach_to_relationship_of_node(n, [&](auto &r) {
                if (rpred(r)) {
                    frontier.push(r.from_node_id());
                }
            });
        }
    }
}

void path_bfs(graph_db_ptr gdb, node::id_t start, bool unidirectional, rship_predicate rpred, path_visitor visit) {

}
