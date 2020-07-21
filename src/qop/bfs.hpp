#ifndef bfs_hpp_
#define bfs_hpp_

#include "graph_db.hpp"

using rship_predicate = std::function<bool(relationship&)>;
using node_visitor = std::function<void(node&)>;

using path = std::vector<offset_t>;
using path_visitor = std::function<void(node&, const path&)>;

void bfs(graph_db_ptr gdb, node::id_t start, bool unidirectional, rship_predicate rpred, node_visitor visit);

void path_bfs(graph_db_ptr gdb, node::id_t start, bool unidirectional, rship_predicate rpred, path_visitor visit);

#endif