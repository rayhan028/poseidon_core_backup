#ifndef qlang_grammar_hpp_
#define qlang_grammar_hpp_

#include <tao/pegtl.hpp>
#include <tao/pegtl/contrib/parse_tree.hpp>

/**
 * The grammar for the Poseidon query language.
 */
namespace qlang {

using namespace tao::pegtl;

struct seps : tao::pegtl::star< ascii::space > {};

struct escaped
    : if_must< one<'\\'>, one<'\\', '"', '\'', 'a', 'f', 'n', 'r', 't', 'v'> > {};
struct regular : not_range<0, 31> {};
struct character : sor<escaped, regular> {};

struct quoted_string : if_must< one<'"'>, until<one<'"'>, character> > {};

template< char Q >
  struct short_string : if_must< one< Q >, until< tao::pegtl::one< Q >, character > > {};

struct literal_string : sor< short_string< '"' >, short_string< '\'' >> {};

struct integer : plus<ascii::digit> {};

struct comma : seq< opt<space>, one<','>, opt<space> > {};

template< typename Key >
  struct key : seq< Key, not_at< identifier_other > > {};

template< typename S, typename O >
  struct left_assoc : seq< S, seps, star_must< O, seps, S, seps > > {};
template< typename S, typename O >
  struct right_assoc : seq< S, seps, opt_must< O, seps, right_assoc< S, O > > > {};
template< char O, char... N >
  struct op_one : seq< one< O >, at< not_one< N... > > > {};

template< char O, char P, char... N >
  struct op_two : seq< string< O, P >, at< not_one< N... > > > {};    

template< typename E >
  struct exponent : opt_must< E, opt< one< '+', '-' > >, plus< digit > > {};
template< typename D, typename E >
   struct numeral_three : seq< if_must< one< '.' >, plus< D > >, exponent< E > > {};
template< typename D, typename E >
   struct numeral_two : seq< plus< D >, opt< one< '.' >, star< D > >, exponent< E > > {};
template< typename D, typename E >
   struct numeral_one : sor< numeral_two< D, E >, numeral_three< D, E > > {};

struct decimal : numeral_one< digit, one< 'e', 'E' > > {};

struct str_or : TAO_PEGTL_STRING("or") {};
struct str_and : TAO_PEGTL_STRING("and") {};
struct str_not : TAO_PEGTL_STRING("not") {};
struct str_true : TAO_PEGTL_STRING("true") {};
struct str_false : TAO_PEGTL_STRING("false") {};

struct key_or : key< str_or > {};
struct key_and : key< str_and > {};
struct key_not : key< str_not > {};
struct key_false : key< str_false > {};
struct key_true : key< str_true > {};

struct sor_keyword : tao::pegtl::sor< str_and, str_or, str_true, str_false > {};

struct keyword : key< sor_keyword > {};

struct expression;
struct bracket_expr : if_must< one< '(' >, seps, expression, seps, one< ')' > > {};

struct name : seq< not_at< keyword >, identifier > {};

struct variable_name : seq< one< '$' >, integer, one< '.' >, name > {};
struct expr_ten;
struct expr_twelve : sor<key_true, key_false, decimal, literal_string, variable_name/*, expr_thirteen*/ > {};
struct unary_operators : sor< one< '-' >, op_one< '~', '=' >, key_not > {};
struct expr_eleven : seq< expr_twelve, seps, opt< one< '^' >, seps, expr_ten, seps > > {};
struct unary_apply : if_must< unary_operators, seps, expr_ten, seps > {};
struct expr_ten : sor< unary_apply, expr_eleven > {};
struct operators_nine : sor< two< '/' >, one< '/' >, one< '*' >, one< '%' > > {};
struct expr_nine : left_assoc< expr_ten, operators_nine > {};
struct operators_eight : sor< one< '+' >, one< '-' > > {};
struct expr_eight : left_assoc< expr_nine, operators_eight > {};
struct operators_six : sor< two< '<' >, two< '>' > > {};
struct expr_six : left_assoc< expr_eight, operators_six > {};
struct operators_cmp : sor< two< '=' >, tao::pegtl::string< '<', '=' >,
                                           tao::pegtl::string< '>', '=' >,
                                           op_one< '<', '<' >,
                                           op_one< '>', '>' >,
                                           tao::pegtl::string< '!', '=' > > {};
struct expr_two : left_assoc< expr_six, operators_cmp > {};
struct expr_one : left_assoc< expr_two, key_and > {};
struct expression : left_assoc< expr_one, key_or> {};

struct key_node_scan : TAO_PEGTL_KEYWORD("NodeScan") {};
struct key_index_scan : TAO_PEGTL_KEYWORD("IndexScan") {};
struct key_filter : TAO_PEGTL_KEYWORD("Filter") {};
struct key_expand : TAO_PEGTL_KEYWORD("Expand") {};
struct key_project : TAO_PEGTL_KEYWORD("Project") {};
struct key_limit : TAO_PEGTL_KEYWORD("Limit") {};
struct key_foreach_rship : TAO_PEGTL_KEYWORD("ForeachRelationship") {};

struct op_name : sor< key_node_scan, 
                    key_index_scan, 
                    key_filter,
                    key_expand,
                    key_project,
                    key_limit,
                    key_foreach_rship
                    > {};

struct directions : sor<TAO_PEGTL_KEYWORD("FROM"), TAO_PEGTL_KEYWORD("TO"), 
                        TAO_PEGTL_KEYWORD("IN"), TAO_PEGTL_KEYWORD("OUT")> {};

struct qoperator;

struct param : sor<literal_string, qoperator, directions, integer, expression> {};

struct param_list : list<param, comma> {};

struct qoperator : seq<op_name, opt<space>, one<'('>, opt<space>, opt<param_list>, opt<space>, one<')'>> {};

/* ------------------------------------------------------------- */

template <typename Rule> struct my_selector : std::false_type {};
template <> struct my_selector<qoperator> : std::true_type {};
template <> struct my_selector<param> : std::true_type {};
template <> struct my_selector<expression> : std::true_type {};
template <> struct my_selector<literal_string> : std::true_type {};
template <> struct my_selector<decimal> : std::true_type {};
template <> struct my_selector<integer> : std::true_type {};
template <> struct my_selector<variable_name> : std::true_type {};
template <> struct my_selector<op_name> : std::true_type {};
template <> struct my_selector<operators_cmp> : std::true_type {};

/* ------------------------------------------------------------- */

template <typename Rule> struct my_control : tao::pegtl::normal<Rule> {
  static const std::string error_message;

  template <typename Input, typename... States>
  static void raise(const Input &in, States &&...) {
    throw tao::pegtl::parse_error(error_message, in);
  }
};

template <typename T>
const std::string my_control<T>::error_message = "parse error";
//   "parse error matching " + tao::pegtl::internal::demangle< T >();

} // namespace qlang

#endif