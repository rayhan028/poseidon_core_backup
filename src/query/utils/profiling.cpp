/*
 * Copyright (C) 2019-2021 DBIS Group - TU Ilmenau, All Rights Reserved.
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

#include <chrono>
#include "profiling.hpp"

#ifdef QOP_PROFILING

void prof_metrics::pre_hook() {
    in_records_++;
    start_ = std::chrono::steady_clock::now();
}

void prof_metrics::post_hook(uint64_t n) {
    out_records_ += n;
    auto end = std::chrono::steady_clock::now();
    proc_time_ += end - start_;
}

std::ostream& prof_metrics::dump(std::ostream& os) const {   
    os << "{ in=" << in_records_ 
       << " | out=" << out_records_ 
       << " | time=" << std::chrono::duration_cast<std::chrono::microseconds>(proc_time_).count()
       << " }";
    return os;
}

#endif