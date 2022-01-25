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
        The method exec should contain the functionality for the actual execution of the query.
    */
    virtual void execute(query_set &queries) = 0;

};

#endif