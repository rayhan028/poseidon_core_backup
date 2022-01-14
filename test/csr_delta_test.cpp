#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do
                          // this in one cpp file

#include "catch.hpp"
#include "config.h"
#include "graph_pool.hpp"
#include "csr_delta.hpp"
#include <chrono>
#include <boost/dynamic_bitset.hpp>

const std::string test_path = poseidon::gPmemPath + "csr_delta_test";

/**
 * A helper class implementing a barrier allowing two threads to coordinate
 * execution.
 */
class barrier {
private:
  std::mutex m;
  std::condition_variable cond_var;
  std::atomic<bool> ready{false};

public:
  barrier() = default;

	/**
	 * Inform the other thread that it can proceed.
	 */
  void notify() {
    std::lock_guard<std::mutex> lock(m);
    ready = true;
    cond_var.notify_one();
  }

	/**
	 * Wait on the barrier until the other threads calls notify.
	 */
  void wait() {
    std::unique_lock<std::mutex> lock(m);
    cond_var.wait(lock, [&] { return ready.load(); });
  }
};

void create_known_data(graph_db_ptr graph) {
  graph->begin_transaction();

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

void update_data(graph_db_ptr graph) {
  graph->begin_transaction();

  //
  //         +--- 10 ---+
  //         |     |     |
  //         |     |     |
  //         v     v     v
  //         0 --> 1 --> 2 --> 3 --> 4
  //         ^     ^     ^           ^
  //         |     |     |           |
  //         |     |     6 <-- 8     |
  //         9     |     ^     |     |
  //               |     |     |     |
  //               +---- 5 ----------+
  //                     ^     |
  //                     |     |
  //                     ------+
  //

  graph->add_relationship(5, 4, ":knows", {}); // adds nodes 5 and 4 to update delta
  graph->add_relationship(5, 1, ":knows", {}); // adds node 1 to update delta
  graph->delete_relationship(0, 6); // adds nodes 0 and 6 to update delta

  for (int i = 7; i < 11; i++) { // adds nodes 7, 8, 9 and 10 to append delta
    graph->add_node("Person",
              {{"name", boost::any(std::string("John Doe"))},
              {"age", boost::any(42)},
              {"id", boost::any(i)},
              {"dummy1", boost::any(std::string("Dummy"))},
              {"dummy2", boost::any(1.2345)}},
              true);
  }
  graph->add_relationship(8, 6, ":knows", {});
  graph->add_relationship(8, 5, ":knows", {});
  graph->add_relationship(9, 0, ":knows", {});
  graph->add_relationship(10, 2, ":knows", {}); // adds node 2 to update delta
  graph->add_relationship(10, 0, ":knows", {});
  graph->add_relationship(10, 1, ":knows", {});

  graph->commit_transaction();
}

TEST_CASE("Removing an edge and updating CSR with delta", "[format_converter]"){
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("my_graph");
  create_known_data(graph);

  uint64_t id = 6;

  csr_arrays csr1;
  auto weight_func = [](auto& r) { return 1.3; };
  graph->run_transaction([&]() {
    graph->poseidon_to_csr(csr1, weight_func);
    return true;
  });

  REQUIRE((csr1.row_offsets[id + 1] - csr1.row_offsets[id]) == 1);
  REQUIRE(csr1.col_indices.size() == 7);

  graph->begin_transaction();
  graph->delete_relationship(6, 2);
  graph->commit_transaction();

  csr_arrays csr2;
  graph->run_transaction([&]() {
    graph->poseidon_to_csr(csr2, weight_func);
    return true;
  });

  REQUIRE((csr2.row_offsets[id + 1] - csr2.row_offsets[id]) == 0);
  REQUIRE(csr2.col_indices.size() == 6);

  graph_pool::destroy(pool);
}

TEST_CASE("Adding an edge and updating CSR with delta", "[format_converter]"){
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("my_graph");
  create_known_data(graph);

  uint64_t id = 0;

  csr_arrays csr1;
  auto weight_func = [](auto& r) { return 1.3; };
  graph->run_transaction([&]() {
    graph->poseidon_to_csr(csr1, weight_func);
    return true;
  });

  REQUIRE((csr1.row_offsets[id + 1] - csr1.row_offsets[id]) == 2);
  REQUIRE(csr1.col_indices.size() == 7);

  graph->begin_transaction();
  graph->add_relationship(0, 5, ":likes", {});
  graph->commit_transaction();

  csr_arrays csr2;
  graph->run_transaction([&]() {
    graph->poseidon_to_csr(csr2, weight_func);
    return true;
  });

  REQUIRE((csr2.row_offsets[id + 1] - csr2.row_offsets[id]) == 3);
  REQUIRE(csr2.col_indices.size() == 8);

  graph_pool::destroy(pool);
}

TEST_CASE("Adding a node and updating CSR with delta", "[format_converter]"){
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("my_graph");
  create_known_data(graph);

  uint64_t id = 7;

  csr_arrays csr1;
  auto weight_func = [](auto& r) { return 1.3; };
  graph->run_transaction([&]() {
    graph->poseidon_to_csr(csr1, weight_func);
    return true;
  });

  REQUIRE(csr1.row_offsets[id] == csr1.row_offsets.back());
  REQUIRE(csr1.col_indices.size() == 7);

  graph->begin_transaction();
  graph->add_node("Person",
              {{"name", boost::any(std::string("John Doe"))},
              {"age", boost::any(42)},
              {"id", boost::any(7)},
              {"dummy1", boost::any(std::string("Dummy"))},
              {"dummy2", boost::any(1.2345)}},
              true);
  graph->add_relationship(7, 4, ":knows", {});
  graph->add_relationship(7, 3, ":knows", {});
  graph->commit_transaction();

  csr_arrays csr2;
  graph->run_transaction([&]() {
    graph->poseidon_to_csr(csr2, weight_func);
    return true;
  });

  REQUIRE((csr2.row_offsets[id + 1] - csr2.row_offsets[id]) == 2);
  REQUIRE(csr2.col_indices.size() == 9);

  graph_pool::destroy(pool);
}

TEST_CASE("Updating graph and updating CSR with delta in a single transaction", "[format_converter]"){
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("my_graph");
  create_known_data(graph);

  csr_arrays csr1, csr2;
  auto weight_func = [](auto& r) { return 1.3; };

  // CSR build
  graph->run_transaction([&]() {
    graph->poseidon_to_csr(csr1, weight_func);
    return true;
  });

  // T (a single update transaction)
  graph->begin_transaction();
  graph->delete_relationship(6, 2);
  graph->add_relationship(0, 5, ":knows", {});

  for (int i = 7; i < 11; i++) { // adds nodes 7, 8, 9 and 10 to append delta
    graph->add_node("Person",
              {{"name", boost::any(std::string("John Doe"))},
              {"age", boost::any(42)},
              {"id", boost::any(i)},
              {"dummy1", boost::any(std::string("Dummy"))},
              {"dummy2", boost::any(1.2345)}},
              true);
  }
  graph->add_relationship(8, 6, ":knows", {});
  graph->add_relationship(8, 5, ":knows", {});
  graph->add_relationship(9, 0, ":knows", {});
  graph->add_relationship(10, 2, ":knows", {});
  graph->add_relationship(10, 0, ":knows", {});
  graph->add_relationship(10, 1, ":knows", {});
  graph->commit_transaction();

  // CSR update with delta
  graph->run_transaction([&]() {
    graph->poseidon_to_csr(csr2, weight_func);
    return true;
  });

  std::vector<uint64_t> row_offs = {0, 3, 4, 5, 6, 6, 7, 7, 7, 9, 10, 13};
  std::vector<uint64_t> col_inds = {5, 6, 1, 2, 3, 4, 6, 5, 6, 0, 1, 0, 2};
  std::vector<float> edge_vals = {1.3, 1.3, 1.3, 1.3, 1.3, 1.3, 1.3,
                          1.3, 1.3, 1.3, 1.3, 1.3, 1.3,};

  REQUIRE(csr2.row_offsets == row_offs);
  REQUIRE(csr2.col_indices == col_inds);
  REQUIRE(csr2.edge_values == edge_vals);

  graph_pool::destroy(pool);
}

TEST_CASE("Updating graph and updating CSR with delta in a series of transactions", "[format_converter]"){
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("my_graph");
  create_known_data(graph);

  csr_arrays csr1, csr2, csr3, csr4, csr5, csr6, csr7;
  auto weight_func = [](auto& r) { return 1.3; };

  // CSR build
  graph->run_transaction([&]() {
    graph->poseidon_to_csr(csr1, weight_func);
    return true;
  });

  // T1 (first update transaction)
  graph->begin_transaction();
  graph->delete_relationship(6, 2);
  graph->commit_transaction();

  // 1st CSR update with delta
  graph->run_transaction([&]() {
    graph->poseidon_to_csr(csr2, weight_func);
    return true;
  });

  // T2
  graph->begin_transaction();
  graph->add_relationship(0, 5, ":knows", {});
  graph->commit_transaction();

  // 2nd CSR update with delta
  graph->run_transaction([&]() {
    graph->poseidon_to_csr(csr3, weight_func);
    return true;
  });

  // T3
  graph->begin_transaction();
  graph->add_node("Person", {{"id", boost::any(7)}}, true);
  graph->commit_transaction();

  // 3rd CSR update with delta
  graph->run_transaction([&]() {
    graph->poseidon_to_csr(csr4, weight_func);
    return true;
  });

  // T4
  graph->begin_transaction();
  graph->add_node("Person", {{"id", boost::any(8)}}, true);
  graph->add_relationship(8, 6, ":knows", {});
  graph->add_relationship(8, 5, ":knows", {});
  graph->commit_transaction();

  // 4th CSR update with delta
  graph->run_transaction([&]() {
    graph->poseidon_to_csr(csr5, weight_func);
    return true;
  });

  // T5
  graph->begin_transaction();
  graph->add_node("Person", {{"id", boost::any(9)}}, true);
  graph->add_relationship(9, 0, ":knows", {});
  graph->commit_transaction();

  // 5th CSR update with delta
  graph->run_transaction([&]() {
    graph->poseidon_to_csr(csr6, weight_func);
    return true;
  });

  // T6
  graph->begin_transaction();
  graph->add_node("Person", {{"id", boost::any(10)}}, true);
  graph->add_relationship(10, 2, ":knows", {});
  graph->add_relationship(10, 0, ":knows", {});
  graph->add_relationship(10, 1, ":knows", {});
  graph->commit_transaction();

  // 6th CSR update with delta
  graph->run_transaction([&]() {
    graph->poseidon_to_csr(csr7, weight_func);
    return true;
  });

  std::vector<uint64_t> row_offs = {0, 3, 4, 5, 6, 6, 7, 7, 7, 9, 10, 13};
  std::vector<uint64_t> col_inds = {5, 6, 1, 2, 3, 4, 6, 5, 6, 0, 1, 0, 2};
  std::vector<float> edge_vals = {1.3, 1.3, 1.3, 1.3, 1.3, 1.3, 1.3,
                          1.3, 1.3, 1.3, 1.3, 1.3, 1.3,};

  REQUIRE(csr7.row_offsets == row_offs);
  REQUIRE(csr7.col_indices == col_inds);
  REQUIRE(csr7.edge_values == edge_vals);

  graph_pool::destroy(pool);
}

TEST_CASE("Consistency Test 1a", "[format_converter]"){
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("my_graph");
  create_known_data(graph);

  barrier b1{}, b2{};
  csr_arrays csr;
  auto weight_func = [](auto& r) { return 1.3; };

  /**
   * t1 starts before t2
   * t2 creates a node and commits, then t1 builds CSR 
   * in CSR build, a read is done for all node ids. 
   * therefore, t1 tries to read the committed node and aborts (no valid version)
   */
  auto t1 = std::thread([&]() {
    graph->begin_transaction();
    b1.notify(); // ensure t1 starts before t2
    b2.wait(); // before t1 builds CSR, ensure t2 adds delta for its updates and commits

    // this CSR build should not include node with id 7 (and its relationship), created by t2
    // t1 tries to read the node to build the CSR and aborts
    REQUIRE_THROWS_AS(graph->poseidon_to_csr(csr, weight_func), unknown_id);
    graph->commit_transaction();
  });

  auto t2 = std::thread([&]() {
    b1.wait(); // wait for t1 to start
    graph->begin_transaction();
    // t1 should not see these updates
    graph->add_node("Person", {{"id", boost::any(7)}}, true);
    graph->add_relationship(7, 0, ":knows", {});
    graph->commit_transaction();
    b2.notify();
  });

  t1.join();
  t2.join();

  graph_pool::destroy(pool);
}

TEST_CASE("Consistency Test 1b", "[format_converter]"){
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("my_graph");
  create_known_data(graph);

  barrier b1{}, b2{}, b3{};
  csr_arrays csr;
  auto weight_func = [](auto& r) { return 1.3; };

  /**
   * t1 starts before t2
   * t2 creates a node and before it commits, t1 builds CSR 
   * in CSR build, a read is done for all node ids. 
   * therefore, t1 tries to read the uncommitted node and aborts (no valid version)
   */
  auto t1 = std::thread([&]() {
    graph->begin_transaction();
    b1.notify(); // ensure t1 starts before t2
    b2.wait();

    // this CSR build should not include node with id 7 (and its relationship), created by t2
    // t1 tries to read the uncommitted node to build the CSR and aborts
    REQUIRE_THROWS_AS(graph->poseidon_to_csr(csr, weight_func), unknown_id);
    b3.notify(); // inform t2 to commit
    graph->commit_transaction();
  });

  auto t2 = std::thread([&]() {
    b1.wait(); // wait for t1 to start
    graph->begin_transaction();
    // t1 should not see these updates
    graph->add_node("Person", {{"id", boost::any(7)}}, true);
    graph->add_relationship(7, 0, ":knows", {});
    b2.notify();
    b3.wait(); // ensure t1 builds CSR before t2 commits
    graph->commit_transaction();
  });

  t1.join();
  t2.join();

  graph_pool::destroy(pool);
}

TEST_CASE("Consistency Test 2a", "[format_converter]"){
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("my_graph");
  create_known_data(graph);

  barrier b1{}, b2{};
  csr_arrays csr1, csr2;
  auto weight_func = [](auto& r) { return 1.3; };

  /**
   * t1 starts before t2
   * t2 creates a new relationship R_add and deletes an existing relationship R_del
   * after t2 commits, t1 builds CSR 
   * in the CSR build, t1 sees R_del but does not see R_add
   */
  auto t1 = std::thread([&]() {
    graph->begin_transaction();
    b1.notify(); // ensure t1 starts before t2
    b2.wait(); // before t1 builds CSR, ensure t2 adds delta for its updates and commits

    // this CSR build should include R_del but not R_add
    graph->poseidon_to_csr(csr1, weight_func);
    graph->commit_transaction();
  });

  auto t2 = std::thread([&]() {
    b1.wait(); // wait for t1 to start
    graph->begin_transaction();
    // t1 should see R_del but not R_add
    graph->add_relationship(0, 5, ":knows", {});
    graph->delete_relationship(6, 2);
    graph->commit_transaction();
    b2.notify();
  });

  t1.join();
  t2.join();

  // the CSR includes R_del but not R_add
  {
    std::vector<uint64_t> row_offs = {0, 2, 3, 4, 5, 5, 6, 7};
    std::vector<uint64_t> col_inds = {6, 1, 2, 3, 4, 6, 2};
    std::vector<float> edge_vals = {1.3, 1.3, 1.3, 1.3, 1.3, 1.3, 1.3};

    REQUIRE(csr1.row_offsets == row_offs);
    REQUIRE(csr1.col_indices == col_inds);
    REQUIRE(csr1.edge_values == edge_vals);
  }

  // update CSR again
  graph->run_transaction([&]() {
    graph->poseidon_to_csr(csr2, weight_func);
    return true;
  });

  // now after another CSR update, the CSR includes R_add but not R_del
  {
    std::vector<uint64_t> row_offs = {0, 3, 4, 5, 6, 6, 7, 7};
    std::vector<uint64_t> col_inds = {5, 6, 1, 2, 3, 4, 6};
    std::vector<float> edge_vals = {1.3, 1.3, 1.3, 1.3, 1.3, 1.3, 1.3};

    REQUIRE(csr2.row_offsets == row_offs);
    REQUIRE(csr2.col_indices == col_inds);
    REQUIRE(csr2.edge_values == edge_vals);
  }

  graph_pool::destroy(pool);
}

TEST_CASE("Consistency Test 2b", "[format_converter]"){
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("my_graph");
  create_known_data(graph);

  barrier b1{}, b2{}, b3{};
  csr_arrays csr1, csr2;
  auto weight_func = [](auto& r) { return 1.3; };

  /**
   * t1 starts before t2
   * t2 creates a new relationship R_add and deletes an existing relationship R_del 
   * before t2 commits, t1 builds CSR 
   * in CSR build, t1 does not see the uncommitted updates of t2
   */
  auto t1 = std::thread([&]() {
    graph->begin_transaction();
    b1.notify(); // ensure t1 starts before t2
    b2.wait();

    // this CSR build should include R_del but not R_add
    graph->poseidon_to_csr(csr1, weight_func);
    b3.notify();
    graph->commit_transaction();
  });

  auto t2 = std::thread([&]() {
    b1.wait(); // wait for t1 to start
    graph->begin_transaction();
    // t1 should see R_del but not R_add
    graph->add_relationship(0, 5, ":knows", {});
    graph->delete_relationship(6, 2);
    b2.notify(); // inform t1 to build CSR
    b3.wait(); // ensure t1 builds CSR before t2 commits
    graph->commit_transaction();
  });

  t1.join();
  t2.join();

  // the CSR includes R_del but not R_add
  {
    std::vector<uint64_t> row_offs = {0, 2, 3, 4, 5, 5, 6, 7};
    std::vector<uint64_t> col_inds = {6, 1, 2, 3, 4, 6, 2};
    std::vector<float> edge_vals = {1.3, 1.3, 1.3, 1.3, 1.3, 1.3, 1.3};

    REQUIRE(csr1.row_offsets == row_offs);
    REQUIRE(csr1.col_indices == col_inds);
    REQUIRE(csr1.edge_values == edge_vals);
  }

  // update CSR again
  graph->run_transaction([&]() {
    graph->poseidon_to_csr(csr2, weight_func);
    return true;
  });

  // now after another CSR update, the CSR includes R_add but not R_del
  {
    std::vector<uint64_t> row_offs = {0, 3, 4, 5, 6, 6, 7, 7};
    std::vector<uint64_t> col_inds = {5, 6, 1, 2, 3, 4, 6};
    std::vector<float> edge_vals = {1.3, 1.3, 1.3, 1.3, 1.3, 1.3, 1.3};

    REQUIRE(csr2.row_offsets == row_offs);
    REQUIRE(csr2.col_indices == col_inds);
    REQUIRE(csr2.edge_values == edge_vals);
  }

  graph_pool::destroy(pool);
}

TEST_CASE("Consistency Test 3a", "[format_converter]"){
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("my_graph");
  create_known_data(graph);

  barrier b1{}, b2{};
  csr_arrays csr1, csr2, csr3;
  auto weight_func = [](auto& r) { return 1.3; };

  // build CSR
  graph->run_transaction([&]() {
    graph->poseidon_to_csr(csr1, weight_func);
    return true;
  });

  /**
   * t1 starts before t2
   * t2 creates a node and commits, then t1 updates CSR 
   * in the CSR update, t1 does not see the committed node
   */
  auto t1 = std::thread([&]() {
    graph->begin_transaction();
    b1.notify(); // ensure t1 starts before t2
    b2.wait(); // before t1 updates CSR, ensure t2 adds delta for its updates and commits

    // this CSR update should not include node with id 7 (and its relationship), created by t2
    graph->poseidon_to_csr(csr2, weight_func);
    graph->commit_transaction();
  });

  auto t2 = std::thread([&]() {
    b1.wait(); // wait for t1 to start
    graph->begin_transaction();
    // t1 should not see this node (and its relationship)
    graph->add_node("Person", {{"id", boost::any(7)}}, true);
    graph->add_relationship(7, 0, ":knows", {});
    graph->commit_transaction();
    b2.notify();
  });

  t1.join();
  t2.join();

  // the CSR does not include the node and its relationship (i.e. the CSR stays the same)
  {
    REQUIRE(csr2.row_offsets.empty());
    REQUIRE(csr2.col_indices.empty());
    REQUIRE(csr2.edge_values.empty());
  }

  // update CSR again
  graph->run_transaction([&]() {
    graph->poseidon_to_csr(csr3, weight_func);
    return true;
  });

  // now after another CSR update, the CSR includes the node and its relationship
  {
    std::vector<uint64_t> row_offs = {0, 2, 3, 4, 5, 5, 6, 7, 8};
    std::vector<uint64_t> col_inds = {6, 1, 2, 3, 4, 6, 2, 0};
    std::vector<float> edge_vals = {1.3, 1.3, 1.3, 1.3, 1.3, 1.3, 1.3, 1.3};

    REQUIRE(csr3.row_offsets == row_offs);
    REQUIRE(csr3.col_indices == col_inds);
    REQUIRE(csr3.edge_values == edge_vals);
  }

  graph_pool::destroy(pool);
}

TEST_CASE("Consistency Test 3b", "[format_converter]"){
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("my_graph");
  create_known_data(graph);

  barrier b1{}, b2{}, b3{};
  csr_arrays csr1, csr2, csr3;
  auto weight_func = [](auto& r) { return 1.3; };

  // build CSR
  graph->run_transaction([&]() {
    graph->poseidon_to_csr(csr1, weight_func);
    return true;
  });

  /**
   * t1 starts before t2
   * t2 creates a node and before it commits, t1 updates CSR 
   * in the CSR update, t1 does not see the uncommitted changes of t2
   */
  auto t1 = std::thread([&]() {
    graph->begin_transaction();
    b1.notify(); // ensure t1 starts before t2
    b2.wait();

    // this CSR update should not include node with id 7 (and its relationship), created by t2
    graph->poseidon_to_csr(csr2, weight_func);
    b3.notify(); // inform t2 to commit
    graph->commit_transaction();
  });

  auto t2 = std::thread([&]() {
    b1.wait(); // wait for t1 to start
    graph->begin_transaction();
    // t1 should not see this node (and its relationship)
    graph->add_node("Person", {{"id", boost::any(7)}}, true);
    graph->add_relationship(7, 0, ":knows", {});
    b2.notify(); // inform t1 to update CSR
    b3.wait(); // ensure t1 updates CSR before t2 commits
    graph->commit_transaction();
  });

  t1.join();
  t2.join();

  // the CSR does not include the node and its relationship (i.e. the CSR stays the same)
  {
    REQUIRE(csr2.row_offsets.empty());
    REQUIRE(csr2.col_indices.empty());
    REQUIRE(csr2.edge_values.empty());
  }

  // update CSR again
  graph->run_transaction([&]() {
    graph->poseidon_to_csr(csr3, weight_func);
    return true;
  });

  // now after another CSR update, the CSR includes the node and its relationship
  {
    std::vector<uint64_t> row_offs = {0, 2, 3, 4, 5, 5, 6, 7, 8};
    std::vector<uint64_t> col_inds = {6, 1, 2, 3, 4, 6, 2, 0};
    std::vector<float> edge_vals = {1.3, 1.3, 1.3, 1.3, 1.3, 1.3, 1.3, 1.3};

    REQUIRE(csr3.row_offsets == row_offs);
    REQUIRE(csr3.col_indices == col_inds);
    REQUIRE(csr3.edge_values == edge_vals);
  }

  graph_pool::destroy(pool);
}

TEST_CASE("Consistency Test 4a", "[format_converter]"){
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("my_graph");
  create_known_data(graph);

  barrier b1{}, b2{};
  csr_arrays csr1, csr2, csr3;
  auto weight_func = [](auto& r) { return 1.3; };

  // build CSR
  graph->run_transaction([&]() {
    graph->poseidon_to_csr(csr1, weight_func);
    return true;
  });

  /**
   * t1 starts before t2
   * t2 creates a new relationship R_add and deletes an existing relationship R_del
   * after t2 commits, t1 updates CSR 
   * in the CSR update, t1 sees R_del but does not see R_add
   */
  auto t1 = std::thread([&]() {
    graph->begin_transaction();
    b1.notify(); // ensure t1 starts before t2
    b2.wait(); // before t1 updates CSR, ensure t2 adds delta for its updates and commits

    // this CSR update should include R_del but not R_add (i.e. the CSR stays the same)
    graph->poseidon_to_csr(csr2, weight_func);
    graph->commit_transaction();
  });

  auto t2 = std::thread([&]() {
    b1.wait(); // wait for t1 to start
    graph->begin_transaction();
    // t1 should see R_del but not R_add
    graph->add_relationship(0, 5, ":knows", {});
    graph->delete_relationship(6, 2);
    graph->commit_transaction();
    b2.notify();
  });

  t1.join();
  t2.join();

  // the CSR includes R_del but not R_add (i.e. the CSR stays the same)
  {
    REQUIRE(csr2.row_offsets.empty());
    REQUIRE(csr2.col_indices.empty());
    REQUIRE(csr2.edge_values.empty());
  }

  // update CSR again
  graph->run_transaction([&]() {
    graph->poseidon_to_csr(csr3, weight_func);
    return true;
  });

  // now after another CSR update, the CSR includes R_add but not R_del
  {
    std::vector<uint64_t> row_offs = {0, 3, 4, 5, 6, 6, 7, 7};
    std::vector<uint64_t> col_inds = {5, 6, 1, 2, 3, 4, 6};
    std::vector<float> edge_vals = {1.3, 1.3, 1.3, 1.3, 1.3, 1.3, 1.3};

    REQUIRE(csr3.row_offsets == row_offs);
    REQUIRE(csr3.col_indices == col_inds);
    REQUIRE(csr3.edge_values == edge_vals);
  }

  graph_pool::destroy(pool);
}

TEST_CASE("Consistency Test 4b", "[format_converter]"){
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("my_graph");
  create_known_data(graph);

  barrier b1{}, b2{}, b3{};
  csr_arrays csr1, csr2, csr3;
  auto weight_func = [](auto& r) { return 1.3; };

  // build CSR
  graph->run_transaction([&]() {
    graph->poseidon_to_csr(csr1, weight_func);
    return true;
  });

  /**
   * t1 starts before t2
   * t2 creates a new relationship R_add and deletes an existing relationship R_del 
   * before t2 commits, t1 updates CSR 
   * in the CSR update, t1 does not see the uncommitted changes of t2
   */
  auto t1 = std::thread([&]() {
    graph->begin_transaction();
    b1.notify(); // ensure t1 starts before t2
    b2.wait();

    // this CSR update should include R_del but not R_add
    graph->poseidon_to_csr(csr2, weight_func);
    b3.notify(); // notify t2 to commit
    graph->commit_transaction();
  });

  auto t2 = std::thread([&]() {
    b1.wait(); // wait for t1 to start
    graph->begin_transaction();
    // t1 should see R_del but not R_add
    graph->add_relationship(0, 5, ":knows", {});
    graph->delete_relationship(6, 2);
    b2.notify(); // notify t1 to update CSR
    b3.wait(); // ensure t1 updates CSR before t2 commits
    graph->commit_transaction();
  });

  t1.join();
  t2.join();

  // the CSR includes R_del but not R_add (i.e. the CSR stays the same)
  {
    REQUIRE(csr2.row_offsets.empty());
    REQUIRE(csr2.col_indices.empty());
    REQUIRE(csr2.edge_values.empty());
  }

  // update CSR again
  graph->run_transaction([&]() {
    graph->poseidon_to_csr(csr3, weight_func);
    return true;
  });

  // now after another CSR update, the CSR includes R_add but not R_del
  {
    std::vector<uint64_t> row_offs = {0, 3, 4, 5, 6, 6, 7, 7};
    std::vector<uint64_t> col_inds = {5, 6, 1, 2, 3, 4, 6};
    std::vector<float> edge_vals = {1.3, 1.3, 1.3, 1.3, 1.3, 1.3, 1.3};

    REQUIRE(csr3.row_offsets == row_offs);
    REQUIRE(csr3.col_indices == col_inds);
    REQUIRE(csr3.edge_values == edge_vals);
  }

  graph_pool::destroy(pool);
}

TEST_CASE("Consistency Test 5a", "[format_converter]"){
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("my_graph");
  create_known_data(graph);

  barrier b1{}, b2{};
  csr_arrays csr;
  auto weight_func = [](auto& r) { return 1.3; };

  /**
   * t2 starts before t1
   * t2 creates a node and commits, then t1 builds CSR 
   * in CSR build, t1 should see the committed node
   */
  auto t1 = std::thread([&]() {
    b1.wait(); // wait for t2 to start
    graph->begin_transaction();
    b2.wait(); // before t1 builds CSR, ensure t2 adds delta for its updates and commits

    // this CSR build should include node with id 7 (and its relationship), created by t2
    REQUIRE_NOTHROW(graph->poseidon_to_csr(csr, weight_func));
    graph->commit_transaction();
  });

  auto t2 = std::thread([&]() {
    graph->begin_transaction();
    b1.notify(); // ensure t2 starts before t1
    // t1 should see these updates
    graph->add_node("Person", {{"id", boost::any(7)}}, true);
    graph->add_relationship(7, 0, ":knows", {});
    graph->commit_transaction();
    b2.notify();
  });

  t1.join();
  t2.join();

  // the CSR includes the node (and its relationship)
  {
    std::vector<uint64_t> row_offs = {0, 2, 3, 4, 5, 5, 6, 7, 8};
    std::vector<uint64_t> col_inds = {6, 1, 2, 3, 4, 6, 2, 0};
    std::vector<float> edge_vals = {1.3, 1.3, 1.3, 1.3, 1.3, 1.3, 1.3, 1.3};

    REQUIRE(csr.row_offsets == row_offs);
    REQUIRE(csr.col_indices == col_inds);
    REQUIRE(csr.edge_values == edge_vals);
  }

  graph_pool::destroy(pool);
}

TEST_CASE("Consistency Test 5b", "[format_converter]"){
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("my_graph");
  create_known_data(graph);

  barrier b1{}, b2{}, b3{};
  csr_arrays csr1{}, csr2{};
  auto weight_func = [](auto& r) { return 1.3; };

  /**
   * t2 starts before t1
   * t2 creates a node and before it commits, t1 builds CSR 
   * in CSR build, a read is done for all node ids. 
   * therefore, t1 tries to read the dirty node with id 0 and aborts (dirty read)
   */
  auto t1 = std::thread([&]() {
    b1.wait(); // wait for t2 to start
    graph->begin_transaction();
    b2.wait();

    // this CSR build should not include node with id 7 (and its relationship), created by t2
    // t1 tries to read the dirty node with id 0 to build the CSR and aborts
    REQUIRE_THROWS_AS(graph->poseidon_to_csr(csr1, weight_func), transaction_abort);
    b3.notify(); // inform t2 to commit
    graph->commit_transaction();
  });

  auto t2 = std::thread([&]() {
    graph->begin_transaction();
    b1.notify(); // ensure t2 starts before t1
    // t1 should not see these updates before commit
    graph->add_node("Person", {{"id", boost::any(7)}}, true);
    graph->add_relationship(7, 0, ":knows", {});
    b2.notify();
    b3.wait(); // ensure t1 builds CSR before t2 commits
    graph->commit_transaction();
  });

  t1.join();
  t2.join();

  graph_pool::destroy(pool);
}

TEST_CASE("Consistency Test 6a", "[format_converter]"){
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("my_graph");
  create_known_data(graph);

  barrier b1{}, b2{};
  csr_arrays csr;
  auto weight_func = [](auto& r) { return 1.3; };

  /**
   * t2 starts before t1
   * t2 creates a new relationship R_add and deletes an existing relationship R_del 
   * after t2 commits, t1 builds CSR 
   * in CSR build, t1 sees R_del but does not see R_add
   */
  auto t1 = std::thread([&]() {
    b1.wait(); // wait for t2 to start
    graph->begin_transaction();
    b2.wait(); // before t1 builds CSR, ensure t2 adds delta for its updates and commits

    // this CSR build should include R_add but not R_del
    graph->poseidon_to_csr(csr, weight_func);
    graph->commit_transaction();
  });

  auto t2 = std::thread([&]() {
    graph->begin_transaction();
    b1.notify(); // ensure t2 starts before t1
    // t1 should see R_add but not R_del
    graph->add_relationship(0, 5, ":knows", {});
    graph->delete_relationship(6, 2);
    graph->commit_transaction();
    b2.notify();
  });

  t1.join();
  t2.join();

  // the CSR includes the relationship
  {
    std::vector<uint64_t> row_offs = {0, 3, 4, 5, 6, 6, 7, 7};
    std::vector<uint64_t> col_inds = {5, 6, 1, 2, 3, 4, 6};
    std::vector<float> edge_vals = {1.3, 1.3, 1.3, 1.3, 1.3, 1.3, 1.3};

    REQUIRE(csr.row_offsets == row_offs);
    REQUIRE(csr.col_indices == col_inds);
    REQUIRE(csr.edge_values == edge_vals);
  }

  graph_pool::destroy(pool);
}

TEST_CASE("Consistency Test 6b", "[format_converter]"){
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("my_graph");
  create_known_data(graph);

  barrier b1{}, b2{}, b3{};
  csr_arrays csr;
  auto weight_func = [](auto& r) { return 1.3; };

  /**
   * t2 starts before t1
   * t2 creates a new relationship R_add and deletes an existing relationship R_del 
   * before t2 commits, t1 builds CSR 
   * in CSR build, a read is done for all node ids. 
   * therefore, t1 tries to read the dirty node with id 0 and aborts (dirty read)
   */
  auto t1 = std::thread([&]() {
    b1.wait(); // wait for t2 to start
    graph->begin_transaction();
    b2.wait();

    // this CSR build should not include the relationship between nodes with ids 0 and 5, created by t2
    // t1 tries to read the dirty node with id 0 to build the CSR and aborts
    REQUIRE_THROWS_AS(graph->poseidon_to_csr(csr, weight_func), transaction_abort);
    b3.notify(); // inform t2 to commit
    graph->commit_transaction();
  });

  auto t2 = std::thread([&]() {
    graph->begin_transaction();
    b1.notify(); // ensure t2 starts before t1
    // t1 should not see the uncommitted updates of t2
    graph->add_relationship(0, 5, ":knows", {});
    graph->delete_relationship(6, 2);
    b2.notify();
    b3.wait(); // ensure t1 builds CSR before t2 commits
    graph->commit_transaction();
  });

  t1.join();
  t2.join();

  graph_pool::destroy(pool);
}

TEST_CASE("Consistency Test 7a", "[format_converter]"){
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("my_graph");
  create_known_data(graph);

  barrier b1{}, b2{};
  csr_arrays csr1, csr2, csr3;
  auto weight_func = [](auto& r) { return 1.3; };

  // build CSR
  graph->run_transaction([&]() {
    graph->poseidon_to_csr(csr1, weight_func);
    return true;
  });

  /**
   * t2 starts before t1
   * t2 creates a node and commits, then t1 updates CSR 
   * in the CSR update, t1 should see the committed node
   */
  auto t1 = std::thread([&]() {
    b1.wait(); // wait for t2 to start
    graph->begin_transaction();
    b2.wait(); // before t1 updates CSR, ensure t2 adds delta for its updates and commits

    // this CSR update should include node with id 7 (and its relationship), created by t2
    graph->poseidon_to_csr(csr2, weight_func);
    graph->commit_transaction();
  });

  auto t2 = std::thread([&]() {
    graph->begin_transaction();
    b1.notify(); // ensure t2 starts before t1
    // t1 should see this node (and its relationship)
    graph->add_node("Person", {{"id", boost::any(7)}}, true);
    graph->add_relationship(7, 0, ":knows", {});
    graph->commit_transaction();
    b2.notify();
  });

  t1.join();
  t2.join();

  // the CSR includes the node (and its relationship)
  {
    std::vector<uint64_t> row_offs = {0, 2, 3, 4, 5, 5, 6, 7, 8};
    std::vector<uint64_t> col_inds = {6, 1, 2, 3, 4, 6, 2, 0};
    std::vector<float> edge_vals = {1.3, 1.3, 1.3, 1.3, 1.3, 1.3, 1.3, 1.3};

    REQUIRE(csr2.row_offsets == row_offs);
    REQUIRE(csr2.col_indices == col_inds);
    REQUIRE(csr2.edge_values == edge_vals);
  }

  graph_pool::destroy(pool);
}

TEST_CASE("Consistency Test 7b", "[format_converter]"){
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("my_graph");
  create_known_data(graph);

  barrier b1{}, b2{}, b3{};
  csr_arrays csr1, csr2, csr3;
  auto weight_func = [](auto& r) { return 1.3; };

  // build CSR
  graph->run_transaction([&]() {
    graph->poseidon_to_csr(csr1, weight_func);
    return true;
  });

  /**
   * t2 starts before t1
   * t2 creates a node and before it commits, t1 updates CSR 
   * in the CSR update, t1 does not see the uncommitted changes of t2
   */
  auto t1 = std::thread([&]() {
    b1.wait(); // wait for t2 to start
    graph->begin_transaction();
    b2.wait();

    // this CSR update should not include node with id 7 (and its relationship), created by t2
    graph->poseidon_to_csr(csr2, weight_func);
    b3.notify(); // inform t2 to commit
    graph->commit_transaction();
  });

  auto t2 = std::thread([&]() {
    graph->begin_transaction();
    b1.notify(); // ensure t2 starts before t1
    // t1 should see this node (and its relationship)
    graph->add_node("Person", {{"id", boost::any(7)}}, true);
    graph->add_relationship(7, 0, ":knows", {});
    b2.notify(); // notify t1 to update CSR
    b3.wait(); // ensure t1 updates CSR before t2 commits
    graph->commit_transaction();
  });

  t1.join();
  t2.join();

  // the CSR does not include the node and its relationship (i.e. the CSR stays the same)
  {
    REQUIRE(csr2.row_offsets.empty());
    REQUIRE(csr2.col_indices.empty());
    REQUIRE(csr2.edge_values.empty());
  }

  // update CSR again
  graph->run_transaction([&]() {
    graph->poseidon_to_csr(csr3, weight_func);
    return true;
  });

  // now after another CSR update, the CSR includes the node and its relationship
  {
    std::vector<uint64_t> row_offs = {0, 2, 3, 4, 5, 5, 6, 7, 8};
    std::vector<uint64_t> col_inds = {6, 1, 2, 3, 4, 6, 2, 0};
    std::vector<float> edge_vals = {1.3, 1.3, 1.3, 1.3, 1.3, 1.3, 1.3, 1.3};

    REQUIRE(csr3.row_offsets == row_offs);
    REQUIRE(csr3.col_indices == col_inds);
    REQUIRE(csr3.edge_values == edge_vals);
  }

  graph_pool::destroy(pool);
}

TEST_CASE("Consistency Test 8a", "[format_converter]"){
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("my_graph");
  create_known_data(graph);

  barrier b1{}, b2{};
  csr_arrays csr1, csr2;
  auto weight_func = [](auto& r) { return 1.3; };

  // build CSR
  graph->run_transaction([&]() {
    graph->poseidon_to_csr(csr1, weight_func);
    return true;
  });

  /**
   * t2 starts before t1
   * t2 creates a new relationship R_add and deletes an existing relationship R_del
   * after t2 commits, t1 updates CSR 
   * in the CSR update, t1 sees R_add but does not see R_del
   */
  auto t1 = std::thread([&]() {
    b1.wait(); // wait for t2 to start
    graph->begin_transaction();
    b2.wait(); // before t1 updates CSR, ensure t2 adds delta for its updates and commits

    // this CSR update should include R_add but not R_del
    graph->poseidon_to_csr(csr2, weight_func);
    graph->commit_transaction();
  });

  auto t2 = std::thread([&]() {
    graph->begin_transaction();
    b1.notify(); // ensure t2 starts before t1
    // t1 should see R_add but not R_del
    graph->add_relationship(0, 5, ":knows", {});
    graph->delete_relationship(6, 2);
    graph->commit_transaction();
    b2.notify();
  });

  t1.join();
  t2.join();

  // the CSR includes R_add but not R_del
  {
    std::vector<uint64_t> row_offs = {0, 3, 4, 5, 6, 6, 7, 7};
    std::vector<uint64_t> col_inds = {5, 6, 1, 2, 3, 4, 6};
    std::vector<float> edge_vals = {1.3, 1.3, 1.3, 1.3, 1.3, 1.3, 1.3};

    REQUIRE(csr2.row_offsets == row_offs);
    REQUIRE(csr2.col_indices == col_inds);
    REQUIRE(csr2.edge_values == edge_vals);
  }

  graph_pool::destroy(pool);
}

TEST_CASE("Consistency Test 8b", "[format_converter]"){
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("my_graph");
  create_known_data(graph);

  barrier b1{}, b2{}, b3{};
  csr_arrays csr1, csr2, csr3;
  auto weight_func = [](auto& r) { return 1.3; };

  // build CSR
  graph->run_transaction([&]() {
    graph->poseidon_to_csr(csr1, weight_func);
    return true;
  });

  /**
   * t2 starts before t1
   * t2 creates a new relationship R_add and deletes an existing relationship R_del
   * before t2 commits, t1 updates CSR  
   * in the CSR update, t1 should not see the uncommitted changes of t2
   */
  auto t1 = std::thread([&]() {
    b1.wait(); // wait for t2 to start
    graph->begin_transaction();
    b2.wait();

    // this CSR update should include R_del but not R_add
    graph->poseidon_to_csr(csr2, weight_func);
    b3.notify(); // notify t2 to commit
    graph->commit_transaction();
  });

  auto t2 = std::thread([&]() {
    graph->begin_transaction();
    b1.notify(); // ensure t2 starts before t1
    // t1 should see R_del but not R_add
    graph->add_relationship(0, 5, ":knows", {});
    graph->delete_relationship(6, 2);
    b2.notify(); // notify t1 to update CSR
    b3.wait(); // ensure t1 updates CSR before t2 commits
    graph->commit_transaction();
  });

  t1.join();
  t2.join();

  // the CSR include R_del but not R_add (i.e. the CSR stays the same)
  {
    REQUIRE(csr2.row_offsets.empty());
    REQUIRE(csr2.col_indices.empty());
    REQUIRE(csr2.edge_values.empty());
  }

  // update CSR again
  graph->run_transaction([&]() {
    graph->poseidon_to_csr(csr3, weight_func);
    return true;
  });

  // now after another CSR update, the CSR includes R_add but not R_del
  {
    std::vector<uint64_t> row_offs = {0, 3, 4, 5, 6, 6, 7, 7};
    std::vector<uint64_t> col_inds = {5, 6, 1, 2, 3, 4, 6};
    std::vector<float> edge_vals = {1.3, 1.3, 1.3, 1.3, 1.3, 1.3, 1.3};

    REQUIRE(csr3.row_offsets == row_offs);
    REQUIRE(csr3.col_indices == col_inds);
    REQUIRE(csr3.edge_values == edge_vals);
  }

  graph_pool::destroy(pool);
}

TEST_CASE("Consistency Test 9a", "[format_converter]"){
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("my_graph");
  create_known_data(graph);

  barrier b1{}, b2{};
  csr_arrays csr1, csr2;
  auto weight_func = [](auto& r) { return 1.3; };

  /**
   * t1 starts before t2
   * t2 builds CSR, then t1 updates CSR 
   * in the CSR update, t1 tries to restore the CSR to an older snapshot, it should abort
   */
  auto t1 = std::thread([&]() {
    graph->begin_transaction();
    b1.notify(); // ensure t1 starts before t2
    b2.wait(); // ensure t2 builds CSR before t1 updates the CSR

    // this CSR update should abort
    REQUIRE_THROWS_AS(graph->poseidon_to_csr(csr1, weight_func), invalid_csr_update);
    graph->commit_transaction();
  });

  auto t2 = std::thread([&]() {
    b1.wait(); // wait for t1 to start
    graph->begin_transaction();
    // build CSR
    graph->poseidon_to_csr(csr2, weight_func);
    b2.notify(); // inform t1 to update CSR
    graph->commit_transaction();
  });

  t1.join();
  t2.join();

  graph_pool::destroy(pool);
}

TEST_CASE("Consistency Test 9b", "[format_converter]"){
  auto pool = graph_pool::create(test_path);
  auto graph = pool->create_graph("my_graph");
  create_known_data(graph);

  barrier b1{}, b2{};
  csr_arrays csr1, csr2, csr3;
  auto weight_func = [](auto& r) { return 1.3; };

  // build CSR
  graph->run_transaction([&]() {
    graph->poseidon_to_csr(csr1, weight_func);
    return true;
  });

  graph->begin_transaction();
  graph->add_relationship(0, 5, ":knows", {});
  graph->commit_transaction();

  /**
   * t1 starts before t2
   * t2 updates CSR, then t1 also updates CSR 
   * in the CSR update, t1 tries to restore the CSR to an older snapshot, it should abort
   */
  auto t1 = std::thread([&]() {
    graph->begin_transaction();
    b1.notify(); // ensure t1 starts before t2
    b2.wait(); // ensure t2 updates CSR before t1 also updates the CSR

    // this CSR update should abort
    REQUIRE_THROWS_AS(graph->poseidon_to_csr(csr1, weight_func), invalid_csr_update);
    graph->commit_transaction();
  });

  auto t2 = std::thread([&]() {
    b1.wait(); // wait for t1 to start
    graph->begin_transaction();
    // update CSR
    graph->poseidon_to_csr(csr2, weight_func);
    b2.notify(); // inform t1 to update CSR
    graph->commit_transaction();
  });

  t1.join();
  t2.join();

  graph_pool::destroy(pool);
}