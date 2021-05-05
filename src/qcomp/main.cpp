#include <set>
#include <iostream>
#include <boost/variant.hpp>
#include "global_definitions.hpp"
#include "grouper.hpp"
#include "config.h"
#include "graph_db.hpp"
#include "graph_pool.hpp"

#include "qoperator.hpp"
#include "queryc.hpp"
#include "query.hpp"

namespace pj = builtin;

#define POOL_SIZE ((unsigned long long)(1024 * 1024 * 40000ull)) // 4000 MiB

struct root {
  graph_db_ptr graph;
};

/*

int cnt = 0;
bool fct(int* np) {
	return cnt++ % 2 ? 1 : 0;
}

thread_local query_result qr;
query_result *afunc(qr_tuple &qrt) {
	auto i = boost::get<int>(qrt[0]);
	auto j = boost::get<int>(qrt[1]);
	auto k = i - j;
	qr = query_result(k);
	return &qr;
}








int nodefunc(node *n) {
	return n->id();
}

bool int_fct(int *i) {
	int in = *i;
	return in % 2 == 0; 
}

std::map<std::size_t, std::vector<std::size_t>> find_chunk_ranges(std::vector<std::size_t> &chunk) {
	std::size_t range_cnt = 0;
	std::size_t last_rng = 0;
	std::map<std::size_t, std::vector<std::size_t>> ranges;
	for(auto c : chunk) {
		if(range_cnt == 0 && last_rng == 0) {
			ranges[range_cnt].push_back(c);
		} else {
			if(last_rng + 1 == c) {
				if(ranges[range_cnt].size() == 25) {
					range_cnt++;
				}
				ranges[range_cnt].push_back(c);
			} else {
				range_cnt++;
				ranges[range_cnt].push_back(c);
			}
		}

		last_rng = c;
	}

	return ranges;
}

std::map<std::size_t, std::vector<std::size_t>>  eval_work(graph_db_ptr gdb, std::map<std::size_t, std::size_t> &cp) {
	std::cout << "Processed chunks: " << cp.size() << std::endl;

	int finished = 0;
	int n_proc = 0;
	auto n_max = gdb->get_nodes()->num_chunks() * 18723;
	for(auto & c : cp) {
		n_proc += c.second;
		if(c.second == 18723)
			finished++;
	}
	auto n_prog = n_proc * 100 / n_max;

	std::cout << "Finished chunks: " << finished << std::endl;
	std::cout << "Progress: " << n_prog << "%" << std::endl;
	std::cout << "Stored results: " << gdb->get_stored_results() << std::endl;


	std::cout << "Not started chunks: ";
	std::vector<std::size_t> rem_chunks;
	for(auto i = 0u; i < gdb->get_nodes()->num_chunks(); i++) {
		if(cp.find(i) == cp.end()) {
			rem_chunks.push_back(i);
			std::cout << i << ", ";
		}
	}
	std::cout << std::endl;
	auto rng = find_chunk_ranges(rem_chunks);
	for(auto & r : rng) {
		std::cout << r.first << ": ";
		for(auto i : r.second) {
			std::cout << i << " ";
		}
		std::cout << std::endl;
	}
	return rng;
}
*/

#ifdef USE_PMDK

#define PMEMOBJ_POOL_SIZE ((unsigned long long)(1024 * 1024 * 40000ull))

namespace nvm = pmem::obj;

nvm::pool_base prepare_pool() {
	const std::string test_path = poseidon::gPmemPath + "jit_qcomp";
	std::string db_name = "jit_qcomp";
	nvm::pool_base pop;
	if (access(test_path.c_str(), F_OK) != 0) {
    	pop = nvm::pool_base::create(test_path, "poseidon", PMEMOBJ_POOL_SIZE);
  	} else {
    	pop = nvm::pool_base::open(test_path, "poseidon");
  	}

	//auto pop = nvm::pool_base::create(test_path, "", PMEMOBJ_POOL_SIZE);
	return pop;
}
#endif

int main() {
	bool init;
	std::string test_path = poseidon::gPmemPath + "jit_qcomp";
	std::string db_name = "jit_qcomp";
#ifdef USE_PMDK
	init = false;
  namespace nvm = pmem::obj;

  nvm::pool<root> pop;
  const auto path = test_path;

  if (access(path.c_str(), F_OK) != 0) {
    pop = nvm::pool<root>::create(path, db_name, POOL_SIZE);
	init = true;
  } else {
    pop = nvm::pool<root>::open(path, db_name);
  }

  auto q = pop.root();
  if (!q->graph) {
    // create a new persistent graph_db object
    nvm::transaction::run(pop, [&] { q->graph = p_make_ptr<graph_db>(); });
  }
  auto graph = q->graph;
  graph->runtime_initialize();
#else
  auto pool = graph_pool::open(test_path);
  auto graph = pool->open_graph("jit_qcomp");
  init = true;
#endif
	if(init) {
		auto tx = graph->begin_transaction();

<<<<<<< HEAD
		int PERSONS = 10;
		int add = 0;
		int j = 1;
		auto id = 0;
		for (int i = 0; i < PERSONS; i++) {
			add = i % j++;
			if(j == 10)
				j = 1;
			auto p = graph->add_node("Person",
					{{"name", boost::any(std::string("John Doe"))},
					{"age", boost::any(42+add)},
					{"id", boost::any(55+id++)},
					{"num", boost::any(uint64_t(1234567890123412)+uint64_t(i))},
					{"dummy1", boost::any(std::string("Dummy"))},
					{"dummy2", boost::any(1.2345)}},
					false);
			auto p2 = graph->add_node("Person",
					{{"name", boost::any(std::string("John Moe"))},
					{"age", boost::any(42+add)},
					{"id", boost::any(56+id++)},
					{"num", boost::any(uint64_t(1234567890123412)+uint64_t(i))},
					{"dummy1", boost::any(std::string("Dummy"))},
					{"dummy2", boost::any(1.2345)}},
					false);
			auto b = graph->add_node("Book",
					{{"name", boost::any(std::string("Title"))},
					{"age", boost::any(42)},
					{"id", boost::any(id++)}},
					false);
			graph->add_relationship(p, b, ":likes", {{"id", boost::any(id++)}}, false);
			graph->add_relationship(p, p2, ":likes", {}, false);
			graph->add_relationship(p, b, ":HAS_READ", {}, false);
			graph->add_relationship(b, p, ":HAS_READ", {}, false);
		}
		
		graph->commit_transaction();
	}

	auto THREAD_NUM = 4;
	auto chunks = graph->get_nodes()->num_chunks();
	auto cv_range = chunks / THREAD_NUM;
	std::cout << "Chunks: " << chunks << std::endl;
	query_engine queryEngine(graph, THREAD_NUM, cv_range);

	//algebra_optr op = qlc.compile_to_plan("Project([$0.name:string, $0.num:uint64], NodeScan('Person'))");
	std::vector<std::string> labels = {"Person", "Book"};

	auto qq  = Scan("Person", ForeachRship(RSHIP_DIR::FROM, {}, ":likes", Expand(EXPAND::OUT, "Book", End())));

	auto r_expr = Scan("Book", End(JOIN_OP::NESTED_LOOP, 0));

	auto fev = Scan(labels, ForeachRship(RSHIP_DIR::FROM, {}, ":likes", Expand(EXPAND::IN, "Person", Join(JOIN_OP::NESTED_LOOP, {0,0}, Collect(), r_expr))));

	auto lamdat = [](int*) -> bool {
		return true;
	};

	auto simp = Scan("Person", GroupBy({0}, Aggr({{"count", 0}}, Collect())));

	scan_task::callee_ = &scan_task::scan;	

	auto cs1 = std::chrono::steady_clock::now();
	queryEngine.generate(simp, false);
	auto ce1 = std::chrono::steady_clock::now();
	
	grouper g1;
	grouper g2;
	grouper g3;
	grouper g4;
	arg_builder ab;
	ab.arg(1, "Person");
	ab.arg(2, &g1);
	ab.arg(3, &g1);
	ab.arg(4, ":likes");
	ab.arg(5, &g2);
	ab.arg(6, &g3);
	ab.arg(7, &g3);
	ab.arg(8, ":likes");
	ab.arg(9, "Person");
	ab.arg(10, &g4);
	ab.arg(11, &g4);
	
	result_set rs;

	auto aq = query(graph).all_nodes("Person").project({PExpr_(0, pj::int_property(res, "age"))}).groupby({0}, {{"pcount", 0}}).collect(rs);

	std::cout << rs << std::endl;
	auto js = std::chrono::steady_clock::now();
	queryEngine.run(&rs, ab, 24);
	//queryEngine.finish(&rs, ab);
	//graph->begin_transaction();
	//query::start({&aq});
	//graph->commit_transaction();
	auto je = std::chrono::steady_clock::now();
	
	std::cout << rs << std::endl;
	std::cout << "JIT: "
		<< std::chrono::duration_cast<std::chrono::milliseconds>(je -
																	js)
				.count()
		<< " CT: "
		<< std::chrono::duration_cast<std::chrono::milliseconds>(ce1 -
																	cs1)
				.count()
		<< " ms" << std::endl;

#ifdef USE_PMDK
	//nvm::transaction::run(pop, [&] { nvm::delete_persistent<graph_db>(graph); });
	//pop.close();
	//remove(test_path.c_str());
#endif
	return 0;
}
