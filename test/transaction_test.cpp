/*
 * Copyright (C) 2019-2020 DBIS Group - TU Ilmenau, All Rights Reserved.
 *
 * This file is part of the Poseidon package.
 *
 * Poseidon is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Poseidon is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Poseidon. If not, see <http://www.gnu.org/licenses/>.
 */

#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do
                          // this in one cpp file

#include <condition_variable>
#include <iostream>
#include <thread>

#include "catch.hpp"
#include "config.h"
#include "defs.hpp"
#include "graph_db.hpp"
#include "transaction.hpp"

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

#ifdef USE_PMDK
#define PMEMOBJ_POOL_SIZE ((size_t)(1024 * 1024 * 80))

namespace nvm = pmem::obj;
const std::string test_path = poseidon::gPmemPath + "transaction_test";

nvm::pool_base prepare_pool() {
  auto pop = nvm::pool_base::create(test_path, "", PMEMOBJ_POOL_SIZE);
  return pop;
}

graph_db_ptr create_graph_db(nvm::pool_base &pop) {
  graph_db_ptr gdb;
  nvm::transaction::run(pop, [&] { gdb = p_make_ptr<graph_db>(); });
  return gdb;
}

void drop_graph_db(nvm::pool_base &pop, graph_db_ptr gdb) {
  nvm::transaction::run(pop, [&] { nvm::delete_persistent<graph_db>(gdb); });
  pop.close();
  remove(test_path.c_str());
}

#else

graph_db_ptr create_graph_db() { return p_make_ptr<graph_db>(); }

#endif


/*
 * Test case for issue : https://dbgit.prakinf.tu-ilmenau.de/code/poseidon_core/-/issues/25
 * Probable Minimal fix : Ensure that below lines are executed atomically
 * https://dbgit.prakinf.tu-ilmenau.de/code/poseidon_core/-/blob/master/src/storage/graph_db.cpp#L160 and
 * https://dbgit.prakinf.tu-ilmenau.de/code/poseidon_core/-/blob/master/src/storage/graph_db.cpp#L162
 *
 * Repeat the fix for Relationships, too
 * Similarly, A fix is needed during concurrent Insert and Read.
 */


TEST_CASE("Test concurrency: update during read"  "[transaction]") {  
	/*
	* If two concurrent write Transactions are triggered, then the end result must be that there are two seperate
	* nodes created
	*/
	#ifdef USE_PMDK
	  auto pop = prepare_pool();
	  auto gdb = create_graph_db(pop);
	#else
	  auto gdb = create_graph_db();
	#endif
	  std::cout<<" Test concurr between read and write \n";
	  node::id_t nid = 0;
	  barrier  b1{}, b2{}, b3{};

	  // Just create a node
		auto tx = gdb->begin_transaction();
		nid = gdb->add_node("Actor",
		                        {{"name", boost::any(std::string("Mark Wahlberg"))},
		                         {"age", boost::any(48)}});
		gdb->commit_transaction();

	/*
	 * Thread #1: read the node
	 */
	  auto t1 = std::thread([&]() {
			// read the node
			auto tx = gdb->begin_transaction();

	    b1.notify();  // so that txn-3 can start.
	    auto &n = gdb->node_by_id(nid);
	    b2.wait(); // wait till txn-3 does a update but not yet committed

	    auto nd = gdb->get_node_description(nid);

      // Since the Read Txn started before Update Txn, it should always read the original version.
			REQUIRE(nd.label == "Actor"); // It fails here because this txn sees updated Actor
			REQUIRE(get_property<int>(nd.properties, "age") == 48); //here too.. it sees 52 instead of 48

			gdb->commit_transaction();
    });

	/*
	 * Thread #2: update the same node
	 */
	  auto t2 = std::thread([&]() {
			// update the node
			b1.wait(); // ensure that update Txn starts after read Txn
			auto tx = gdb->begin_transaction();
			auto &n = gdb->node_by_id(nid);
			gdb->update_node(n,  //update
				   {
				 { "age", boost::any(52)},
				  },
				   "Updated Actor");

			b2.notify();

			gdb->commit_transaction();


	});

	  t1.join();
	  t2.join();

	#ifdef USE_PMDK
	  drop_graph_db(pop, gdb);
	#endif
}

TEST_CASE("Test concurrency: update + commit during read"  "[transaction]") {  
	/*
	* If two concurrent write Transactions are triggered, then the end result must be that there are two seperate
	* nodes created
	*/
	#ifdef USE_PMDK
	  auto pop = prepare_pool();
	  auto gdb = create_graph_db(pop);
	#else
	  auto gdb = create_graph_db();
	#endif
	  std::cout<<" Test concurr between read and write \n";
	  node::id_t nid = 0;
	  barrier  b1{}, b2{}, b3{};

	  // Just create a node
		auto tx = gdb->begin_transaction();
		nid = gdb->add_node("Actor",
		                        {{"name", boost::any(std::string("Mark Wahlberg"))},
		                         {"age", boost::any(48)}});
		gdb->commit_transaction();

	/*
	 * Thread #1: read the node
	 */
	  auto t1 = std::thread([&]() {
			// read the node
			auto tx = gdb->begin_transaction();

	    b1.notify();  // so that txn-3 can start.
	    b2.wait(); // wait till txn-3 does a update but not yet committed

	    auto &n = gdb->node_by_id(nid);
	    auto nd = gdb->get_node_description(nid);
      // Since the Read Txn started before Update Txn, it should always read the original version.
			REQUIRE(nd.label == "Actor"); // It fails here because this txn sees updated Actor
			REQUIRE(get_property<int>(nd.properties, "age") == 48); //here too.. it sees 52 instead of 48

			gdb->commit_transaction();
    });

	/*
	 * Thread #2: update the same node
	 */
	  auto t2 = std::thread([&]() {
			// update the node
			b1.wait(); // ensure that update Txn starts after read Txn
			auto tx = gdb->begin_transaction();
			auto &n = gdb->node_by_id(nid);
			gdb->update_node(n,  //update
				   {
				 { "age", boost::any(52)},
				  },
				   "Updated Actor");
			gdb->commit_transaction();
			b2.notify();
	});

	  t1.join();
	  t2.join();

	#ifdef USE_PMDK
	  drop_graph_db(pop, gdb);
	#endif
}

/* Test case for issue: https://dbgit.prakinf.tu-ilmenau.de/code/poseidon_core/-/issues/27
 * Where is the issue ? : It crashes when Read Txn has entered this else block at line https://dbgit.prakinf.tu-ilmenau.de/code/poseidon_core/-/blob/master/src/storage/graph_db.cpp#L580
 *                              while the update Txn deletes the dirty version. 
 * Probable fix : Retain the dirty version until a older Read Txn commits. But now there are 2 identical versions: one on Pmem and one in dirty list.
 *
 */

TEST_CASE("Test concurrency between update abort and read"  "[transaction]") { 
#ifdef USE_PMDK
  auto pop = prepare_pool();
  auto gdb = create_graph_db(pop);
#else
  auto gdb = create_graph_db();
#endif

  node::id_t nid = 0;
  barrier  b1{}, b2{}, b3{};

  // Just create a node
	auto tx = gdb->begin_transaction(); 
	nid = gdb->add_node("Actor",
	                        {{"name", boost::any(std::string("Mark Wahlberg"))},
	                         {"age", boost::any(48)}});
  gdb->commit_transaction();

 /*
  * Thread #1: read the node
  */
  auto t1 = std::thread([&]() { 
		// read the node
		auto tx = gdb->begin_transaction();
		b1.notify(); // Inform thread #2 to start
		b2.wait(); // wait until thread #2 has performed the update
		
		auto &n = gdb->node_by_id(nid); 
		b3.notify(); // inform thread #2 that we have read the node 

		auto nd = gdb->get_node_description(nid); //<===== Here it tries to access a deleted dirty version and crashes!

		REQUIRE(nd.label == "Actor"); 
		REQUIRE(get_property<int>(nd.properties, "age") == 48); 
		gdb->commit_transaction();
  });

  /*
   * Thread #2: update the same node
  */
  auto t2 = std::thread([&]() { 
		// update the node
		b1.wait(); // ensure that update starts after the read transaction (thread #1)
		auto tx = gdb->begin_transaction();
		auto &n = gdb->node_by_id(nid);
		gdb->update_node(n,  
			   {{ "age", boost::any(52)}}, "Updated Actor");

		b2.notify(); // Inform txn-2 that update is done but not yet committed or aborted
	  b3.wait(); // wait until Txn-2 has accessed a dirty version

		gdb->abort_transaction();  // abort the update
  });

  t1.join();
  t2.join();

#ifdef USE_PMDK
  drop_graph_db(pop, gdb);
#endif
}

TEST_CASE("Test two concurrent Transactions trying to create nodes"  "[transaction]") {
/*
* If two concurrent write Transactions are triggered, then the end result must be that there are two seperate
* nodes created
*/
#ifdef USE_PMDK
  auto pop = prepare_pool();
  auto gdb = create_graph_db(pop);
#else
  auto gdb = create_graph_db();
#endif
  node::id_t nid1 = 0, nid2 = 0;

  /*
   * Thread #1: Add node
   */

  auto t1 = std::thread([&]() {
	    auto tx = gdb->begin_transaction();
	    nid1 = gdb->add_node("New Actor",
	                        {{"name", boost::any(std::string("Mark Wahlberg"))},
	                         {"age", boost::any(48)}});
	                         
	    gdb->commit_transaction();

  });

  /*
   * Thread #2: concurrently add node
   */

	  auto t2 = std::thread([&]() {
		    auto tx = gdb->begin_transaction();
		    nid2 = gdb->add_node("Actor",
		                        {{"name", boost::any(std::string("Peter"))},
		                         {"age", boost::any(22)}});
		    gdb->commit_transaction();

 });
  
//---------------------------------------------------------------

  t1.join();
  t2.join();
  
  //verify that two seperate nodes are created.
    {
    // check the node
    auto tx = gdb->begin_transaction();
    auto &n1 = gdb->node_by_id(nid1);
    auto nd1 = gdb->get_node_description(nid1);
    REQUIRE(nd1.label == "New Actor");
    REQUIRE(get_property<int>(nd1.properties, "age") == 48);
    
    
    auto &n2 = gdb->node_by_id(nid2);
    auto nd2 = gdb->get_node_description(nid2);
    REQUIRE(nd2.label == "Actor");
    REQUIRE(get_property<int>(nd2.properties, "age") == 22);  
    gdb->commit_transaction();
  }

#ifdef USE_PMDK
  drop_graph_db(pop, gdb);
#endif
}

/* ----------------------------------------------------------------------- */

TEST_CASE("Checking that a newly inserted node exist in the transaction",
          "[transaction]") {
#ifdef USE_PMDK
  auto pop = prepare_pool();
  auto gdb = create_graph_db(pop);
#else
  auto gdb = create_graph_db();
#endif
  auto tx = gdb->begin_transaction();
  auto nid = gdb->add_node("Movie", {});
  auto &my_node = gdb->node_by_id(nid);
  REQUIRE(my_node.id() == nid);
  gdb->commit_transaction();

#ifdef USE_PMDK
  drop_graph_db(pop, gdb);
#endif
}

/* ----------------------------------------------------------------------- */

TEST_CASE("Checking that a newly inserted relationship exist in the transaction",
          "[transaction]") {
#ifdef USE_PMDK
  auto pop = prepare_pool();
  auto gdb = create_graph_db(pop);
#else
  auto gdb = create_graph_db();
#endif
  auto tx = gdb->begin_transaction();
  auto m = gdb->add_node("Movie", {});
  auto a = gdb->add_node("Actor", {});
  auto rid = gdb->add_relationship(a, m, ":PLAYED_IN", {});
 
  auto &my_rship = gdb->rship_by_id(rid);
  REQUIRE(my_rship.id() == rid);
  gdb->commit_transaction();

#ifdef USE_PMDK
  drop_graph_db(pop, gdb);
#endif
}

/* ----------------------------------------------------------------------- */

TEST_CASE("Checking that a node update is undone after abort", "[transaction]") {
#ifdef USE_PMDK
  auto pop = prepare_pool();
  auto gdb = create_graph_db(pop);
#else
  auto gdb = create_graph_db();
#endif

  node::id_t nid = 0;
  {
    // create node
    auto tx = gdb->begin_transaction();
    nid = gdb->add_node("Actor",
                        {{"name", boost::any(std::string("Mark Wahlberg"))},
                         {"age", boost::any(48)}});
    gdb->commit_transaction();
  }

  {
    // update node
    auto tx = gdb->begin_transaction();
    auto &n = gdb->node_by_id(nid);
    gdb->update_node(n,
                     {{
                         "age",
                         boost::any(52),
                     }},
                     "Updated Actor");
    // but abort
    gdb->abort_transaction();
  }

  {
    // check the node
    auto tx = gdb->begin_transaction();
    auto &n = gdb->node_by_id(nid);
    auto nd = gdb->get_node_description(nid);
    REQUIRE(nd.label == "Actor");
    REQUIRE(get_property<int>(nd.properties, "age") == 48);
    gdb->abort_transaction();
  }

#ifdef USE_PMDK
  drop_graph_db(pop, gdb);
#endif
}

/* ----------------------------------------------------------------------- */

TEST_CASE("Checking that a relationship update is undone after abort", "[transaction]") {
#ifdef USE_PMDK
  auto pop = prepare_pool();
  auto gdb = create_graph_db(pop);
#else
  auto gdb = create_graph_db();
#endif

  relationship::id_t rid = 0;
  {
    // create node
    auto tx = gdb->begin_transaction();
    auto m = gdb->add_node("Movie", {});
    auto a = gdb->add_node("Actor", {});
    rid = gdb->add_relationship(a, m, ":PLAYED_IN", {{"role", boost::any(std::string("Killer"))}});

    gdb->commit_transaction();
  }

  {
    // update relationship
    auto tx = gdb->begin_transaction();
    auto &r = gdb->rship_by_id(rid);
    gdb->update_relationship(r,
                     {{
                         "role",
                         boost::any(std::string("Cop")),
                     }},
                     ":PLAYED_AS");
    // but abort
    gdb->abort_transaction();
  }

  {
    // check the node
    auto tx = gdb->begin_transaction();
    auto &r = gdb->rship_by_id(rid);
    auto rd = gdb->get_rship_description(rid);
    REQUIRE(rd.label == ":PLAYED_IN");
    REQUIRE(get_property<std::string>(rd.properties, "role") == "Killer");
    gdb->abort_transaction();
  }

#ifdef USE_PMDK
  drop_graph_db(pop, gdb);
#endif
}

/* ----------------------------------------------------------------------- */

TEST_CASE("Checking that a node insert is undone after abort", "[transaction]") {
#ifdef USE_PMDK
  auto pop = prepare_pool();
  auto gdb = create_graph_db(pop);
#else
  auto gdb = create_graph_db();
#endif

  node::id_t nid = 0;
  {
    auto tx = gdb->begin_transaction();
    nid = gdb->add_node("Movie", {});
    gdb->abort_transaction();
  }

  {
    auto tx = gdb->begin_transaction();
    REQUIRE_THROWS_AS(gdb->node_by_id(nid), unknown_id);
  }

#ifdef USE_PMDK
  drop_graph_db(pop, gdb);
#endif
}

/* ----------------------------------------------------------------------- */

TEST_CASE("Checking that a relationship insert is undone after abort", "[transaction]") {
#ifdef USE_PMDK
  auto pop = prepare_pool();
  auto gdb = create_graph_db(pop);
#else
  auto gdb = create_graph_db();
#endif

  node::id_t m, a;
  relationship::id_t rid;

  {
    auto tx = gdb->begin_transaction();
    m = gdb->add_node("Movie", {});
    a = gdb->add_node("Actor", {});
    gdb->commit_transaction();
  }

  { 
    auto tx = gdb->begin_transaction();
    rid = gdb->add_relationship(a, m, ":PLAYED_IN", {{"role", boost::any(std::string("Killer"))}});
    gdb->abort_transaction();
  }

  {
    // try to access via rid
    auto tx = gdb->begin_transaction();

    REQUIRE_THROWS_AS(gdb->rship_by_id(rid), unknown_id);

    // try to access via node
    auto& n = gdb->node_by_id(a);
    
    bool found = false;
    gdb->foreach_from_relationship_of_node(n, [&found](auto& rship) {
      found = true;
    });
    
    REQUIRE(!found);
    gdb->foreach_to_relationship_of_node(n, [&found](auto& rship) {
      found = true;
    });
    REQUIRE(!found);
  }

#ifdef USE_PMDK
  drop_graph_db(pop, gdb);
#endif
}

/* ----------------------------------------------------------------------- */

TEST_CASE("Checking that a newly inserted node is not visible in another "
          "transaction",
          "[transaction]") {
#ifdef USE_PMDK
  auto pop = prepare_pool();
  auto gdb = create_graph_db(pop);
#else
  auto gdb = create_graph_db();
#endif

  node::id_t nid = 0;
  barrier b1, b2;
  /*
   * Thread #1: create a new node
   */
  auto t1 = std::thread([&]() {
    auto tx = gdb->begin_transaction();
    nid = gdb->add_node("Actor", {});

    // inform thread #2 that we have created a new node
    b1.notify();
    // but wait until thread #2 had a chance to try to read it..
    b2.wait();

    // now we can commit
    gdb->commit_transaction();
  });

  /*
   * Thread #2: try to read this node (should fail)
   */
  auto t2 = std::thread([&]() {
    // wait for thread #1
    b1.wait();
    auto tx = gdb->begin_transaction();
    // std::cout << "-------------------1---------------\n";
    // gdb->dump();
    REQUIRE_THROWS_AS(gdb->node_by_id(nid), unknown_id);
    gdb->commit_transaction();

    // inform thread #1 that we are finished
    b2.notify();
  });

  t1.join();
  t2.join();

#ifdef USE_PMDK
  drop_graph_db(pop, gdb);
#endif
}

/* ----------------------------------------------------------------------- */

TEST_CASE("Checking that a newly inserted relationship is not visible in another "
          "transaction",
          "[transaction]") {
#ifdef USE_PMDK
  auto pop = prepare_pool();
  auto gdb = create_graph_db(pop);
#else
  auto gdb = create_graph_db();
#endif
  node::id_t m, a;
  {
    auto tx = gdb->begin_transaction();
    m = gdb->add_node("Movie", {});
    a = gdb->add_node("Actor", {});
    gdb->commit_transaction();
  }

  barrier b1, b2;
  relationship::id_t rid;

  /*
   * Thread #1: create a new relationship
   */
  auto t1 = std::thread([&]() {
    auto tx = gdb->begin_transaction();
    rid = gdb->add_relationship(a, m, ":PLAYED_IN", {{"role", boost::any(std::string("Killer"))}});

    // inform thread #2 that we have created a new node
    b1.notify();
    // but wait until thread #2 had a chance to try to read it..
    b2.wait();

    // now we can commit
    gdb->commit_transaction();
  });

  /*
   * Thread #2: try to read this relationship (should fail)
   */
  auto t2 = std::thread([&]() {
    // wait for thread #1
    b1.wait();
    auto tx = gdb->begin_transaction();
    // std::cout << "-------------------1---------------\n";
    // gdb->dump();
    REQUIRE_THROWS_AS(gdb->rship_by_id(rid), unknown_id);
    gdb->commit_transaction();

    // inform thread #1 that we are finished
    b2.notify();
  });

  t1.join();
  t2.join();

#ifdef USE_PMDK
  drop_graph_db(pop, gdb);
#endif
}
/* ----------------------------------------------------------------------- */

TEST_CASE("Checking that a newly inserted node becomes visible after commit",
          "[transaction]") {
#ifdef USE_PMDK
  auto pop = prepare_pool();
  auto gdb = create_graph_db(pop);
#else
  auto gdb = create_graph_db();
#endif
  node::id_t nid = 0;
  {
    auto tx = gdb->begin_transaction();
    nid = gdb->add_node("Movie", {});
    auto &my_node = gdb->node_by_id(nid);
    REQUIRE(my_node.id() == nid);
    gdb->commit_transaction();
  }

  REQUIRE_THROWS(check_tx_context());

  {
    auto tx = gdb->begin_transaction();
    auto &my_node = gdb->node_by_id(nid);
    auto descr = gdb->get_node_description(nid);
    REQUIRE(descr.id == nid);
    REQUIRE(descr.label == "Movie");
    gdb->commit_transaction();
  }

#ifdef USE_PMDK
  drop_graph_db(pop, gdb);
#endif
}

/* ----------------------------------------------------------------------- */

TEST_CASE("Checking that a newly inserted relationship becomes visible after commit",
          "[transaction]") {
#ifdef USE_PMDK
  auto pop = prepare_pool();
  auto gdb = create_graph_db(pop);
#else
  auto gdb = create_graph_db();
#endif
  node::id_t m, a;
  {
    auto tx = gdb->begin_transaction();
    m = gdb->add_node("Movie", {});
    a = gdb->add_node("Actor", {});
    gdb->commit_transaction();
  }

  relationship::id_t rid = 0;
  {
    auto tx = gdb->begin_transaction();
    rid = gdb->add_relationship(a, m, ":PLAYED_IN", {{"role", boost::any(std::string("Killer"))}});
    gdb->commit_transaction();
  }

  REQUIRE_THROWS(check_tx_context());

  {
    auto tx = gdb->begin_transaction();
    auto &my_rship = gdb->rship_by_id(rid);
    auto descr = gdb->get_rship_description(rid);
    REQUIRE(descr.id == rid);
    REQUIRE(descr.label == ":PLAYED_IN");
    gdb->commit_transaction();
  }

#ifdef USE_PMDK
  drop_graph_db(pop, gdb);
#endif
}

/* ----------------------------------------------------------------------- */
TEST_CASE("Checking that a read transaction reads the correct version of a "
          "updated node",
          "[transaction]") {
#ifdef USE_PMDK
  auto pop = prepare_pool();
  auto gdb = create_graph_db(pop);
#else
  auto gdb = create_graph_db();
#endif
  node::id_t nid = 0;
  barrier b1, b2;

  /**
   *  First, we create a new node.
   */
  {
    auto tx = gdb->begin_transaction();
    nid = gdb->add_node("Actor",
                        {{"name", boost::any(std::string("Mark Wahlberg"))},
                         {"age", boost::any(48)}});
    gdb->commit_transaction();
  }
  /*
   * Thread #1: start a transaction to read the object but wait for the other
   * transaction.
   */
  auto t1 = std::thread([&]() {
    auto tx = gdb->begin_transaction();

    // inform thread #2 that we have started the transaction
    b1.notify();

    // wait until thread #2 has committed.
    b2.wait();

    auto &n = gdb->node_by_id(nid);
    // spdlog::info("read node @{}", (unsigned long)&n);
    auto nd = gdb->get_node_description(nid);
    REQUIRE(nd.label == "Actor");
    REQUIRE(get_property<int>(nd.properties, "age") == 48);

    gdb->commit_transaction();
  });

  /*
   * Thread #2: for updating
   */
  auto t2 = std::thread([&]() {
    // wait for thread #1
    b1.wait();

    auto tx = gdb->begin_transaction(); // wait until thread #1 starts

    // update the node
    auto &n = gdb->node_by_id(nid);
    gdb->update_node(
        n,
        {{"name", boost::any(std::string("Mark Wahlberg updated"))},
         {
             "age",
             boost::any(49),
         }},
        "Updated Actor");

    // and commit
    gdb->commit_transaction();

    // inform thread #1 that we are finished
    b2.notify();
  });

  t1.join();
  t2.join();

  {
    auto tx = gdb->begin_transaction();
    auto &n = gdb->node_by_id(nid);
    auto nd = gdb->get_node_description(nid);
    REQUIRE(nd.label == "Updated Actor");
    REQUIRE(get_property<int>(nd.properties, "age") == 49);
    REQUIRE(get_property<std::string>(nd.properties, "name") ==
            "Mark Wahlberg updated");

    gdb->commit_transaction();
    // gdb->dump();
  }
#ifdef USE_PMDK
  drop_graph_db(pop, gdb);
#endif
}

/* ----------------------------------------------------------------------- */

TEST_CASE("Checking that a update transaction is aborted if the object is "
          "already locked by another transaction",
          "[transaction]") {
#ifdef USE_PMDK
  auto pop = prepare_pool();
  auto gdb = create_graph_db(pop);
#else
  auto gdb = create_graph_db();
#endif
barrier b1, b2, b3;
  // 1. create a new node
  node::id_t nid = 0;
	{
		auto tx = gdb->begin_transaction();
		// spdlog::info("BOT #1: {}", short_ts(tx->xid()));
    nid = gdb->add_node("Actor",
                        {{"name", boost::any(std::string("Mark Wahlberg"))},
                         {"age", boost::any(48)}});
   //  spdlog::info("updated #1");
		gdb->commit_transaction();
	}
  // 2. start a transaction to update
    auto t1 = std::thread([&]() {
		b1.wait();
		auto tx = gdb->begin_transaction();
		// spdlog::info("BOT #2: {}", short_ts(tx->xid()));
		// perform update
    // spdlog::info("node_by_id #2");
   		auto &n = gdb->node_by_id(nid);
    // spdlog::info("update_node #2");
    	gdb->update_node(n,
                     {{
                         "age",
                         boost::any(52),
                     }},
                     "Updated Actor");
		b2.notify();
		// but don't commit yet - let's make sure that the other transaction tries 
		// to update, too
		b3.wait();
		gdb->commit_transaction();
	});
  // 3. start a concurrent transaction which tries to read the same node
    auto t2 = std::thread([&]() {
		b1.notify();
		auto tx = gdb->begin_transaction();
		b2.wait();

   		auto &n = gdb->node_by_id(nid);
		// try to update: should fail
    	REQUIRE_THROWS_AS(gdb->update_node(n,
                     {{
                         "age",
                         boost::any(55),
                     }}), transaction_abort);
		b3.notify();
		gdb->abort_transaction();
	});

	t1.join();
	t2.join();

#ifdef USE_PMDK
  drop_graph_db(pop, gdb);
#endif
}

/* ----------------------------------------------------------------------- */

TEST_CASE("Checking basic transaction level GC", "[transaction][gc]") {
#ifdef USE_PMDK
  auto pop = prepare_pool();
  auto gdb = create_graph_db(pop);
#else
  auto gdb = create_graph_db();
#endif
  node::id_t nid = 0;
	{
  // create a new node
		auto tx = gdb->begin_transaction();
    	nid = gdb->add_node("Actor",
                        {{"name", boost::any(std::string("Mark Wahlberg"))},
                         {"age", boost::any(48)}});

		gdb->commit_transaction();
	}
	{
		// try to read it - the dirty list should be empty
		auto tx = gdb->begin_transaction();
		auto &n = gdb->node_by_id(nid);
		REQUIRE(n.get_dirty_objects().has_value() == false);
		gdb->abort_transaction();
	}

#ifdef USE_PMDK
  drop_graph_db(pop, gdb);
#endif
}

/* ----------------------------------------------------------------------- */

TEST_CASE("Checking GC for concurrent transactions", "[transaction][gc]") {
#ifdef USE_PMDK
  auto pop = prepare_pool();
  auto gdb = create_graph_db(pop);
#else
  auto gdb = create_graph_db();
#endif
	barrier b1, b2, b3, b4;
  node::id_t nid = 0;
	{
  // create a new node
		auto tx = gdb->begin_transaction();
    	nid = gdb->add_node("Actor",
                        {{"name", boost::any(std::string("Mark Wahlberg"))},
                         {"age", boost::any(48)}});

		gdb->commit_transaction();
	}

		
	auto t1 = std::thread([&]() {
    // wait for start of tx #2 to ensure that t1 is newer
    b1.wait();
		auto tx = gdb->begin_transaction();
		// spdlog::info("BOT #1: {}", short_ts(tx->xid()));

		// perform update
    // spdlog::info("read #1");
		auto &n = gdb->node_by_id(nid);
    // spdlog::info("update #1");
		gdb->update_node(n,
                     {{
                         "age",
                         boost::any(55),
                     }}, "Updated Actor");
    // inform tx #2 that we have updated the object
		b2.notify();
		// gdb->dump();
    b3.wait();
		gdb->commit_transaction();
		// spdlog::info("COMMIT #1");

		REQUIRE(n.get_dirty_objects().has_value());
	  b4.notify();
	});

	auto t2 = std::thread([&]() {
		auto tx = gdb->begin_transaction();
		// spdlog::info("BOT #2 {}", short_ts(tx->xid()));

    b1.notify();
		// read
    // spdlog::info("read #2.1");
    // gdb->dump();
		auto &n = gdb->node_by_id(nid);

   	auto nd = gdb->get_node_description(nid);
    REQUIRE(nd.label == "Actor");
    REQUIRE(get_property<int>(nd.properties, "age") == 48);
    // spdlog::info("read #2.1 done {}", n.is_dirty());
 
    b2.wait();
		// make sure we still can read our version after the update of tx #1
    // spdlog::info("read #2.2 {}", n.is_dirty());
   	auto nd2 = gdb->get_node_description(nid);
    REQUIRE(nd2.label == "Actor");
    REQUIRE(get_property<int>(nd2.properties, "age") == 48);
    b3.notify();

    b4.wait();
		// make sure we still can read our version after the commit of tx #1
    // spdlog::info("read #2.3 {}", n.is_dirty());
    // gdb->dump();
   	auto nd3 = gdb->get_node_description(nid);
    REQUIRE(get_property<int>(nd3.properties, "age") == 48);
    REQUIRE(nd3.label == "Actor");

		gdb->commit_transaction();
		// spdlog::info("COMMIT #2");
	});

	t1.join();
	t2.join();

	{
		auto tx = gdb->begin_transaction();
		auto &n = gdb->node_by_id(nid);

		// the dirty_list should still be empty now
    if (n.get_dirty_objects().has_value())
      gdb->dump();
		// REQUIRE(n.get_dirty_objects().has_value() == false);
		gdb->abort_transaction();

	}
#ifdef USE_PMDK
  drop_graph_db(pop, gdb);
#endif
}

/* ----------------------------------------------------------------------- */

TEST_CASE("Checking that deleting a node works", "[transaction]") {
#ifdef USE_PMDK
  auto pop = prepare_pool();
  auto gdb = create_graph_db(pop);
#else
  auto gdb = create_graph_db();
#endif
  node::id_t nid;
  {
    // add a few nodes
    auto tx = gdb->begin_transaction();
      gdb->add_node(":Person", {{"name", boost::any(std::string("John"))},
                                  {"age", boost::any(42)}});
     nid = gdb->add_node(":Person", {{"name", boost::any(std::string("Ann"))},
                                  {"age", boost::any(36)}});
      gdb->add_node(":Person", {{"name", boost::any(std::string("Pete"))},
                                  {"age", boost::any(58)}});

    gdb->commit_transaction();
  }
  {
    // delete the node
    auto tx = gdb->begin_transaction();
    gdb->delete_node(nid);
    gdb->commit_transaction();
  }

  {
    // check that the node doesn't exist anymore
    auto tx = gdb->begin_transaction();
    REQUIRE_THROWS_AS(gdb->node_by_id(nid), unknown_id);
    gdb->abort_transaction();
  }
#ifdef USE_PMDK
  drop_graph_db(pop, gdb);
#endif
}

/* ----------------------------------------------------------------------- */

TEST_CASE("Checking that deleting a node works also within a transaction", "[transaction]") {
#ifdef USE_PMDK
  auto pop = prepare_pool();
  auto gdb = create_graph_db(pop);
#else
  auto gdb = create_graph_db();
#endif
  node::id_t nid;
  {
    // add a few nodes
    auto tx = gdb->begin_transaction();
    gdb->add_node(":Person", {{"name", boost::any(std::string("John"))},
                                  {"age", boost::any(42)}});
    nid =
      gdb->add_node(":Person", {{"name", boost::any(std::string("Ann"))},
                                  {"age", boost::any(36)}});
    gdb->add_node(":Person", {{"name", boost::any(std::string("Pete"))},
                                  {"age", boost::any(58)}});

    gdb->commit_transaction();
  }
  {
    // delete the node
    auto tx = gdb->begin_transaction();
    gdb->delete_node(nid);
    REQUIRE_THROWS_AS(gdb->node_by_id(nid), unknown_id);
    gdb->commit_transaction();
  }

#ifdef USE_PMDK
  drop_graph_db(pop, gdb);
#endif
}

/* ----------------------------------------------------------------------- */

TEST_CASE("Checking that aborting a delete transaction works", "[transaction]") {
#ifdef USE_PMDK
  auto pop = prepare_pool();
  auto gdb = create_graph_db(pop);
#else
  auto gdb = create_graph_db();
#endif
  node::id_t nid;
  {
    auto tx = gdb->begin_transaction();
    gdb->add_node(":Person", {{"name", boost::any(std::string("John"))},
                                  {"age", boost::any(42)}});
    nid =
      gdb->add_node(":Person", {{"name", boost::any(std::string("Ann"))},
                                  {"age", boost::any(36)}});
    gdb->commit_transaction();
  }
  {
    auto tx = gdb->begin_transaction();
    gdb->delete_node(nid);
    gdb->abort_transaction();
  }
  {
    auto tx = gdb->begin_transaction();
    REQUIRE_NOTHROW(gdb->node_by_id(nid));
    gdb->abort_transaction();
  }
  
#ifdef USE_PMDK
  drop_graph_db(pop, gdb);
#endif
}

/* ----------------------------------------------------------------------- */

TEST_CASE("Checking that a delete transaction does not interfer with another transaction", "[transaction]") {
#ifdef USE_PMDK
  auto pop = prepare_pool();
  auto gdb = create_graph_db(pop);
#else
  auto gdb = create_graph_db();
#endif

  barrier b1, b2, b3;
  node::id_t nid;

 {
    auto tx = gdb->begin_transaction();
    gdb->add_node(":Person", {{"name", boost::any(std::string("John"))},
                                  {"age", boost::any(42)}});
    nid =
      gdb->add_node(":Person", {{"name", boost::any(std::string("Ann"))},
                                  {"age", boost::any(36)}});
    gdb->commit_transaction();
  }

  // we create a thread that tries to read "Ann"
  auto t1 = std::thread([&]() {
    auto tx = gdb->begin_transaction();

    // inform tx #2 that we have started the transaction
    b1.notify();

    // wait until tx #2 has deleted the node.
    b2.wait();

    auto &n = gdb->node_by_id(nid);
    // spdlog::info("read node @{}", (unsigned long)&n);
    auto nd = gdb->get_node_description(nid);
    REQUIRE(nd.label == ":Person");
    REQUIRE(get_property<int>(nd.properties, "age") == 36);

    // now, tx #2 can commit
    b3.notify();
    gdb->commit_transaction();
  });

 // we create another thread that tries to delete "Ann"
  auto t2 = std::thread([&]() {
    // make sure that tx #1 has already started 
    b1.wait();

    auto tx = gdb->begin_transaction();
    gdb->delete_node(nid);
    
    // inform tx #1 that we have deleted the node
    b2.notify();

    // wait until tx #1 has read the node
    b3.wait();
    gdb->commit_transaction();
  });
  t1.join();
  t2.join();

#ifdef USE_PMDK
  drop_graph_db(pop, gdb);
#endif
}

TEST_CASE("Checking two concurrent transactions trying to create node", "[transaction]") {
#ifdef USE_PMDK
  auto pop = prepare_pool();
  auto gdb = create_graph_db(pop);
#else
  auto gdb = create_graph_db();
#endif
  node::id_t nid1 = 0, nid2 = 0;

  // Thread 1: Add node
  auto t1 = std::thread([&]() {
    auto tx = gdb->begin_transaction();

    nid1 = gdb->add_node("New Actor",
                        {{"name", boost::any(std::string("Mark Wahlberg"))},
                         {"age", boost::any(48)}});

    gdb->commit_transaction();
  });
  //  Thread 2: concurrently add node
  auto t2 = std::thread([&]() {
    auto tx = gdb->begin_transaction();

	  nid2 = gdb->add_node("Actor",
	                        {{"name", boost::any(std::string("Peter"))},
	                         {"age", boost::any(22)}});

	  gdb->commit_transaction();
  });
  t1.join();
  t2.join();

  REQUIRE(gdb->get_nodes()->num_chunks() == 1);
  
  gdb->dump();
#ifdef USE_PMDK
  drop_graph_db(pop, gdb);
#endif
} 
/* -------------------------------------------------------------------------------- */

#if TEST_INCORRECT

TEST_CASE("Checking the transaction level GC basic functionality",
          "[transaction][gc]") {
  spdlog::info("TEST #6");

#ifdef USE_PMDK
  auto pop = prepare_pool();
  auto gdb = create_graph_db(pop);
#else
  auto gdb = create_graph_db();
#endif
  std::mutex m;
  std::condition_variable cond_var1, cond_var2, cond_var3, cond_var4;
  std::atomic<bool> ready1{false}, ready2{false}, ready3{false}, ready4{false};

  node::id_t nid = 0;

  /** Initially, we create a new node.
   */
  {
    auto tx = gdb->begin_transaction();
    nid = gdb->add_node("Actor",
                        {{"name", boost::any(std::string("Mark Wahlberg"))},
                         {"salary", boost::any(300)}});
    gdb->commit_transaction();
  }
  /*
   * Thread #1: start a transaction to write transaction.
   */
  auto t1 = std::thread([&]() {
    auto tx = gdb->begin_transaction();
    {
      // Notify thread #2, that it can start.
      std::lock_guard<std::mutex> lock(m);
      ready2 = true;
      cond_var2.notify_one();
    }
    // Perform an update
    auto &n = gdb->node_by_id(nid);
    gdb->update_node(n, {{"salary", boost::any(300)}});
    gdb->commit_transaction();

    // Dirty list should not exist
    REQUIRE(n.get_dirty_objects().has_value() == false);
    {
      // Notify that thread #1  is committed.
      std::lock_guard<std::mutex> lock(m);
      ready1 = true;
      cond_var1.notify_one();
    }
  });

  /*
   * Thread #2: Start a transaction to read
   */
  auto t2 = std::thread([&]() {
    {
      // wait for  Transaction-1  to commit
      std::unique_lock<std::mutex> lock(m);
      cond_var1.wait(lock, [&] { return ready1.load(); });
    }
    auto tx = gdb->begin_transaction();
    {
      // Notify thread #3  to start
      std::lock_guard<std::mutex> lock(m);
      ready3 = true;
      cond_var3.notify_one();
    }
    { // Read after the updated values, should still read old dirty value.
      auto &n = gdb->node_by_id(nid);
      auto nd = gdb->get_node_description(n);
      REQUIRE(get_property<int>(nd.properties, "salary") == 300);
    }
    {
      // wait for thread#3 to commit
      std::unique_lock<std::mutex> lock(m);
      cond_var4.wait(lock, [&] { return ready4.load(); });
    }
    {
      const auto &n = gdb->node_by_id(nid);
      REQUIRE(n.get_dirty_objects().has_value() == true);

      // There must be one dirty object in dirty list
      REQUIRE(n.get_dirty_objects().value()->size() == 1);
      auto nd = gdb->get_node_description(
          n.get_dirty_objects().value()->front()->elem_);
      // The dirty object in dirty list is still the same old value.
      REQUIRE(get_property<int>(nd.properties, "salary") == 300);
    }
    gdb->commit_transaction();
  });
  /*
   * Thread #3: start a transaction to write transaction.
   */
  auto t3 = std::thread([&]() {
    {
      // wait for t3 start signal
      std::unique_lock<std::mutex> lock(m);
      cond_var3.wait(lock, [&] { return ready3.load(); });
    }
    auto tx = gdb->begin_transaction();
    auto &n = gdb->node_by_id(nid);

    // update
    gdb->update_node(n, {{"salary", boost::any(400)}});
    gdb->commit_transaction();

    // Dirty list should still exist after transaction #2 committed
    REQUIRE(n.get_dirty_objects().has_value() == true);
    // There must be one dirty object in dirty list
    REQUIRE(n.get_dirty_objects().value()->size() == 1);
    {
      // inform thread #3  committed
      std::lock_guard<std::mutex> lock(m);
      ready4 = true;
      cond_var4.notify_all();
    }
  });
  /*
   * Thread #4: start a transaction to read the object.
   */
  auto t4 = std::thread([&]() {
    {
      // wait for t3 to commit
      std::unique_lock<std::mutex> lock(m);
      cond_var4.wait(lock, [&] { return ready4.load(); });
    }

    // Read transaction
    auto tx = gdb->begin_transaction();
    {
      const auto &n = gdb->node_by_id(nid);
      auto nd = gdb->get_node_description(n);
      // Read the committed value
      REQUIRE(get_property<int>(nd.properties, "salary") == 400);
      // Dirty list should still exist
      REQUIRE(n.get_dirty_objects().has_value() == true);
      // There must be one dirty object in dirty list
      REQUIRE(n.get_dirty_objects().value()->size() == 1);
      // The property: salary on main table should have new updated value 400,
      // while property: salary in dirty list should still contain salary as 300
      const auto &dn_ptr = n.get_dirty_objects().value()->front();
      auto props = gdb->get_properties()->build_properties_from_pitems(
          dn_ptr->properties_, gdb->get_dictionary());
      auto dn_nd =
          node_description{dn_ptr->elem_.id(),
                           std::string(gdb->get_dictionary()->lookup_code(
                               dn_ptr->elem_.node_label)),
                           props};
      // New salary:400 on main table, old salary:300 on dirty list
      REQUIRE(get_property<int>(nd.properties, "salary") !=
              get_property<int>(dn_nd.properties, "salary"));
    }
    gdb->commit_transaction();
  });

  t1.join();
  t2.join();
  t3.join();
  t4.join();

#ifdef USE_PMDK
  drop_graph_db(pop, gdb);
#endif
}

/* ---------------------------------------------------------------------------
 */

TEST_CASE("Checking the Garbage Collector functionality: Maintain multiple "
          "dirty version"
          "updated node",
          "[transaction][gc]") {
  spdlog::info("TEST #7");

#ifdef USE_PMDK
  auto pop = prepare_pool();
  graph_db_ptr gdb;
  nvm::transaction::run(pop, [&] { gdb = p_make_ptr<graph_db>(); });
#else
  auto gdb = p_make_ptr<graph_db>();
#endif
  std::mutex m;
  std::condition_variable cond_var1, cond_var2, cond_var3, cond_var4, cond_var5;
  std::atomic<bool> ready1{false}, ready2{false}, ready3{false}, ready4{false};

  node::id_t nid = 0;

  /**
   *  Initially, we create a new node.
   */
  {
    auto tx = gdb->begin_transaction();
    nid = gdb->add_node("Director", {{"name", boost::any(std::string("John"))},
                                     {"salary", boost::any(1000)}});
    gdb->commit_transaction();
  }

  /**
   *  Thread#1  will start a transaction to write
   */
  auto t1 = std::thread([&]() {
    auto tx = gdb->begin_transaction();
    {
      std::cout << "T1.1" << std::endl;
      // Notify thread #2, to start.
      std::lock_guard<std::mutex> lock(m);
      ready2 = true;
      cond_var2.notify_one();
    }
    {
      // wait for transaction#2 to read
      std::unique_lock<std::mutex> lock(m);
      cond_var4.wait(lock, [&] { return ready3.load(); });
    }

    // Then do an update. Ideally, this should result in T2 abort.
    // TODO : Yet to be implemented
    std::cout << "T1.3" << std::endl;
    auto &n = gdb->node_by_id(nid);
    gdb->update_node(n, {{"salary", boost::any(2000)}});

    gdb->commit_transaction();
  });

  /*
   * Thread #2: Start a transaction to read.
   */
  auto t2 = std::thread([&]() {
    auto tx = gdb->begin_transaction();
    {
      std::cout << "T2.1" << std::endl;
      auto &n = gdb->node_by_id(nid);
      auto nd = gdb->get_node_description(n);
      // Should read the old value
      REQUIRE(get_property<int>(nd.properties, "salary") == 1000);
    }

    // Notify thread#1 that transaction#2 has read.
    {
      std::lock_guard<std::mutex> lock(m);
      ready3 = true;
      cond_var4.notify_one();
    }
    {
      // wait for transaction-5 to commit
      std::unique_lock<std::mutex> lock(m);
      cond_var5.wait(lock, [&] { return ready4.load(); });
    }
    gdb->commit_transaction();
    std::cout << "T2.2" << std::endl;
  });

  t1.join();

  {
    std::cout << "T3.1" << std::endl;
    auto tx = gdb->begin_transaction();
    auto &n = gdb->node_by_id(nid);
    // update transaction#3
    gdb->update_node(n, {{"salary", boost::any(3000)}});
    gdb->commit_transaction();

    // Since transaction#2 is still active, there must be an object in dirty
    // list
    REQUIRE(n.get_dirty_objects().has_value() == true);
  }

  {
    std::cout << "T4.1" << std::endl;
    auto tx = gdb->begin_transaction();
    auto &n = gdb->node_by_id(nid);
    // Update Transaction-4.
    gdb->update_node(n, {{"salary", boost::any(4000)}});
    gdb->commit_transaction();

    // Since transaction#2 is still active, there must be  objects in dirty
    // list. The GC cannot empty the list
    REQUIRE(n.get_dirty_objects().has_value() == true);
  }

  {
    std::cout << "T5.1" << std::endl;
    auto tx = gdb->begin_transaction();
    auto &n = gdb->node_by_id(nid);

    // Update Transaction-5.
    gdb->update_node(n, {{"salary", boost::any(5000)}});
    gdb->commit_transaction();

    // Since transaction#2 is still active, the dirty version keeps accumulating
    REQUIRE(n.get_dirty_objects().has_value() == true);
    {
      // Notify transaction #5  committed
      std::lock_guard<std::mutex> lock(m);
      ready4 = true;
      cond_var5.notify_one();
    }
  }

  t2.join();

  // Now a new update, should clear the dirty list
  auto tx = gdb->begin_transaction();
  // Then do an upate
  auto &n = gdb->node_by_id(nid);

  // Update Transaction-6.
  gdb->update_node(n, {{"salary", boost::any(6000)}});
  // just before commit, we should see all versions in dirty list
  REQUIRE(n.get_dirty_objects().has_value() == true);
  // The top most element must be the latest updated value
  const auto &dn_ptr = n.get_dirty_objects().value()->front();
  auto props = gdb->get_properties()->build_properties_from_pitems(
      dn_ptr->properties_, gdb->get_dictionary());
  auto dn_nd = node_description{
      dn_ptr->elem_.id(),
      std::string(gdb->get_dictionary()->lookup_code(dn_ptr->elem_.node_label)),
      props};
  // check if the top most element must be the latest updated value i.e. 6000
  REQUIRE(get_property<int>(dn_nd.properties, "salary") == 6000);
  // commit update
  gdb->commit_transaction();
  // After commit, the dirty list must be cleared and removed by the Garbage
  // collector
  REQUIRE(n.get_dirty_objects().has_value() == false);

#ifdef USE_PMDK
  nvm::transaction::run(pop, [&] { nvm::delete_persistent<graph_db>(gdb); });
  pop.close();
  remove(test_path.c_str());
#endif
}

#endif
