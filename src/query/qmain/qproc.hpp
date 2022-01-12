/*
 * Copyright (C) 2019-2022 DBIS Group - TU Ilmenau, All Rights Reserved.
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
#ifndef qproc_hpp_
#define qproc_hpp_

#include "qparser.hpp"
#include "qplanner.hpp"
#include "qinterp.hpp"
#include "qcompiler.hpp"

/**
 * qproc is the main entry class for the Poseidon query processor. It encapsulates the parser, the query planner as well as
 * the query compiler and interpreter.
 */
class qproc {
public:
    enum mode {
        Interpret,
        Compile,
        Adaptive
    };

    qproc(graph_db_ptr gdb) : gdb_(gdb), compiler_(gdb) {}
    ~qproc() = default;

    void execute_query(mode m, const std::string& qstr);
    
    query_set prepare_query(const std::string& qstr);
    
    void interp_query(query_set& plan);
    void compile_query(query_set& plan);

private:
    graph_db_ptr gdb_;
    qparser parser_;
    qplanner planner_;   
    qinterp interp_; 
    qcompiler compiler_;
};

#endif