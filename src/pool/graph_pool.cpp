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

#include <boost/filesystem.hpp>

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

void graph_pool::drop_graph(const std::string& name) {
    // TODO
}

#else

graph_pool_ptr graph_pool::create(const std::string& path, unsigned long long pool_size) {
    auto self = std::make_unique<graph_pool>();
#ifdef USE_PFILES
    self->path_ = path;
    boost::filesystem::path path_obj(path);
    // check if path exists and is of a regular file
    if (! boost::filesystem::exists(path_obj)) {
        boost::filesystem::create_directory(path_obj);
    }
#endif
    return self;
}

graph_pool_ptr graph_pool::open(const std::string& path, bool init) {
    auto self = std::make_unique<graph_pool>();
    self->path_ = path;
#ifdef USE_PFILES
    boost::filesystem::path path_obj(path);
    // check if path exists and is of a regular file
    if (! boost::filesystem::exists(path_obj)) {
        spdlog::info("FATAL: graph_pool '{}' doesn't exist.", path);
        abort();
    }
#endif
    return self;
}

void graph_pool::destroy(graph_pool_ptr& p) {
    for (auto& gp : p->graphs_) { 
        graph_db::destroy(gp.second);
    }
#ifdef USE_PFILES
    boost::filesystem::path path_obj(p->path_);
    boost::filesystem::remove_all(path_obj);  
#endif
}

graph_pool::graph_pool() {   
}

graph_pool::~graph_pool() {}

graph_db_ptr graph_pool::create_graph(const std::string& name) {
    auto gptr = p_make_ptr<graph_db>(name, path_);
    graphs_.insert({ name, gptr});
    return gptr;
}

graph_db_ptr graph_pool::open_graph(const std::string& name) {
    // TODO: check whether graph directory exists
#ifdef USE_PFILES
    boost::filesystem::path path_obj(path_);
    path_obj /= name;
    // check if path exists and is of a regular file
    if (! boost::filesystem::exists(path_obj)) {
        spdlog::info("FATAL: graph '{}' doesn't exist in pool '{}'.", name, path_);
        throw unknown_db();
    }
#endif   
    auto gptr = p_make_ptr<graph_db>(name, path_);
    gptr->runtime_initialize();
    graphs_.insert({ name, gptr});
    return gptr;
}

void graph_pool::drop_graph(const std::string& name) {
    auto iter = graphs_.find(name);
    if (iter == graphs_.end())
        throw unknown_db();
#ifdef USE_PFILES
    boost::filesystem::path path_obj(name);
    boost::filesystem::remove_all(path_obj);
#endif
    graphs_.erase(iter);
}

void graph_pool::close() {   
    for (auto& gp : graphs_) { 
        gp.second->flush();
        gp.second->purge_bufferpool();
        gp.second->close_files();
    }
}

#endif


