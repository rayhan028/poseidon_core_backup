/*
 * Copyright (C) 2019-2020 DBIS Group - TU Ilmenau, All Rights Reserved.
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

#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do
                          // this in one cpp file

#include "catch.hpp"
#include "config.h"
#include "defs.hpp"


#ifdef USE_PMDK

#include "pmlog.hpp"

#define PMEMOBJ_POOL_SIZE ((size_t)(1024 * 1024 * 80))

namespace nvm = pmem::obj;
const std::string test_path = poseidon::gPmemPath + "log_test";

struct root {
  nvm::persistent_ptr<pmlog> log_p;
};

TEST_CASE("creating a log and appending some entreis", "[pmlog]") {
    auto pop = nvm::pool<root>::create(test_path, "", PMEMOBJ_POOL_SIZE);
    auto root_obj = pop.root();

    nvm::transaction::run(
        pop, [&] { root_obj->log_p = nvm::make_persistent<pmlog>(); });

    pmlog &ulog = *(root_obj->log_p);

    auto lid1 = ulog.transaction_begin(42);
    auto lid2 = ulog.transaction_begin(44);

    log_ins_record r1{ pmlog::log_insert, pmlog::log_node, 1234ul };
    ulog.append(lid1, &r1, sizeof(r1));
 
    log_upd_node_record r2{ pmlog::log_update, pmlog::log_node, 1236ul, 42, 101ul, 102ul, 103ul };
    ulog.append(lid1, &r2, sizeof(r2));

    log_ins_record r3{ pmlog::log_insert, pmlog::log_node, 1235ul };
    ulog.append(lid2, &r3, size(r3));

    ulog.transaction_end(lid2);

    auto lid3 = ulog.transaction_begin(46);
    log_upd_node_record r4{ pmlog::log_update, pmlog::log_node, 1237ul, 44, 201ul, 202ul, 203ul };
    ulog.append(lid3, &r4, sizeof(r4));

    pop.close();


    remove(test_path.c_str());
}

#endif