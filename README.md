# Poseidon Graph Database

[![pipeline status](https://dbgit.prakinf.tu-ilmenau.de/code/poseidon_core/badges/master/pipeline.svg)](https://dbgit.prakinf.tu-ilmenau.de/code/poseidon_core/commits/master)
[![coverage report](https://dbgit.prakinf.tu-ilmenau.de/code/poseidon_core/badges/master/coverage.svg?job=coverage)](https://dbgit.prakinf.tu-ilmenau.de/code/poseidon_core/commits/master)

Poseidon is a graph database system for persistent memory. It uses persistent memory to store the graph data, i.e. the data is not copied between disk and memory. The data model used in Poseidon is the property graph model where nodes and relationships have labels (type names) and properties (key-value pairs). This module `poseidon_core` provides the implementation for the storage and transaction manager as well as the query execution engine.

## Installation

Simply, clone the repository, create a build directory and run the build tools `cmake` and `make`:

```bash
mkdir build; cd build
cmake ..
make

```

Make sure you have the Intel PMDK installed. If PMDK is not installed, Poseidon runs as in-memory database only.

We also provide a Dockerfile to build your own image with the poseidon_core source. Just build the image in the root directory of the source tree:

```
docker build --tag poseidon .
```

Then, you can run it:

```
docker run -it poseidon
```

## Using Poseidon Core

Poseidon is implemented as a C++ library `libposeidon_core` which can be used to implement applications for accessing the graph data stored in persistent memory and executing queries such as Poseidon CLI provided in a separate module.

In order to use a persistent graph we have to create a persistent memory pool and a graph first:

```c++
auto pool = graph_pool::create(pool_path);
auto graph = pool->create_graph("my_graph");
```

Note that the pool path must point to the memory mapped file representing the persistent memory which is usually something like `/mnt/pmem0/...`. Once you have created a graph you can later just open it:

```c++
auto pool = graph_pool::open(pool_path);
auto graph = pool->open_graph("my_graph");
 
```

Using a graph object we can add nodes and relationships. All these operations have to performed in the context of a transaction:

```c++
auto tx = graph->begin_transaction();
auto p1 = graph->add_node(":Person",
                              {{"name", boost::any(std::string("John Doe"))},
                               {"age", boost::any(42)}});
auto p2 = graph->add_node(":Person",
                              {{"name", boost::any(std::string("Sarah Jones"))},
                               {"age", boost::any(38)}});
graph->add_relationship(p1, p2, ":KNOWS", {});
graph->commit_transaction();
```

There is an alternative way to implement transactions:

```c++
graph->run_transaction([&]() {
  auto p1 = graph->add_node(":Person",
                              {{"name", boost::any(std::string("John Doe"))},
                               {"age", boost::any(42)}});
  auto p2 = graph->add_node(":Person",
                              {{"name", boost::any(std::string("Sarah Jones"))},
                               {"age", boost::any(38)}});
  graph->add_relationship(p1, p2, ":KNOWS", {});
  return true; // commit the transaction
});
```

As shown in the example, properties are passed as `boost::any` types. However, internally property values are always typed. Thus, make sure that you always use the correct type when you create the `any` values.
 
## Querying Poseidon database

Poseidon provides a simple algebra-like query language which allows to formulate query plans. Note, that the query plans are executed as specified, i.e., no query optimization is performed. A query looks like 

```
Project([ $1.Name, $1.ID ], 
   Expand(OUT, "Post", 
      ForeachRelationship(FROM, ':IsLocatedIn', 
         NodeScan("Person"))))
```

Such queries can be either passed to the `queryc` class which accepts a query string, compiles it to JIT code, and executes the query. In addition, the `qlc` program can be used to enter and execute queries interactively.

Currently, the following query operators are supported:

Operator | Parameter | Description
---------| ----------|------------
NodeScan | NodeType | Scans the node table and returns all nodes of the given type.
IndexScan | NodeType, selection predicate | Performs an index lookup and returns all nodes of the given type that satisfy the predicate condition 
Filter | filter expression, input | Processes the input list of nodes and rships and returns all tuples satisfying the given condition.
Expand | IN or OUT, NodeType, input | Gets all the source or destination nodes of the given type
Project | [ projection list ], input | Projects query results based on the given projection list
Limit | number of tuples, input | Limits the input list to the given number of tuples
ForeachRelationship | TO or FROM, RelationshipType, input | Traverses all incoming or outgoing relationships of the given type
Sort |  sort function, input | Orders tuples according to the sorting function
Group | [ GroupKey list ], input | Groups all tuples based on grouping key(s)
Aggregate | [ AggregateType list ], input | Applies aggregate function(s) and appends the output to tuple
AppendToTuple | result function, input | Computes a query result and appends it to tuple
Union | [ query list ], input | Combines the tuples of multiple queries
AlgoShortestPath | SPATHTYPE, relationship predicate, weight function, input | Performs a uni/bi-directional search for the weighted/unweighted or top-k shortest paths between node pairs and appends the paths and/or their weights to tuple
Create | (n:NodeType { key: val, ...} ), input | Creates a new node from the literals or the input
Create | ($1)-[r:RelationshipType { key: val, ...} ]->($2), input | Creates a new relationship from the literals or the input



Query plans can be also directly implemented in C++ by using the `query` class. This class provides methods to construct a query plan from a set of separate operators. Poseidon provides a push-based query engine, i.e. the query plan starts with scans. The following example shows an implementation of LDBC interactive short query 1:

```c++
namespace pj = builtin;

auto q = query(gdb)
          .nodes_where("Person", "id", [&](auto &p) { return p.equal(933); })
          .from_relationships(":isLocatedIn")
          .to_node("Place")
          .project(
              {PExpr_(0, pj::string_property(res, "firstName")),
               PExpr_(0, pj::string_property(res, "lastName")),
               PExpr_(0, pj::pr_date(res, "birthday")),
               PExpr_(0, pj::string_property(res, "locationIP")),
               PExpr_(0, pj::string_property(res, "browserUsed")),
               PExpr_(2, pj::uint64_property(res, "id")),
               PExpr_(0, pj::string_property(res, "gender")),
               PExpr_(0, pj::ptime_property(res, "creationDate")) })
          .print();
  q.start();
```

### Poseidon Python API

In addition, there is a thin wrapper library for providing a Python API. This API allows creating and opening graph databases, importing CVS data, and specifying query execution plans. The latter reflects the C++ query class (see above).

In the following Python example the database `imdb` is created by loading the IMDB data from three files.

```python
import poseidon

# create a new graph database
graph = poseidon.graph_db()
graph.create("imdb")

# we need a mapping table for mapping logical node ids to ids used for
# creating relationships
m = graph.mapping()

# import nodes and relationships for the IMDB database
n = graph.import_nodes("Movies", "imdb-data/movies.csv", m)
print(n, "movies imported.")
n = graph.import_nodes("Actor", "imdb-data/actors.csv", m)
print(n, "actors imported.")
n = graph.import_relationships("imdb-data/roles.csv", m)
print(n, "roles imported.")
```

After creating the graph database, it can be queried:

```python
q = poseidon.query(graph) \
    .all_nodes("Movies") \
    .print()

q.start()
```

## Storage structure

Poseidon stores nodes and relationships in separate persistent vectors where each vector is implemented as a chunked vector, i.e. a linked list of array of fixed size. Furthermore, properties are stored separately in a third persistent vector. Whereas for nodes and relationships each object is represented by its own record, properties belonging to the same node or relationship are stored in batches of five properties per record. Note, that strings a compressed via dictionary compression and neither stored directly in nodes, relationships, or properties.

![Storage structure](docs/poseidon.png)

In addition to these plain table-based storage, Poseidon also supports indexing using B+trees. These B+trees are also stored in PMem and can be built per node label and property. As an example the following C++ statement builds an index on the `id` property for all nodes with the label `Person`:

```
gdb->create_index("Person", "id");
```

Indexes can be utilized in query processing via a special index lookup/scan operator `nodes_where_indexed`. The following example illustrates this: 

```
  auto q = query(gdb)
              .nodes_where_indexed("Person", "id", 933)
              ...
```
  
## Transaction Processing

For transaction processing Poseidon implements a multiversion timestamp ordering (MVTO) protocol. Here, the most recent committed version is always kept in persistent memory while dirty versions (nodes/relationships which are currently inserted or updated) as well as outdated versions are stored in a dirty list in volatile memory.

![MVTO data structures](docs/mvto.png)
