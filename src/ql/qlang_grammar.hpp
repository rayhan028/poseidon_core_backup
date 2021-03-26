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


struct ws : opt<tao::pegtl::star<space>> {};

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

struct variable_name : seq< one< '$' >, integer, opt<one< '.' >, name> > {};
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
struct key_lojoin : TAO_PEGTL_KEYWORD("LeftOuterJoin") {};
struct key_hashjoin : TAO_PEGTL_KEYWORD("HashJoin") {};
struct key_aggregate : TAO_PEGTL_KEYWORD("Aggregate") {};
struct key_groupby : TAO_PEGTL_KEYWORD("GroupBy") {};
struct key_sort : TAO_PEGTL_KEYWORD("Sort") {};
struct key_create : TAO_PEGTL_KEYWORD("Create") {};

struct op_name : sor< key_node_scan, 
                    key_index_scan, 
                    key_filter,
                    key_expand,
                    key_project,
                    key_limit,
                    key_foreach_rship,
                    key_hashjoin,
                    key_lojoin,
                    key_groupby,
                    key_sort,
                    key_create
                    > {};

struct directions : sor<TAO_PEGTL_KEYWORD("FROM"), TAO_PEGTL_KEYWORD("TO"), 
                        TAO_PEGTL_KEYWORD("IN"), TAO_PEGTL_KEYWORD("OUT")> {};

struct sort_order : sor<TAO_PEGTL_KEYWORD("ASC"), TAO_PEGTL_KEYWORD("DESC")> {}; 

struct key_int : TAO_PEGTL_KEYWORD("int") {};
struct key_uint64 : TAO_PEGTL_KEYWORD("uint64") {};
struct key_float : TAO_PEGTL_KEYWORD("float") {};
struct key_string : TAO_PEGTL_KEYWORD("string") {};
struct key_dtime : TAO_PEGTL_KEYWORD("datetime") {};
struct key_node : TAO_PEGTL_KEYWORD("node") {};
struct key_relationship : TAO_PEGTL_KEYWORD("relationship") {};

struct dtype : sor< key_int, key_uint64, key_float, key_string, key_dtime, key_node, key_relationship> {};

struct proj_expr : seq< variable_name, one<':'>, dtype, ws, opt<sort_order>> {};

struct proj_array : seq< one<'['>, ws, list<proj_expr, comma>, ws, one<']'> > {};

struct key_avg : TAO_PEGTL_KEYWORD("avg") {};
struct key_sum : TAO_PEGTL_KEYWORD("sum") {};
struct key_count : TAO_PEGTL_KEYWORD("count") {};
struct key_min : TAO_PEGTL_KEYWORD("min") {};
struct key_max : TAO_PEGTL_KEYWORD("max") {};

struct func_name : sor<key_avg, key_sum, key_count, key_min, key_max> {};

struct func_expr : if_must< func_name, one<'('>, ws, variable_name, one<':'>, dtype, ws, one<')'>> {};

struct func_array : if_must< one<'['>, ws, list<func_expr, comma>, ws, one<']'> > {};

struct property : if_must< name, opt<space>, one<':'>, opt<space>, sor<decimal, literal_string, variable_name> > {};

struct prop_list : if_must<one<'{'>, ws, list<property, comma>, ws, one<'}'> > {};

struct node_or_rship_label : seq<name, opt<space>, one<':'>, opt<space>, name> {};
struct node_or_rship_pattern : seq<node_or_rship_label, opt<space>, opt<prop_list> > {};
struct node_pattern : seq<one<'('>, opt<space>, node_or_rship_pattern, opt<space>, one<')'> > {};

struct snode : seq<one<'('>, opt<space>, one<'$'>, integer, opt<space>, one<')'>> {};

struct right_rship_dir : seq< one<'-'>, opt< one<'>'>>> {};
struct left_rship_dir : seq< opt< one<'<'>>, one<'-'>> {};

struct rship_pattern : if_must<snode, 
                          left_rship_dir, 
                          one<'['>, opt<space>, node_or_rship_pattern, opt<space>, one <']'>, 
                          right_rship_dir, 
                          snode> {};

struct qoperator;

struct param : sor<literal_string, qoperator, directions, integer, expression, proj_array, func_array, 
                    node_pattern, rship_pattern> {};

struct param_list : list<param, comma> {};

struct qoperator : seq<ws, op_name, ws, one<'('>, ws, opt<param_list>, ws, one<')'>, ws> {};

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
template <> struct my_selector<proj_array> : std::true_type {};
template <> struct my_selector<proj_expr> : std::true_type {};
template <> struct my_selector<func_array> : std::true_type {};
template <> struct my_selector<func_expr> : std::true_type {};
template <> struct my_selector<func_name> : std::true_type {};
template <> struct my_selector<dtype> : std::true_type {};
template <> struct my_selector<property> : std::true_type {};
template <> struct my_selector<prop_list> : std::true_type {};
template <> struct my_selector<node_or_rship_pattern> : std::true_type {};
template <> struct my_selector<left_rship_dir> : std::true_type {};
template <> struct my_selector<right_rship_dir> : std::true_type {};
template <> struct my_selector<node_or_rship_label> : std::true_type {};
template <> struct my_selector<node_pattern> : std::true_type {};
template <> struct my_selector<rship_pattern> : std::true_type {};
template <> struct my_selector<directions> : std::true_type {};
template <> struct my_selector<sort_order> : std::true_type {};

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