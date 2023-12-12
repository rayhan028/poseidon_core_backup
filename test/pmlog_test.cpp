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

#include <catch2/catch_test_macros.hpp>
#include "config.h"
#include "defs.hpp"

#include "pm_ulog.hpp"

#ifdef USE_PMDK

#define PMEMOBJ_POOL_SIZE ((size_t)(1024 * 1024 * 80))

namespace nvm = pmem::obj;
const std::string test_path = poseidon::gPmemPath + "log_test";

struct root {
  nvm::persistent_ptr<pm_ulog> log_p;
};

#endif

TEST_CASE("creating a log and appending some entries", "[pm_ulog]") {
#ifdef USE_PMDK
    auto pop = nvm::pool<root>::create(test_path, "", PMEMOBJ_POOL_SIZE);
    auto root_obj = pop.root();

    nvm::transaction::run(
        pop, [&] { root_obj->log_p = nvm::make_persistent<pm_ulog>(); });

    pm_ulog &ulog = *(root_obj->log_p);
#else
    pm_ulog ulog;
#endif

    auto lid1 = ulog.transaction_begin(42);
    auto lid2 = ulog.transaction_begin(44);

    pmlog::log_ins_record r1(log_insert, log_node, 1234ul);
    ulog.append(lid1, static_cast<void *>(&r1), sizeof(r1));
 
    pmlog::log_node_record r2(log_update, 1236ul, 42, 101ul, 102ul, 103ul);
    ulog.append(lid1, static_cast<void *>(&r2), sizeof(r2));

    pmlog::log_ins_record r3(log_insert, log_node, 1235ul);
    ulog.append(lid2, static_cast<void *>(&r3), sizeof(r3));

    ulog.transaction_end(lid2);

    auto lid3 = ulog.transaction_begin(46);
    pmlog::log_node_record r4(log_update, 1237ul, 44, 201ul, 202ul, 203ul);
    ulog.append(lid3, static_cast<void *>(&r4), sizeof(r4));

#ifdef USE_PMDK
    pop.close();

    std::cout << "reopen pool ..." << std::endl;
    auto pop2 = nvm::pool<root>::open(test_path, "");
    auto root_obj2 = pop2.root();
    REQUIRE(root_obj2 != nullptr);

    pm_ulog &ulog2 = *(root_obj2->log_p);
#else
    pm_ulog &ulog2 = ulog;
#endif

    // std::cout << "dump log ..." << std::endl;
    // ulog2.dump();
    
    int nlogs = 0;
    int ninserts = 0, nupdates = 0;
    for(auto li = ulog2.log_begin(); li != ulog2.log_end(); ++li) {
        if (li.valid()) {
            nlogs++;
            
            for (auto l = li.begin(); l != li.end(); ++l) {
                if (l.log_type() == log_insert) {
                    auto rec = l.get<pmlog::log_ins_record>(); 
                    REQUIRE(rec->oid == 1234);
                    ninserts++;
                }

                else if (l.log_type() == log_update) {
                    if (l.obj_type() == log_node) {
                        auto rec = l.get<pmlog::log_node_record>();
                        if (rec->oid == 1236)
                            REQUIRE(rec->label == 42);
                        if (rec->oid == 1237)
                            REQUIRE(rec->label == 44);
                        nupdates++;
                    }
                }
            }
            
        }
    }
    REQUIRE(ninserts == 1);
    REQUIRE(nupdates == 2);
    REQUIRE(nlogs == 2);
    
#ifdef USE_PMDK
    pop2.close();
    remove(test_path.c_str());
#endif
}
