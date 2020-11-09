#ifndef POSEIDON_CORE_COLLECTOR_HPP
#define POSEIDON_CORE_COLLECTOR_HPP
#include "global_definitions.hpp"

/**
 * helper class to manage the materialized tuples
 */
struct result_collector {
    /**
     * the actual list type for materilized tuples
     */
    using result_list = std::vector<qr_list*>;

    /**
     * the mapping from join id to the appropriate tuple list
     */ 
    using result_collection = std::map<int, result_list>;
public:
    /**
     * the constructor for a new collector
     */
    result_collector(unsigned num_threads);

    /**
     * method to push tuple results to the result list
     */
    void push_result(unsigned thread_id, qr_list* qrl);


    /**
     * the actual tuple list
     */
    result_collection rl_;

    /**
     * number of participating threads
     */
    unsigned num_threads_;

    /**
     * counter for the number of collect tuples
     */
    unsigned collected_;
};


#endif //POSEIDON_CORE_COLLECTOR_HPP
