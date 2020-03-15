#ifndef index_map_hh_
#define index_map_hh_

#include "defs.hpp"
#include "btree.hpp"


#ifdef USE_PMDK
#else
#include <unordered_map>
#endif

class index_map {
public:
    index_map();
    ~index_map();

    void register_index(const std::string& idx_name, index_id idx);
    void unregister_index(const std::string& idx_name);
    index_id get_index(const std::string& idx_name);

private:
#ifdef USE_PMDK
#else
    std::unordered_map<std::string, index_id> indexes_;
#endif
};

#endif