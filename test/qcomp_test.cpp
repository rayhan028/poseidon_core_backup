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

    queryc qlc;

    SECTION("Transform a scan query into a valid graph algebra expression") {
        std::string scan_query = "NodeScan('Person')";
        auto op = qlc.compile_to_plan(scan_query);

        REQUIRE(op->type_ == qop_type::scan);

        auto scanop = std::dynamic_pointer_cast<scan_op>(op);
        REQUIRE(boost::equals(scanop->label_, "Person"));

        REQUIRE(op->inputs_[0]->name_ == "Collect");
        REQUIRE(op->inputs_[0]->type_ == qop_type::collect);
    }

    SECTION("Transform a ForeachRship FROM query into a valid graph algebra expression") {
        std::string scan_query = "ForeachRelationship(FROM, ':HAS_READ', NodeScan('Person'))";
        auto op = qlc.compile_to_plan(scan_query);

        REQUIRE(op->type_ == qop_type::scan);
        
        auto fe = op->inputs_[0];
        REQUIRE(fe->type_ == qop_type::foreach_rship);

        auto fe_op = std::dynamic_pointer_cast<foreach_rship_op>(fe);
        REQUIRE(boost::equals(fe_op->label_, ":HAS_READ"));
        REQUIRE(fe_op->dir_ == RSHIP_DIR::FROM);

        REQUIRE(fe->inputs_[0]->type_ == qop_type::collect);
    }

    SECTION("Transform a ForeachRship TO query into a valid graph algebra expression") {
        std::string scan_query = "ForeachRelationship(TO, ':HAS_READ', NodeScan('Person'))";
        auto op = qlc.compile_to_plan(scan_query);

        REQUIRE(op->type_ == qop_type::scan);
        
        auto fe = op->inputs_[0];
        REQUIRE(fe->type_ == qop_type::foreach_rship);

        auto fe_op = std::dynamic_pointer_cast<foreach_rship_op>(fe);
        REQUIRE(boost::equals(fe_op->label_, ":HAS_READ"));
        REQUIRE(fe_op->dir_ == RSHIP_DIR::TO);

        REQUIRE(fe->inputs_[0]->type_ == qop_type::collect);
    }

    SECTION("Transform a ExpandIn query into a valid graph algebra expression") {
        std::string scan_query = "Expand(IN, 'Book', ForeachRelationship('FROM', ':HAS_READ', NodeScan('Person')))";
        auto op = qlc.compile_to_plan(scan_query);

        REQUIRE(op->type_ == qop_type::scan);
        
        auto fe = op->inputs_[0];
        REQUIRE(fe->type_ == qop_type::foreach_rship);

        auto fe_op = std::dynamic_pointer_cast<foreach_rship_op>(fe);
        REQUIRE(boost::equals(fe_op->label_, ":HAS_READ"));
        REQUIRE(fe_op->dir_ == RSHIP_DIR::FROM);

        auto exp = fe->inputs_[0];
        REQUIRE(exp->type_ == qop_type::expand);

        auto exp_op = std::dynamic_pointer_cast<expand_op>(exp);
        REQUIRE(boost::equals(exp_op->label_, "Book"));
        REQUIRE(exp_op->exp_ == EXPAND::IN);

        REQUIRE(exp->inputs_[0]->type_ == qop_type::collect);
    }

    SECTION("Transform a ExpandOut query into a valid graph algebra expression") {
        std::string scan_query = "Expand(OUT, 'Book', ForeachRelationship(FROM, ':HAS_READ', NodeScan('Person')))";
        auto op = qlc.compile_to_plan(scan_query);

        REQUIRE(op->type_ == qop_type::scan);
        
        auto fe = op->inputs_[0];
        REQUIRE(fe->type_ == qop_type::foreach_rship);

        auto fe_op = std::dynamic_pointer_cast<foreach_rship_op>(fe);
        REQUIRE(boost::equals(fe_op->label_, ":HAS_READ"));
        REQUIRE(fe_op->dir_ == RSHIP_DIR::FROM);

        auto exp = fe->inputs_[0];
        REQUIRE(exp->type_ == qop_type::expand);

        auto exp_op = std::dynamic_pointer_cast<expand_op>(exp);
        REQUIRE(boost::equals(exp_op->label_, "Book"));
        REQUIRE(exp_op->exp_ == EXPAND::OUT);

        REQUIRE(exp->inputs_[0]->type_ == qop_type::collect);
    }

    SECTION("Transform a Filter query into a valid graph algebra expression") {
        std::string filter_query = "Filter($2.age == 42, NodeScan('Person'))";
        auto op = qlc.compile_to_plan(filter_query);

        REQUIRE(op->type_ == qop_type::scan);

        auto filter = op->inputs_[0];
        REQUIRE(filter->type_ == qop_type::filter);

        auto fil_op = std::dynamic_pointer_cast<filter_op>(filter);
        auto fexp = fil_op->fexpr_;

        REQUIRE(fexp->name_ == "EQ");

        auto bin_expr = std::dynamic_pointer_cast<binary_predicate>(fexp);
        REQUIRE(bin_expr->fop_ == FOP::EQ);

        auto lhs = bin_expr->left_;
        REQUIRE(lhs->ftype_ == FOP_TYPE::KEY);

        auto rhs = bin_expr->right_;
        REQUIRE(rhs->ftype_ == FOP_TYPE::INT);
    }

    SECTION("Transform a Projection query into a valid graph algebra expression") {
        std::string prj_query = "Project([$0.age:int, $42.name:string], NodeScan('Person'))";
        auto op = qlc.compile_to_plan(prj_query);

        REQUIRE(op->type_ == qop_type::scan);

        auto prj = op->inputs_[0];
        REQUIRE(prj->type_ == qop_type::project);

        auto prj_op = std::dynamic_pointer_cast<project>(prj);
        auto pexp = prj_op->prexpr_;
        REQUIRE(pexp.size() == 2);

        auto prj1 = pexp.front();
        auto prj2 = pexp.back();

        REQUIRE(prj1.id == 0);
        REQUIRE(prj1.key == "age");
        REQUIRE(prj1.type == FTYPE::INT);

        REQUIRE(prj2.id == 42);
        REQUIRE(prj2.key == "name");
        REQUIRE(prj2.type == FTYPE::STRING);
    }

    SECTION("Transform a Filter and Projection into valid graph algebra expression") {
        std::string prj_query = "Project([$0.age:int, $42.name:string], Filter($2.age == 42 ,NodeScan('Person')))";
        auto op = qlc.compile_to_plan(prj_query);

        REQUIRE(op->type_ == qop_type::scan);

        auto filter = op->inputs_[0];
        REQUIRE(filter->type_ == qop_type::filter);

        auto fil_op = std::dynamic_pointer_cast<filter_op>(filter);
        auto fexp = fil_op->fexpr_;

        REQUIRE(fexp->name_ == "EQ");

        auto bin_expr = std::dynamic_pointer_cast<binary_predicate>(fexp);
        REQUIRE(bin_expr->fop_ == FOP::EQ);

        auto lhs = bin_expr->left_;
        REQUIRE(lhs->ftype_ == FOP_TYPE::KEY);

        auto rhs = bin_expr->right_;
        REQUIRE(rhs->ftype_ == FOP_TYPE::INT);

        auto prj = filter->inputs_[0];
        REQUIRE(prj->type_ == qop_type::project);

        auto prj_op = std::dynamic_pointer_cast<project>(prj);
        auto pexp = prj_op->prexpr_;
        REQUIRE(pexp.size() == 2);

        auto prj1 = pexp.front();
        auto prj2 = pexp.back();

        REQUIRE(prj1.id == 0);
        REQUIRE(prj1.key == "age");
        REQUIRE(prj1.type == FTYPE::INT);

        REQUIRE(prj2.id == 42);
        REQUIRE(prj2.key == "name");
        REQUIRE(prj2.type == FTYPE::STRING);
    }

#ifdef USE_PMDK
	nvm::transaction::run(pop, [&] { nvm::delete_persistent<graph_db>(graph); });
	pop.close();
	remove(test_path.c_str());
#endif
}