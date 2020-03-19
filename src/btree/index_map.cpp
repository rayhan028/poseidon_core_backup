#include "exceptions.hpp"
#include "index_map.hpp"

index_map::index_map() {
#ifdef USE_PMDK
    auto pop = pmem::obj::pool_by_vptr(this);
    pmem::obj::transaction::run(pop, [&] {
        indexes_ = pmem::obj::make_persistent<hashmap>();
    });
#endif
}

index_map::~index_map() {
#ifdef USE_PMDK
    auto pop = pmem::obj::pool_by_vptr(this);
    pmem::obj::transaction::run(pop, [&] {
        pmem::obj::delete_persistent<hashmap>(indexes_);
    });
#endif
}

void index_map::register_index(const std::string& idx_name, index_id idx) {
#ifdef USE_PMDK
    string_t str(idx_name);
    hashmap::accessor ac;
    indexes_->insert(ac, str);
    ac->second = idx;
    ac.release();
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
    hashmap::const_accessor ac;
    if (indexes_->find(ac, string_t(idx_name)))
        return ac->second;
    else
        throw unknown_index();
#else
    auto it = indexes_.find(idx_name);
    if (it == indexes_.end())
        throw unknown_index();
    return it->second;
#endif
}
