#include <set>
#include <iostream>
#include <boost/variant.hpp>

#include "config.h"
#include "graph_db.hpp"
#include "graph_pool.hpp"

#include "qoperator.hpp"
#include "queryc.hpp"
#include "query.hpp"

const std::string test_path = poseidon::gPmemPath + "query_recovery";

int main() {
#ifdef USE_PMDK
	graph_pool_ptr pool;
	graph_db_ptr graph;
	
	if (access(test_path.c_str(), F_OK) != 0) {
    	std::cout << "PMem file not found" << std::endl;
        return 0;
  	} else {
    	pool = graph_pool::open(test_path);
		graph = pool->open_graph("query_recovery");
  	}

#else
  auto pool = graph_pool::open(test_path);
  auto graph = pool->open_graph("jit_qcomp");
#endif

    result_set rs;

    auto cp = graph->restore_positions();

    auto q = query(graph)
                .continue_scan(cp)
                    .has_label("Person")
                        .to_relationships(":likes")
                            .to_node("Person")
                                .persist()
                                    .collect(rs);   


    graph->begin_transaction();
    graph->restore_results(rs.data);
    graph->commit_transaction();

    graph->begin_transaction();
    query::start({&q});
    rs.wait();
    graph->commit_transaction();

    std::cout << "Total results: " << rs.data.size() << std::endl;

    return 0;
}