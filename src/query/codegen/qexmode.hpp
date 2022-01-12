#ifndef QEXMODE_HPP
#define QEXMODE_HPP

#include <vector>
#include "query_set.hpp"
#include "qoperator.hpp"

class base_op;
/*
    qexmode is a common interface for the query execution modes in Poseidon.
    It provides two methods for to start the processing of a given query.
*/
class qexmode {
public:
    qexmode() = default;

    /*
        The method add should contain the functionality for pre-processing of the queries.
        E.g. transforming the queries into another format.
    */
    virtual void add(query_set queries) = 0;
    virtual void add(std::vector<std::shared_ptr<base_op>> queries) = 0;

    /*
        The method exec should contain the functionality for the actual execution of the query.
    */
    virtual void exec(result_set *rs) = 0;

};

#endif