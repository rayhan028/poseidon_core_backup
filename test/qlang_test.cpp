#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do
                          // this in one cpp file

#include <tao/pegtl.hpp>
#include <tao/pegtl/contrib/analyze.hpp>
#include <tao/pegtl/contrib/parse_tree.hpp>
#include <tao/pegtl/contrib/parse_tree_to_dot.hpp>

// #include <tao/pegtl/contrib/tracer.hpp>

#include "catch.hpp"
#include "qlang_grammar.hpp"

namespace pegtl = tao::pegtl;

TEST_CASE("Analyzing the poseidon grammar", "[qlang]") {
  const std::size_t issues_found =
      tao::pegtl::analyze<qlang::qoperator>();
  std::cout << "Analyzing the grammar: " << issues_found << " issues found.\n";
  REQUIRE(issues_found == 0);
}

TEST_CASE("Testing the poseidon parser", "[qlang]") {
  REQUIRE(pegtl::parse<qlang::expression, pegtl::nothing>(
      pegtl::string_input<>("42 == 21", "")));
  REQUIRE(pegtl::parse<qlang::expression, pegtl::nothing>(
      pegtl::string_input<>("42.159 >= 1E-01", "")));
  REQUIRE(pegtl::parse<qlang::expression, pegtl::nothing>(
      pegtl::string_input<>("'Hello' != \"World\"", "")));
  REQUIRE(pegtl::parse<qlang::expression, pegtl::nothing>(
      pegtl::string_input<>("42 > 21 and 13 < 22", "")));
  REQUIRE(pegtl::parse<qlang::variable_name, pegtl::nothing>(
      pegtl::string_input<>("$1.Age", "")));
  REQUIRE(pegtl::parse<qlang::variable_name, pegtl::nothing>(
      pegtl::string_input<>("$2.Name", "")));
 REQUIRE(pegtl::parse<qlang::variable_name, pegtl::nothing>(
      pegtl::string_input<>("$12.City", "")));
 REQUIRE_THROWS(pegtl::parse<qlang::property, pegtl::nothing>(
      pegtl::string_input<>("Name - Max", "")));
 REQUIRE(pegtl::parse<qlang::property, pegtl::nothing>(
      pegtl::string_input<>("Name: 'Max'", "")));
 REQUIRE_THROWS(pegtl::parse<qlang::property, pegtl::nothing>(
      pegtl::string_input<>("Name: Max", "")));
 REQUIRE(pegtl::parse<qlang::prop_list, pegtl::nothing>(
      pegtl::string_input<>("{ Name: 'Max', Age: 42 }", "")));
 REQUIRE_THROWS(pegtl::parse<qlang::prop_list, pegtl::nothing>(
      pegtl::string_input<>("{ Name: Max ]", "")));
 REQUIRE(pegtl::parse<qlang::func_array, pegtl::nothing>(
      pegtl::string_input<>("[ count($0.Name:string) ]", "")));
 REQUIRE_THROWS(pegtl::parse<qlang::func_array, pegtl::nothing>(
      pegtl::string_input<>("[ count($0.Name:string) }", "")));
 REQUIRE_THROWS(pegtl::parse<qlang::func_array, pegtl::nothing>(
      pegtl::string_input<>("[ $0.Name:string ]", "")));

  REQUIRE(pegtl::parse<qlang::qoperator, pegtl::nothing>(
      pegtl::string_input<>("NodeScan()", "")));
  // REQUIRE_THROWS(pegtl::parse<qlang::qoperator, pegtl::nothing>(
  //    pegtl::string_input<>("NodeScan(12 awq))", "")));
  REQUIRE(pegtl::parse<qlang::qoperator, pegtl::nothing>(
      pegtl::string_input<>("NodeScan( )", "")));
  REQUIRE(pegtl::parse<qlang::qoperator, pegtl::nothing>(
      pegtl::string_input<>("NodeScan(\"Person\")", "")));
  REQUIRE(pegtl::parse<qlang::qoperator, pegtl::nothing>(
      pegtl::string_input<>("NodeScan('Person')", "")));
  REQUIRE(pegtl::parse<qlang::qoperator, pegtl::nothing>(
      pegtl::string_input<>("NodeScan( \"Person\" )", "")));
  REQUIRE(pegtl::parse<qlang::qoperator, pegtl::nothing>(
      pegtl::string_input<>("ForeachRelationship(FROM, \":IsLocatedIn\")", "")));
  REQUIRE(pegtl::parse<qlang::qoperator, pegtl::nothing>(
      pegtl::string_input<>("Expand(OUT, \"Post\")", "")));
  REQUIRE(pegtl::parse<qlang::qoperator, pegtl::nothing>(
      pegtl::string_input<>("Limit(20)", "")));
  REQUIRE(pegtl::parse<qlang::qoperator, pegtl::nothing>(
      pegtl::string_input<>("ForeachRelationship(FROM, \":IsLocatedIn\", NodeScan('Person'))", "")));
  REQUIRE(pegtl::parse<qlang::qoperator, pegtl::nothing>(
      pegtl::string_input<>("Expand(OUT, \"Post\", ForeachRelationship(FROM, ':IsLocatedIn', NodeScan(\"Person\")))", "")));
  REQUIRE(pegtl::parse<qlang::qoperator, pegtl::nothing>(
      pegtl::string_input<>("Filter($1.Age == 42)", "")));
  REQUIRE(pegtl::parse<qlang::qoperator, pegtl::nothing>(
      pegtl::string_input<>("Filter($2.Name != 'John')", "")));
  REQUIRE(pegtl::parse<qlang::qoperator, pegtl::nothing>(
      pegtl::string_input<>("NodeScan(Filter($1.Age >= 42))", "")));
  REQUIRE(pegtl::parse<qlang::proj_expr, pegtl::nothing>(
      pegtl::string_input<>("$1.Age:int", "")));
  REQUIRE(pegtl::parse<qlang::proj_array, pegtl::nothing>(
      pegtl::string_input<>("[$1.Age:int, $1.Name:string, $0:node]", "")));
  REQUIRE(pegtl::parse<qlang::qoperator, pegtl::nothing>(
      pegtl::string_input<>("Project([$1.Age:int, $1.Name:string])", "")));
  REQUIRE(pegtl::parse<qlang::prop_list, pegtl::nothing>(
      pegtl::string_input<>("{ name1: 'Val1', name2: 42 }", "")));
  REQUIRE(pegtl::parse<qlang::node_pattern, pegtl::nothing>(
      pegtl::string_input<>("(n:Label { name1: 'Val1', name2: 42 })", "")));
  REQUIRE(pegtl::parse<qlang::rship_pattern, pegtl::nothing>(
      pegtl::string_input<>("($1)-[r:Label { name1: 'Val1', name2: 42 }]->($2)", "")));
  REQUIRE(pegtl::parse<qlang::rship_pattern, pegtl::nothing>(
      pegtl::string_input<>("($1)-[r:Label]->($2)", "")));
  REQUIRE(pegtl::parse<qlang::rship_pattern, pegtl::nothing>(
      pegtl::string_input<>("($1)<-[r:Label]-($2)", "")));
  REQUIRE(pegtl::parse<qlang::rship_pattern, pegtl::nothing>(
      pegtl::string_input<>("($1)-[r:Label]-($2)", "")));
  REQUIRE(pegtl::parse<qlang::rship_pattern, pegtl::nothing>(
      pegtl::string_input<>("($1)<-[r:Label]->($2)", "")));
  REQUIRE_THROWS(pegtl::parse<qlang::rship_pattern, pegtl::nothing>(
      pegtl::string_input<>("($1)<-(r:Label)->($2)", "")));

  REQUIRE(pegtl::parse<qlang::qoperator, pegtl::nothing>(
      pegtl::string_input<>("Create((n:Label { name1: 'Val1', name2: 42 }))", "")));
  REQUIRE(pegtl::parse<qlang::qoperator, pegtl::nothing>(
      pegtl::string_input<>("Create((n:Label { name1: 'Val1', name2: 42 }), NodeScan('Person'))", "")));
  REQUIRE(pegtl::parse<qlang::qoperator, pegtl::nothing>(
      pegtl::string_input<>("GroupBy([$0.Id:int, $1.Name:string, $3.Age:int], [count($0.Name:string), avg($3.Age:int)])", "")));
  REQUIRE(pegtl::parse<qlang::qoperator, pegtl::nothing>(
      pegtl::string_input<>("Limit(20, Sort([$4.Age:int DESC, $1.Name:string ASC]))", "")));
  REQUIRE(pegtl::parse<qlang::qoperator, pegtl::nothing>(
      pegtl::string_input<>("LeftOuterJoin($0.Id == $1.PId, NodeScan('Person'), NodeScan('Order'))", "")));
  REQUIRE(pegtl::parse<qlang::qoperator, pegtl::nothing>(
      pegtl::string_input<>("HashJoin($0.Id == $1.PId, NodeScan('Person'), NodeScan('Order'))", "")));
  REQUIRE(pegtl::parse<qlang::qoperator, pegtl::nothing>(
      pegtl::string_input<>("      Project([$0.firstName:string, $0.lastName:string, $0.birthday:datetime, $0.locationIP:string, $0.browserUsed:string, $2.id:uint64, $0.gender:string, $0.creationDate:datetime],ForeachRelationship(FROM, ':isLocatedIn', Filter($0.id == 42, NodeScan('Person'))))", "")));
 }