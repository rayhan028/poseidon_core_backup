#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do
                          // this in one cpp file
#define CATCH_CONFIG_CONSOLE_WIDTH 160

#include <iostream>
#include "catch.hpp"
#include "qparser.hpp"
#include "ast.hpp"

std::size_t string_diff(const std::string& s1, const std::string& s2) {
    if (s1.length() != s2.length()) return std::min(s1.length(), s2.length());
    for (auto i = 0ul; i < s1.length(); i++) {
        if (s1.at(i) != s2.at(i))
            return i;
    }
    return std::string::npos;
}

TEST_CASE("Constructing an AST from a query string", "[qlang]") {
    qparser qc;

    SECTION("simple scan operator") {
        auto ast = qc.parse("NodeScan('Person')");
        std::ostringstream os;
        ast_to_stream(ast, os);
        REQUIRE(os.str() == "NodeScan(Person )\n");
    }
    SECTION("projection + scan") {
        auto ast = qc.parse("Project([$0.firstName:string, $0.lastName:string], NodeScan('Person'))");
        std::ostringstream os;
        ast_to_stream(ast, os);
        REQUIRE(os.str() == "Project([ $0.firstName, $0.lastName ] )\n└── NodeScan(Person )\n");
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
        REQUIRE(os.str() == "Project([ $0.firstName, $0.lastName ] )\n└── Filter($0.id==42 )\n    └── NodeScan(Person )\n");
    }

    SECTION("filter + scan") {
        auto ast = qc.parse("Filter($0.id == 42, NodeScan('Person'))");
        std::ostringstream os;
        ast_to_stream(ast, os);
        REQUIRE(os.str() == "Filter($0.id==42 )\n└── NodeScan(Person )\n");
    }

    SECTION("expand + foreach") {
        auto ast = qc.parse("Expand(OUT, 'Place', ForeachRelationship(FROM, ':isLocatedIn', NodeScan('Person')))");
        std::ostringstream os;
        ast_to_stream(ast, os);
        REQUIRE(os.str() == "Expand(OUT Place )\n└── ForeachRelationship(FROM :isLocatedIn )\n    └── NodeScan(Person )\n");
    }

    SECTION("multiple expand + foreach") {
        auto ast = qc.parse("Expand(OUT, 'Person', ForeachRelationship($0, FROM, ':knows', Expand(OUT, 'Place', ForeachRelationship(FROM, ':isLocatedIn', NodeScan('Person')))))");
        std::ostringstream os;
        ast_to_stream(ast, os);
        REQUIRE(os.str() == "Expand(OUT Person )\n└── ForeachRelationship($0 FROM :knows )\n    └── Expand(OUT Place )\n        └── ForeachRelationship(FROM :isLocatedIn )\n            └── NodeScan(Person )\n");
    }

    SECTION("left outer join") {
        auto ast = qc.parse("LeftOuterJoin($0.id == $0.id, NodeScan('Person'), NodeScan('Post'))");
        std::ostringstream os;
        ast_to_stream(ast, os);
        REQUIRE(os.str() == "LeftOuterJoin($0.id==$0.id )\n├── NodeScan(Post )\n└── NodeScan(Person )\n");
    }

    SECTION("hash join") {
        auto ast = qc.parse("HashJoin($0.id == $0.id, NodeScan('Person'), NodeScan('Post'))");
        std::ostringstream os;
        ast_to_stream(ast, os);
        REQUIRE(os.str() == "HashJoin($0.id==$0.id )\n├── NodeScan(Post )\n└── NodeScan(Person )\n");
    }

    SECTION("sort + limit") {
        auto ast = qc.parse("Limit(20, Sort([$4.Age:int DESC, $1.Name:string ASC]))");
        std::ostringstream os;
        ast_to_stream(ast, os);
        REQUIRE(os.str() == "Limit(20 )\n└── Sort([ $4.Age 2, $1.Name 1 ] )\n");
    }
    SECTION("sort after project") {
        auto ast = qc.parse("Sort([$0:int DESC, $2:string ASC]))");
        std::ostringstream os;
        ast_to_stream(ast, os);
        REQUIRE(os.str() == "Sort([ $0 2, $2 1 ] )\n");
    }

   SECTION("groupby") {
        auto ast = qc.parse("GroupBy([$0.Name:string], [count($0.Id:int), avg($0.Age:int)], NodeScan('Person'))");
        std::ostringstream os;
        ast_to_stream(ast, os);
        REQUIRE(os.str() == "GroupBy([ $0.Name ] [ count($0.Id):int, avg($0.Age):int ] )\n└── NodeScan(Person )\n");
    }

    SECTION("groupby after project") {
        auto ast = qc.parse("GroupBy([$0:int], [count($0:int), avg($1:int)], NodeScan('Person'))");
        std::ostringstream os;
        ast_to_stream(ast, os);
        REQUIRE(os.str() == "GroupBy([ $0 ] [ count($0):int, avg($1):int ] )\n└── NodeScan(Person )\n");
    }

   SECTION("scan + create") {
        auto ast = qc.parse("Create((n:Label { name: $0.Name, age: $0.Age }), NodeScan('Person'))");
        std::ostringstream os;
        ast_to_stream(ast, os);
        REQUIRE(os.str() == "CreateNode(Label { name: $0.Name, age: $0.Age } )\n└── NodeScan(Person )\n");
    }

   SECTION("scan + create rship") {
        auto ast = qc.parse("Create(($0)-[r:Label { id: 'Bla'} ]->($1), HashJoin($0.Id == $1.PId, NodeScan('Person'), NodeScan('Order')))");
        std::ostringstream os;
        ast_to_stream(ast, os);
        REQUIRE(os.str() == "CreateRelationship(-> 0 1 Label { id: Bla } )\n└── HashJoin($0.Id==$1.PId )\n    ├── NodeScan(Order )\n    └── NodeScan(Person )\n");
    }

   SECTION("scan + create rship") {
        auto ast = qc.parse("Create(($0)<-[r:Label]->($1), HashJoin($0.Id == $1.PId, NodeScan('Person'), NodeScan('Order')))");
        std::ostringstream os;
        ast_to_stream(ast, os);
        REQUIRE(os.str() == "CreateRelationship(<-> 0 1 Label )\n└── HashJoin($0.Id==$1.PId )\n    ├── NodeScan(Order )\n    └── NodeScan(Person )\n");
    }
}

std::string load_string(const std::string& fname) {
    std::string qstr, line;

    std::ifstream myfile(fname);
    REQUIRE (myfile.is_open());
    while (getline(myfile, line)) {
        qstr.append(line);
        qstr.append("\n");
    }
    myfile.close();
    return qstr;
}

TEST_CASE("Constructing an AST from LDBC benchmark queries (IS)", "[qlang]") {
    qparser qc;
    char buf[1024];
    spdlog::info("getcwd {}", getcwd(buf, 1024)); 
    std::string prefix_is(buf); 
    prefix_is += "/../../queries/ldbc/is/is";  
    std::vector<int> query_set = { 1, 2, 3, 4, 5, 6, 7 };
    for (auto q : query_set) {
        auto fname = prefix_is + std::to_string(q) + ".q";
        spdlog::info("processing file: {}", fname);
        std::cout << "processing file: {}" << fname << std::endl;
        auto qstr = load_string(fname); 
        auto ast = qc.parse(qstr);
        std::ostringstream os;
        ast_to_stream(ast, os);
        auto pname = prefix_is + std::to_string(q) + ".plan";
        auto pstr = load_string(pname);
        auto diff = string_diff(os.str(), pstr);
        if (diff != std::string::npos)
            std::cout << "strings differ at pos " << diff << std::endl;
        REQUIRE(os.str() == pstr);
    }
}

TEST_CASE("Constructing an AST from LDBC benchmark queries (BI)", "[qlang]") {
    qparser qc;
    char buf[1024];
    spdlog::info("getcwd {}", getcwd(buf, 1024)); 
    std::string prefix_is(buf); 
    prefix_is += "/../../queries/ldbc/bi/bi";  
    std::vector<int> query_set = { 1, 2, 3/*, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20*/ };
    for (auto q : query_set) {
        auto fname = prefix_is + std::to_string(q) + ".q";
        spdlog::info("processing file: {}", fname);
        std::cout << "processing file: {}" << fname << std::endl;
        auto qstr = load_string(fname); 
        auto ast = qc.parse(qstr);
        std::ostringstream os;
        ast_to_stream(ast, os);
        auto pname = prefix_is + std::to_string(q) + ".plan";
        auto pstr = load_string(pname);
        auto diff = string_diff(os.str(), pstr);
        if (diff != std::string::npos)
            std::cout << "strings differ at pos " << diff << std::endl;
        REQUIRE(os.str() == pstr);
    }
}