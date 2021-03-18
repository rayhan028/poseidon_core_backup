#include <set>
#include <iostream>
#include <boost/variant.hpp>

#include "config.h"
#include "graph_db.hpp"
#include "graph_pool.hpp"

#include "qoperator.hpp"
#include "queryc.hpp"
#include "query.hpp"

#include <unistd.h>

const std::string test_path = poseidon::gPmemPath + "query_recovery";

void init_data(graph_db_ptr &graph) {
    auto tx = graph->begin_transaction();

    int PERSONS = 50000;
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
        graph->add_relationship(p, b, ":likes", {{"id", boost::any(id++)}}, false);
        graph->add_relationship(p, p2, ":likes", {}, false);
        graph->add_relationship(p, b, ":HAS_READ", {}, false);
        graph->add_relationship(b, p, ":HAS_READ", {}, false);
    }
    
    graph->commit_transaction();
}

int main() {
    bool init;
#ifdef USE_PMDK
	init = false;
	graph_pool_ptr pool;
	graph_db_ptr graph;
	
	if (access(test_path.c_str(), F_OK) != 0) {
    	pool = graph_pool::create(test_path);
		graph = pool->create_graph("query_recovery");
		init = true;
  	} else {
    	pool = graph_pool::open(test_path);
		graph = pool->open_graph("query_recovery");
  	}

#else
  auto pool = graph_pool::open(test_path);
  auto graph = pool->open_graph("jit_qcomp");
  init = true;
#endif
    if(init) init_data(graph);

    result_set rs;

    auto q = query(graph)
                .all_nodes()
                    .has_label("Person")
                        .to_relationships(":likes")
                            .to_node("Person")
                                .persist()
                                    .collect(rs);   

    graph->begin_transaction();
    query::start({&q});
    rs.wait();
    graph->commit_transaction();

    usleep(30000);

    std::cout << "Total results: " << rs.data.size() << std::endl;

    return 0;
}