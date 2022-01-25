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

#include "qproc.hpp"

qresult_iterator qproc::execute_query(qproc::mode m, const std::string& qstr) {
    if (m == Interpret) {
        auto qplan = prepare_query(qstr);
        result_set result;
        qplan.append_collect(result);
        // qplan.append_printer();
        interp_query(qplan);
        qplan.print_plan();
        return qresult_iterator(std::move(result));
    }
}

query_set qproc::prepare_query(const std::string& qstr) {
    auto op_tree = parser_.parse(qstr);
    return planner_.transform(gdb_, op_tree);
}
    
void qproc::interp_query(query_set& plan) {
    interp_.execute(gdb_, plan);
}

void qproc::compile_query(query_set& plan) {
    compiler_.execute(plan);    
}