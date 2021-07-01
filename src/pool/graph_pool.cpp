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
#include "graph_pool.hpp"

#ifdef USE_PMDK

namespace nvm = pmem::obj;

graph_pool_ptr graph_pool::create(const std::string& path,
    unsigned long long pool_size) {
    auto self = std::make_unique<graph_pool>();

    self->pop_ = nvm::pool<root>::create(path, "poseidon", pool_size);
    self->path_ = path;

    nvm::transaction::run(self->pop_, [&] {
        self->pop_.root()->graphs_ = nvm::make_persistent<hashmap>();
    });
    return self;
}

graph_pool_ptr graph_pool::open(const std::string& path, bool init) {
    auto self = std::make_unique<graph_pool>();

    self->pop_ = nvm::pool<root>::open(path, "poseidon");
    self->path_ = path;
    if (init) {
      spdlog::info("Initializing pool...");
      nvm::transaction::run(self->pop_, [&] {
          self->pop_.root()->graphs_ = nvm::make_persistent<hashmap>();
      });
    }
    else
       self->pop_.root()->graphs_->runtime_initialize();
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
    if (pop_.root()->graphs_->find(ac, string_t(name))) {
        auto gdb = ac->second;
        gdb->runtime_initialize();
        return gdb;
    }
    else
        throw unknown_db();
}

#else

graph_pool_ptr graph_pool::create(const std::string& path, unsigned long long pool_size) {
    return std::make_unique<graph_pool>();
}

graph_pool_ptr graph_pool::open(const std::string& path, bool init) {
    return std::make_unique<graph_pool>();
}

void graph_pool::destroy(graph_pool_ptr& p) {
#ifdef USE_MMFILE
    for (auto& gp : p->graphs_) { 
        std::cout << "delete mm files...: " << gp.first << std::endl;
        graph_db::destroy(gp.second);
        /*
        remove("nodes.db");
        remove("slots_nodes.db");
        remove("rships.db");
        remove("slots_rships.db");
        remove("props.db");
        remove("slots_props.db");
        */
    }
#endif    
}

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

void graph_pool::close() {}

#endif


