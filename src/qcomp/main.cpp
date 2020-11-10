#include <set>
#include <iostream>

#include "config.h"
#include "graph_db.hpp"
#include "graph_pool.hpp"

#include "p_context.hpp"
#include "JitFromScratch.hpp"
#include "qoperator.hpp"

#define PMEMOBJ_POOL_SIZE ((size_t)(1024 * 1024 * 80))

namespace nvm = pmem::obj;
const std::string test_path = poseidon::gPmemPath + "jit_test";

nvm::pool_base prepare_pool() {
	auto pop = nvm::pool_base::create(test_path, "", PMEMOBJ_POOL_SIZE);
	return pop;
}

int main() {
	auto pop = prepare_pool();
	graph_db_ptr graph;
	nvm::transaction::run(pop, [&] { graph = p_make_ptr<graph_db>(); });

	auto tx = graph->begin_transaction();


	int PERSONS = 100;
	int NO_PERSONS = 42;
	for (int i = 0; i < PERSONS; i++) {
		auto p = graph->add_node("Person",
				{{"name", boost::any(std::string("John Doe"))},
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
	queryEngine.prepare();

	/*queryEngine.start_[0](graph.get(), 0, graph->get_nodes()->num_chunks(), tx, 1, 
			&queryEngine.type_vec_[0], &rs, nullptr, 
			queryEngine.finish_[0], 0, args.args.data());*/
	queryEngine.run(&rs, args.args);


	std::cout << rs.data.size() << std::endl;
	nvm::transaction::run(pop, [&] { nvm::delete_persistent<graph_db>(graph); });
	pop.close();
	remove(test_path.c_str());

	return 0;
}
