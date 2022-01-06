#ifndef QEXMODE_HPP
#define QEXMODE_HPP

#include <vector>
#include "qoperator.hpp"

class qexmode {

protected:

public:
    qexmode() = default;

    virtual void add() = 0;

    virtual void exec() = 0;

};

#endif