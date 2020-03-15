#include "exceptions.hpp"
#include "index_map.hpp"

index_map::index_map() {
#ifdef USE_PMDK
#endif
}

index_map::~index_map() {
#ifdef USE_PMDK
    // TODO
#endif
}

void index_map::register_index(const std::string& idx_name, btree_ptr idx) {
#ifdef USE_PMDK
    // TODO
#else
    indexes_.insert({ idx_name, idx });
#endif
}

void index_map::unregister_index(const std::string& idx_name) {
#ifdef USE_PMDK
    // TODO
#else
    auto it = indexes_.find(idx_name);
    if (it == indexes_.end())
        throw unknown_index();
    indexes_.erase(it);
#endif
}

index_id index_map::get_index(const std::string& idx_name) {
#ifdef USE_PMDK
    // TODO
#else
    auto it = indexes_.find(idx_name);
    if (it == indexes_.end())
        throw unknown_index();
    return it->second;
#endif
}
