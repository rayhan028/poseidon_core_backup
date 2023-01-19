#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do
                          // this in one cpp file

#include <catch2/catch_test_macros.hpp>

#include <set>
#include <iostream>
#include <boost/variant.hpp>
#include <boost/algorithm/string.hpp>

#include "config.h"
#include "graph_db.hpp"
#include "graph_pool.hpp"

#ifdef QOP_RECOVERY
#include "radix_index.hpp"

#include "qoperator.hpp"
#include "queryc.hpp"


const std::string test_path = PMDK_PATH("radix_tst");


#ifdef USE_PMDK

#define PMEMOBJ_POOL_SIZE ((size_t)(1024 * 1024 * 80))

namespace nvm = pmem::obj;

nvm::pool_base prepare_pool() {
	auto pop = nvm::pool_base::create(test_path, "", PMEMOBJ_POOL_SIZE);
	return pop;
}
#endif



TEST_CASE("Insert queries into radix indexx", "[radix_index]") {
#ifdef USE_PMDK
	auto pop = prepare_pool();
	graph_db_ptr graph;
	nvm::transaction::run(pop, [&] { graph = p_make_ptr<graph_db>(); });
#else
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("my_graph");
#endif

	p_ptr<dict> dct;
#ifdef USE_PMDK
	nvm::transaction::run(pop, [&] {
#endif
		dct = p_make_ptr<dict>();
#ifdef USE_PMDK
	});
#endif

    queryc qlc;

    SECTION("Insert scan") {
        std::string scan_query = "NodeScan('Person')";
        auto op = qlc.compile_to_plan(scan_query);

        radix_tree rt;

        rt.insert(op);

        
    }

#ifdef USE_PMDK
	nvm::transaction::run(pop, [&] { nvm::delete_persistent<graph_db>(graph); });
	pop.close();
	remove(test_path.c_str());
#endif
}
#endif