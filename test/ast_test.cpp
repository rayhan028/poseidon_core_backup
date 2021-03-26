#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do
                          // this in one cpp file

#include <iostream>
#include "catch.hpp"
#include "queryc.hpp"
#include "ast.hpp"

TEST_CASE("Constructing an AST from a query string", "[qlang]") {
    queryc qc;

    SECTION("simple scan operator") {
        auto ast = qc.parse("NodeScan('Person')");
        std::ostringstream os;
        ast_to_stream(ast, os);
        REQUIRE(os.str() == "NodeScan('Person' )\n");
    }
    SECTION("projection + scan") {
        auto ast = qc.parse("Project([$0.firstName:string, $0.lastName:string], NodeScan('Person'))");
        std::ostringstream os;
        ast_to_stream(ast, os);
        REQUIRE(os.str() == "Project([ $0.firstName, $0.lastName ] )\n└── NodeScan('Person' )\n");
    }

    SECTION("filter") {
        auto ast = qc.parse("Filter($0.id == 42)");
        std::ostringstream os;
        ast_to_stream(ast, os);
        REQUIRE(os.str() == "Filter($0.id==42 )\n");
    }

    SECTION("filter, projection") {
        auto ast = qc.parse("Project([$0.firstName:string, $0.lastName:string], Filter($0.id == 42))");
        std::ostringstream os;
        ast_to_stream(ast, os);
        REQUIRE(os.str() == "Project([ $0.firstName, $0.lastName ] )\n└── Filter($0.id==42 )\n");
    }

    SECTION("filter, projection + scan") {
        auto ast = qc.parse("Project([$0.firstName:string, $0.lastName:string], Filter($0.id == 42, NodeScan('Person')))");
        std::ostringstream os;
        ast_to_stream(ast, os);
        REQUIRE(os.str() == "Project([ $0.firstName, $0.lastName ] )\n└── Filter($0.id==42 )\n    └── NodeScan('Person' )\n");
    }

    SECTION("filter + scan") {
        auto ast = qc.parse("Filter($0.id == 42, NodeScan('Person'))");
        std::ostringstream os;
        ast_to_stream(ast, os);
        REQUIRE(os.str() == "Filter($0.id==42 )\n└── NodeScan('Person' )\n");
    }

    SECTION("expand + foreach") {
        auto ast = qc.parse("Expand(OUT, 'Place', ForeachRelationship(FROM, ':isLocatedIn', NodeScan('Person')))");
        std::ostringstream os;
        ast_to_stream(ast, os);
        REQUIRE(os.str() == "Expand(OUT 'Place' )\n└── ForeachRelationship(FROM ':isLocatedIn' )\n    └── NodeScan('Person' )\n");
    }

    SECTION("left outer join") {
        auto ast = qc.parse("LeftOuterJoin($0.id == $0.id, NodeScan('Person'), NodeScan('Post'))");
        std::ostringstream os;
        ast_to_stream(ast, os);
        REQUIRE(os.str() == "LeftOuterJoin($0.id==$0.id )\n├── NodeScan('Post' )\n└── NodeScan('Person' )\n");
    }

    SECTION("hash join") {
        auto ast = qc.parse("HashJoin($0.id == $0.id, NodeScan('Person'), NodeScan('Post'))");
        std::ostringstream os;
        ast_to_stream(ast, os);
        REQUIRE(os.str() == "HashJoin($0.id==$0.id )\n├── NodeScan('Post' )\n└── NodeScan('Person' )\n");
    }

    SECTION("sort + limit") {
        auto ast = qc.parse("Limit(20, Sort([$4.Age:int DESC, $1.Name:string ASC]))");
        std::ostringstream os;
        ast_to_stream(ast, os);
        REQUIRE(os.str() == "Limit(20 )\n└── Sort([ $4.Age 2, $1.Name 1 ] )\n");
    }

   SECTION("groupby") {
        auto ast = qc.parse("GroupBy([$0.Name:string], [count($0.Id:int), avg($0.Age:int)], NodeScan('Person'))");
        std::ostringstream os;
        ast_to_stream(ast, os);
        REQUIRE(os.str() == "GroupBy([ $0.Name ] [ count($0.Id):int, avg($0.Age):int ] )\n└── NodeScan('Person' )\n");
    }

   SECTION("scan + create") {
        auto ast = qc.parse("Create((n:Label { name: $0.Name, age: $0.Age }), NodeScan('Person'))");
        std::ostringstream os;
        ast_to_stream(ast, os);
        REQUIRE(os.str() == "CreateNode(n:Label { name:  $0.Name, age:  $0.Age } )\n└── NodeScan('Person' )\n");
    }

   SECTION("scan + create rship") {
        auto ast = qc.parse("Create(($0)-[r:Label { id: 'Bla'} ]->($1), HashJoin($0.Id == $1.PId, NodeScan('Person'), NodeScan('Order')))");
        std::ostringstream os;
        ast_to_stream(ast, os);
        REQUIRE(os.str() == "CreateRelationship(-> 0 1 r:Label { id:  'Bla' } )\n└── HashJoin($0.Id==$1.Id )\n    ├── NodeScan('Order' )\n    └── NodeScan('Person' )\n");
    }

   SECTION("scan + create rship") {
        auto ast = qc.parse("Create(($0)<-[r:Label]->($1), HashJoin($0.Id == $1.PId, NodeScan('Person'), NodeScan('Order')))");
        std::ostringstream os;
        ast_to_stream(ast, os);
        REQUIRE(os.str() == "CreateRelationship(<-> 0 1 r:Label )\n└── HashJoin($0.Id==$1.Id )\n    ├── NodeScan('Order' )\n    └── NodeScan('Person' )\n");
    }
}