#ifndef graph_pool_hpp_
#define graph_pool_hpp_

#include "defs.hpp"
#include "graph_db.hpp"
#include "config.h"

#include <unordered_map>

class graph_pool;
using graph_pool_ptr = std::unique_ptr<graph_pool>;

class graph_pool {

public:
    static graph_pool_ptr create(const std::string& path, unsigned long long pool_size = 1024*1024*40000ull);
    static graph_pool_ptr open(const std::string& path);
    static void destroy(graph_pool_ptr& p);

    ~graph_pool();

    graph_db_ptr create_graph(const std::string& name);
    graph_db_ptr open_graph(const std::string& name);

private:
    graph_pool();

#ifdef USE_PMDK
    namespace nvm = pmem::obj;
    using hashmap = nvm::concurrent_hash_map<string_t, graph_db_ptr, string_hasher>;
    p_ptr<hashmap> indexes_;

    struct root {
        p_ptr<hashmap> graph_;
    };

    nvm::pool<root> pop_;
    std::string path_;
#else
    std::unordered_map<std::string, graph_db_ptr> graphs_;
#endif
};

#endif