#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do
                          // this in one cpp file

#include "catch.hpp"

#include <set>
#include <iostream>
#include <boost/variant.hpp>
#include <boost/algorithm/string.hpp>

#include "config.h"
#include "graph_db.hpp"
#include "graph_pool.hpp"

#include "qoperator.hpp"
#include "queryc.hpp"


const std::string test_path = poseidon::gPmemPath + "qcomp_test";

#ifdef USE_PMDK

#define PMEMOBJ_POOL_SIZE ((size_t)(1024 * 1024 * 80))

namespace nvm = pmem::obj;

nvm::pool_base prepare_pool() {
	auto pop = nvm::pool_base::create(test_path, "", PMEMOBJ_POOL_SIZE);
	return pop;
}
#endif

TEST_CASE("Transform a given query into graph algebra", "[qcomp]") {
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

    queryc qlc(dct);

    SECTION("Transform a scan query into a valid graph algebra expression") {
        algebra_optr op;
        std::string scan_query = "NodeScan('Person')";
        qlc.compile(scan_query, op);

        REQUIRE(op->type_ == qop_type::scan);

        auto scanop = std::dynamic_pointer_cast<scan_op>(op);
        REQUIRE(boost::equals(scanop->label_, "'Person'"));

        REQUIRE(op->inputs_[0]->type_ == qop_type::collect);
    }

    SECTION("Transform a ForeachRship query into a valid graph algebra expression") {
        algebra_optr op;
        std::string scan_query = "ForeachRelationship('FROM', ':HAS_READ', NodeScan('Person'))";
        qlc.compile(scan_query, op);

        REQUIRE(op->type_ == qop_type::scan);
        
        auto fe = op->inputs_[0];
        REQUIRE(fe->type_ == qop_type::foreach_rship);

        auto fe_op = std::dynamic_pointer_cast<foreach_rship_op>(fe);
        REQUIRE(boost::equals(fe_op->label_, "':HAS_READ'"));
        REQUIRE(fe_op->dir_ == RSHIP_DIR::FROM);

        REQUIRE(fe->inputs_[0]->type_ == qop_type::collect);
    }

    SECTION("Transform a Expand query into a valid graph algebra expression") {
        algebra_optr op;
        std::string scan_query = "Expand('OUT', 'Book', ForeachRelationship('FROM', ':HAS_READ', NodeScan('Person')))";
        qlc.compile(scan_query, op);

        REQUIRE(op->type_ == qop_type::scan);
        
        auto fe = op->inputs_[0];
        REQUIRE(fe->type_ == qop_type::foreach_rship);

        auto fe_op = std::dynamic_pointer_cast<foreach_rship_op>(fe);
        REQUIRE(boost::equals(fe_op->label_, "':HAS_READ'"));
        REQUIRE(fe_op->dir_ == RSHIP_DIR::FROM);

        auto exp = fe->inputs_[0];
        REQUIRE(exp->type_ == qop_type::expand);

        auto exp_op = std::dynamic_pointer_cast<expand_op>(exp);
        REQUIRE(boost::equals(exp_op->label_, "'Book'"));
        REQUIRE(exp_op->exp_ == EXPAND::OUT);

        REQUIRE(exp->inputs_[0]->type_ == qop_type::collect);
    }

#ifdef USE_PMDK
	nvm::transaction::run(pop, [&] { nvm::delete_persistent<graph_db>(graph); });
	pop.close();
	remove(test_path.c_str());
#endif
}