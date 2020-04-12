#include "graph_pool.hpp"

#ifdef USE_PMDK

namespace nvm = pmem::obj;

graph_pool_ptr graph_pool::create(const std::string& path, unsigned long long pool_size) {
    struct enabler : public graph_pool { using graph_pool::graph_pool; };
    auto self = std::make_unique<enabler>();
  
    self->pop_ = nvm::pool<root>::create(path, "", pool_size);
    self->path_ = path;

    nvm::transaction::run(self->pop_, [&] {
        self->pop_.root()->graphs_ = nvm::make_persistent<hashmap>();
    });
    return self;
}

graph_pool_ptr graph_pool::open(const std::string& path) {
    struct enabler : public graph_pool { using graph_pool::graph_pool; };
    auto self = std::make_unique<enabler>();
  
    self->pop_ = nvm::pool<root>::open(path, "");
    self->path_ = path;
    return self;
}

void graph_pool::destroy(graph_pool_ptr& p) {
  p->pop_.close();
  remove(p->path_.c_str());
}

void graph_pool::close() {
    pop_.close();
}

graph_pool::graph_pool() {
}

graph_pool::~graph_pool() {
}

graph_db_ptr graph_pool::create_graph(const std::string& name) {
    graph_db_ptr graph;
    nvm::transaction::run(pop_, [&] { graph = p_make_ptr<graph_db>(); });
    graph->runtime_initialize();

    string_t str(name);
    hashmap::accessor ac;
    pop_.root()->graphs_->insert(ac, str);
    ac->second = graph;
    ac.release();
    return graph;
}

graph_db_ptr graph_pool::open_graph(const std::string& name) {
   hashmap::const_accessor ac;
    if (pop_.root()->graphs_->find(ac, string_t(name)))
        return ac->second;
    else
        throw unknown_db();
}

#else

graph_pool_ptr graph_pool::create(const std::string& path, unsigned long long pool_size) {
    struct enabler : public graph_pool { using graph_pool::graph_pool; };
    return std::make_unique<enabler>();
}

graph_pool_ptr graph_pool::open(const std::string& path) {
    struct enabler : public graph_pool { using graph_pool::graph_pool; };
    return std::make_unique<enabler>();
}

void graph_pool::destroy(graph_pool_ptr& p) {}

graph_pool::graph_pool() {}

graph_pool::~graph_pool() {}

graph_db_ptr graph_pool::create_graph(const std::string& name) {
    auto gptr = p_make_ptr<graph_db>(name);  
    graphs_.insert({ name, gptr});
    return gptr;  
}

graph_db_ptr graph_pool::open_graph(const std::string& name) {
    auto iter = graphs_.find(name);
    if (iter == graphs_.end()) 
        throw unknown_db();    
    return iter->second;
}

#endif


