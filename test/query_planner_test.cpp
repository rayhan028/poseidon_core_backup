#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do
                          // this in one cpp file

#include <catch2/catch_test_macros.hpp>

#include "query_proc.hpp"

TEST_CASE("Testing the poseidon query processor", "[query_proc]") {
    query_ctx qctx;
    query_proc qp(qctx);

    auto plan = qp.prepare_query("NodeScan('Person')");
    plan.print_plan();

    plan = qp.prepare_query("Limit(20, NodeScan('Person'))");
    plan.print_plan();

    plan = qp.prepare_query("Limit(20, CrossJoin(NodeScan('Comment'), NodeScan('Person')))");
    plan.print_plan();

    plan = qp.prepare_query("Project([$1.attr:datetime, $0.attr:string, $0.attr:int], NodeScan('Person'))");
    plan.print_plan();

    plan = qp.prepare_query("Expand(OUT, 'Person', ForeachRelationship(FROM, ':knows', 1, 3, NodeScan('Person')))");
    plan.print_plan();

    plan = qp.prepare_query("Aggregate([count($0.attr:string), sum($1.attr:int)], NodeScan('Nodes'))");
    plan.print_plan();

    plan = qp.prepare_query("Filter($0.attr > 42, NodeScan('Nodes'))");
    plan.print_plan();

    plan = qp.prepare_query("Create((n:Label { name1: 'Val1', name2: 42 }))");
    plan.print_plan();

    plan = qp.prepare_query("Create(($0)-[r:knows { creationDate: '2010-07-21' } ]->($1), CrossJoin(Filter($0.id == 1, NodeScan('Person')), Filter($0.id == 2, NodeScan('Person'))))");
    plan.print_plan();
}
