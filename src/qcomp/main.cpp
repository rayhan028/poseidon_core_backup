#include <set>
#include <iostream>
#include <boost/variant.hpp>

#include "config.h"
#include "graph_db.hpp"
#include "graph_pool.hpp"

#include "qoperator.hpp"
#include "queryc.hpp"
#include "query.hpp"
namespace pj = builtin;
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

const std::string test_path = poseidon::gPmemPath + "jit_qcomp";

int nodefunc(node *n) {
	return n->id();
}

bool int_fct(int *i) {
	int in = *i;
	return in % 2 == 0; 
}


#ifdef USE_PMDK

#define PMEMOBJ_POOL_SIZE ((unsigned long long)(1024 * 1024 * 40000ull))

namespace nvm = pmem::obj;

nvm::pool_base prepare_pool() {
	nvm::pool_base pop;
	if (access(test_path.c_str(), F_OK) != 0) {
    	pop = nvm::pool_base::create(test_path, "jit_qcomp", PMEMOBJ_POOL_SIZE);
  	} else {
    	pop = nvm::pool_base::open(test_path, "jit_qcomp");
  	}

	//auto pop = nvm::pool_base::create(test_path, "", PMEMOBJ_POOL_SIZE);
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
  auto graph = pool->create_graph("jit_qcomp");
#endif

	auto tx = graph->begin_transaction();

	int PERSONS = 5;
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
				{"id", boost::any(id++)},
				{"num", boost::any(uint64_t(1234567890123412)+uint64_t(i))},
				{"dummy1", boost::any(std::string("Dummy"))},
				{"dummy2", boost::any(1.2345)}},
				false);
		auto p2 = graph->add_node("Person",
				{{"name", boost::any(std::string("John Moe"))},
				{"age", boost::any(42+add)},
				{"id", boost::any(id++)},
				{"num", boost::any(uint64_t(1234567890123412)+uint64_t(i))},
				{"dummy1", boost::any(std::string("Dummy"))},
				{"dummy2", boost::any(1.2345)}},
				false);
		auto b = graph->add_node("Book",
				{{"name", boost::any(std::string("Title"))},
				{"age", boost::any(42)},
				{"id", boost::any(id++)}},
				false);
		graph->add_relationship(p, b, ":likes", {}, false);
		graph->add_relationship(p, p2, ":likes", {}, false);
		graph->add_relationship(p, b, ":HAS_READ", {}, false);
		graph->add_relationship(b, p, ":HAS_READ", {}, false);
	}
	auto l = graph->add_node("Peter",
				{{"name", boost::any(std::string("Title"))},
				{"age", boost::any(42)},
				{"id", boost::any(id++)}},
				true);
	auto d = graph->add_node("Uwe",
				{{"name", boost::any(std::string("Title"))},
				{"age", boost::any(42)},
				{"id", boost::any(id++)}},
				true);
	graph->add_relationship(l, d, ":likes", {}, false);
	graph->commit_transaction();

	auto THREAD_NUM = 4;
	auto chunks = graph->get_nodes()->num_chunks();
	auto cv_range = chunks / THREAD_NUM;
	std::cout << "Chunks: " << chunks << std::endl;
	query_engine queryEngine(graph, THREAD_NUM, cv_range);

	p_ptr<dict> dct;
#ifdef USE_PMDK
	nvm::transaction::run(pop, [&] {
#endif
		dct = p_make_ptr<dict>();
#ifdef USE_PMDK
	});
#endif
	
	//algebra_optr op = qlc.compile_to_plan("Project([$0.name:string, $0.num:uint64], NodeScan('Person'))");
	std::vector<std::string> labels = {"Person", "Book"};

	auto qq  = Scan("Person", ForeachRship(RSHIP_DIR::FROM, {}, ":likes", Expand(EXPAND::OUT, "Book", End())));

	auto r_expr = Scan(labels, End(JOIN_OP::NESTED_LOOP, 0));

    auto l_expr = Scan("Person", Join(JOIN_OP::CROSS, {}, 
                        Project({{0, "name", FTYPE::STRING}, {0, "age", FTYPE::INT}, {0, "id", FTYPE::INT}
                                  /*{3, "title", FTYPE::STRING}, {3, "Age", FTYPE::INT}, {0, "id", FTYPE::INT}, {0, "name", FTYPE::STRING}*/}, 
							Collect()), r_expr));

	auto fev = Scan(labels, ForeachRship(RSHIP_DIR::FROM, {}, ":likes", Expand(EXPAND::IN, "Person", Join(JOIN_OP::NESTED_LOOP, {0,0}, Collect(), r_expr))));

	auto lamdat = [](int*) -> bool {
		return true;
	};

	auto simp = Scan("Person", 
					Filter(Call(Key(0, "id"), Fct(lamdat)), 
						ForeachRship(RSHIP_DIR::FROM, {}, ":likes", 
								GroupBy({0},
                            		Aggr({{"count", 0}, {"avg", 0}}, 
										ForeachRship(RSHIP_DIR::FROM, ":likes", 0,
											ExpandOut("Book",
												GroupBy({0},
                            						Aggr({{"count", 0}}, 
														ForeachRship(RSHIP_DIR::FROM, ":likes", 0,
															ExpandOut("Book",
																Collect())))))))))));



	auto multi = Scan(labels, Project({{0, "name", FTYPE::STRING}, {0, {"dumm1", "dummy2"}, {"true", "false"}}, {0, nodefunc}, {0}}, Collect()));
	auto multi_exp = Scan("Person", ForeachRship(RSHIP_DIR::FROM, {}, ":likes", 
	Expand(EXPAND::OUT, labels, 
	Project({{0, "name", FTYPE::STRING}, {0, {"dumm1", "dummy2"}, {"true", "false"}}, {0}}, Collect()))));

	//auto filter_exp = Scan("Person", Filter(Call(Key(0, "id"), Fct(int_fct)), ForeachRship(RSHIP_DIR::TO, {}, ":likes", Expand(EXPAND::IN, "Person", Project({{0, nodefunc}}, Collect())))));
	auto filter_exp = Scan("Person", Filter(Call(Key(0, "id"), Fct(int_fct)), ForeachRship(RSHIP_DIR::FROM, {}, ":likes", Expand(EXPAND::OUT, "Person", 
			Project({{0, "name", FTYPE::STRING}, {2, "name", FTYPE::STRING}, {0, {"dumm1", "dummy2"}, {"true", "false"}}, {0, nodefunc}}, Collect())))));
	scan_task::callee_ = &scan_task::scan;	

	auto cs1 = std::chrono::steady_clock::now();
	queryEngine.generate(simp, false);
	auto ce1 = std::chrono::steady_clock::now();
	
	arg_builder ab;
	ab.arg(1, "Person");
	ab.arg(2, "id");
	ab.arg(3, ":likes");
	ab.arg(4, "Person");
	ab.arg(5, "Book");
	ab.arg(6, ":likes");
	ab.arg(7, "Book");
	ab.arg(10, ":likes");
	ab.arg(11, "Person");
	ab.arg(12, "Book");
	ab.arg(13, ":HAS_READ");
	ab.arg(14, "Book");
	ab.arg(15, "Book");

	result_set rs;

  	auto js = std::chrono::steady_clock::now();
	//graph->begin_transaction();
	queryEngine.run(&rs, ab.args, 24);
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

/*
	std::cout << "Cached" << std::endl;
	
	rs.data.clear();

//#ifndef USE_PMDK
	queryEngine.~query_engine();
//#endif 

	std::cout << "deleted" << std::endl;
	query_engine queryEngine2(graph, THREAD_NUM, cv_range);

	auto r_expr2 = Scan(labels, End(JOIN_OP::NESTED_LOOP, 0));

	auto fev2 = Scan(labels, ForeachRship(RSHIP_DIR::FROM, {}, ":likes", Expand(EXPAND::IN, "Person", Join(JOIN_OP::NESTED_LOOP, {0,0}, Collect(), r_expr))));

	auto cs2 = std::chrono::steady_clock::now();
	queryEngine2.generate(fev, false);
	auto ce2 = std::chrono::steady_clock::now();

  	auto js2 = std::chrono::steady_clock::now();
	queryEngine2.run(&rs, ab.args);
	auto je2 = std::chrono::steady_clock::now();

	std::cout << rs << std::endl;
	std::cout << "JIT: "
		<< std::chrono::duration_cast<std::chrono::milliseconds>(je2 -
																	js2)
				.count()
		<< " CT: "
		<< std::chrono::duration_cast<std::chrono::milliseconds>(ce2 -
																	cs2)
				.count()
		<< " ms" << std::endl;
*/
#ifdef USE_PMDK
	//nvm::transaction::run(pop, [&] { nvm::delete_persistent<graph_db>(graph); });
	pop.close();
	//remove(test_path.c_str());
#endif
	return 0;
}
