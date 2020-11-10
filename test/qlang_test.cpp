#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do
                          // this in one cpp file

#include <tao/pegtl.hpp>
#include <tao/pegtl/analyze.hpp>
#include <tao/pegtl/contrib/parse_tree.hpp>
#include <tao/pegtl/contrib/parse_tree_to_dot.hpp>

#include <tao/pegtl/contrib/tracer.hpp>

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

  REQUIRE(pegtl::parse<qlang::qoperator, pegtl::nothing>(
      pegtl::string_input<>("NodeScan()", "")));
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
  REQUIRE(pegtl::parse<qlang::qoperator, pegtl::nothing>(
      pegtl::string_input<>("Project($1.Age, $1.Name)", "")));
}