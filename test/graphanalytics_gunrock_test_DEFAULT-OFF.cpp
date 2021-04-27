#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do
                          // this in one cpp file

#include "catch.hpp"
#include "config.h"
#include "graph_pool.hpp"
#include "SSSP_gunrock.hpp"
#include <chrono> // for elapsed time measurement
#include <boost/dynamic_bitset.hpp> // for random graph creation

const std::string test_path = poseidon::gPmemPath + "graphanalytics_gunrock_test";

/* 
 * This random edge generation is best for dense graphs and has a runtime complexity of O(n+m). 
 * Unfortunately it will lead to dublicated edges. The higher the density the higher the likeliness 
 * of redundant edges. Use the function create_random_dense_data for dense graph creation. 
 * Drawback: create_random_dense_data has time complexity O(n^2). Since for dense graphs m = O(n^2)
 * anyway, that's acceptable. 
 */
uint64_t create_random_sparse_data(graph_db_ptr graph, uint64_t numberVertices, uint64_t numberEdges, bool quiet){

    // Fixed outDegree. The graph is random, but rather uniformly structured
    double outDegree = double(numberEdges) / double(numberVertices); 

    if(!quiet) { std::cout << "Creating random graph with n = " << numberVertices << " and m = " << numberEdges << std::endl; }
    if(!quiet) { std::cout << "Progress: |start                                          end|" << std::endl; }
    if(!quiet) { std::cout << "nodes:    |" << std::flush; }

    auto tx = graph->begin_transaction();

    // Creating nodes
    for (uint64_t i = 0; i < numberVertices; i++) {
        graph->add_node("",{},true);
        if(((i+1)%(numberVertices/50))==0 && !quiet) { std::cout << "=" << std::flush; }
    }
    
    if(!quiet) { std::cout << "|\nedges:    |" << std::flush; }

    srand(time(0)); 
    
    uint64_t edge_count = 0;
    double outDegree_decimalPlace = std::fmod(outDegree,1);
    uint64_t outDegree_int = (uint64_t) outDegree;

    // Creating relationships
    for (uint64_t from_node = 0; from_node < numberVertices; from_node++) {
        for(uint64_t i = 0; i < outDegree_int; i++){
            uint64_t to_node = rand()%numberVertices;
            edge_count++;
            graph->add_relationship(from_node, to_node, "", {});
            if((((edge_count)%(numberEdges/50))==0) && !quiet) { std::cout << "=" << std::flush; }
        }
        if(double(rand())/RAND_MAX <= outDegree_decimalPlace){
            uint64_t to_node = rand()%numberVertices;
            edge_count++;
            graph->add_relationship(from_node, to_node, "", {});
            if((((edge_count)%(numberEdges/50))==0) && !quiet) { std::cout << "=" << std::flush; }
        }
    }
    
    if(!quiet) { std::cout << "|\n"; }
    graph->commit_transaction();
    return edge_count;
}

/* 
 * This random graph generation is best for dense graphs and has a runtime complexity
 * of O(n^2). Calling with big numberVertices (>10.000) could take quite a while. Use the 
 * function create_random_sparse_data for sparse graphs in order to save yourself some time.
 * Drawback: create_random_sparse_data creates duplicate edges, but for very sparse graphs 
 * that's accaptable. 
 */
uint64_t create_random_dense_data(graph_db_ptr graph, uint64_t numberVertices, uint64_t numberEdges, bool quiet){

    // Fixed outDegree. The graph is random, but rather uniformly structured
    double density = (double(numberEdges) / double(numberVertices)) / double(numberVertices); 

    if(!quiet) { std::cout << "Creating random graph with n = " << numberVertices << " and m = " << numberEdges << std::endl; }
    if(!quiet) { std::cout << "Progress: |start                                          end|" << std::endl; }
    if(!quiet) { std::cout << "nodes:    |" << std::flush; }

    auto tx = graph->begin_transaction();

    // Creating nodes
    for (uint64_t i = 0; i < numberVertices; i++) {
        graph->add_node("",{},true);
        if(((i+1)%(numberVertices/50))==0 && !quiet) { std::cout << "=" << std::flush; }
    }
    
    if(!quiet) { std::cout << "|\nedges:    |" << std::flush; }

    // Creating relationships
    srand(time(0)); 
    uint64_t edge_count = 0;
    for (uint64_t from_node = 0; from_node < numberVertices; from_node++) {
        for(uint64_t to_node = 0; to_node < numberVertices; to_node++){
            if(double(rand())/RAND_MAX <= density){
                edge_count++;
                graph->add_relationship(from_node, to_node, "", {});
                if((((edge_count)%(numberEdges/50))==0) && !quiet) { std::cout << "=" << std::flush; }
            }
        }
    }
    
    if(!quiet) { std::cout << "|\n"; }
    graph->commit_transaction();
    return edge_count;
}

/* 
 * Creates a predefined graph for the correctness check. Copied from graphanytics_test.cpp.
 */
void create_known_data(graph_db_ptr graph) {
  auto tx = graph->begin_transaction();

    //
    //  0 --> 1 --> 2 --> 3 --> 4
    //  |           ^
    //  |           |
    //  +---------> 6
    //              ^
    //              |
    //             5
    //

  for (int i = 0; i < 7; i++) {
    graph->add_node("Person",
                                  {{"name", boost::any(std::string("John Doe"))},
                                  {"age", boost::any(42)},
                                  {"id", boost::any(i)},
                                  {"dummy1", boost::any(std::string("Dummy"))},
                                  {"dummy2", boost::any(1.2345)}},
                                  true);
  }
  graph->add_relationship(0, 1, ":knows", {});
  graph->add_relationship(1, 2, ":knows", {});
  graph->add_relationship(2, 3, ":knows", {});
  graph->add_relationship(3, 4, ":knows", {});
  graph->add_relationship(5, 6, ":knows", {});
  graph->add_relationship(6, 2, ":knows", {});
  graph->add_relationship(0, 6, ":likes", {});
  graph->commit_transaction();
}

/* 
 * Run weighted_SSSP_sequential(...), weighted_SSSP_gunrock_COO(...) and weighted_SSSP_gunrock_CSR
 * on a predefined graph, both bidirectional and unidirectional, and check whether the results
 * match our expectations. 
 * 
 * Current behaviour: 
 * - predecessor of source node is UNKNOWN
 * - predecessor of unreachable node is the node itself
 * - distance to source node is 0
 * - distance to unreachable node is std::numeric_limits<float>::max()
 */
TEST_CASE("Check correctness of results on predefined graphs"){
    auto pool = graph_pool::create(test_path);
    auto graph = pool->create_graph("my_graph");
    create_known_data(graph);
    std::cout << std::endl;

    SECTION("bidirectional sequential") {
        SSSP_result rslt = SSSP_result();
        auto tx = graph->begin_transaction(); 
        weighted_SSSP_sequential(graph, 6, true, [](auto& r) { return true; }, [](auto& r) { return 1; }, rslt, false);
        graph->commit_transaction();
        
        bool test = ((rslt.getPredecessor(5) == 6) &
                    (rslt.getPredecessor(6) == UNKNOWN) &
                    (rslt.getPredecessor(4) == 3) &
                    (rslt.getPredecessor(0) == 6) &
                    (rslt.getDistance(0) == 1) &
                    (rslt.getDistance(1) == 2) &
                    (rslt.getDistance(6) == 0));
        if(test) { std::cout << "Bidirectional sequential correctness test passed.\n\n"; }
        REQUIRE(test);
    }

    SECTION("unidirectional sequential") {
        SSSP_result rslt = SSSP_result();
        auto tx = graph->begin_transaction(); 
        weighted_SSSP_sequential(graph, 6, false, [](auto& r) { return true; }, [](auto& r) { return 1; }, rslt, false);
        graph->commit_transaction();
        
        bool test = ((rslt.getPredecessor(5) == 5) &
                    (rslt.getPredecessor(6) == UNKNOWN) &
                    (rslt.getPredecessor(4) == 3) &
                    (rslt.getPredecessor(0) == 0) &
                    (rslt.getDistance(0) == std::numeric_limits<float>::max()) &
                    (rslt.getDistance(1) == std::numeric_limits<float>::max()) &
                    (rslt.getDistance(6) == 0));
        if(test) { std::cout << "Unidirectional sequential correctness test passed.\n\n"; }
        REQUIRE(test);
    }

    SECTION("bidirectional gunrock COO") {
        SSSP_result rslt = SSSP_result();
        
        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        auto tx = graph->begin_transaction(); 
        weighted_SSSP_gunrock_COO(graph, 6, true, [](auto& r) { return true; }, [](auto& r) { return 1; }, rslt, false);
        graph->commit_transaction();
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        
        std::cout << rslt.getPredecessor(0) << " " 
                << rslt.getPredecessor(1)<< " "  
                << rslt.getPredecessor(2)<< " "  
                << rslt.getPredecessor(3)<< " "  
                << rslt.getPredecessor(4)<< " "  
                << rslt.getPredecessor(5)<< " "  
                << rslt.getPredecessor(6) 
                << std::endl;
                
        bool test = ((rslt.getPredecessor(5) == 6) &
                    (rslt.getPredecessor(6) == UNKNOWN) &
                    (rslt.getPredecessor(4) == 3) &
                    (rslt.getPredecessor(0) == 6) &
                    (rslt.getDistance(0) == 1) &
                    (rslt.getDistance(1) == 2) &
                    (rslt.getDistance(6) == 0));
        if(test) { std::cout << "Bidirectional Gunrock COO correctness test passed.\n\n"; }
        REQUIRE(test);

    }

    SECTION("unidirectional gunrock COO") {
        SSSP_result rslt = SSSP_result();
        
        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        auto tx = graph->begin_transaction(); 
        weighted_SSSP_gunrock_COO(graph, 6, false, [](auto& r) { return true; }, [](auto& r) { return 1; }, rslt, false);
        graph->commit_transaction();
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        
        std::cout << rslt.getPredecessor(0) << " " 
                << rslt.getPredecessor(1)<< " "  
                << rslt.getPredecessor(2)<< " "  
                << rslt.getPredecessor(3)<< " "  
                << rslt.getPredecessor(4)<< " "  
                << rslt.getPredecessor(5)<< " "  
                << rslt.getPredecessor(6) 
                << std::endl;

        bool test = ((rslt.getPredecessor(5) == 5) &
                    (rslt.getPredecessor(6) == UNKNOWN) &
                    (rslt.getPredecessor(4) == 3) &
                    (rslt.getPredecessor(0) == 0) &
                    (rslt.getDistance(0) == std::numeric_limits<float>::max()) &
                    (rslt.getDistance(1) == std::numeric_limits<float>::max()) &
                    (rslt.getDistance(6) == 0));
        if(test) { std::cout << "Unidirectional Gunrock COO correctness test passed.\n\n"; }
        REQUIRE(test);
    }

    SECTION("bidirectional gunrock CSR") {
        SSSP_result rslt = SSSP_result();
        
        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        auto tx = graph->begin_transaction(); 
        weighted_SSSP_gunrock_CSR(graph, 6, true, [](auto& r) { return true; }, [](auto& r) { return 1; }, rslt, false);
        graph->commit_transaction();
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        
        bool test = ((rslt.getPredecessor(5) == 6) &
                    (rslt.getPredecessor(6) == UNKNOWN) &
                    (rslt.getPredecessor(4) == 3) &
                    (rslt.getPredecessor(0) == 6) &
                    (rslt.getDistance(0) == 1) &
                    (rslt.getDistance(1) == 2) &
                    (rslt.getDistance(6) == 0));
        if(test) { std::cout << "Bidirectional Gunrock CSR correctness test passed.\n\n"; }
        REQUIRE(test);
    }

    SECTION("unidirectional gunrock CSR") {
        SSSP_result rslt = SSSP_result();
        
        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        auto tx = graph->begin_transaction(); 
        weighted_SSSP_gunrock_CSR(graph, 6, false, [](auto& r) { return true; }, [](auto& r) { return 1; }, rslt, false);
        graph->commit_transaction();
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        
        bool test = ((rslt.getPredecessor(5) == 5) &
                    (rslt.getPredecessor(6) == UNKNOWN) &
                    (rslt.getPredecessor(4) == 3) &
                    (rslt.getPredecessor(0) == 0) &
                    (rslt.getDistance(0) == std::numeric_limits<float>::max()) &
                    (rslt.getDistance(1) == std::numeric_limits<float>::max()) &
                    (rslt.getDistance(6) == 0));
        if(test) { std::cout << "Unidirectional Gunrock CSR correctness test passed.\n\n"; }
        REQUIRE(test);
    }

    graph_pool::destroy(pool);
}

/*
 * The following testcases were used to create a performance analysis. In order to not run them
 * every time, they are commented out right now, but can be used if required. Take care with the
 * parameters, since the graphs can grow big really fast and easily exceed the memory capactiy 
 * of your system, if run on a regular PC or Laptop. 
 */
/*TEST_CASE("Performance comparison: Fixed n, variable m"){

    // Parameters //
    uint64_t num_vertices = 110000; //60000;
    uint64_t min_edges = num_vertices;
    uint64_t max_edges = 4000000;
    uint64_t number_samples = 50;
    bool bidirectional = false;

    // Logarithmic Sampling //
    double exponent_max_edges = std::log10(max_edges);
    double exponent_min_edges = std::log10(min_edges);
    double exponent_delta = (exponent_max_edges-exponent_min_edges)/(number_samples-1);

    std::cout << "\n\nPerformance comparison: Fixed n, variable m\n\n";

    std::vector<double> results;
    SSSP_result sssp_rslt = SSSP_result();

    for(double exponent = exponent_min_edges; exponent <= exponent_max_edges; exponent+=exponent_delta){
        // Graph creation
        auto pool = graph_pool::create(test_path);
        auto graph = pool->create_graph("my_graph");
        uint64_t edges = create_random_sparse_data(graph, num_vertices, uint64_t(std::pow(10,exponent)), true);
        std::cout << edges << "/" << max_edges << std::endl;

        // Runtime tests
        auto tx = graph->begin_transaction(); 
        results.push_back(edges);
        results.push_back(weighted_SSSP_sequential(graph, 0, bidirectional, [](auto& r) { return true; }, [](auto& r) { return 1; }, sssp_rslt, true));
        results.push_back(weighted_SSSP_gunrock_COO(graph, 0, bidirectional, [](auto& r) { return true; }, [](auto& r) { return 1; }, sssp_rslt, true));
        results.push_back(weighted_SSSP_gunrock_CSR(graph, 0, bidirectional, [](auto& r) { return true; }, [](auto& r) { return 1; }, sssp_rslt, true));
        graph->commit_transaction();
        graph_pool::destroy(pool);
    }

    std::cout << "\n\nnum_vertices,min_edges,max_edges,number_samples,bidirectional\n";
    std::cout << num_vertices << "," << min_edges << "," << max_edges << "," << number_samples << "," << bidirectional << "\n";
    std::cout << "SSSP_gunrock_CSR,SSSP_gunrock_COO,SSSP_sequential_Dijkstra,edges\n";
    while(!results.empty()){
        std::cout << results.back() << ",";
        results.pop_back();
        std::cout << results.back() << ",";
        results.pop_back();
        std::cout << results.back() << ",";
        results.pop_back();
        std::cout << results.back() << "\n";
        results.pop_back();
    }
}*/

/*TEST_CASE("Performance comparison: Fixed m, variable n"){

    uint64_t num_edges = 5000000;

    uint64_t min_vertices = 50000;
    uint64_t max_vertices = 2500000;
    uint64_t number_samples = 15;
    bool bidirectional = false;

    std::cout << "\n\nPerformance comparison: Fixed m, variable n\n\n";
    
    std::vector<double> results;
    SSSP_result sssp_rslt = SSSP_result();

    for(uint64_t vertices = min_vertices; vertices <= max_vertices; vertices+=uint64_t((max_vertices-min_vertices)/(number_samples-1))){
        // Graph creation
        auto pool = graph_pool::create(test_path);
        auto graph = pool->create_graph("my_graph");
        create_random_sparse_data(graph, vertices, num_edges, true);
        std::cout << vertices << "/" << max_vertices << std::endl;

        // Runtime tests
        auto tx = graph->begin_transaction(); 
        results.push_back(vertices);
        results.push_back(weighted_SSSP_sequential(graph, bidirectional, false, [](auto& r) { return true; }, [](auto& r) { return 1; }, sssp_rslt, true));
        results.push_back(weighted_SSSP_gunrock_COO(graph, bidirectional, false, [](auto& r) { return true; }, [](auto& r) { return 1; }, sssp_rslt, true));
        results.push_back(weighted_SSSP_gunrock_CSR(graph, bidirectional, false, [](auto& r) { return true; }, [](auto& r) { return 1; }, sssp_rslt, true));
        graph->commit_transaction();
        graph_pool::destroy(pool);
    }

    std::cout << "\n\nmin_vertices,max_vertices,num_edges,number_samples,bidirectional\n";
    std::cout << min_vertices << "," << max_vertices << "," << num_edges << "," << number_samples << "," << bidirectional << "\n";
    std::cout << "SSSP_gunrock_CSR,SSSP_gunrock_COO,SSSP_sequential_Dijkstra,vertices\n";
    while(!results.empty()){
        std::cout << results.back() << ",";
        results.pop_back();
        std::cout << results.back() << ",";
        results.pop_back();
        std::cout << results.back() << ",";
        results.pop_back();
        std::cout << results.back() << "\n";
        results.pop_back();
    }
}*/

/*TEST_CASE("Performance comparison - Fixed n and m, bi- and unidirectional"){

    auto pool = graph_pool::create(test_path);
    auto graph = pool->create_graph("my_graph");
    uint64_t num_vertices = 1000000;
    uint64_t num_edges = 10000000;

    SECTION("Bidirectional"){
        SSSP_result rslt = SSSP_result();
        std::cout << std::endl << "Bidirectional performance test:\n" << std::endl;
        create_random_sparse_data(graph, num_vertices, num_edges, false);
        auto tx = graph->begin_transaction(); 
        std::cout << std::endl;
        weighted_SSSP_sequential(graph, 0, true, [](auto& r) { return true; }, [](auto& r) { return 1; }, rslt, false);
        std::cout << std::endl;
        weighted_SSSP_gunrock_COO(graph, 0, true, [](auto& r) { return true; }, [](auto& r) { return 1; }, rslt, false);
        std::cout << std::endl;
        weighted_SSSP_gunrock_CSR(graph, 0, true, [](auto& r) { return true; }, [](auto& r) { return 1; }, rslt, false);
        std::cout << std::endl;
        graph->commit_transaction();
    }

    SECTION("Unidirectional"){
        SSSP_result rslt = SSSP_result();
        std::cout << std::endl << "Unidirectional performance test:\n" << std::endl;
        create_random_sparse_data(graph, num_vertices, num_edges, false);
        auto tx = graph->begin_transaction(); 
        std::cout << std::endl;
        weighted_SSSP_sequential(graph, 0, false, [](auto& r) { return true; }, [](auto& r) { return 1; }, rslt, false);
        std::cout << std::endl;
        weighted_SSSP_gunrock_COO(graph, 0, false, [](auto& r) { return true; }, [](auto& r) { return 1; }, rslt, false);
        std::cout << std::endl;
        weighted_SSSP_gunrock_CSR(graph, 0, false, [](auto& r) { return true; }, [](auto& r) { return 1; }, rslt, false);
        std::cout << std::endl;
        graph->commit_transaction();
    }

    graph_pool::destroy(pool);
}*/
