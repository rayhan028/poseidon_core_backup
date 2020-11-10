#include <set>
#include <iostream>
#include <boost/variant.hpp>

#include "config.h"
#include "graph_db.hpp"
#include "graph_pool.hpp"

#include "p_context.hpp"
#include "JitFromScratch.hpp"
#include "qoperator.hpp"

const std::string test_path = poseidon::gPmemPath + "jit_test";

#ifdef USE_PMDK

#define PMEMOBJ_POOL_SIZE ((size_t)(1024 * 1024 * 80))

namespace nvm = pmem::obj;

nvm::pool_base prepare_pool() {
	auto pop = nvm::pool_base::create(test_path, "", PMEMOBJ_POOL_SIZE);
	return pop;
}
#endif

int main() {
#ifdef USE_PMDK
	auto pop = prepare_pool();
	graph_db_ptr graph;
	nvm::transaction::run(pop, [&] { graph = p_make_ptr<graph_db>(); });
#else
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("my_graph");
#endif
	auto tx = graph->begin_transaction();


	int PERSONS = 100;
	int NO_PERSONS = 42;
	for (int i = 0; i < PERSONS; i++) {
		auto p = graph->add_node("Person",
				{{"name", boost::any(std::string("John Doe")+std::to_string(i))},
				{"age", boost::any(42)},
				{"id", boost::any(i)},
				{"dummy1", boost::any(std::string("Dummy"))},
				{"dummy2", boost::any(1.2345)}},
				true);
		auto b = graph->add_node("Book",
				{{"title", boost::any(std::string("Title"))},
				{"id", boost::any(i)}},
				true);
		graph->add_relationship(p, b, ":HAS_READ", {});
	}

	graph->commit_transaction();

	auto THREAD_NUM = 4;
	auto chunks = graph->get_nodes()->num_chunks();
	auto cv_range = chunks / THREAD_NUM;

	query_engine queryEngine(graph, THREAD_NUM, cv_range);

	arg_builder args;
	args.arg(1, "Person");
	args.arg(2, 42);
	args.arg(3, ":HAS_READ");
	args.arg(4, "Book");

	result_set rs;
	auto scan_expr = Scan("Person", Filter(EQ(Key("id"), Int(42)), ForeachRship(RSHIP_DIR::FROM, {}, ":HAS_READ", Expand(EXPAND::OUT, "Book", Project({{0, "name", FTYPE::STRING}}, Collect(rs))))));
	
	queryEngine.generate(scan_expr, false);

	queryEngine.run(&rs, args.args);

	std::cout << boost::get<std::string>(rs.data.front()[0]) << std::endl;

#ifdef USE_PMDK
	nvm::transaction::run(pop, [&] { nvm::delete_persistent<graph_db>(graph); });
	pop.close();
	remove(test_path.c_str());
#endif
	return 0;
}
