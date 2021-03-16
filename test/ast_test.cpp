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
        REQUIRE(os.str() == "Filter($0.id == 42 )\n");
    }

    SECTION("filter, projection") {
        auto ast = qc.parse("Project([$0.firstName:string, $0.lastName:string], Filter($0.id == 42))");
        std::ostringstream os;
        ast_to_stream(ast, os);
        REQUIRE(os.str() == "Project([ $0.firstName, $0.lastName ] )\n└── Filter($0.id == 42 )\n");
    }

    SECTION("filter, projection + scan") {
        auto ast = qc.parse("Project([$0.firstName:string, $0.lastName:string], Filter($0.id == 42, NodeScan('Person')))");
        std::ostringstream os;
        ast_to_stream(ast, os);
        REQUIRE(os.str() == "Project([ $0.firstName, $0.lastName ] )\n└── Filter($0.id == 42 )\n    └── NodeScan('Person' )\n");
    }

    SECTION("filter + scan") {
        auto ast = qc.parse("Filter($0.id == 42, NodeScan('Person'))");
        std::ostringstream os;
        ast_to_stream(ast, os);
        REQUIRE(os.str() == "Filter($0.id == 42 )\n└── NodeScan('Person' )\n");
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
        REQUIRE(os.str() == "LeftOuterJoin($0.id == $0.id )\n├── NodeScan('Post' )\n└── NodeScan('Person' )\n");
    }

}