
// Generated from src/query/parser/poseidon.g4 by ANTLR 4.13.0


#include "poseidonVisitor.h"

#include "poseidonParser.h"


using namespace antlrcpp;

using namespace antlr4;

namespace {

struct PoseidonParserStaticData final {
  PoseidonParserStaticData(std::vector<std::string> ruleNames,
                        std::vector<std::string> literalNames,
                        std::vector<std::string> symbolicNames)
      : ruleNames(std::move(ruleNames)), literalNames(std::move(literalNames)),
        symbolicNames(std::move(symbolicNames)),
        vocabulary(this->literalNames, this->symbolicNames) {}

  PoseidonParserStaticData(const PoseidonParserStaticData&) = delete;
  PoseidonParserStaticData(PoseidonParserStaticData&&) = delete;
  PoseidonParserStaticData& operator=(const PoseidonParserStaticData&) = delete;
  PoseidonParserStaticData& operator=(PoseidonParserStaticData&&) = delete;

  std::vector<antlr4::dfa::DFA> decisionToDFA;
  antlr4::atn::PredictionContextCache sharedContextCache;
  const std::vector<std::string> ruleNames;
  const std::vector<std::string> literalNames;
  const std::vector<std::string> symbolicNames;
  const antlr4::dfa::Vocabulary vocabulary;
  antlr4::atn::SerializedATNView serializedATN;
  std::unique_ptr<antlr4::atn::ATN> atn;
};

::antlr4::internal::OnceFlag poseidonParserOnceFlag;
#if ANTLR4_USE_THREAD_LOCAL_CACHE
static thread_local
#endif
PoseidonParserStaticData *poseidonParserStaticData = nullptr;

void poseidonParserInitialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  if (poseidonParserStaticData != nullptr) {
    return;
  }
#else
  assert(poseidonParserStaticData == nullptr);
#endif
  auto staticData = std::make_unique<PoseidonParserStaticData>(
    std::vector<std::string>{
      "query", "query_operator", "node_scan_op", "scan_param", "scan_list", 
      "index_scan_op", "index_scan_param", "project_op", "proj_list", "proj_expr", 
      "type_spec", "limit_op", "crossjoin_op", "hashjoin_op", "leftouterjoin_op", 
      "foreach_relationship_op", "rship_dir", "rship_cardinality", "rship_source_var", 
      "expand_op", "expand_dir", "match_op", "path_pattern", "path_component", 
      "node_pattern", "rship_pattern", "cardinality_spec", "min_cardinality", 
      "max_cardinality", "dir_spec", "left_dir", "right_dir", "no_dir", 
      "aggregate_op", "aggregate_list", "aggr_expr", "aggr_func", "union_op", 
      "group_by_op", "grouping_list", "grouping_expr", "filter_op", "logical_expr", 
      "boolean_expr", "equality_expr", "relational_expr", "additive_expr", 
      "multiplicative_expr", "unary_expr", "primary_expr", "variable", "value", 
      "function_call", "udf_prefix", "param_list", "param", "sort_op", "sort_list", 
      "sort_expr", "sort_spec", "create_op", "create_node", "property_list", 
      "property", "create_rship", "node_var"
    },
    std::vector<std::string>{
      "", "'.'", "'..'", "'<-'", "'->'", "'-'", "'{'", "'}'", "'Filter'", 
      "'NodeScan'", "'IndexScan'", "'Match'", "'Project'", "'Limit'", "'CrossJoin'", 
      "'HashJoin'", "'LeftOuterJoin'", "'Expand'", "'ForeachRelationship'", 
      "'Aggregate'", "'GroupBy'", "'Sort'", "'Create'", "'Union'", "'int'", 
      "'uint64'", "'double'", "'string'", "'datetime'", "'qresult'", "'count'", 
      "'sum'", "'avg'", "'min'", "'max'", "'udf'", "'IN'", "'OUT'", "'FROM'", 
      "'TO'", "'ALL'", "'DESC'", "'ASC'", "", "", "", "", "", "", "':'", 
      "'::'", "','", "'('", "')'", "'['", "']'", "", "", "", "", "'<'", 
      "'<='", "'>'", "'>='", "'+'", "'*'", "'/'", "'%'"
    },
    std::vector<std::string>{
      "", "", "", "", "", "", "", "", "Filter_", "Nodescan_", "Indexscan_", 
      "Match_", "Project_", "Limit_", "CrossJoin_", "HashJoin_", "LeftOuterJoin_", 
      "Expand_", "ForeachRelationship_", "Aggregate_", "GroupBy_", "Sort_", 
      "Create_", "Union_", "IntType_", "Uint64Type_", "DoubleType_", "StringType_", 
      "DateType_", "ResultType_", "Count_", "Sum_", "Avg_", "Min_", "Max_", 
      "UDF_", "InExpandDir_", "OutExpandDir_", "FromDir_", "ToDir_", "AllDir_", 
      "DescOrder_", "AscOrder_", "INTEGER", "FLOAT", "Identifier_", "Var", 
      "NAME_", "STRING_", "COLON_", "DOUBLE_COLON", "COMMA_", "LPAREN", 
      "RPAREN", "LBRACKET", "RBRACKET", "OR", "AND", "EQUALS", "NOTEQUALS", 
      "LT", "LTEQ", "GT", "GTEQ", "PLUS_", "MULT", "DIV", "MOD", "NOT", 
      "WHITESPACE"
    }
  );
  static const int32_t serializedATNSegment[] = {
  	4,1,69,589,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,6,2,
  	7,7,7,2,8,7,8,2,9,7,9,2,10,7,10,2,11,7,11,2,12,7,12,2,13,7,13,2,14,7,
  	14,2,15,7,15,2,16,7,16,2,17,7,17,2,18,7,18,2,19,7,19,2,20,7,20,2,21,7,
  	21,2,22,7,22,2,23,7,23,2,24,7,24,2,25,7,25,2,26,7,26,2,27,7,27,2,28,7,
  	28,2,29,7,29,2,30,7,30,2,31,7,31,2,32,7,32,2,33,7,33,2,34,7,34,2,35,7,
  	35,2,36,7,36,2,37,7,37,2,38,7,38,2,39,7,39,2,40,7,40,2,41,7,41,2,42,7,
  	42,2,43,7,43,2,44,7,44,2,45,7,45,2,46,7,46,2,47,7,47,2,48,7,48,2,49,7,
  	49,2,50,7,50,2,51,7,51,2,52,7,52,2,53,7,53,2,54,7,54,2,55,7,55,2,56,7,
  	56,2,57,7,57,2,58,7,58,2,59,7,59,2,60,7,60,2,61,7,61,2,62,7,62,2,63,7,
  	63,2,64,7,64,2,65,7,65,1,0,1,0,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,3,1,152,8,1,1,2,1,2,1,2,3,2,157,8,2,1,2,
  	1,2,1,3,1,3,3,3,163,8,3,1,4,1,4,1,4,1,4,5,4,169,8,4,10,4,12,4,172,9,4,
  	1,4,1,4,1,5,1,5,1,5,1,5,1,5,1,6,1,6,1,6,1,6,1,6,1,6,1,7,1,7,1,7,1,7,1,
  	7,1,7,1,7,1,8,1,8,1,8,1,8,5,8,198,8,8,10,8,12,8,201,9,8,1,8,1,8,1,9,1,
  	9,1,9,1,9,3,9,209,8,9,1,9,1,9,3,9,213,8,9,1,10,1,10,1,11,1,11,1,11,1,
  	11,1,11,1,11,1,11,1,12,1,12,1,12,1,12,1,12,1,12,1,12,1,13,1,13,1,13,1,
  	13,1,13,1,13,1,13,1,13,1,13,1,14,1,14,1,14,1,14,1,14,1,14,1,14,1,14,1,
  	14,1,15,1,15,1,15,1,15,1,15,1,15,1,15,3,15,256,8,15,1,15,1,15,3,15,260,
  	8,15,1,15,1,15,1,15,1,15,1,16,1,16,1,17,1,17,1,17,1,17,1,18,1,18,1,19,
  	1,19,1,19,1,19,1,19,1,19,1,19,1,19,1,19,1,20,1,20,1,21,1,21,1,21,1,21,
  	1,21,1,22,1,22,5,22,292,8,22,10,22,12,22,295,9,22,1,23,1,23,1,23,1,24,
  	1,24,3,24,302,8,24,1,24,1,24,1,24,3,24,307,8,24,1,24,1,24,1,25,1,25,1,
  	25,3,25,314,8,25,1,25,1,25,1,25,3,25,319,8,25,1,25,1,25,1,25,1,26,1,26,
  	1,26,1,26,3,26,328,8,26,1,27,1,27,1,28,1,28,1,29,1,29,1,29,3,29,337,8,
  	29,1,30,1,30,1,31,1,31,1,32,1,32,1,33,1,33,1,33,1,33,1,33,1,33,1,33,1,
  	34,1,34,1,34,1,34,5,34,356,8,34,10,34,12,34,359,9,34,1,34,1,34,1,35,1,
  	35,1,35,1,35,1,35,1,36,1,36,1,37,1,37,1,37,1,37,1,37,1,37,1,37,1,38,1,
  	38,1,38,1,38,1,38,1,38,1,38,1,38,1,38,1,39,1,39,1,39,1,39,5,39,390,8,
  	39,10,39,12,39,393,9,39,1,39,1,39,1,40,1,40,1,40,3,40,400,8,40,1,40,1,
  	40,1,40,1,41,1,41,1,41,1,41,1,41,1,41,1,41,1,42,1,42,1,42,5,42,415,8,
  	42,10,42,12,42,418,9,42,1,43,1,43,1,43,5,43,423,8,43,10,43,12,43,426,
  	9,43,1,44,1,44,1,44,5,44,431,8,44,10,44,12,44,434,9,44,1,45,1,45,1,45,
  	5,45,439,8,45,10,45,12,45,442,9,45,1,46,1,46,1,46,5,46,447,8,46,10,46,
  	12,46,450,9,46,1,47,1,47,1,47,5,47,455,8,47,10,47,12,47,458,9,47,1,48,
  	3,48,461,8,48,1,48,1,48,1,49,1,49,1,49,1,49,1,49,1,49,1,49,3,49,472,8,
  	49,1,50,1,50,1,50,1,50,1,51,1,51,1,52,1,52,1,52,1,52,3,52,484,8,52,1,
  	52,1,52,1,53,1,53,1,53,1,54,1,54,1,54,5,54,494,8,54,10,54,12,54,497,9,
  	54,1,55,1,55,1,55,1,55,3,55,503,8,55,1,55,1,55,3,55,507,8,55,1,56,1,56,
  	1,56,1,56,1,56,1,56,1,56,1,57,1,57,1,57,1,57,5,57,520,8,57,10,57,12,57,
  	523,9,57,1,57,1,57,1,58,1,58,1,58,1,58,1,58,1,59,1,59,1,60,1,60,1,60,
  	1,60,3,60,538,8,60,1,60,1,60,3,60,542,8,60,1,60,1,60,1,61,1,61,1,61,1,
  	61,1,61,3,61,551,8,61,1,61,1,61,1,62,1,62,1,62,1,62,5,62,559,8,62,10,
  	62,12,62,562,9,62,1,62,1,62,1,63,1,63,1,63,1,63,1,64,1,64,1,64,1,64,3,
  	64,574,8,64,1,64,1,64,1,64,3,64,579,8,64,1,64,1,64,1,64,1,64,1,65,1,65,
  	1,65,1,65,1,65,0,0,66,0,2,4,6,8,10,12,14,16,18,20,22,24,26,28,30,32,34,
  	36,38,40,42,44,46,48,50,52,54,56,58,60,62,64,66,68,70,72,74,76,78,80,
  	82,84,86,88,90,92,94,96,98,100,102,104,106,108,110,112,114,116,118,120,
  	122,124,126,128,130,0,10,1,0,24,29,1,0,38,40,1,0,36,37,1,0,30,34,1,0,
  	58,59,1,0,60,63,2,0,5,5,64,64,1,0,65,67,2,0,43,44,48,48,1,0,41,42,577,
  	0,132,1,0,0,0,2,151,1,0,0,0,4,153,1,0,0,0,6,162,1,0,0,0,8,164,1,0,0,0,
  	10,175,1,0,0,0,12,180,1,0,0,0,14,186,1,0,0,0,16,193,1,0,0,0,18,212,1,
  	0,0,0,20,214,1,0,0,0,22,216,1,0,0,0,24,223,1,0,0,0,26,230,1,0,0,0,28,
  	239,1,0,0,0,30,248,1,0,0,0,32,265,1,0,0,0,34,267,1,0,0,0,36,271,1,0,0,
  	0,38,273,1,0,0,0,40,282,1,0,0,0,42,284,1,0,0,0,44,289,1,0,0,0,46,296,
  	1,0,0,0,48,299,1,0,0,0,50,310,1,0,0,0,52,323,1,0,0,0,54,329,1,0,0,0,56,
  	331,1,0,0,0,58,336,1,0,0,0,60,338,1,0,0,0,62,340,1,0,0,0,64,342,1,0,0,
  	0,66,344,1,0,0,0,68,351,1,0,0,0,70,362,1,0,0,0,72,367,1,0,0,0,74,369,
  	1,0,0,0,76,376,1,0,0,0,78,385,1,0,0,0,80,396,1,0,0,0,82,404,1,0,0,0,84,
  	411,1,0,0,0,86,419,1,0,0,0,88,427,1,0,0,0,90,435,1,0,0,0,92,443,1,0,0,
  	0,94,451,1,0,0,0,96,460,1,0,0,0,98,471,1,0,0,0,100,473,1,0,0,0,102,477,
  	1,0,0,0,104,479,1,0,0,0,106,487,1,0,0,0,108,490,1,0,0,0,110,506,1,0,0,
  	0,112,508,1,0,0,0,114,515,1,0,0,0,116,526,1,0,0,0,118,531,1,0,0,0,120,
  	533,1,0,0,0,122,545,1,0,0,0,124,554,1,0,0,0,126,565,1,0,0,0,128,569,1,
  	0,0,0,130,584,1,0,0,0,132,133,3,2,1,0,133,134,5,0,0,1,134,1,1,0,0,0,135,
  	152,3,82,41,0,136,152,3,4,2,0,137,152,3,10,5,0,138,152,3,42,21,0,139,
  	152,3,14,7,0,140,152,3,22,11,0,141,152,3,24,12,0,142,152,3,26,13,0,143,
  	152,3,28,14,0,144,152,3,30,15,0,145,152,3,38,19,0,146,152,3,66,33,0,147,
  	152,3,76,38,0,148,152,3,74,37,0,149,152,3,112,56,0,150,152,3,120,60,0,
  	151,135,1,0,0,0,151,136,1,0,0,0,151,137,1,0,0,0,151,138,1,0,0,0,151,139,
  	1,0,0,0,151,140,1,0,0,0,151,141,1,0,0,0,151,142,1,0,0,0,151,143,1,0,0,
  	0,151,144,1,0,0,0,151,145,1,0,0,0,151,146,1,0,0,0,151,147,1,0,0,0,151,
  	148,1,0,0,0,151,149,1,0,0,0,151,150,1,0,0,0,152,3,1,0,0,0,153,154,5,9,
  	0,0,154,156,5,52,0,0,155,157,3,6,3,0,156,155,1,0,0,0,156,157,1,0,0,0,
  	157,158,1,0,0,0,158,159,5,53,0,0,159,5,1,0,0,0,160,163,5,48,0,0,161,163,
  	3,8,4,0,162,160,1,0,0,0,162,161,1,0,0,0,163,7,1,0,0,0,164,165,5,54,0,
  	0,165,170,5,48,0,0,166,167,5,51,0,0,167,169,5,48,0,0,168,166,1,0,0,0,
  	169,172,1,0,0,0,170,168,1,0,0,0,170,171,1,0,0,0,171,173,1,0,0,0,172,170,
  	1,0,0,0,173,174,5,55,0,0,174,9,1,0,0,0,175,176,5,10,0,0,176,177,5,52,
  	0,0,177,178,3,12,6,0,178,179,5,53,0,0,179,11,1,0,0,0,180,181,5,48,0,0,
  	181,182,5,51,0,0,182,183,5,48,0,0,183,184,5,51,0,0,184,185,3,102,51,0,
  	185,13,1,0,0,0,186,187,5,12,0,0,187,188,5,52,0,0,188,189,3,16,8,0,189,
  	190,5,51,0,0,190,191,3,2,1,0,191,192,5,53,0,0,192,15,1,0,0,0,193,194,
  	5,54,0,0,194,199,3,18,9,0,195,196,5,51,0,0,196,198,3,18,9,0,197,195,1,
  	0,0,0,198,201,1,0,0,0,199,197,1,0,0,0,199,200,1,0,0,0,200,202,1,0,0,0,
  	201,199,1,0,0,0,202,203,5,55,0,0,203,17,1,0,0,0,204,213,3,104,52,0,205,
  	208,5,46,0,0,206,207,5,1,0,0,207,209,5,45,0,0,208,206,1,0,0,0,208,209,
  	1,0,0,0,209,210,1,0,0,0,210,211,5,49,0,0,211,213,3,20,10,0,212,204,1,
  	0,0,0,212,205,1,0,0,0,213,19,1,0,0,0,214,215,7,0,0,0,215,21,1,0,0,0,216,
  	217,5,13,0,0,217,218,5,52,0,0,218,219,5,43,0,0,219,220,5,51,0,0,220,221,
  	3,2,1,0,221,222,5,53,0,0,222,23,1,0,0,0,223,224,5,14,0,0,224,225,5,52,
  	0,0,225,226,3,2,1,0,226,227,5,51,0,0,227,228,3,2,1,0,228,229,5,53,0,0,
  	229,25,1,0,0,0,230,231,5,15,0,0,231,232,5,52,0,0,232,233,3,84,42,0,233,
  	234,5,51,0,0,234,235,3,2,1,0,235,236,5,51,0,0,236,237,3,2,1,0,237,238,
  	5,53,0,0,238,27,1,0,0,0,239,240,5,16,0,0,240,241,5,52,0,0,241,242,3,84,
  	42,0,242,243,5,51,0,0,243,244,3,2,1,0,244,245,5,51,0,0,245,246,3,2,1,
  	0,246,247,5,53,0,0,247,29,1,0,0,0,248,249,5,18,0,0,249,250,5,52,0,0,250,
  	251,3,32,16,0,251,252,5,51,0,0,252,255,5,48,0,0,253,254,5,51,0,0,254,
  	256,3,34,17,0,255,253,1,0,0,0,255,256,1,0,0,0,256,259,1,0,0,0,257,258,
  	5,51,0,0,258,260,3,36,18,0,259,257,1,0,0,0,259,260,1,0,0,0,260,261,1,
  	0,0,0,261,262,5,51,0,0,262,263,3,2,1,0,263,264,5,53,0,0,264,31,1,0,0,
  	0,265,266,7,1,0,0,266,33,1,0,0,0,267,268,5,43,0,0,268,269,5,51,0,0,269,
  	270,5,43,0,0,270,35,1,0,0,0,271,272,5,46,0,0,272,37,1,0,0,0,273,274,5,
  	17,0,0,274,275,5,52,0,0,275,276,3,40,20,0,276,277,5,51,0,0,277,278,5,
  	48,0,0,278,279,5,51,0,0,279,280,3,2,1,0,280,281,5,53,0,0,281,39,1,0,0,
  	0,282,283,7,2,0,0,283,41,1,0,0,0,284,285,5,11,0,0,285,286,5,52,0,0,286,
  	287,3,44,22,0,287,288,5,53,0,0,288,43,1,0,0,0,289,293,3,48,24,0,290,292,
  	3,46,23,0,291,290,1,0,0,0,292,295,1,0,0,0,293,291,1,0,0,0,293,294,1,0,
  	0,0,294,45,1,0,0,0,295,293,1,0,0,0,296,297,3,50,25,0,297,298,3,48,24,
  	0,298,47,1,0,0,0,299,301,5,52,0,0,300,302,5,45,0,0,301,300,1,0,0,0,301,
  	302,1,0,0,0,302,303,1,0,0,0,303,304,5,49,0,0,304,306,5,45,0,0,305,307,
  	3,124,62,0,306,305,1,0,0,0,306,307,1,0,0,0,307,308,1,0,0,0,308,309,5,
  	53,0,0,309,49,1,0,0,0,310,311,3,58,29,0,311,313,5,54,0,0,312,314,5,45,
  	0,0,313,312,1,0,0,0,313,314,1,0,0,0,314,315,1,0,0,0,315,316,5,49,0,0,
  	316,318,5,45,0,0,317,319,3,52,26,0,318,317,1,0,0,0,318,319,1,0,0,0,319,
  	320,1,0,0,0,320,321,5,55,0,0,321,322,3,58,29,0,322,51,1,0,0,0,323,324,
  	5,65,0,0,324,325,3,54,27,0,325,327,5,2,0,0,326,328,3,56,28,0,327,326,
  	1,0,0,0,327,328,1,0,0,0,328,53,1,0,0,0,329,330,5,43,0,0,330,55,1,0,0,
  	0,331,332,5,43,0,0,332,57,1,0,0,0,333,337,3,60,30,0,334,337,3,62,31,0,
  	335,337,3,64,32,0,336,333,1,0,0,0,336,334,1,0,0,0,336,335,1,0,0,0,337,
  	59,1,0,0,0,338,339,5,3,0,0,339,61,1,0,0,0,340,341,5,4,0,0,341,63,1,0,
  	0,0,342,343,5,5,0,0,343,65,1,0,0,0,344,345,5,19,0,0,345,346,5,52,0,0,
  	346,347,3,68,34,0,347,348,5,51,0,0,348,349,3,2,1,0,349,350,5,53,0,0,350,
  	67,1,0,0,0,351,352,5,54,0,0,352,357,3,70,35,0,353,354,5,51,0,0,354,356,
  	3,70,35,0,355,353,1,0,0,0,356,359,1,0,0,0,357,355,1,0,0,0,357,358,1,0,
  	0,0,358,360,1,0,0,0,359,357,1,0,0,0,360,361,5,55,0,0,361,69,1,0,0,0,362,
  	363,3,72,36,0,363,364,5,52,0,0,364,365,3,18,9,0,365,366,5,53,0,0,366,
  	71,1,0,0,0,367,368,7,3,0,0,368,73,1,0,0,0,369,370,5,23,0,0,370,371,5,
  	52,0,0,371,372,3,2,1,0,372,373,5,51,0,0,373,374,3,2,1,0,374,375,5,53,
  	0,0,375,75,1,0,0,0,376,377,5,20,0,0,377,378,5,52,0,0,378,379,3,78,39,
  	0,379,380,5,51,0,0,380,381,3,68,34,0,381,382,5,51,0,0,382,383,3,2,1,0,
  	383,384,5,53,0,0,384,77,1,0,0,0,385,386,5,54,0,0,386,391,3,80,40,0,387,
  	388,5,51,0,0,388,390,3,80,40,0,389,387,1,0,0,0,390,393,1,0,0,0,391,389,
  	1,0,0,0,391,392,1,0,0,0,392,394,1,0,0,0,393,391,1,0,0,0,394,395,5,55,
  	0,0,395,79,1,0,0,0,396,399,5,46,0,0,397,398,5,1,0,0,398,400,5,45,0,0,
  	399,397,1,0,0,0,399,400,1,0,0,0,400,401,1,0,0,0,401,402,5,49,0,0,402,
  	403,3,20,10,0,403,81,1,0,0,0,404,405,5,8,0,0,405,406,5,52,0,0,406,407,
  	3,84,42,0,407,408,5,51,0,0,408,409,3,2,1,0,409,410,5,53,0,0,410,83,1,
  	0,0,0,411,416,3,86,43,0,412,413,5,56,0,0,413,415,3,86,43,0,414,412,1,
  	0,0,0,415,418,1,0,0,0,416,414,1,0,0,0,416,417,1,0,0,0,417,85,1,0,0,0,
  	418,416,1,0,0,0,419,424,3,88,44,0,420,421,5,57,0,0,421,423,3,88,44,0,
  	422,420,1,0,0,0,423,426,1,0,0,0,424,422,1,0,0,0,424,425,1,0,0,0,425,87,
  	1,0,0,0,426,424,1,0,0,0,427,432,3,90,45,0,428,429,7,4,0,0,429,431,3,90,
  	45,0,430,428,1,0,0,0,431,434,1,0,0,0,432,430,1,0,0,0,432,433,1,0,0,0,
  	433,89,1,0,0,0,434,432,1,0,0,0,435,440,3,92,46,0,436,437,7,5,0,0,437,
  	439,3,92,46,0,438,436,1,0,0,0,439,442,1,0,0,0,440,438,1,0,0,0,440,441,
  	1,0,0,0,441,91,1,0,0,0,442,440,1,0,0,0,443,448,3,94,47,0,444,445,7,6,
  	0,0,445,447,3,94,47,0,446,444,1,0,0,0,447,450,1,0,0,0,448,446,1,0,0,0,
  	448,449,1,0,0,0,449,93,1,0,0,0,450,448,1,0,0,0,451,456,3,96,48,0,452,
  	453,7,7,0,0,453,455,3,96,48,0,454,452,1,0,0,0,455,458,1,0,0,0,456,454,
  	1,0,0,0,456,457,1,0,0,0,457,95,1,0,0,0,458,456,1,0,0,0,459,461,5,68,0,
  	0,460,459,1,0,0,0,460,461,1,0,0,0,461,462,1,0,0,0,462,463,3,98,49,0,463,
  	97,1,0,0,0,464,465,5,52,0,0,465,466,3,84,42,0,466,467,5,53,0,0,467,472,
  	1,0,0,0,468,472,3,104,52,0,469,472,3,102,51,0,470,472,3,100,50,0,471,
  	464,1,0,0,0,471,468,1,0,0,0,471,469,1,0,0,0,471,470,1,0,0,0,472,99,1,
  	0,0,0,473,474,5,46,0,0,474,475,5,1,0,0,475,476,5,45,0,0,476,101,1,0,0,
  	0,477,478,7,8,0,0,478,103,1,0,0,0,479,480,3,106,53,0,480,481,5,45,0,0,
  	481,483,5,52,0,0,482,484,3,108,54,0,483,482,1,0,0,0,483,484,1,0,0,0,484,
  	485,1,0,0,0,485,486,5,53,0,0,486,105,1,0,0,0,487,488,5,35,0,0,488,489,
  	5,50,0,0,489,107,1,0,0,0,490,495,3,110,55,0,491,492,5,51,0,0,492,494,
  	3,110,55,0,493,491,1,0,0,0,494,497,1,0,0,0,495,493,1,0,0,0,495,496,1,
  	0,0,0,496,109,1,0,0,0,497,495,1,0,0,0,498,507,3,102,51,0,499,502,5,46,
  	0,0,500,501,5,1,0,0,501,503,5,45,0,0,502,500,1,0,0,0,502,503,1,0,0,0,
  	503,504,1,0,0,0,504,505,5,49,0,0,505,507,3,20,10,0,506,498,1,0,0,0,506,
  	499,1,0,0,0,507,111,1,0,0,0,508,509,5,21,0,0,509,510,5,52,0,0,510,511,
  	3,114,57,0,511,512,5,51,0,0,512,513,3,2,1,0,513,514,5,53,0,0,514,113,
  	1,0,0,0,515,516,5,54,0,0,516,521,3,116,58,0,517,518,5,51,0,0,518,520,
  	3,116,58,0,519,517,1,0,0,0,520,523,1,0,0,0,521,519,1,0,0,0,521,522,1,
  	0,0,0,522,524,1,0,0,0,523,521,1,0,0,0,524,525,5,55,0,0,525,115,1,0,0,
  	0,526,527,5,46,0,0,527,528,5,49,0,0,528,529,3,20,10,0,529,530,3,118,59,
  	0,530,117,1,0,0,0,531,532,7,9,0,0,532,119,1,0,0,0,533,534,5,22,0,0,534,
  	537,5,52,0,0,535,538,3,128,64,0,536,538,3,122,61,0,537,535,1,0,0,0,537,
  	536,1,0,0,0,538,541,1,0,0,0,539,540,5,51,0,0,540,542,3,2,1,0,541,539,
  	1,0,0,0,541,542,1,0,0,0,542,543,1,0,0,0,543,544,5,53,0,0,544,121,1,0,
  	0,0,545,546,5,52,0,0,546,547,5,45,0,0,547,548,5,49,0,0,548,550,5,45,0,
  	0,549,551,3,124,62,0,550,549,1,0,0,0,550,551,1,0,0,0,551,552,1,0,0,0,
  	552,553,5,53,0,0,553,123,1,0,0,0,554,555,5,6,0,0,555,560,3,126,63,0,556,
  	557,5,51,0,0,557,559,3,126,63,0,558,556,1,0,0,0,559,562,1,0,0,0,560,558,
  	1,0,0,0,560,561,1,0,0,0,561,563,1,0,0,0,562,560,1,0,0,0,563,564,5,7,0,
  	0,564,125,1,0,0,0,565,566,5,45,0,0,566,567,5,49,0,0,567,568,3,102,51,
  	0,568,127,1,0,0,0,569,570,3,130,65,0,570,571,5,5,0,0,571,573,5,54,0,0,
  	572,574,5,45,0,0,573,572,1,0,0,0,573,574,1,0,0,0,574,575,1,0,0,0,575,
  	576,5,49,0,0,576,578,5,45,0,0,577,579,3,124,62,0,578,577,1,0,0,0,578,
  	579,1,0,0,0,579,580,1,0,0,0,580,581,5,55,0,0,581,582,5,4,0,0,582,583,
  	3,130,65,0,583,129,1,0,0,0,584,585,5,52,0,0,585,586,5,46,0,0,586,587,
  	5,53,0,0,587,131,1,0,0,0,38,151,156,162,170,199,208,212,255,259,293,301,
  	306,313,318,327,336,357,391,399,416,424,432,440,448,456,460,471,483,495,
  	502,506,521,537,541,550,560,573,578
  };
  staticData->serializedATN = antlr4::atn::SerializedATNView(serializedATNSegment, sizeof(serializedATNSegment) / sizeof(serializedATNSegment[0]));

  antlr4::atn::ATNDeserializer deserializer;
  staticData->atn = deserializer.deserialize(staticData->serializedATN);

  const size_t count = staticData->atn->getNumberOfDecisions();
  staticData->decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    staticData->decisionToDFA.emplace_back(staticData->atn->getDecisionState(i), i);
  }
  poseidonParserStaticData = staticData.release();
}

}

poseidonParser::poseidonParser(TokenStream *input) : poseidonParser(input, antlr4::atn::ParserATNSimulatorOptions()) {}

poseidonParser::poseidonParser(TokenStream *input, const antlr4::atn::ParserATNSimulatorOptions &options) : Parser(input) {
  poseidonParser::initialize();
  _interpreter = new atn::ParserATNSimulator(this, *poseidonParserStaticData->atn, poseidonParserStaticData->decisionToDFA, poseidonParserStaticData->sharedContextCache, options);
}

poseidonParser::~poseidonParser() {
  delete _interpreter;
}

const atn::ATN& poseidonParser::getATN() const {
  return *poseidonParserStaticData->atn;
}

std::string poseidonParser::getGrammarFileName() const {
  return "poseidon.g4";
}

const std::vector<std::string>& poseidonParser::getRuleNames() const {
  return poseidonParserStaticData->ruleNames;
}

const dfa::Vocabulary& poseidonParser::getVocabulary() const {
  return poseidonParserStaticData->vocabulary;
}

antlr4::atn::SerializedATNView poseidonParser::getSerializedATN() const {
  return poseidonParserStaticData->serializedATN;
}


//----------------- QueryContext ------------------------------------------------------------------

poseidonParser::QueryContext::QueryContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

poseidonParser::Query_operatorContext* poseidonParser::QueryContext::query_operator() {
  return getRuleContext<poseidonParser::Query_operatorContext>(0);
}

tree::TerminalNode* poseidonParser::QueryContext::EOF() {
  return getToken(poseidonParser::EOF, 0);
}


size_t poseidonParser::QueryContext::getRuleIndex() const {
  return poseidonParser::RuleQuery;
}


std::any poseidonParser::QueryContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<poseidonVisitor*>(visitor))
    return parserVisitor->visitQuery(this);
  else
    return visitor->visitChildren(this);
}

poseidonParser::QueryContext* poseidonParser::query() {
  QueryContext *_localctx = _tracker.createInstance<QueryContext>(_ctx, getState());
  enterRule(_localctx, 0, poseidonParser::RuleQuery);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(132);
    query_operator();
    setState(133);
    match(poseidonParser::EOF);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Query_operatorContext ------------------------------------------------------------------

poseidonParser::Query_operatorContext::Query_operatorContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

poseidonParser::Filter_opContext* poseidonParser::Query_operatorContext::filter_op() {
  return getRuleContext<poseidonParser::Filter_opContext>(0);
}

poseidonParser::Node_scan_opContext* poseidonParser::Query_operatorContext::node_scan_op() {
  return getRuleContext<poseidonParser::Node_scan_opContext>(0);
}

poseidonParser::Index_scan_opContext* poseidonParser::Query_operatorContext::index_scan_op() {
  return getRuleContext<poseidonParser::Index_scan_opContext>(0);
}

poseidonParser::Match_opContext* poseidonParser::Query_operatorContext::match_op() {
  return getRuleContext<poseidonParser::Match_opContext>(0);
}

poseidonParser::Project_opContext* poseidonParser::Query_operatorContext::project_op() {
  return getRuleContext<poseidonParser::Project_opContext>(0);
}

poseidonParser::Limit_opContext* poseidonParser::Query_operatorContext::limit_op() {
  return getRuleContext<poseidonParser::Limit_opContext>(0);
}

poseidonParser::Crossjoin_opContext* poseidonParser::Query_operatorContext::crossjoin_op() {
  return getRuleContext<poseidonParser::Crossjoin_opContext>(0);
}

poseidonParser::Hashjoin_opContext* poseidonParser::Query_operatorContext::hashjoin_op() {
  return getRuleContext<poseidonParser::Hashjoin_opContext>(0);
}

poseidonParser::Leftouterjoin_opContext* poseidonParser::Query_operatorContext::leftouterjoin_op() {
  return getRuleContext<poseidonParser::Leftouterjoin_opContext>(0);
}

poseidonParser::Foreach_relationship_opContext* poseidonParser::Query_operatorContext::foreach_relationship_op() {
  return getRuleContext<poseidonParser::Foreach_relationship_opContext>(0);
}

poseidonParser::Expand_opContext* poseidonParser::Query_operatorContext::expand_op() {
  return getRuleContext<poseidonParser::Expand_opContext>(0);
}

poseidonParser::Aggregate_opContext* poseidonParser::Query_operatorContext::aggregate_op() {
  return getRuleContext<poseidonParser::Aggregate_opContext>(0);
}

poseidonParser::Group_by_opContext* poseidonParser::Query_operatorContext::group_by_op() {
  return getRuleContext<poseidonParser::Group_by_opContext>(0);
}

poseidonParser::Union_opContext* poseidonParser::Query_operatorContext::union_op() {
  return getRuleContext<poseidonParser::Union_opContext>(0);
}

poseidonParser::Sort_opContext* poseidonParser::Query_operatorContext::sort_op() {
  return getRuleContext<poseidonParser::Sort_opContext>(0);
}

poseidonParser::Create_opContext* poseidonParser::Query_operatorContext::create_op() {
  return getRuleContext<poseidonParser::Create_opContext>(0);
}


size_t poseidonParser::Query_operatorContext::getRuleIndex() const {
  return poseidonParser::RuleQuery_operator;
}


std::any poseidonParser::Query_operatorContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<poseidonVisitor*>(visitor))
    return parserVisitor->visitQuery_operator(this);
  else
    return visitor->visitChildren(this);
}

poseidonParser::Query_operatorContext* poseidonParser::query_operator() {
  Query_operatorContext *_localctx = _tracker.createInstance<Query_operatorContext>(_ctx, getState());
  enterRule(_localctx, 2, poseidonParser::RuleQuery_operator);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(151);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case poseidonParser::Filter_: {
        enterOuterAlt(_localctx, 1);
        setState(135);
        filter_op();
        break;
      }

      case poseidonParser::Nodescan_: {
        enterOuterAlt(_localctx, 2);
        setState(136);
        node_scan_op();
        break;
      }

      case poseidonParser::Indexscan_: {
        enterOuterAlt(_localctx, 3);
        setState(137);
        index_scan_op();
        break;
      }

      case poseidonParser::Match_: {
        enterOuterAlt(_localctx, 4);
        setState(138);
        match_op();
        break;
      }

      case poseidonParser::Project_: {
        enterOuterAlt(_localctx, 5);
        setState(139);
        project_op();
        break;
      }

      case poseidonParser::Limit_: {
        enterOuterAlt(_localctx, 6);
        setState(140);
        limit_op();
        break;
      }

      case poseidonParser::CrossJoin_: {
        enterOuterAlt(_localctx, 7);
        setState(141);
        crossjoin_op();
        break;
      }

      case poseidonParser::HashJoin_: {
        enterOuterAlt(_localctx, 8);
        setState(142);
        hashjoin_op();
        break;
      }

      case poseidonParser::LeftOuterJoin_: {
        enterOuterAlt(_localctx, 9);
        setState(143);
        leftouterjoin_op();
        break;
      }

      case poseidonParser::ForeachRelationship_: {
        enterOuterAlt(_localctx, 10);
        setState(144);
        foreach_relationship_op();
        break;
      }

      case poseidonParser::Expand_: {
        enterOuterAlt(_localctx, 11);
        setState(145);
        expand_op();
        break;
      }

      case poseidonParser::Aggregate_: {
        enterOuterAlt(_localctx, 12);
        setState(146);
        aggregate_op();
        break;
      }

      case poseidonParser::GroupBy_: {
        enterOuterAlt(_localctx, 13);
        setState(147);
        group_by_op();
        break;
      }

      case poseidonParser::Union_: {
        enterOuterAlt(_localctx, 14);
        setState(148);
        union_op();
        break;
      }

      case poseidonParser::Sort_: {
        enterOuterAlt(_localctx, 15);
        setState(149);
        sort_op();
        break;
      }

      case poseidonParser::Create_: {
        enterOuterAlt(_localctx, 16);
        setState(150);
        create_op();
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Node_scan_opContext ------------------------------------------------------------------

poseidonParser::Node_scan_opContext::Node_scan_opContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* poseidonParser::Node_scan_opContext::Nodescan_() {
  return getToken(poseidonParser::Nodescan_, 0);
}

tree::TerminalNode* poseidonParser::Node_scan_opContext::LPAREN() {
  return getToken(poseidonParser::LPAREN, 0);
}

tree::TerminalNode* poseidonParser::Node_scan_opContext::RPAREN() {
  return getToken(poseidonParser::RPAREN, 0);
}

poseidonParser::Scan_paramContext* poseidonParser::Node_scan_opContext::scan_param() {
  return getRuleContext<poseidonParser::Scan_paramContext>(0);
}


size_t poseidonParser::Node_scan_opContext::getRuleIndex() const {
  return poseidonParser::RuleNode_scan_op;
}


std::any poseidonParser::Node_scan_opContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<poseidonVisitor*>(visitor))
    return parserVisitor->visitNode_scan_op(this);
  else
    return visitor->visitChildren(this);
}

poseidonParser::Node_scan_opContext* poseidonParser::node_scan_op() {
  Node_scan_opContext *_localctx = _tracker.createInstance<Node_scan_opContext>(_ctx, getState());
  enterRule(_localctx, 4, poseidonParser::RuleNode_scan_op);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(153);
    match(poseidonParser::Nodescan_);
    setState(154);
    match(poseidonParser::LPAREN);
    setState(156);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == poseidonParser::STRING_

    || _la == poseidonParser::LBRACKET) {
      setState(155);
      scan_param();
    }
    setState(158);
    match(poseidonParser::RPAREN);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Scan_paramContext ------------------------------------------------------------------

poseidonParser::Scan_paramContext::Scan_paramContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* poseidonParser::Scan_paramContext::STRING_() {
  return getToken(poseidonParser::STRING_, 0);
}

poseidonParser::Scan_listContext* poseidonParser::Scan_paramContext::scan_list() {
  return getRuleContext<poseidonParser::Scan_listContext>(0);
}


size_t poseidonParser::Scan_paramContext::getRuleIndex() const {
  return poseidonParser::RuleScan_param;
}


std::any poseidonParser::Scan_paramContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<poseidonVisitor*>(visitor))
    return parserVisitor->visitScan_param(this);
  else
    return visitor->visitChildren(this);
}

poseidonParser::Scan_paramContext* poseidonParser::scan_param() {
  Scan_paramContext *_localctx = _tracker.createInstance<Scan_paramContext>(_ctx, getState());
  enterRule(_localctx, 6, poseidonParser::RuleScan_param);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(162);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case poseidonParser::STRING_: {
        enterOuterAlt(_localctx, 1);
        setState(160);
        match(poseidonParser::STRING_);
        break;
      }

      case poseidonParser::LBRACKET: {
        enterOuterAlt(_localctx, 2);
        setState(161);
        scan_list();
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Scan_listContext ------------------------------------------------------------------

poseidonParser::Scan_listContext::Scan_listContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* poseidonParser::Scan_listContext::LBRACKET() {
  return getToken(poseidonParser::LBRACKET, 0);
}

std::vector<tree::TerminalNode *> poseidonParser::Scan_listContext::STRING_() {
  return getTokens(poseidonParser::STRING_);
}

tree::TerminalNode* poseidonParser::Scan_listContext::STRING_(size_t i) {
  return getToken(poseidonParser::STRING_, i);
}

tree::TerminalNode* poseidonParser::Scan_listContext::RBRACKET() {
  return getToken(poseidonParser::RBRACKET, 0);
}

std::vector<tree::TerminalNode *> poseidonParser::Scan_listContext::COMMA_() {
  return getTokens(poseidonParser::COMMA_);
}

tree::TerminalNode* poseidonParser::Scan_listContext::COMMA_(size_t i) {
  return getToken(poseidonParser::COMMA_, i);
}


size_t poseidonParser::Scan_listContext::getRuleIndex() const {
  return poseidonParser::RuleScan_list;
}


std::any poseidonParser::Scan_listContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<poseidonVisitor*>(visitor))
    return parserVisitor->visitScan_list(this);
  else
    return visitor->visitChildren(this);
}

poseidonParser::Scan_listContext* poseidonParser::scan_list() {
  Scan_listContext *_localctx = _tracker.createInstance<Scan_listContext>(_ctx, getState());
  enterRule(_localctx, 8, poseidonParser::RuleScan_list);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(164);
    match(poseidonParser::LBRACKET);
    setState(165);
    match(poseidonParser::STRING_);
    setState(170);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == poseidonParser::COMMA_) {
      setState(166);
      match(poseidonParser::COMMA_);
      setState(167);
      match(poseidonParser::STRING_);
      setState(172);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(173);
    match(poseidonParser::RBRACKET);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Index_scan_opContext ------------------------------------------------------------------

poseidonParser::Index_scan_opContext::Index_scan_opContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* poseidonParser::Index_scan_opContext::Indexscan_() {
  return getToken(poseidonParser::Indexscan_, 0);
}

tree::TerminalNode* poseidonParser::Index_scan_opContext::LPAREN() {
  return getToken(poseidonParser::LPAREN, 0);
}

poseidonParser::Index_scan_paramContext* poseidonParser::Index_scan_opContext::index_scan_param() {
  return getRuleContext<poseidonParser::Index_scan_paramContext>(0);
}

tree::TerminalNode* poseidonParser::Index_scan_opContext::RPAREN() {
  return getToken(poseidonParser::RPAREN, 0);
}


size_t poseidonParser::Index_scan_opContext::getRuleIndex() const {
  return poseidonParser::RuleIndex_scan_op;
}


std::any poseidonParser::Index_scan_opContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<poseidonVisitor*>(visitor))
    return parserVisitor->visitIndex_scan_op(this);
  else
    return visitor->visitChildren(this);
}

poseidonParser::Index_scan_opContext* poseidonParser::index_scan_op() {
  Index_scan_opContext *_localctx = _tracker.createInstance<Index_scan_opContext>(_ctx, getState());
  enterRule(_localctx, 10, poseidonParser::RuleIndex_scan_op);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(175);
    match(poseidonParser::Indexscan_);
    setState(176);
    match(poseidonParser::LPAREN);
    setState(177);
    index_scan_param();
    setState(178);
    match(poseidonParser::RPAREN);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Index_scan_paramContext ------------------------------------------------------------------

poseidonParser::Index_scan_paramContext::Index_scan_paramContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<tree::TerminalNode *> poseidonParser::Index_scan_paramContext::STRING_() {
  return getTokens(poseidonParser::STRING_);
}

tree::TerminalNode* poseidonParser::Index_scan_paramContext::STRING_(size_t i) {
  return getToken(poseidonParser::STRING_, i);
}

std::vector<tree::TerminalNode *> poseidonParser::Index_scan_paramContext::COMMA_() {
  return getTokens(poseidonParser::COMMA_);
}

tree::TerminalNode* poseidonParser::Index_scan_paramContext::COMMA_(size_t i) {
  return getToken(poseidonParser::COMMA_, i);
}

poseidonParser::ValueContext* poseidonParser::Index_scan_paramContext::value() {
  return getRuleContext<poseidonParser::ValueContext>(0);
}


size_t poseidonParser::Index_scan_paramContext::getRuleIndex() const {
  return poseidonParser::RuleIndex_scan_param;
}


std::any poseidonParser::Index_scan_paramContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<poseidonVisitor*>(visitor))
    return parserVisitor->visitIndex_scan_param(this);
  else
    return visitor->visitChildren(this);
}

poseidonParser::Index_scan_paramContext* poseidonParser::index_scan_param() {
  Index_scan_paramContext *_localctx = _tracker.createInstance<Index_scan_paramContext>(_ctx, getState());
  enterRule(_localctx, 12, poseidonParser::RuleIndex_scan_param);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(180);
    match(poseidonParser::STRING_);
    setState(181);
    match(poseidonParser::COMMA_);
    setState(182);
    match(poseidonParser::STRING_);
    setState(183);
    match(poseidonParser::COMMA_);
    setState(184);
    value();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Project_opContext ------------------------------------------------------------------

poseidonParser::Project_opContext::Project_opContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* poseidonParser::Project_opContext::Project_() {
  return getToken(poseidonParser::Project_, 0);
}

tree::TerminalNode* poseidonParser::Project_opContext::LPAREN() {
  return getToken(poseidonParser::LPAREN, 0);
}

poseidonParser::Proj_listContext* poseidonParser::Project_opContext::proj_list() {
  return getRuleContext<poseidonParser::Proj_listContext>(0);
}

tree::TerminalNode* poseidonParser::Project_opContext::COMMA_() {
  return getToken(poseidonParser::COMMA_, 0);
}

poseidonParser::Query_operatorContext* poseidonParser::Project_opContext::query_operator() {
  return getRuleContext<poseidonParser::Query_operatorContext>(0);
}

tree::TerminalNode* poseidonParser::Project_opContext::RPAREN() {
  return getToken(poseidonParser::RPAREN, 0);
}


size_t poseidonParser::Project_opContext::getRuleIndex() const {
  return poseidonParser::RuleProject_op;
}


std::any poseidonParser::Project_opContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<poseidonVisitor*>(visitor))
    return parserVisitor->visitProject_op(this);
  else
    return visitor->visitChildren(this);
}

poseidonParser::Project_opContext* poseidonParser::project_op() {
  Project_opContext *_localctx = _tracker.createInstance<Project_opContext>(_ctx, getState());
  enterRule(_localctx, 14, poseidonParser::RuleProject_op);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(186);
    match(poseidonParser::Project_);
    setState(187);
    match(poseidonParser::LPAREN);
    setState(188);
    proj_list();
    setState(189);
    match(poseidonParser::COMMA_);
    setState(190);
    query_operator();
    setState(191);
    match(poseidonParser::RPAREN);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Proj_listContext ------------------------------------------------------------------

poseidonParser::Proj_listContext::Proj_listContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* poseidonParser::Proj_listContext::LBRACKET() {
  return getToken(poseidonParser::LBRACKET, 0);
}

std::vector<poseidonParser::Proj_exprContext *> poseidonParser::Proj_listContext::proj_expr() {
  return getRuleContexts<poseidonParser::Proj_exprContext>();
}

poseidonParser::Proj_exprContext* poseidonParser::Proj_listContext::proj_expr(size_t i) {
  return getRuleContext<poseidonParser::Proj_exprContext>(i);
}

tree::TerminalNode* poseidonParser::Proj_listContext::RBRACKET() {
  return getToken(poseidonParser::RBRACKET, 0);
}

std::vector<tree::TerminalNode *> poseidonParser::Proj_listContext::COMMA_() {
  return getTokens(poseidonParser::COMMA_);
}

tree::TerminalNode* poseidonParser::Proj_listContext::COMMA_(size_t i) {
  return getToken(poseidonParser::COMMA_, i);
}


size_t poseidonParser::Proj_listContext::getRuleIndex() const {
  return poseidonParser::RuleProj_list;
}


std::any poseidonParser::Proj_listContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<poseidonVisitor*>(visitor))
    return parserVisitor->visitProj_list(this);
  else
    return visitor->visitChildren(this);
}

poseidonParser::Proj_listContext* poseidonParser::proj_list() {
  Proj_listContext *_localctx = _tracker.createInstance<Proj_listContext>(_ctx, getState());
  enterRule(_localctx, 16, poseidonParser::RuleProj_list);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(193);
    match(poseidonParser::LBRACKET);
    setState(194);
    proj_expr();
    setState(199);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == poseidonParser::COMMA_) {
      setState(195);
      match(poseidonParser::COMMA_);
      setState(196);
      proj_expr();
      setState(201);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(202);
    match(poseidonParser::RBRACKET);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Proj_exprContext ------------------------------------------------------------------

poseidonParser::Proj_exprContext::Proj_exprContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

poseidonParser::Function_callContext* poseidonParser::Proj_exprContext::function_call() {
  return getRuleContext<poseidonParser::Function_callContext>(0);
}

tree::TerminalNode* poseidonParser::Proj_exprContext::Var() {
  return getToken(poseidonParser::Var, 0);
}

tree::TerminalNode* poseidonParser::Proj_exprContext::COLON_() {
  return getToken(poseidonParser::COLON_, 0);
}

poseidonParser::Type_specContext* poseidonParser::Proj_exprContext::type_spec() {
  return getRuleContext<poseidonParser::Type_specContext>(0);
}

tree::TerminalNode* poseidonParser::Proj_exprContext::Identifier_() {
  return getToken(poseidonParser::Identifier_, 0);
}


size_t poseidonParser::Proj_exprContext::getRuleIndex() const {
  return poseidonParser::RuleProj_expr;
}


std::any poseidonParser::Proj_exprContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<poseidonVisitor*>(visitor))
    return parserVisitor->visitProj_expr(this);
  else
    return visitor->visitChildren(this);
}

poseidonParser::Proj_exprContext* poseidonParser::proj_expr() {
  Proj_exprContext *_localctx = _tracker.createInstance<Proj_exprContext>(_ctx, getState());
  enterRule(_localctx, 18, poseidonParser::RuleProj_expr);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(212);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case poseidonParser::UDF_: {
        enterOuterAlt(_localctx, 1);
        setState(204);
        function_call();
        break;
      }

      case poseidonParser::Var: {
        enterOuterAlt(_localctx, 2);
        setState(205);
        match(poseidonParser::Var);
        setState(208);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (_la == poseidonParser::T__0) {
          setState(206);
          match(poseidonParser::T__0);
          setState(207);
          match(poseidonParser::Identifier_);
        }
        setState(210);
        match(poseidonParser::COLON_);
        setState(211);
        type_spec();
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Type_specContext ------------------------------------------------------------------

poseidonParser::Type_specContext::Type_specContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* poseidonParser::Type_specContext::IntType_() {
  return getToken(poseidonParser::IntType_, 0);
}

tree::TerminalNode* poseidonParser::Type_specContext::DoubleType_() {
  return getToken(poseidonParser::DoubleType_, 0);
}

tree::TerminalNode* poseidonParser::Type_specContext::Uint64Type_() {
  return getToken(poseidonParser::Uint64Type_, 0);
}

tree::TerminalNode* poseidonParser::Type_specContext::StringType_() {
  return getToken(poseidonParser::StringType_, 0);
}

tree::TerminalNode* poseidonParser::Type_specContext::DateType_() {
  return getToken(poseidonParser::DateType_, 0);
}

tree::TerminalNode* poseidonParser::Type_specContext::ResultType_() {
  return getToken(poseidonParser::ResultType_, 0);
}


size_t poseidonParser::Type_specContext::getRuleIndex() const {
  return poseidonParser::RuleType_spec;
}


std::any poseidonParser::Type_specContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<poseidonVisitor*>(visitor))
    return parserVisitor->visitType_spec(this);
  else
    return visitor->visitChildren(this);
}

poseidonParser::Type_specContext* poseidonParser::type_spec() {
  Type_specContext *_localctx = _tracker.createInstance<Type_specContext>(_ctx, getState());
  enterRule(_localctx, 20, poseidonParser::RuleType_spec);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(214);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 1056964608) != 0))) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Limit_opContext ------------------------------------------------------------------

poseidonParser::Limit_opContext::Limit_opContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* poseidonParser::Limit_opContext::Limit_() {
  return getToken(poseidonParser::Limit_, 0);
}

tree::TerminalNode* poseidonParser::Limit_opContext::LPAREN() {
  return getToken(poseidonParser::LPAREN, 0);
}

tree::TerminalNode* poseidonParser::Limit_opContext::INTEGER() {
  return getToken(poseidonParser::INTEGER, 0);
}

tree::TerminalNode* poseidonParser::Limit_opContext::COMMA_() {
  return getToken(poseidonParser::COMMA_, 0);
}

poseidonParser::Query_operatorContext* poseidonParser::Limit_opContext::query_operator() {
  return getRuleContext<poseidonParser::Query_operatorContext>(0);
}

tree::TerminalNode* poseidonParser::Limit_opContext::RPAREN() {
  return getToken(poseidonParser::RPAREN, 0);
}


size_t poseidonParser::Limit_opContext::getRuleIndex() const {
  return poseidonParser::RuleLimit_op;
}


std::any poseidonParser::Limit_opContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<poseidonVisitor*>(visitor))
    return parserVisitor->visitLimit_op(this);
  else
    return visitor->visitChildren(this);
}

poseidonParser::Limit_opContext* poseidonParser::limit_op() {
  Limit_opContext *_localctx = _tracker.createInstance<Limit_opContext>(_ctx, getState());
  enterRule(_localctx, 22, poseidonParser::RuleLimit_op);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(216);
    match(poseidonParser::Limit_);
    setState(217);
    match(poseidonParser::LPAREN);
    setState(218);
    match(poseidonParser::INTEGER);
    setState(219);
    match(poseidonParser::COMMA_);
    setState(220);
    query_operator();
    setState(221);
    match(poseidonParser::RPAREN);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Crossjoin_opContext ------------------------------------------------------------------

poseidonParser::Crossjoin_opContext::Crossjoin_opContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* poseidonParser::Crossjoin_opContext::CrossJoin_() {
  return getToken(poseidonParser::CrossJoin_, 0);
}

tree::TerminalNode* poseidonParser::Crossjoin_opContext::LPAREN() {
  return getToken(poseidonParser::LPAREN, 0);
}

std::vector<poseidonParser::Query_operatorContext *> poseidonParser::Crossjoin_opContext::query_operator() {
  return getRuleContexts<poseidonParser::Query_operatorContext>();
}

poseidonParser::Query_operatorContext* poseidonParser::Crossjoin_opContext::query_operator(size_t i) {
  return getRuleContext<poseidonParser::Query_operatorContext>(i);
}

tree::TerminalNode* poseidonParser::Crossjoin_opContext::COMMA_() {
  return getToken(poseidonParser::COMMA_, 0);
}

tree::TerminalNode* poseidonParser::Crossjoin_opContext::RPAREN() {
  return getToken(poseidonParser::RPAREN, 0);
}


size_t poseidonParser::Crossjoin_opContext::getRuleIndex() const {
  return poseidonParser::RuleCrossjoin_op;
}


std::any poseidonParser::Crossjoin_opContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<poseidonVisitor*>(visitor))
    return parserVisitor->visitCrossjoin_op(this);
  else
    return visitor->visitChildren(this);
}

poseidonParser::Crossjoin_opContext* poseidonParser::crossjoin_op() {
  Crossjoin_opContext *_localctx = _tracker.createInstance<Crossjoin_opContext>(_ctx, getState());
  enterRule(_localctx, 24, poseidonParser::RuleCrossjoin_op);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(223);
    match(poseidonParser::CrossJoin_);
    setState(224);
    match(poseidonParser::LPAREN);
    setState(225);
    query_operator();
    setState(226);
    match(poseidonParser::COMMA_);
    setState(227);
    query_operator();
    setState(228);
    match(poseidonParser::RPAREN);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Hashjoin_opContext ------------------------------------------------------------------

poseidonParser::Hashjoin_opContext::Hashjoin_opContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* poseidonParser::Hashjoin_opContext::HashJoin_() {
  return getToken(poseidonParser::HashJoin_, 0);
}

tree::TerminalNode* poseidonParser::Hashjoin_opContext::LPAREN() {
  return getToken(poseidonParser::LPAREN, 0);
}

poseidonParser::Logical_exprContext* poseidonParser::Hashjoin_opContext::logical_expr() {
  return getRuleContext<poseidonParser::Logical_exprContext>(0);
}

std::vector<tree::TerminalNode *> poseidonParser::Hashjoin_opContext::COMMA_() {
  return getTokens(poseidonParser::COMMA_);
}

tree::TerminalNode* poseidonParser::Hashjoin_opContext::COMMA_(size_t i) {
  return getToken(poseidonParser::COMMA_, i);
}

std::vector<poseidonParser::Query_operatorContext *> poseidonParser::Hashjoin_opContext::query_operator() {
  return getRuleContexts<poseidonParser::Query_operatorContext>();
}

poseidonParser::Query_operatorContext* poseidonParser::Hashjoin_opContext::query_operator(size_t i) {
  return getRuleContext<poseidonParser::Query_operatorContext>(i);
}

tree::TerminalNode* poseidonParser::Hashjoin_opContext::RPAREN() {
  return getToken(poseidonParser::RPAREN, 0);
}


size_t poseidonParser::Hashjoin_opContext::getRuleIndex() const {
  return poseidonParser::RuleHashjoin_op;
}


std::any poseidonParser::Hashjoin_opContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<poseidonVisitor*>(visitor))
    return parserVisitor->visitHashjoin_op(this);
  else
    return visitor->visitChildren(this);
}

poseidonParser::Hashjoin_opContext* poseidonParser::hashjoin_op() {
  Hashjoin_opContext *_localctx = _tracker.createInstance<Hashjoin_opContext>(_ctx, getState());
  enterRule(_localctx, 26, poseidonParser::RuleHashjoin_op);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(230);
    match(poseidonParser::HashJoin_);
    setState(231);
    match(poseidonParser::LPAREN);
    setState(232);
    logical_expr();
    setState(233);
    match(poseidonParser::COMMA_);
    setState(234);
    query_operator();
    setState(235);
    match(poseidonParser::COMMA_);
    setState(236);
    query_operator();
    setState(237);
    match(poseidonParser::RPAREN);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Leftouterjoin_opContext ------------------------------------------------------------------

poseidonParser::Leftouterjoin_opContext::Leftouterjoin_opContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* poseidonParser::Leftouterjoin_opContext::LeftOuterJoin_() {
  return getToken(poseidonParser::LeftOuterJoin_, 0);
}

tree::TerminalNode* poseidonParser::Leftouterjoin_opContext::LPAREN() {
  return getToken(poseidonParser::LPAREN, 0);
}

poseidonParser::Logical_exprContext* poseidonParser::Leftouterjoin_opContext::logical_expr() {
  return getRuleContext<poseidonParser::Logical_exprContext>(0);
}

std::vector<tree::TerminalNode *> poseidonParser::Leftouterjoin_opContext::COMMA_() {
  return getTokens(poseidonParser::COMMA_);
}

tree::TerminalNode* poseidonParser::Leftouterjoin_opContext::COMMA_(size_t i) {
  return getToken(poseidonParser::COMMA_, i);
}

std::vector<poseidonParser::Query_operatorContext *> poseidonParser::Leftouterjoin_opContext::query_operator() {
  return getRuleContexts<poseidonParser::Query_operatorContext>();
}

poseidonParser::Query_operatorContext* poseidonParser::Leftouterjoin_opContext::query_operator(size_t i) {
  return getRuleContext<poseidonParser::Query_operatorContext>(i);
}

tree::TerminalNode* poseidonParser::Leftouterjoin_opContext::RPAREN() {
  return getToken(poseidonParser::RPAREN, 0);
}


size_t poseidonParser::Leftouterjoin_opContext::getRuleIndex() const {
  return poseidonParser::RuleLeftouterjoin_op;
}


std::any poseidonParser::Leftouterjoin_opContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<poseidonVisitor*>(visitor))
    return parserVisitor->visitLeftouterjoin_op(this);
  else
    return visitor->visitChildren(this);
}

poseidonParser::Leftouterjoin_opContext* poseidonParser::leftouterjoin_op() {
  Leftouterjoin_opContext *_localctx = _tracker.createInstance<Leftouterjoin_opContext>(_ctx, getState());
  enterRule(_localctx, 28, poseidonParser::RuleLeftouterjoin_op);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(239);
    match(poseidonParser::LeftOuterJoin_);
    setState(240);
    match(poseidonParser::LPAREN);
    setState(241);
    logical_expr();
    setState(242);
    match(poseidonParser::COMMA_);
    setState(243);
    query_operator();
    setState(244);
    match(poseidonParser::COMMA_);
    setState(245);
    query_operator();
    setState(246);
    match(poseidonParser::RPAREN);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Foreach_relationship_opContext ------------------------------------------------------------------

poseidonParser::Foreach_relationship_opContext::Foreach_relationship_opContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* poseidonParser::Foreach_relationship_opContext::ForeachRelationship_() {
  return getToken(poseidonParser::ForeachRelationship_, 0);
}

tree::TerminalNode* poseidonParser::Foreach_relationship_opContext::LPAREN() {
  return getToken(poseidonParser::LPAREN, 0);
}

poseidonParser::Rship_dirContext* poseidonParser::Foreach_relationship_opContext::rship_dir() {
  return getRuleContext<poseidonParser::Rship_dirContext>(0);
}

std::vector<tree::TerminalNode *> poseidonParser::Foreach_relationship_opContext::COMMA_() {
  return getTokens(poseidonParser::COMMA_);
}

tree::TerminalNode* poseidonParser::Foreach_relationship_opContext::COMMA_(size_t i) {
  return getToken(poseidonParser::COMMA_, i);
}

tree::TerminalNode* poseidonParser::Foreach_relationship_opContext::STRING_() {
  return getToken(poseidonParser::STRING_, 0);
}

poseidonParser::Query_operatorContext* poseidonParser::Foreach_relationship_opContext::query_operator() {
  return getRuleContext<poseidonParser::Query_operatorContext>(0);
}

tree::TerminalNode* poseidonParser::Foreach_relationship_opContext::RPAREN() {
  return getToken(poseidonParser::RPAREN, 0);
}

poseidonParser::Rship_cardinalityContext* poseidonParser::Foreach_relationship_opContext::rship_cardinality() {
  return getRuleContext<poseidonParser::Rship_cardinalityContext>(0);
}

poseidonParser::Rship_source_varContext* poseidonParser::Foreach_relationship_opContext::rship_source_var() {
  return getRuleContext<poseidonParser::Rship_source_varContext>(0);
}


size_t poseidonParser::Foreach_relationship_opContext::getRuleIndex() const {
  return poseidonParser::RuleForeach_relationship_op;
}


std::any poseidonParser::Foreach_relationship_opContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<poseidonVisitor*>(visitor))
    return parserVisitor->visitForeach_relationship_op(this);
  else
    return visitor->visitChildren(this);
}

poseidonParser::Foreach_relationship_opContext* poseidonParser::foreach_relationship_op() {
  Foreach_relationship_opContext *_localctx = _tracker.createInstance<Foreach_relationship_opContext>(_ctx, getState());
  enterRule(_localctx, 30, poseidonParser::RuleForeach_relationship_op);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(248);
    match(poseidonParser::ForeachRelationship_);
    setState(249);
    match(poseidonParser::LPAREN);
    setState(250);
    rship_dir();
    setState(251);
    match(poseidonParser::COMMA_);
    setState(252);
    match(poseidonParser::STRING_);
    setState(255);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 7, _ctx)) {
    case 1: {
      setState(253);
      match(poseidonParser::COMMA_);
      setState(254);
      rship_cardinality();
      break;
    }

    default:
      break;
    }
    setState(259);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 8, _ctx)) {
    case 1: {
      setState(257);
      match(poseidonParser::COMMA_);
      setState(258);
      rship_source_var();
      break;
    }

    default:
      break;
    }
    setState(261);
    match(poseidonParser::COMMA_);
    setState(262);
    query_operator();
    setState(263);
    match(poseidonParser::RPAREN);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Rship_dirContext ------------------------------------------------------------------

poseidonParser::Rship_dirContext::Rship_dirContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* poseidonParser::Rship_dirContext::FromDir_() {
  return getToken(poseidonParser::FromDir_, 0);
}

tree::TerminalNode* poseidonParser::Rship_dirContext::ToDir_() {
  return getToken(poseidonParser::ToDir_, 0);
}

tree::TerminalNode* poseidonParser::Rship_dirContext::AllDir_() {
  return getToken(poseidonParser::AllDir_, 0);
}


size_t poseidonParser::Rship_dirContext::getRuleIndex() const {
  return poseidonParser::RuleRship_dir;
}


std::any poseidonParser::Rship_dirContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<poseidonVisitor*>(visitor))
    return parserVisitor->visitRship_dir(this);
  else
    return visitor->visitChildren(this);
}

poseidonParser::Rship_dirContext* poseidonParser::rship_dir() {
  Rship_dirContext *_localctx = _tracker.createInstance<Rship_dirContext>(_ctx, getState());
  enterRule(_localctx, 32, poseidonParser::RuleRship_dir);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(265);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 1924145348608) != 0))) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Rship_cardinalityContext ------------------------------------------------------------------

poseidonParser::Rship_cardinalityContext::Rship_cardinalityContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<tree::TerminalNode *> poseidonParser::Rship_cardinalityContext::INTEGER() {
  return getTokens(poseidonParser::INTEGER);
}

tree::TerminalNode* poseidonParser::Rship_cardinalityContext::INTEGER(size_t i) {
  return getToken(poseidonParser::INTEGER, i);
}

tree::TerminalNode* poseidonParser::Rship_cardinalityContext::COMMA_() {
  return getToken(poseidonParser::COMMA_, 0);
}


size_t poseidonParser::Rship_cardinalityContext::getRuleIndex() const {
  return poseidonParser::RuleRship_cardinality;
}


std::any poseidonParser::Rship_cardinalityContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<poseidonVisitor*>(visitor))
    return parserVisitor->visitRship_cardinality(this);
  else
    return visitor->visitChildren(this);
}

poseidonParser::Rship_cardinalityContext* poseidonParser::rship_cardinality() {
  Rship_cardinalityContext *_localctx = _tracker.createInstance<Rship_cardinalityContext>(_ctx, getState());
  enterRule(_localctx, 34, poseidonParser::RuleRship_cardinality);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(267);
    match(poseidonParser::INTEGER);
    setState(268);
    match(poseidonParser::COMMA_);
    setState(269);
    match(poseidonParser::INTEGER);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Rship_source_varContext ------------------------------------------------------------------

poseidonParser::Rship_source_varContext::Rship_source_varContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* poseidonParser::Rship_source_varContext::Var() {
  return getToken(poseidonParser::Var, 0);
}


size_t poseidonParser::Rship_source_varContext::getRuleIndex() const {
  return poseidonParser::RuleRship_source_var;
}


std::any poseidonParser::Rship_source_varContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<poseidonVisitor*>(visitor))
    return parserVisitor->visitRship_source_var(this);
  else
    return visitor->visitChildren(this);
}

poseidonParser::Rship_source_varContext* poseidonParser::rship_source_var() {
  Rship_source_varContext *_localctx = _tracker.createInstance<Rship_source_varContext>(_ctx, getState());
  enterRule(_localctx, 36, poseidonParser::RuleRship_source_var);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(271);
    match(poseidonParser::Var);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Expand_opContext ------------------------------------------------------------------

poseidonParser::Expand_opContext::Expand_opContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* poseidonParser::Expand_opContext::Expand_() {
  return getToken(poseidonParser::Expand_, 0);
}

tree::TerminalNode* poseidonParser::Expand_opContext::LPAREN() {
  return getToken(poseidonParser::LPAREN, 0);
}

poseidonParser::Expand_dirContext* poseidonParser::Expand_opContext::expand_dir() {
  return getRuleContext<poseidonParser::Expand_dirContext>(0);
}

std::vector<tree::TerminalNode *> poseidonParser::Expand_opContext::COMMA_() {
  return getTokens(poseidonParser::COMMA_);
}

tree::TerminalNode* poseidonParser::Expand_opContext::COMMA_(size_t i) {
  return getToken(poseidonParser::COMMA_, i);
}

tree::TerminalNode* poseidonParser::Expand_opContext::STRING_() {
  return getToken(poseidonParser::STRING_, 0);
}

poseidonParser::Query_operatorContext* poseidonParser::Expand_opContext::query_operator() {
  return getRuleContext<poseidonParser::Query_operatorContext>(0);
}

tree::TerminalNode* poseidonParser::Expand_opContext::RPAREN() {
  return getToken(poseidonParser::RPAREN, 0);
}


size_t poseidonParser::Expand_opContext::getRuleIndex() const {
  return poseidonParser::RuleExpand_op;
}


std::any poseidonParser::Expand_opContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<poseidonVisitor*>(visitor))
    return parserVisitor->visitExpand_op(this);
  else
    return visitor->visitChildren(this);
}

poseidonParser::Expand_opContext* poseidonParser::expand_op() {
  Expand_opContext *_localctx = _tracker.createInstance<Expand_opContext>(_ctx, getState());
  enterRule(_localctx, 38, poseidonParser::RuleExpand_op);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(273);
    match(poseidonParser::Expand_);
    setState(274);
    match(poseidonParser::LPAREN);
    setState(275);
    expand_dir();
    setState(276);
    match(poseidonParser::COMMA_);
    setState(277);
    match(poseidonParser::STRING_);
    setState(278);
    match(poseidonParser::COMMA_);
    setState(279);
    query_operator();
    setState(280);
    match(poseidonParser::RPAREN);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Expand_dirContext ------------------------------------------------------------------

poseidonParser::Expand_dirContext::Expand_dirContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* poseidonParser::Expand_dirContext::InExpandDir_() {
  return getToken(poseidonParser::InExpandDir_, 0);
}

tree::TerminalNode* poseidonParser::Expand_dirContext::OutExpandDir_() {
  return getToken(poseidonParser::OutExpandDir_, 0);
}


size_t poseidonParser::Expand_dirContext::getRuleIndex() const {
  return poseidonParser::RuleExpand_dir;
}


std::any poseidonParser::Expand_dirContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<poseidonVisitor*>(visitor))
    return parserVisitor->visitExpand_dir(this);
  else
    return visitor->visitChildren(this);
}

poseidonParser::Expand_dirContext* poseidonParser::expand_dir() {
  Expand_dirContext *_localctx = _tracker.createInstance<Expand_dirContext>(_ctx, getState());
  enterRule(_localctx, 40, poseidonParser::RuleExpand_dir);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(282);
    _la = _input->LA(1);
    if (!(_la == poseidonParser::InExpandDir_

    || _la == poseidonParser::OutExpandDir_)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Match_opContext ------------------------------------------------------------------

poseidonParser::Match_opContext::Match_opContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* poseidonParser::Match_opContext::Match_() {
  return getToken(poseidonParser::Match_, 0);
}

tree::TerminalNode* poseidonParser::Match_opContext::LPAREN() {
  return getToken(poseidonParser::LPAREN, 0);
}

poseidonParser::Path_patternContext* poseidonParser::Match_opContext::path_pattern() {
  return getRuleContext<poseidonParser::Path_patternContext>(0);
}

tree::TerminalNode* poseidonParser::Match_opContext::RPAREN() {
  return getToken(poseidonParser::RPAREN, 0);
}


size_t poseidonParser::Match_opContext::getRuleIndex() const {
  return poseidonParser::RuleMatch_op;
}


std::any poseidonParser::Match_opContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<poseidonVisitor*>(visitor))
    return parserVisitor->visitMatch_op(this);
  else
    return visitor->visitChildren(this);
}

poseidonParser::Match_opContext* poseidonParser::match_op() {
  Match_opContext *_localctx = _tracker.createInstance<Match_opContext>(_ctx, getState());
  enterRule(_localctx, 42, poseidonParser::RuleMatch_op);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(284);
    match(poseidonParser::Match_);
    setState(285);
    match(poseidonParser::LPAREN);
    setState(286);
    path_pattern();
    setState(287);
    match(poseidonParser::RPAREN);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Path_patternContext ------------------------------------------------------------------

poseidonParser::Path_patternContext::Path_patternContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

poseidonParser::Node_patternContext* poseidonParser::Path_patternContext::node_pattern() {
  return getRuleContext<poseidonParser::Node_patternContext>(0);
}

std::vector<poseidonParser::Path_componentContext *> poseidonParser::Path_patternContext::path_component() {
  return getRuleContexts<poseidonParser::Path_componentContext>();
}

poseidonParser::Path_componentContext* poseidonParser::Path_patternContext::path_component(size_t i) {
  return getRuleContext<poseidonParser::Path_componentContext>(i);
}


size_t poseidonParser::Path_patternContext::getRuleIndex() const {
  return poseidonParser::RulePath_pattern;
}


std::any poseidonParser::Path_patternContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<poseidonVisitor*>(visitor))
    return parserVisitor->visitPath_pattern(this);
  else
    return visitor->visitChildren(this);
}

poseidonParser::Path_patternContext* poseidonParser::path_pattern() {
  Path_patternContext *_localctx = _tracker.createInstance<Path_patternContext>(_ctx, getState());
  enterRule(_localctx, 44, poseidonParser::RulePath_pattern);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(289);
    node_pattern();
    setState(293);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 56) != 0)) {
      setState(290);
      path_component();
      setState(295);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Path_componentContext ------------------------------------------------------------------

poseidonParser::Path_componentContext::Path_componentContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

poseidonParser::Rship_patternContext* poseidonParser::Path_componentContext::rship_pattern() {
  return getRuleContext<poseidonParser::Rship_patternContext>(0);
}

poseidonParser::Node_patternContext* poseidonParser::Path_componentContext::node_pattern() {
  return getRuleContext<poseidonParser::Node_patternContext>(0);
}


size_t poseidonParser::Path_componentContext::getRuleIndex() const {
  return poseidonParser::RulePath_component;
}


std::any poseidonParser::Path_componentContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<poseidonVisitor*>(visitor))
    return parserVisitor->visitPath_component(this);
  else
    return visitor->visitChildren(this);
}

poseidonParser::Path_componentContext* poseidonParser::path_component() {
  Path_componentContext *_localctx = _tracker.createInstance<Path_componentContext>(_ctx, getState());
  enterRule(_localctx, 46, poseidonParser::RulePath_component);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(296);
    rship_pattern();
    setState(297);
    node_pattern();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Node_patternContext ------------------------------------------------------------------

poseidonParser::Node_patternContext::Node_patternContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* poseidonParser::Node_patternContext::LPAREN() {
  return getToken(poseidonParser::LPAREN, 0);
}

tree::TerminalNode* poseidonParser::Node_patternContext::COLON_() {
  return getToken(poseidonParser::COLON_, 0);
}

std::vector<tree::TerminalNode *> poseidonParser::Node_patternContext::Identifier_() {
  return getTokens(poseidonParser::Identifier_);
}

tree::TerminalNode* poseidonParser::Node_patternContext::Identifier_(size_t i) {
  return getToken(poseidonParser::Identifier_, i);
}

tree::TerminalNode* poseidonParser::Node_patternContext::RPAREN() {
  return getToken(poseidonParser::RPAREN, 0);
}

poseidonParser::Property_listContext* poseidonParser::Node_patternContext::property_list() {
  return getRuleContext<poseidonParser::Property_listContext>(0);
}


size_t poseidonParser::Node_patternContext::getRuleIndex() const {
  return poseidonParser::RuleNode_pattern;
}


std::any poseidonParser::Node_patternContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<poseidonVisitor*>(visitor))
    return parserVisitor->visitNode_pattern(this);
  else
    return visitor->visitChildren(this);
}

poseidonParser::Node_patternContext* poseidonParser::node_pattern() {
  Node_patternContext *_localctx = _tracker.createInstance<Node_patternContext>(_ctx, getState());
  enterRule(_localctx, 48, poseidonParser::RuleNode_pattern);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(299);
    match(poseidonParser::LPAREN);
    setState(301);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == poseidonParser::Identifier_) {
      setState(300);
      match(poseidonParser::Identifier_);
    }
    setState(303);
    match(poseidonParser::COLON_);
    setState(304);
    match(poseidonParser::Identifier_);
    setState(306);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == poseidonParser::T__5) {
      setState(305);
      property_list();
    }
    setState(308);
    match(poseidonParser::RPAREN);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Rship_patternContext ------------------------------------------------------------------

poseidonParser::Rship_patternContext::Rship_patternContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<poseidonParser::Dir_specContext *> poseidonParser::Rship_patternContext::dir_spec() {
  return getRuleContexts<poseidonParser::Dir_specContext>();
}

poseidonParser::Dir_specContext* poseidonParser::Rship_patternContext::dir_spec(size_t i) {
  return getRuleContext<poseidonParser::Dir_specContext>(i);
}

tree::TerminalNode* poseidonParser::Rship_patternContext::LBRACKET() {
  return getToken(poseidonParser::LBRACKET, 0);
}

tree::TerminalNode* poseidonParser::Rship_patternContext::COLON_() {
  return getToken(poseidonParser::COLON_, 0);
}

std::vector<tree::TerminalNode *> poseidonParser::Rship_patternContext::Identifier_() {
  return getTokens(poseidonParser::Identifier_);
}

tree::TerminalNode* poseidonParser::Rship_patternContext::Identifier_(size_t i) {
  return getToken(poseidonParser::Identifier_, i);
}

tree::TerminalNode* poseidonParser::Rship_patternContext::RBRACKET() {
  return getToken(poseidonParser::RBRACKET, 0);
}

poseidonParser::Cardinality_specContext* poseidonParser::Rship_patternContext::cardinality_spec() {
  return getRuleContext<poseidonParser::Cardinality_specContext>(0);
}


size_t poseidonParser::Rship_patternContext::getRuleIndex() const {
  return poseidonParser::RuleRship_pattern;
}


std::any poseidonParser::Rship_patternContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<poseidonVisitor*>(visitor))
    return parserVisitor->visitRship_pattern(this);
  else
    return visitor->visitChildren(this);
}

poseidonParser::Rship_patternContext* poseidonParser::rship_pattern() {
  Rship_patternContext *_localctx = _tracker.createInstance<Rship_patternContext>(_ctx, getState());
  enterRule(_localctx, 50, poseidonParser::RuleRship_pattern);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(310);
    dir_spec();
    setState(311);
    match(poseidonParser::LBRACKET);
    setState(313);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == poseidonParser::Identifier_) {
      setState(312);
      match(poseidonParser::Identifier_);
    }
    setState(315);
    match(poseidonParser::COLON_);
    setState(316);
    match(poseidonParser::Identifier_);
    setState(318);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == poseidonParser::MULT) {
      setState(317);
      cardinality_spec();
    }
    setState(320);
    match(poseidonParser::RBRACKET);
    setState(321);
    dir_spec();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Cardinality_specContext ------------------------------------------------------------------

poseidonParser::Cardinality_specContext::Cardinality_specContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* poseidonParser::Cardinality_specContext::MULT() {
  return getToken(poseidonParser::MULT, 0);
}

poseidonParser::Min_cardinalityContext* poseidonParser::Cardinality_specContext::min_cardinality() {
  return getRuleContext<poseidonParser::Min_cardinalityContext>(0);
}

poseidonParser::Max_cardinalityContext* poseidonParser::Cardinality_specContext::max_cardinality() {
  return getRuleContext<poseidonParser::Max_cardinalityContext>(0);
}


size_t poseidonParser::Cardinality_specContext::getRuleIndex() const {
  return poseidonParser::RuleCardinality_spec;
}


std::any poseidonParser::Cardinality_specContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<poseidonVisitor*>(visitor))
    return parserVisitor->visitCardinality_spec(this);
  else
    return visitor->visitChildren(this);
}

poseidonParser::Cardinality_specContext* poseidonParser::cardinality_spec() {
  Cardinality_specContext *_localctx = _tracker.createInstance<Cardinality_specContext>(_ctx, getState());
  enterRule(_localctx, 52, poseidonParser::RuleCardinality_spec);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(323);
    match(poseidonParser::MULT);
    setState(324);
    min_cardinality();
    setState(325);
    match(poseidonParser::T__1);
    setState(327);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == poseidonParser::INTEGER) {
      setState(326);
      max_cardinality();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Min_cardinalityContext ------------------------------------------------------------------

poseidonParser::Min_cardinalityContext::Min_cardinalityContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* poseidonParser::Min_cardinalityContext::INTEGER() {
  return getToken(poseidonParser::INTEGER, 0);
}


size_t poseidonParser::Min_cardinalityContext::getRuleIndex() const {
  return poseidonParser::RuleMin_cardinality;
}


std::any poseidonParser::Min_cardinalityContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<poseidonVisitor*>(visitor))
    return parserVisitor->visitMin_cardinality(this);
  else
    return visitor->visitChildren(this);
}

poseidonParser::Min_cardinalityContext* poseidonParser::min_cardinality() {
  Min_cardinalityContext *_localctx = _tracker.createInstance<Min_cardinalityContext>(_ctx, getState());
  enterRule(_localctx, 54, poseidonParser::RuleMin_cardinality);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(329);
    match(poseidonParser::INTEGER);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Max_cardinalityContext ------------------------------------------------------------------

poseidonParser::Max_cardinalityContext::Max_cardinalityContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* poseidonParser::Max_cardinalityContext::INTEGER() {
  return getToken(poseidonParser::INTEGER, 0);
}


size_t poseidonParser::Max_cardinalityContext::getRuleIndex() const {
  return poseidonParser::RuleMax_cardinality;
}


std::any poseidonParser::Max_cardinalityContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<poseidonVisitor*>(visitor))
    return parserVisitor->visitMax_cardinality(this);
  else
    return visitor->visitChildren(this);
}

poseidonParser::Max_cardinalityContext* poseidonParser::max_cardinality() {
  Max_cardinalityContext *_localctx = _tracker.createInstance<Max_cardinalityContext>(_ctx, getState());
  enterRule(_localctx, 56, poseidonParser::RuleMax_cardinality);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(331);
    match(poseidonParser::INTEGER);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Dir_specContext ------------------------------------------------------------------

poseidonParser::Dir_specContext::Dir_specContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

poseidonParser::Left_dirContext* poseidonParser::Dir_specContext::left_dir() {
  return getRuleContext<poseidonParser::Left_dirContext>(0);
}

poseidonParser::Right_dirContext* poseidonParser::Dir_specContext::right_dir() {
  return getRuleContext<poseidonParser::Right_dirContext>(0);
}

poseidonParser::No_dirContext* poseidonParser::Dir_specContext::no_dir() {
  return getRuleContext<poseidonParser::No_dirContext>(0);
}


size_t poseidonParser::Dir_specContext::getRuleIndex() const {
  return poseidonParser::RuleDir_spec;
}


std::any poseidonParser::Dir_specContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<poseidonVisitor*>(visitor))
    return parserVisitor->visitDir_spec(this);
  else
    return visitor->visitChildren(this);
}

poseidonParser::Dir_specContext* poseidonParser::dir_spec() {
  Dir_specContext *_localctx = _tracker.createInstance<Dir_specContext>(_ctx, getState());
  enterRule(_localctx, 58, poseidonParser::RuleDir_spec);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(336);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case poseidonParser::T__2: {
        enterOuterAlt(_localctx, 1);
        setState(333);
        left_dir();
        break;
      }

      case poseidonParser::T__3: {
        enterOuterAlt(_localctx, 2);
        setState(334);
        right_dir();
        break;
      }

      case poseidonParser::T__4: {
        enterOuterAlt(_localctx, 3);
        setState(335);
        no_dir();
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Left_dirContext ------------------------------------------------------------------

poseidonParser::Left_dirContext::Left_dirContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t poseidonParser::Left_dirContext::getRuleIndex() const {
  return poseidonParser::RuleLeft_dir;
}


std::any poseidonParser::Left_dirContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<poseidonVisitor*>(visitor))
    return parserVisitor->visitLeft_dir(this);
  else
    return visitor->visitChildren(this);
}

poseidonParser::Left_dirContext* poseidonParser::left_dir() {
  Left_dirContext *_localctx = _tracker.createInstance<Left_dirContext>(_ctx, getState());
  enterRule(_localctx, 60, poseidonParser::RuleLeft_dir);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(338);
    match(poseidonParser::T__2);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Right_dirContext ------------------------------------------------------------------

poseidonParser::Right_dirContext::Right_dirContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t poseidonParser::Right_dirContext::getRuleIndex() const {
  return poseidonParser::RuleRight_dir;
}


std::any poseidonParser::Right_dirContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<poseidonVisitor*>(visitor))
    return parserVisitor->visitRight_dir(this);
  else
    return visitor->visitChildren(this);
}

poseidonParser::Right_dirContext* poseidonParser::right_dir() {
  Right_dirContext *_localctx = _tracker.createInstance<Right_dirContext>(_ctx, getState());
  enterRule(_localctx, 62, poseidonParser::RuleRight_dir);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(340);
    match(poseidonParser::T__3);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- No_dirContext ------------------------------------------------------------------

poseidonParser::No_dirContext::No_dirContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t poseidonParser::No_dirContext::getRuleIndex() const {
  return poseidonParser::RuleNo_dir;
}


std::any poseidonParser::No_dirContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<poseidonVisitor*>(visitor))
    return parserVisitor->visitNo_dir(this);
  else
    return visitor->visitChildren(this);
}

poseidonParser::No_dirContext* poseidonParser::no_dir() {
  No_dirContext *_localctx = _tracker.createInstance<No_dirContext>(_ctx, getState());
  enterRule(_localctx, 64, poseidonParser::RuleNo_dir);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(342);
    match(poseidonParser::T__4);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Aggregate_opContext ------------------------------------------------------------------

poseidonParser::Aggregate_opContext::Aggregate_opContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* poseidonParser::Aggregate_opContext::Aggregate_() {
  return getToken(poseidonParser::Aggregate_, 0);
}

tree::TerminalNode* poseidonParser::Aggregate_opContext::LPAREN() {
  return getToken(poseidonParser::LPAREN, 0);
}

poseidonParser::Aggregate_listContext* poseidonParser::Aggregate_opContext::aggregate_list() {
  return getRuleContext<poseidonParser::Aggregate_listContext>(0);
}

tree::TerminalNode* poseidonParser::Aggregate_opContext::COMMA_() {
  return getToken(poseidonParser::COMMA_, 0);
}

poseidonParser::Query_operatorContext* poseidonParser::Aggregate_opContext::query_operator() {
  return getRuleContext<poseidonParser::Query_operatorContext>(0);
}

tree::TerminalNode* poseidonParser::Aggregate_opContext::RPAREN() {
  return getToken(poseidonParser::RPAREN, 0);
}


size_t poseidonParser::Aggregate_opContext::getRuleIndex() const {
  return poseidonParser::RuleAggregate_op;
}


std::any poseidonParser::Aggregate_opContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<poseidonVisitor*>(visitor))
    return parserVisitor->visitAggregate_op(this);
  else
    return visitor->visitChildren(this);
}

poseidonParser::Aggregate_opContext* poseidonParser::aggregate_op() {
  Aggregate_opContext *_localctx = _tracker.createInstance<Aggregate_opContext>(_ctx, getState());
  enterRule(_localctx, 66, poseidonParser::RuleAggregate_op);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(344);
    match(poseidonParser::Aggregate_);
    setState(345);
    match(poseidonParser::LPAREN);
    setState(346);
    aggregate_list();
    setState(347);
    match(poseidonParser::COMMA_);
    setState(348);
    query_operator();
    setState(349);
    match(poseidonParser::RPAREN);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Aggregate_listContext ------------------------------------------------------------------

poseidonParser::Aggregate_listContext::Aggregate_listContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* poseidonParser::Aggregate_listContext::LBRACKET() {
  return getToken(poseidonParser::LBRACKET, 0);
}

std::vector<poseidonParser::Aggr_exprContext *> poseidonParser::Aggregate_listContext::aggr_expr() {
  return getRuleContexts<poseidonParser::Aggr_exprContext>();
}

poseidonParser::Aggr_exprContext* poseidonParser::Aggregate_listContext::aggr_expr(size_t i) {
  return getRuleContext<poseidonParser::Aggr_exprContext>(i);
}

tree::TerminalNode* poseidonParser::Aggregate_listContext::RBRACKET() {
  return getToken(poseidonParser::RBRACKET, 0);
}

std::vector<tree::TerminalNode *> poseidonParser::Aggregate_listContext::COMMA_() {
  return getTokens(poseidonParser::COMMA_);
}

tree::TerminalNode* poseidonParser::Aggregate_listContext::COMMA_(size_t i) {
  return getToken(poseidonParser::COMMA_, i);
}


size_t poseidonParser::Aggregate_listContext::getRuleIndex() const {
  return poseidonParser::RuleAggregate_list;
}


std::any poseidonParser::Aggregate_listContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<poseidonVisitor*>(visitor))
    return parserVisitor->visitAggregate_list(this);
  else
    return visitor->visitChildren(this);
}

poseidonParser::Aggregate_listContext* poseidonParser::aggregate_list() {
  Aggregate_listContext *_localctx = _tracker.createInstance<Aggregate_listContext>(_ctx, getState());
  enterRule(_localctx, 68, poseidonParser::RuleAggregate_list);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(351);
    match(poseidonParser::LBRACKET);
    setState(352);
    aggr_expr();
    setState(357);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == poseidonParser::COMMA_) {
      setState(353);
      match(poseidonParser::COMMA_);
      setState(354);
      aggr_expr();
      setState(359);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(360);
    match(poseidonParser::RBRACKET);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Aggr_exprContext ------------------------------------------------------------------

poseidonParser::Aggr_exprContext::Aggr_exprContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

poseidonParser::Aggr_funcContext* poseidonParser::Aggr_exprContext::aggr_func() {
  return getRuleContext<poseidonParser::Aggr_funcContext>(0);
}

tree::TerminalNode* poseidonParser::Aggr_exprContext::LPAREN() {
  return getToken(poseidonParser::LPAREN, 0);
}

poseidonParser::Proj_exprContext* poseidonParser::Aggr_exprContext::proj_expr() {
  return getRuleContext<poseidonParser::Proj_exprContext>(0);
}

tree::TerminalNode* poseidonParser::Aggr_exprContext::RPAREN() {
  return getToken(poseidonParser::RPAREN, 0);
}


size_t poseidonParser::Aggr_exprContext::getRuleIndex() const {
  return poseidonParser::RuleAggr_expr;
}


std::any poseidonParser::Aggr_exprContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<poseidonVisitor*>(visitor))
    return parserVisitor->visitAggr_expr(this);
  else
    return visitor->visitChildren(this);
}

poseidonParser::Aggr_exprContext* poseidonParser::aggr_expr() {
  Aggr_exprContext *_localctx = _tracker.createInstance<Aggr_exprContext>(_ctx, getState());
  enterRule(_localctx, 70, poseidonParser::RuleAggr_expr);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(362);
    aggr_func();
    setState(363);
    match(poseidonParser::LPAREN);
    setState(364);
    proj_expr();
    setState(365);
    match(poseidonParser::RPAREN);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Aggr_funcContext ------------------------------------------------------------------

poseidonParser::Aggr_funcContext::Aggr_funcContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* poseidonParser::Aggr_funcContext::Count_() {
  return getToken(poseidonParser::Count_, 0);
}

tree::TerminalNode* poseidonParser::Aggr_funcContext::Sum_() {
  return getToken(poseidonParser::Sum_, 0);
}

tree::TerminalNode* poseidonParser::Aggr_funcContext::Avg_() {
  return getToken(poseidonParser::Avg_, 0);
}

tree::TerminalNode* poseidonParser::Aggr_funcContext::Min_() {
  return getToken(poseidonParser::Min_, 0);
}

tree::TerminalNode* poseidonParser::Aggr_funcContext::Max_() {
  return getToken(poseidonParser::Max_, 0);
}


size_t poseidonParser::Aggr_funcContext::getRuleIndex() const {
  return poseidonParser::RuleAggr_func;
}


std::any poseidonParser::Aggr_funcContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<poseidonVisitor*>(visitor))
    return parserVisitor->visitAggr_func(this);
  else
    return visitor->visitChildren(this);
}

poseidonParser::Aggr_funcContext* poseidonParser::aggr_func() {
  Aggr_funcContext *_localctx = _tracker.createInstance<Aggr_funcContext>(_ctx, getState());
  enterRule(_localctx, 72, poseidonParser::RuleAggr_func);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(367);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 33285996544) != 0))) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Union_opContext ------------------------------------------------------------------

poseidonParser::Union_opContext::Union_opContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* poseidonParser::Union_opContext::Union_() {
  return getToken(poseidonParser::Union_, 0);
}

tree::TerminalNode* poseidonParser::Union_opContext::LPAREN() {
  return getToken(poseidonParser::LPAREN, 0);
}

std::vector<poseidonParser::Query_operatorContext *> poseidonParser::Union_opContext::query_operator() {
  return getRuleContexts<poseidonParser::Query_operatorContext>();
}

poseidonParser::Query_operatorContext* poseidonParser::Union_opContext::query_operator(size_t i) {
  return getRuleContext<poseidonParser::Query_operatorContext>(i);
}

tree::TerminalNode* poseidonParser::Union_opContext::COMMA_() {
  return getToken(poseidonParser::COMMA_, 0);
}

tree::TerminalNode* poseidonParser::Union_opContext::RPAREN() {
  return getToken(poseidonParser::RPAREN, 0);
}


size_t poseidonParser::Union_opContext::getRuleIndex() const {
  return poseidonParser::RuleUnion_op;
}


std::any poseidonParser::Union_opContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<poseidonVisitor*>(visitor))
    return parserVisitor->visitUnion_op(this);
  else
    return visitor->visitChildren(this);
}

poseidonParser::Union_opContext* poseidonParser::union_op() {
  Union_opContext *_localctx = _tracker.createInstance<Union_opContext>(_ctx, getState());
  enterRule(_localctx, 74, poseidonParser::RuleUnion_op);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(369);
    match(poseidonParser::Union_);
    setState(370);
    match(poseidonParser::LPAREN);
    setState(371);
    query_operator();
    setState(372);
    match(poseidonParser::COMMA_);
    setState(373);
    query_operator();
    setState(374);
    match(poseidonParser::RPAREN);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Group_by_opContext ------------------------------------------------------------------

poseidonParser::Group_by_opContext::Group_by_opContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* poseidonParser::Group_by_opContext::GroupBy_() {
  return getToken(poseidonParser::GroupBy_, 0);
}

tree::TerminalNode* poseidonParser::Group_by_opContext::LPAREN() {
  return getToken(poseidonParser::LPAREN, 0);
}

poseidonParser::Grouping_listContext* poseidonParser::Group_by_opContext::grouping_list() {
  return getRuleContext<poseidonParser::Grouping_listContext>(0);
}

std::vector<tree::TerminalNode *> poseidonParser::Group_by_opContext::COMMA_() {
  return getTokens(poseidonParser::COMMA_);
}

tree::TerminalNode* poseidonParser::Group_by_opContext::COMMA_(size_t i) {
  return getToken(poseidonParser::COMMA_, i);
}

poseidonParser::Aggregate_listContext* poseidonParser::Group_by_opContext::aggregate_list() {
  return getRuleContext<poseidonParser::Aggregate_listContext>(0);
}

poseidonParser::Query_operatorContext* poseidonParser::Group_by_opContext::query_operator() {
  return getRuleContext<poseidonParser::Query_operatorContext>(0);
}

tree::TerminalNode* poseidonParser::Group_by_opContext::RPAREN() {
  return getToken(poseidonParser::RPAREN, 0);
}


size_t poseidonParser::Group_by_opContext::getRuleIndex() const {
  return poseidonParser::RuleGroup_by_op;
}


std::any poseidonParser::Group_by_opContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<poseidonVisitor*>(visitor))
    return parserVisitor->visitGroup_by_op(this);
  else
    return visitor->visitChildren(this);
}

poseidonParser::Group_by_opContext* poseidonParser::group_by_op() {
  Group_by_opContext *_localctx = _tracker.createInstance<Group_by_opContext>(_ctx, getState());
  enterRule(_localctx, 76, poseidonParser::RuleGroup_by_op);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(376);
    match(poseidonParser::GroupBy_);
    setState(377);
    match(poseidonParser::LPAREN);
    setState(378);
    grouping_list();
    setState(379);
    match(poseidonParser::COMMA_);
    setState(380);
    aggregate_list();
    setState(381);
    match(poseidonParser::COMMA_);
    setState(382);
    query_operator();
    setState(383);
    match(poseidonParser::RPAREN);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Grouping_listContext ------------------------------------------------------------------

poseidonParser::Grouping_listContext::Grouping_listContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* poseidonParser::Grouping_listContext::LBRACKET() {
  return getToken(poseidonParser::LBRACKET, 0);
}

std::vector<poseidonParser::Grouping_exprContext *> poseidonParser::Grouping_listContext::grouping_expr() {
  return getRuleContexts<poseidonParser::Grouping_exprContext>();
}

poseidonParser::Grouping_exprContext* poseidonParser::Grouping_listContext::grouping_expr(size_t i) {
  return getRuleContext<poseidonParser::Grouping_exprContext>(i);
}

tree::TerminalNode* poseidonParser::Grouping_listContext::RBRACKET() {
  return getToken(poseidonParser::RBRACKET, 0);
}

std::vector<tree::TerminalNode *> poseidonParser::Grouping_listContext::COMMA_() {
  return getTokens(poseidonParser::COMMA_);
}

tree::TerminalNode* poseidonParser::Grouping_listContext::COMMA_(size_t i) {
  return getToken(poseidonParser::COMMA_, i);
}


size_t poseidonParser::Grouping_listContext::getRuleIndex() const {
  return poseidonParser::RuleGrouping_list;
}


std::any poseidonParser::Grouping_listContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<poseidonVisitor*>(visitor))
    return parserVisitor->visitGrouping_list(this);
  else
    return visitor->visitChildren(this);
}

poseidonParser::Grouping_listContext* poseidonParser::grouping_list() {
  Grouping_listContext *_localctx = _tracker.createInstance<Grouping_listContext>(_ctx, getState());
  enterRule(_localctx, 78, poseidonParser::RuleGrouping_list);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(385);
    match(poseidonParser::LBRACKET);
    setState(386);
    grouping_expr();
    setState(391);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == poseidonParser::COMMA_) {
      setState(387);
      match(poseidonParser::COMMA_);
      setState(388);
      grouping_expr();
      setState(393);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(394);
    match(poseidonParser::RBRACKET);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Grouping_exprContext ------------------------------------------------------------------

poseidonParser::Grouping_exprContext::Grouping_exprContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* poseidonParser::Grouping_exprContext::Var() {
  return getToken(poseidonParser::Var, 0);
}

tree::TerminalNode* poseidonParser::Grouping_exprContext::COLON_() {
  return getToken(poseidonParser::COLON_, 0);
}

poseidonParser::Type_specContext* poseidonParser::Grouping_exprContext::type_spec() {
  return getRuleContext<poseidonParser::Type_specContext>(0);
}

tree::TerminalNode* poseidonParser::Grouping_exprContext::Identifier_() {
  return getToken(poseidonParser::Identifier_, 0);
}


size_t poseidonParser::Grouping_exprContext::getRuleIndex() const {
  return poseidonParser::RuleGrouping_expr;
}


std::any poseidonParser::Grouping_exprContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<poseidonVisitor*>(visitor))
    return parserVisitor->visitGrouping_expr(this);
  else
    return visitor->visitChildren(this);
}

poseidonParser::Grouping_exprContext* poseidonParser::grouping_expr() {
  Grouping_exprContext *_localctx = _tracker.createInstance<Grouping_exprContext>(_ctx, getState());
  enterRule(_localctx, 80, poseidonParser::RuleGrouping_expr);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(396);
    match(poseidonParser::Var);
    setState(399);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == poseidonParser::T__0) {
      setState(397);
      match(poseidonParser::T__0);
      setState(398);
      match(poseidonParser::Identifier_);
    }
    setState(401);
    match(poseidonParser::COLON_);
    setState(402);
    type_spec();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Filter_opContext ------------------------------------------------------------------

poseidonParser::Filter_opContext::Filter_opContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* poseidonParser::Filter_opContext::Filter_() {
  return getToken(poseidonParser::Filter_, 0);
}

tree::TerminalNode* poseidonParser::Filter_opContext::LPAREN() {
  return getToken(poseidonParser::LPAREN, 0);
}

poseidonParser::Logical_exprContext* poseidonParser::Filter_opContext::logical_expr() {
  return getRuleContext<poseidonParser::Logical_exprContext>(0);
}

tree::TerminalNode* poseidonParser::Filter_opContext::COMMA_() {
  return getToken(poseidonParser::COMMA_, 0);
}

poseidonParser::Query_operatorContext* poseidonParser::Filter_opContext::query_operator() {
  return getRuleContext<poseidonParser::Query_operatorContext>(0);
}

tree::TerminalNode* poseidonParser::Filter_opContext::RPAREN() {
  return getToken(poseidonParser::RPAREN, 0);
}


size_t poseidonParser::Filter_opContext::getRuleIndex() const {
  return poseidonParser::RuleFilter_op;
}


std::any poseidonParser::Filter_opContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<poseidonVisitor*>(visitor))
    return parserVisitor->visitFilter_op(this);
  else
    return visitor->visitChildren(this);
}

poseidonParser::Filter_opContext* poseidonParser::filter_op() {
  Filter_opContext *_localctx = _tracker.createInstance<Filter_opContext>(_ctx, getState());
  enterRule(_localctx, 82, poseidonParser::RuleFilter_op);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(404);
    match(poseidonParser::Filter_);
    setState(405);
    match(poseidonParser::LPAREN);
    setState(406);
    logical_expr();
    setState(407);
    match(poseidonParser::COMMA_);
    setState(408);
    query_operator();
    setState(409);
    match(poseidonParser::RPAREN);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Logical_exprContext ------------------------------------------------------------------

poseidonParser::Logical_exprContext::Logical_exprContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<poseidonParser::Boolean_exprContext *> poseidonParser::Logical_exprContext::boolean_expr() {
  return getRuleContexts<poseidonParser::Boolean_exprContext>();
}

poseidonParser::Boolean_exprContext* poseidonParser::Logical_exprContext::boolean_expr(size_t i) {
  return getRuleContext<poseidonParser::Boolean_exprContext>(i);
}

std::vector<tree::TerminalNode *> poseidonParser::Logical_exprContext::OR() {
  return getTokens(poseidonParser::OR);
}

tree::TerminalNode* poseidonParser::Logical_exprContext::OR(size_t i) {
  return getToken(poseidonParser::OR, i);
}


size_t poseidonParser::Logical_exprContext::getRuleIndex() const {
  return poseidonParser::RuleLogical_expr;
}


std::any poseidonParser::Logical_exprContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<poseidonVisitor*>(visitor))
    return parserVisitor->visitLogical_expr(this);
  else
    return visitor->visitChildren(this);
}

poseidonParser::Logical_exprContext* poseidonParser::logical_expr() {
  Logical_exprContext *_localctx = _tracker.createInstance<Logical_exprContext>(_ctx, getState());
  enterRule(_localctx, 84, poseidonParser::RuleLogical_expr);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(411);
    boolean_expr();
    setState(416);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == poseidonParser::OR) {
      setState(412);
      match(poseidonParser::OR);
      setState(413);
      boolean_expr();
      setState(418);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Boolean_exprContext ------------------------------------------------------------------

poseidonParser::Boolean_exprContext::Boolean_exprContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<poseidonParser::Equality_exprContext *> poseidonParser::Boolean_exprContext::equality_expr() {
  return getRuleContexts<poseidonParser::Equality_exprContext>();
}

poseidonParser::Equality_exprContext* poseidonParser::Boolean_exprContext::equality_expr(size_t i) {
  return getRuleContext<poseidonParser::Equality_exprContext>(i);
}

std::vector<tree::TerminalNode *> poseidonParser::Boolean_exprContext::AND() {
  return getTokens(poseidonParser::AND);
}

tree::TerminalNode* poseidonParser::Boolean_exprContext::AND(size_t i) {
  return getToken(poseidonParser::AND, i);
}


size_t poseidonParser::Boolean_exprContext::getRuleIndex() const {
  return poseidonParser::RuleBoolean_expr;
}


std::any poseidonParser::Boolean_exprContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<poseidonVisitor*>(visitor))
    return parserVisitor->visitBoolean_expr(this);
  else
    return visitor->visitChildren(this);
}

poseidonParser::Boolean_exprContext* poseidonParser::boolean_expr() {
  Boolean_exprContext *_localctx = _tracker.createInstance<Boolean_exprContext>(_ctx, getState());
  enterRule(_localctx, 86, poseidonParser::RuleBoolean_expr);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(419);
    equality_expr();
    setState(424);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == poseidonParser::AND) {
      setState(420);
      match(poseidonParser::AND);
      setState(421);
      equality_expr();
      setState(426);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Equality_exprContext ------------------------------------------------------------------

poseidonParser::Equality_exprContext::Equality_exprContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<poseidonParser::Relational_exprContext *> poseidonParser::Equality_exprContext::relational_expr() {
  return getRuleContexts<poseidonParser::Relational_exprContext>();
}

poseidonParser::Relational_exprContext* poseidonParser::Equality_exprContext::relational_expr(size_t i) {
  return getRuleContext<poseidonParser::Relational_exprContext>(i);
}

std::vector<tree::TerminalNode *> poseidonParser::Equality_exprContext::EQUALS() {
  return getTokens(poseidonParser::EQUALS);
}

tree::TerminalNode* poseidonParser::Equality_exprContext::EQUALS(size_t i) {
  return getToken(poseidonParser::EQUALS, i);
}

std::vector<tree::TerminalNode *> poseidonParser::Equality_exprContext::NOTEQUALS() {
  return getTokens(poseidonParser::NOTEQUALS);
}

tree::TerminalNode* poseidonParser::Equality_exprContext::NOTEQUALS(size_t i) {
  return getToken(poseidonParser::NOTEQUALS, i);
}


size_t poseidonParser::Equality_exprContext::getRuleIndex() const {
  return poseidonParser::RuleEquality_expr;
}


std::any poseidonParser::Equality_exprContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<poseidonVisitor*>(visitor))
    return parserVisitor->visitEquality_expr(this);
  else
    return visitor->visitChildren(this);
}

poseidonParser::Equality_exprContext* poseidonParser::equality_expr() {
  Equality_exprContext *_localctx = _tracker.createInstance<Equality_exprContext>(_ctx, getState());
  enterRule(_localctx, 88, poseidonParser::RuleEquality_expr);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(427);
    relational_expr();
    setState(432);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == poseidonParser::EQUALS

    || _la == poseidonParser::NOTEQUALS) {
      setState(428);
      _la = _input->LA(1);
      if (!(_la == poseidonParser::EQUALS

      || _la == poseidonParser::NOTEQUALS)) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(429);
      relational_expr();
      setState(434);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Relational_exprContext ------------------------------------------------------------------

poseidonParser::Relational_exprContext::Relational_exprContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<poseidonParser::Additive_exprContext *> poseidonParser::Relational_exprContext::additive_expr() {
  return getRuleContexts<poseidonParser::Additive_exprContext>();
}

poseidonParser::Additive_exprContext* poseidonParser::Relational_exprContext::additive_expr(size_t i) {
  return getRuleContext<poseidonParser::Additive_exprContext>(i);
}

std::vector<tree::TerminalNode *> poseidonParser::Relational_exprContext::LT() {
  return getTokens(poseidonParser::LT);
}

tree::TerminalNode* poseidonParser::Relational_exprContext::LT(size_t i) {
  return getToken(poseidonParser::LT, i);
}

std::vector<tree::TerminalNode *> poseidonParser::Relational_exprContext::LTEQ() {
  return getTokens(poseidonParser::LTEQ);
}

tree::TerminalNode* poseidonParser::Relational_exprContext::LTEQ(size_t i) {
  return getToken(poseidonParser::LTEQ, i);
}

std::vector<tree::TerminalNode *> poseidonParser::Relational_exprContext::GT() {
  return getTokens(poseidonParser::GT);
}

tree::TerminalNode* poseidonParser::Relational_exprContext::GT(size_t i) {
  return getToken(poseidonParser::GT, i);
}

std::vector<tree::TerminalNode *> poseidonParser::Relational_exprContext::GTEQ() {
  return getTokens(poseidonParser::GTEQ);
}

tree::TerminalNode* poseidonParser::Relational_exprContext::GTEQ(size_t i) {
  return getToken(poseidonParser::GTEQ, i);
}


size_t poseidonParser::Relational_exprContext::getRuleIndex() const {
  return poseidonParser::RuleRelational_expr;
}


std::any poseidonParser::Relational_exprContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<poseidonVisitor*>(visitor))
    return parserVisitor->visitRelational_expr(this);
  else
    return visitor->visitChildren(this);
}

poseidonParser::Relational_exprContext* poseidonParser::relational_expr() {
  Relational_exprContext *_localctx = _tracker.createInstance<Relational_exprContext>(_ctx, getState());
  enterRule(_localctx, 90, poseidonParser::RuleRelational_expr);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(435);
    additive_expr();
    setState(440);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & -1152921504606846976) != 0)) {
      setState(436);
      _la = _input->LA(1);
      if (!((((_la & ~ 0x3fULL) == 0) &&
        ((1ULL << _la) & -1152921504606846976) != 0))) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(437);
      additive_expr();
      setState(442);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Additive_exprContext ------------------------------------------------------------------

poseidonParser::Additive_exprContext::Additive_exprContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<poseidonParser::Multiplicative_exprContext *> poseidonParser::Additive_exprContext::multiplicative_expr() {
  return getRuleContexts<poseidonParser::Multiplicative_exprContext>();
}

poseidonParser::Multiplicative_exprContext* poseidonParser::Additive_exprContext::multiplicative_expr(size_t i) {
  return getRuleContext<poseidonParser::Multiplicative_exprContext>(i);
}

std::vector<tree::TerminalNode *> poseidonParser::Additive_exprContext::PLUS_() {
  return getTokens(poseidonParser::PLUS_);
}

tree::TerminalNode* poseidonParser::Additive_exprContext::PLUS_(size_t i) {
  return getToken(poseidonParser::PLUS_, i);
}


size_t poseidonParser::Additive_exprContext::getRuleIndex() const {
  return poseidonParser::RuleAdditive_expr;
}


std::any poseidonParser::Additive_exprContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<poseidonVisitor*>(visitor))
    return parserVisitor->visitAdditive_expr(this);
  else
    return visitor->visitChildren(this);
}

poseidonParser::Additive_exprContext* poseidonParser::additive_expr() {
  Additive_exprContext *_localctx = _tracker.createInstance<Additive_exprContext>(_ctx, getState());
  enterRule(_localctx, 92, poseidonParser::RuleAdditive_expr);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(443);
    multiplicative_expr();
    setState(448);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == poseidonParser::T__4

    || _la == poseidonParser::PLUS_) {
      setState(444);
      _la = _input->LA(1);
      if (!(_la == poseidonParser::T__4

      || _la == poseidonParser::PLUS_)) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(445);
      multiplicative_expr();
      setState(450);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Multiplicative_exprContext ------------------------------------------------------------------

poseidonParser::Multiplicative_exprContext::Multiplicative_exprContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<poseidonParser::Unary_exprContext *> poseidonParser::Multiplicative_exprContext::unary_expr() {
  return getRuleContexts<poseidonParser::Unary_exprContext>();
}

poseidonParser::Unary_exprContext* poseidonParser::Multiplicative_exprContext::unary_expr(size_t i) {
  return getRuleContext<poseidonParser::Unary_exprContext>(i);
}

std::vector<tree::TerminalNode *> poseidonParser::Multiplicative_exprContext::MULT() {
  return getTokens(poseidonParser::MULT);
}

tree::TerminalNode* poseidonParser::Multiplicative_exprContext::MULT(size_t i) {
  return getToken(poseidonParser::MULT, i);
}

std::vector<tree::TerminalNode *> poseidonParser::Multiplicative_exprContext::DIV() {
  return getTokens(poseidonParser::DIV);
}

tree::TerminalNode* poseidonParser::Multiplicative_exprContext::DIV(size_t i) {
  return getToken(poseidonParser::DIV, i);
}

std::vector<tree::TerminalNode *> poseidonParser::Multiplicative_exprContext::MOD() {
  return getTokens(poseidonParser::MOD);
}

tree::TerminalNode* poseidonParser::Multiplicative_exprContext::MOD(size_t i) {
  return getToken(poseidonParser::MOD, i);
}


size_t poseidonParser::Multiplicative_exprContext::getRuleIndex() const {
  return poseidonParser::RuleMultiplicative_expr;
}


std::any poseidonParser::Multiplicative_exprContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<poseidonVisitor*>(visitor))
    return parserVisitor->visitMultiplicative_expr(this);
  else
    return visitor->visitChildren(this);
}

poseidonParser::Multiplicative_exprContext* poseidonParser::multiplicative_expr() {
  Multiplicative_exprContext *_localctx = _tracker.createInstance<Multiplicative_exprContext>(_ctx, getState());
  enterRule(_localctx, 94, poseidonParser::RuleMultiplicative_expr);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(451);
    unary_expr();
    setState(456);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (((((_la - 65) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 65)) & 7) != 0)) {
      setState(452);
      _la = _input->LA(1);
      if (!(((((_la - 65) & ~ 0x3fULL) == 0) &&
        ((1ULL << (_la - 65)) & 7) != 0))) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(453);
      unary_expr();
      setState(458);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Unary_exprContext ------------------------------------------------------------------

poseidonParser::Unary_exprContext::Unary_exprContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

poseidonParser::Primary_exprContext* poseidonParser::Unary_exprContext::primary_expr() {
  return getRuleContext<poseidonParser::Primary_exprContext>(0);
}

tree::TerminalNode* poseidonParser::Unary_exprContext::NOT() {
  return getToken(poseidonParser::NOT, 0);
}


size_t poseidonParser::Unary_exprContext::getRuleIndex() const {
  return poseidonParser::RuleUnary_expr;
}


std::any poseidonParser::Unary_exprContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<poseidonVisitor*>(visitor))
    return parserVisitor->visitUnary_expr(this);
  else
    return visitor->visitChildren(this);
}

poseidonParser::Unary_exprContext* poseidonParser::unary_expr() {
  Unary_exprContext *_localctx = _tracker.createInstance<Unary_exprContext>(_ctx, getState());
  enterRule(_localctx, 96, poseidonParser::RuleUnary_expr);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(460);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == poseidonParser::NOT) {
      setState(459);
      match(poseidonParser::NOT);
    }
    setState(462);
    primary_expr();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Primary_exprContext ------------------------------------------------------------------

poseidonParser::Primary_exprContext::Primary_exprContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* poseidonParser::Primary_exprContext::LPAREN() {
  return getToken(poseidonParser::LPAREN, 0);
}

poseidonParser::Logical_exprContext* poseidonParser::Primary_exprContext::logical_expr() {
  return getRuleContext<poseidonParser::Logical_exprContext>(0);
}

tree::TerminalNode* poseidonParser::Primary_exprContext::RPAREN() {
  return getToken(poseidonParser::RPAREN, 0);
}

poseidonParser::Function_callContext* poseidonParser::Primary_exprContext::function_call() {
  return getRuleContext<poseidonParser::Function_callContext>(0);
}

poseidonParser::ValueContext* poseidonParser::Primary_exprContext::value() {
  return getRuleContext<poseidonParser::ValueContext>(0);
}

poseidonParser::VariableContext* poseidonParser::Primary_exprContext::variable() {
  return getRuleContext<poseidonParser::VariableContext>(0);
}


size_t poseidonParser::Primary_exprContext::getRuleIndex() const {
  return poseidonParser::RulePrimary_expr;
}


std::any poseidonParser::Primary_exprContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<poseidonVisitor*>(visitor))
    return parserVisitor->visitPrimary_expr(this);
  else
    return visitor->visitChildren(this);
}

poseidonParser::Primary_exprContext* poseidonParser::primary_expr() {
  Primary_exprContext *_localctx = _tracker.createInstance<Primary_exprContext>(_ctx, getState());
  enterRule(_localctx, 98, poseidonParser::RulePrimary_expr);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(471);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case poseidonParser::LPAREN: {
        enterOuterAlt(_localctx, 1);
        setState(464);
        match(poseidonParser::LPAREN);
        setState(465);
        logical_expr();
        setState(466);
        match(poseidonParser::RPAREN);
        break;
      }

      case poseidonParser::UDF_: {
        enterOuterAlt(_localctx, 2);
        setState(468);
        function_call();
        break;
      }

      case poseidonParser::INTEGER:
      case poseidonParser::FLOAT:
      case poseidonParser::STRING_: {
        enterOuterAlt(_localctx, 3);
        setState(469);
        value();
        break;
      }

      case poseidonParser::Var: {
        enterOuterAlt(_localctx, 4);
        setState(470);
        variable();
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- VariableContext ------------------------------------------------------------------

poseidonParser::VariableContext::VariableContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* poseidonParser::VariableContext::Var() {
  return getToken(poseidonParser::Var, 0);
}

tree::TerminalNode* poseidonParser::VariableContext::Identifier_() {
  return getToken(poseidonParser::Identifier_, 0);
}


size_t poseidonParser::VariableContext::getRuleIndex() const {
  return poseidonParser::RuleVariable;
}


std::any poseidonParser::VariableContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<poseidonVisitor*>(visitor))
    return parserVisitor->visitVariable(this);
  else
    return visitor->visitChildren(this);
}

poseidonParser::VariableContext* poseidonParser::variable() {
  VariableContext *_localctx = _tracker.createInstance<VariableContext>(_ctx, getState());
  enterRule(_localctx, 100, poseidonParser::RuleVariable);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(473);
    match(poseidonParser::Var);
    setState(474);
    match(poseidonParser::T__0);
    setState(475);
    match(poseidonParser::Identifier_);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ValueContext ------------------------------------------------------------------

poseidonParser::ValueContext::ValueContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* poseidonParser::ValueContext::INTEGER() {
  return getToken(poseidonParser::INTEGER, 0);
}

tree::TerminalNode* poseidonParser::ValueContext::FLOAT() {
  return getToken(poseidonParser::FLOAT, 0);
}

tree::TerminalNode* poseidonParser::ValueContext::STRING_() {
  return getToken(poseidonParser::STRING_, 0);
}


size_t poseidonParser::ValueContext::getRuleIndex() const {
  return poseidonParser::RuleValue;
}


std::any poseidonParser::ValueContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<poseidonVisitor*>(visitor))
    return parserVisitor->visitValue(this);
  else
    return visitor->visitChildren(this);
}

poseidonParser::ValueContext* poseidonParser::value() {
  ValueContext *_localctx = _tracker.createInstance<ValueContext>(_ctx, getState());
  enterRule(_localctx, 102, poseidonParser::RuleValue);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(477);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 307863255777280) != 0))) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Function_callContext ------------------------------------------------------------------

poseidonParser::Function_callContext::Function_callContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

poseidonParser::Udf_prefixContext* poseidonParser::Function_callContext::udf_prefix() {
  return getRuleContext<poseidonParser::Udf_prefixContext>(0);
}

tree::TerminalNode* poseidonParser::Function_callContext::Identifier_() {
  return getToken(poseidonParser::Identifier_, 0);
}

tree::TerminalNode* poseidonParser::Function_callContext::LPAREN() {
  return getToken(poseidonParser::LPAREN, 0);
}

tree::TerminalNode* poseidonParser::Function_callContext::RPAREN() {
  return getToken(poseidonParser::RPAREN, 0);
}

poseidonParser::Param_listContext* poseidonParser::Function_callContext::param_list() {
  return getRuleContext<poseidonParser::Param_listContext>(0);
}


size_t poseidonParser::Function_callContext::getRuleIndex() const {
  return poseidonParser::RuleFunction_call;
}


std::any poseidonParser::Function_callContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<poseidonVisitor*>(visitor))
    return parserVisitor->visitFunction_call(this);
  else
    return visitor->visitChildren(this);
}

poseidonParser::Function_callContext* poseidonParser::function_call() {
  Function_callContext *_localctx = _tracker.createInstance<Function_callContext>(_ctx, getState());
  enterRule(_localctx, 104, poseidonParser::RuleFunction_call);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(479);
    udf_prefix();
    setState(480);
    match(poseidonParser::Identifier_);
    setState(481);
    match(poseidonParser::LPAREN);
    setState(483);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 378231999954944) != 0)) {
      setState(482);
      param_list();
    }
    setState(485);
    match(poseidonParser::RPAREN);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Udf_prefixContext ------------------------------------------------------------------

poseidonParser::Udf_prefixContext::Udf_prefixContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* poseidonParser::Udf_prefixContext::UDF_() {
  return getToken(poseidonParser::UDF_, 0);
}

tree::TerminalNode* poseidonParser::Udf_prefixContext::DOUBLE_COLON() {
  return getToken(poseidonParser::DOUBLE_COLON, 0);
}


size_t poseidonParser::Udf_prefixContext::getRuleIndex() const {
  return poseidonParser::RuleUdf_prefix;
}


std::any poseidonParser::Udf_prefixContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<poseidonVisitor*>(visitor))
    return parserVisitor->visitUdf_prefix(this);
  else
    return visitor->visitChildren(this);
}

poseidonParser::Udf_prefixContext* poseidonParser::udf_prefix() {
  Udf_prefixContext *_localctx = _tracker.createInstance<Udf_prefixContext>(_ctx, getState());
  enterRule(_localctx, 106, poseidonParser::RuleUdf_prefix);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(487);
    match(poseidonParser::UDF_);
    setState(488);
    match(poseidonParser::DOUBLE_COLON);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Param_listContext ------------------------------------------------------------------

poseidonParser::Param_listContext::Param_listContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<poseidonParser::ParamContext *> poseidonParser::Param_listContext::param() {
  return getRuleContexts<poseidonParser::ParamContext>();
}

poseidonParser::ParamContext* poseidonParser::Param_listContext::param(size_t i) {
  return getRuleContext<poseidonParser::ParamContext>(i);
}

std::vector<tree::TerminalNode *> poseidonParser::Param_listContext::COMMA_() {
  return getTokens(poseidonParser::COMMA_);
}

tree::TerminalNode* poseidonParser::Param_listContext::COMMA_(size_t i) {
  return getToken(poseidonParser::COMMA_, i);
}


size_t poseidonParser::Param_listContext::getRuleIndex() const {
  return poseidonParser::RuleParam_list;
}


std::any poseidonParser::Param_listContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<poseidonVisitor*>(visitor))
    return parserVisitor->visitParam_list(this);
  else
    return visitor->visitChildren(this);
}

poseidonParser::Param_listContext* poseidonParser::param_list() {
  Param_listContext *_localctx = _tracker.createInstance<Param_listContext>(_ctx, getState());
  enterRule(_localctx, 108, poseidonParser::RuleParam_list);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(490);
    param();
    setState(495);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == poseidonParser::COMMA_) {
      setState(491);
      match(poseidonParser::COMMA_);
      setState(492);
      param();
      setState(497);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ParamContext ------------------------------------------------------------------

poseidonParser::ParamContext::ParamContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

poseidonParser::ValueContext* poseidonParser::ParamContext::value() {
  return getRuleContext<poseidonParser::ValueContext>(0);
}

tree::TerminalNode* poseidonParser::ParamContext::Var() {
  return getToken(poseidonParser::Var, 0);
}

tree::TerminalNode* poseidonParser::ParamContext::COLON_() {
  return getToken(poseidonParser::COLON_, 0);
}

poseidonParser::Type_specContext* poseidonParser::ParamContext::type_spec() {
  return getRuleContext<poseidonParser::Type_specContext>(0);
}

tree::TerminalNode* poseidonParser::ParamContext::Identifier_() {
  return getToken(poseidonParser::Identifier_, 0);
}


size_t poseidonParser::ParamContext::getRuleIndex() const {
  return poseidonParser::RuleParam;
}


std::any poseidonParser::ParamContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<poseidonVisitor*>(visitor))
    return parserVisitor->visitParam(this);
  else
    return visitor->visitChildren(this);
}

poseidonParser::ParamContext* poseidonParser::param() {
  ParamContext *_localctx = _tracker.createInstance<ParamContext>(_ctx, getState());
  enterRule(_localctx, 110, poseidonParser::RuleParam);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(506);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case poseidonParser::INTEGER:
      case poseidonParser::FLOAT:
      case poseidonParser::STRING_: {
        enterOuterAlt(_localctx, 1);
        setState(498);
        value();
        break;
      }

      case poseidonParser::Var: {
        enterOuterAlt(_localctx, 2);
        setState(499);
        match(poseidonParser::Var);
        setState(502);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (_la == poseidonParser::T__0) {
          setState(500);
          match(poseidonParser::T__0);
          setState(501);
          match(poseidonParser::Identifier_);
        }
        setState(504);
        match(poseidonParser::COLON_);
        setState(505);
        type_spec();
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Sort_opContext ------------------------------------------------------------------

poseidonParser::Sort_opContext::Sort_opContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* poseidonParser::Sort_opContext::Sort_() {
  return getToken(poseidonParser::Sort_, 0);
}

tree::TerminalNode* poseidonParser::Sort_opContext::LPAREN() {
  return getToken(poseidonParser::LPAREN, 0);
}

poseidonParser::Sort_listContext* poseidonParser::Sort_opContext::sort_list() {
  return getRuleContext<poseidonParser::Sort_listContext>(0);
}

tree::TerminalNode* poseidonParser::Sort_opContext::COMMA_() {
  return getToken(poseidonParser::COMMA_, 0);
}

poseidonParser::Query_operatorContext* poseidonParser::Sort_opContext::query_operator() {
  return getRuleContext<poseidonParser::Query_operatorContext>(0);
}

tree::TerminalNode* poseidonParser::Sort_opContext::RPAREN() {
  return getToken(poseidonParser::RPAREN, 0);
}


size_t poseidonParser::Sort_opContext::getRuleIndex() const {
  return poseidonParser::RuleSort_op;
}


std::any poseidonParser::Sort_opContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<poseidonVisitor*>(visitor))
    return parserVisitor->visitSort_op(this);
  else
    return visitor->visitChildren(this);
}

poseidonParser::Sort_opContext* poseidonParser::sort_op() {
  Sort_opContext *_localctx = _tracker.createInstance<Sort_opContext>(_ctx, getState());
  enterRule(_localctx, 112, poseidonParser::RuleSort_op);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(508);
    match(poseidonParser::Sort_);
    setState(509);
    match(poseidonParser::LPAREN);
    setState(510);
    sort_list();
    setState(511);
    match(poseidonParser::COMMA_);
    setState(512);
    query_operator();
    setState(513);
    match(poseidonParser::RPAREN);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Sort_listContext ------------------------------------------------------------------

poseidonParser::Sort_listContext::Sort_listContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* poseidonParser::Sort_listContext::LBRACKET() {
  return getToken(poseidonParser::LBRACKET, 0);
}

std::vector<poseidonParser::Sort_exprContext *> poseidonParser::Sort_listContext::sort_expr() {
  return getRuleContexts<poseidonParser::Sort_exprContext>();
}

poseidonParser::Sort_exprContext* poseidonParser::Sort_listContext::sort_expr(size_t i) {
  return getRuleContext<poseidonParser::Sort_exprContext>(i);
}

tree::TerminalNode* poseidonParser::Sort_listContext::RBRACKET() {
  return getToken(poseidonParser::RBRACKET, 0);
}

std::vector<tree::TerminalNode *> poseidonParser::Sort_listContext::COMMA_() {
  return getTokens(poseidonParser::COMMA_);
}

tree::TerminalNode* poseidonParser::Sort_listContext::COMMA_(size_t i) {
  return getToken(poseidonParser::COMMA_, i);
}


size_t poseidonParser::Sort_listContext::getRuleIndex() const {
  return poseidonParser::RuleSort_list;
}


std::any poseidonParser::Sort_listContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<poseidonVisitor*>(visitor))
    return parserVisitor->visitSort_list(this);
  else
    return visitor->visitChildren(this);
}

poseidonParser::Sort_listContext* poseidonParser::sort_list() {
  Sort_listContext *_localctx = _tracker.createInstance<Sort_listContext>(_ctx, getState());
  enterRule(_localctx, 114, poseidonParser::RuleSort_list);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(515);
    match(poseidonParser::LBRACKET);
    setState(516);
    sort_expr();
    setState(521);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == poseidonParser::COMMA_) {
      setState(517);
      match(poseidonParser::COMMA_);
      setState(518);
      sort_expr();
      setState(523);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(524);
    match(poseidonParser::RBRACKET);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Sort_exprContext ------------------------------------------------------------------

poseidonParser::Sort_exprContext::Sort_exprContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* poseidonParser::Sort_exprContext::Var() {
  return getToken(poseidonParser::Var, 0);
}

tree::TerminalNode* poseidonParser::Sort_exprContext::COLON_() {
  return getToken(poseidonParser::COLON_, 0);
}

poseidonParser::Type_specContext* poseidonParser::Sort_exprContext::type_spec() {
  return getRuleContext<poseidonParser::Type_specContext>(0);
}

poseidonParser::Sort_specContext* poseidonParser::Sort_exprContext::sort_spec() {
  return getRuleContext<poseidonParser::Sort_specContext>(0);
}


size_t poseidonParser::Sort_exprContext::getRuleIndex() const {
  return poseidonParser::RuleSort_expr;
}


std::any poseidonParser::Sort_exprContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<poseidonVisitor*>(visitor))
    return parserVisitor->visitSort_expr(this);
  else
    return visitor->visitChildren(this);
}

poseidonParser::Sort_exprContext* poseidonParser::sort_expr() {
  Sort_exprContext *_localctx = _tracker.createInstance<Sort_exprContext>(_ctx, getState());
  enterRule(_localctx, 116, poseidonParser::RuleSort_expr);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(526);
    match(poseidonParser::Var);
    setState(527);
    match(poseidonParser::COLON_);
    setState(528);
    type_spec();
    setState(529);
    sort_spec();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Sort_specContext ------------------------------------------------------------------

poseidonParser::Sort_specContext::Sort_specContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* poseidonParser::Sort_specContext::DescOrder_() {
  return getToken(poseidonParser::DescOrder_, 0);
}

tree::TerminalNode* poseidonParser::Sort_specContext::AscOrder_() {
  return getToken(poseidonParser::AscOrder_, 0);
}


size_t poseidonParser::Sort_specContext::getRuleIndex() const {
  return poseidonParser::RuleSort_spec;
}


std::any poseidonParser::Sort_specContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<poseidonVisitor*>(visitor))
    return parserVisitor->visitSort_spec(this);
  else
    return visitor->visitChildren(this);
}

poseidonParser::Sort_specContext* poseidonParser::sort_spec() {
  Sort_specContext *_localctx = _tracker.createInstance<Sort_specContext>(_ctx, getState());
  enterRule(_localctx, 118, poseidonParser::RuleSort_spec);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(531);
    _la = _input->LA(1);
    if (!(_la == poseidonParser::DescOrder_

    || _la == poseidonParser::AscOrder_)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Create_opContext ------------------------------------------------------------------

poseidonParser::Create_opContext::Create_opContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* poseidonParser::Create_opContext::Create_() {
  return getToken(poseidonParser::Create_, 0);
}

tree::TerminalNode* poseidonParser::Create_opContext::LPAREN() {
  return getToken(poseidonParser::LPAREN, 0);
}

tree::TerminalNode* poseidonParser::Create_opContext::RPAREN() {
  return getToken(poseidonParser::RPAREN, 0);
}

poseidonParser::Create_rshipContext* poseidonParser::Create_opContext::create_rship() {
  return getRuleContext<poseidonParser::Create_rshipContext>(0);
}

poseidonParser::Create_nodeContext* poseidonParser::Create_opContext::create_node() {
  return getRuleContext<poseidonParser::Create_nodeContext>(0);
}

tree::TerminalNode* poseidonParser::Create_opContext::COMMA_() {
  return getToken(poseidonParser::COMMA_, 0);
}

poseidonParser::Query_operatorContext* poseidonParser::Create_opContext::query_operator() {
  return getRuleContext<poseidonParser::Query_operatorContext>(0);
}


size_t poseidonParser::Create_opContext::getRuleIndex() const {
  return poseidonParser::RuleCreate_op;
}


std::any poseidonParser::Create_opContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<poseidonVisitor*>(visitor))
    return parserVisitor->visitCreate_op(this);
  else
    return visitor->visitChildren(this);
}

poseidonParser::Create_opContext* poseidonParser::create_op() {
  Create_opContext *_localctx = _tracker.createInstance<Create_opContext>(_ctx, getState());
  enterRule(_localctx, 120, poseidonParser::RuleCreate_op);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(533);
    match(poseidonParser::Create_);
    setState(534);
    match(poseidonParser::LPAREN);
    setState(537);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 32, _ctx)) {
    case 1: {
      setState(535);
      create_rship();
      break;
    }

    case 2: {
      setState(536);
      create_node();
      break;
    }

    default:
      break;
    }
    setState(541);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == poseidonParser::COMMA_) {
      setState(539);
      match(poseidonParser::COMMA_);
      setState(540);
      query_operator();
    }
    setState(543);
    match(poseidonParser::RPAREN);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Create_nodeContext ------------------------------------------------------------------

poseidonParser::Create_nodeContext::Create_nodeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* poseidonParser::Create_nodeContext::LPAREN() {
  return getToken(poseidonParser::LPAREN, 0);
}

std::vector<tree::TerminalNode *> poseidonParser::Create_nodeContext::Identifier_() {
  return getTokens(poseidonParser::Identifier_);
}

tree::TerminalNode* poseidonParser::Create_nodeContext::Identifier_(size_t i) {
  return getToken(poseidonParser::Identifier_, i);
}

tree::TerminalNode* poseidonParser::Create_nodeContext::COLON_() {
  return getToken(poseidonParser::COLON_, 0);
}

tree::TerminalNode* poseidonParser::Create_nodeContext::RPAREN() {
  return getToken(poseidonParser::RPAREN, 0);
}

poseidonParser::Property_listContext* poseidonParser::Create_nodeContext::property_list() {
  return getRuleContext<poseidonParser::Property_listContext>(0);
}


size_t poseidonParser::Create_nodeContext::getRuleIndex() const {
  return poseidonParser::RuleCreate_node;
}


std::any poseidonParser::Create_nodeContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<poseidonVisitor*>(visitor))
    return parserVisitor->visitCreate_node(this);
  else
    return visitor->visitChildren(this);
}

poseidonParser::Create_nodeContext* poseidonParser::create_node() {
  Create_nodeContext *_localctx = _tracker.createInstance<Create_nodeContext>(_ctx, getState());
  enterRule(_localctx, 122, poseidonParser::RuleCreate_node);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(545);
    match(poseidonParser::LPAREN);
    setState(546);
    match(poseidonParser::Identifier_);
    setState(547);
    match(poseidonParser::COLON_);
    setState(548);
    match(poseidonParser::Identifier_);
    setState(550);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == poseidonParser::T__5) {
      setState(549);
      property_list();
    }
    setState(552);
    match(poseidonParser::RPAREN);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Property_listContext ------------------------------------------------------------------

poseidonParser::Property_listContext::Property_listContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<poseidonParser::PropertyContext *> poseidonParser::Property_listContext::property() {
  return getRuleContexts<poseidonParser::PropertyContext>();
}

poseidonParser::PropertyContext* poseidonParser::Property_listContext::property(size_t i) {
  return getRuleContext<poseidonParser::PropertyContext>(i);
}

std::vector<tree::TerminalNode *> poseidonParser::Property_listContext::COMMA_() {
  return getTokens(poseidonParser::COMMA_);
}

tree::TerminalNode* poseidonParser::Property_listContext::COMMA_(size_t i) {
  return getToken(poseidonParser::COMMA_, i);
}


size_t poseidonParser::Property_listContext::getRuleIndex() const {
  return poseidonParser::RuleProperty_list;
}


std::any poseidonParser::Property_listContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<poseidonVisitor*>(visitor))
    return parserVisitor->visitProperty_list(this);
  else
    return visitor->visitChildren(this);
}

poseidonParser::Property_listContext* poseidonParser::property_list() {
  Property_listContext *_localctx = _tracker.createInstance<Property_listContext>(_ctx, getState());
  enterRule(_localctx, 124, poseidonParser::RuleProperty_list);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(554);
    match(poseidonParser::T__5);
    setState(555);
    property();
    setState(560);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == poseidonParser::COMMA_) {
      setState(556);
      match(poseidonParser::COMMA_);
      setState(557);
      property();
      setState(562);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(563);
    match(poseidonParser::T__6);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- PropertyContext ------------------------------------------------------------------

poseidonParser::PropertyContext::PropertyContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* poseidonParser::PropertyContext::Identifier_() {
  return getToken(poseidonParser::Identifier_, 0);
}

tree::TerminalNode* poseidonParser::PropertyContext::COLON_() {
  return getToken(poseidonParser::COLON_, 0);
}

poseidonParser::ValueContext* poseidonParser::PropertyContext::value() {
  return getRuleContext<poseidonParser::ValueContext>(0);
}


size_t poseidonParser::PropertyContext::getRuleIndex() const {
  return poseidonParser::RuleProperty;
}


std::any poseidonParser::PropertyContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<poseidonVisitor*>(visitor))
    return parserVisitor->visitProperty(this);
  else
    return visitor->visitChildren(this);
}

poseidonParser::PropertyContext* poseidonParser::property() {
  PropertyContext *_localctx = _tracker.createInstance<PropertyContext>(_ctx, getState());
  enterRule(_localctx, 126, poseidonParser::RuleProperty);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(565);
    match(poseidonParser::Identifier_);
    setState(566);
    match(poseidonParser::COLON_);
    setState(567);
    value();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Create_rshipContext ------------------------------------------------------------------

poseidonParser::Create_rshipContext::Create_rshipContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<poseidonParser::Node_varContext *> poseidonParser::Create_rshipContext::node_var() {
  return getRuleContexts<poseidonParser::Node_varContext>();
}

poseidonParser::Node_varContext* poseidonParser::Create_rshipContext::node_var(size_t i) {
  return getRuleContext<poseidonParser::Node_varContext>(i);
}

tree::TerminalNode* poseidonParser::Create_rshipContext::LBRACKET() {
  return getToken(poseidonParser::LBRACKET, 0);
}

tree::TerminalNode* poseidonParser::Create_rshipContext::COLON_() {
  return getToken(poseidonParser::COLON_, 0);
}

std::vector<tree::TerminalNode *> poseidonParser::Create_rshipContext::Identifier_() {
  return getTokens(poseidonParser::Identifier_);
}

tree::TerminalNode* poseidonParser::Create_rshipContext::Identifier_(size_t i) {
  return getToken(poseidonParser::Identifier_, i);
}

tree::TerminalNode* poseidonParser::Create_rshipContext::RBRACKET() {
  return getToken(poseidonParser::RBRACKET, 0);
}

poseidonParser::Property_listContext* poseidonParser::Create_rshipContext::property_list() {
  return getRuleContext<poseidonParser::Property_listContext>(0);
}


size_t poseidonParser::Create_rshipContext::getRuleIndex() const {
  return poseidonParser::RuleCreate_rship;
}


std::any poseidonParser::Create_rshipContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<poseidonVisitor*>(visitor))
    return parserVisitor->visitCreate_rship(this);
  else
    return visitor->visitChildren(this);
}

poseidonParser::Create_rshipContext* poseidonParser::create_rship() {
  Create_rshipContext *_localctx = _tracker.createInstance<Create_rshipContext>(_ctx, getState());
  enterRule(_localctx, 128, poseidonParser::RuleCreate_rship);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(569);
    node_var();
    setState(570);
    match(poseidonParser::T__4);
    setState(571);
    match(poseidonParser::LBRACKET);
    setState(573);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == poseidonParser::Identifier_) {
      setState(572);
      match(poseidonParser::Identifier_);
    }
    setState(575);
    match(poseidonParser::COLON_);
    setState(576);
    match(poseidonParser::Identifier_);
    setState(578);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == poseidonParser::T__5) {
      setState(577);
      property_list();
    }
    setState(580);
    match(poseidonParser::RBRACKET);
    setState(581);
    match(poseidonParser::T__3);
    setState(582);
    node_var();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Node_varContext ------------------------------------------------------------------

poseidonParser::Node_varContext::Node_varContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* poseidonParser::Node_varContext::LPAREN() {
  return getToken(poseidonParser::LPAREN, 0);
}

tree::TerminalNode* poseidonParser::Node_varContext::Var() {
  return getToken(poseidonParser::Var, 0);
}

tree::TerminalNode* poseidonParser::Node_varContext::RPAREN() {
  return getToken(poseidonParser::RPAREN, 0);
}


size_t poseidonParser::Node_varContext::getRuleIndex() const {
  return poseidonParser::RuleNode_var;
}


std::any poseidonParser::Node_varContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<poseidonVisitor*>(visitor))
    return parserVisitor->visitNode_var(this);
  else
    return visitor->visitChildren(this);
}

poseidonParser::Node_varContext* poseidonParser::node_var() {
  Node_varContext *_localctx = _tracker.createInstance<Node_varContext>(_ctx, getState());
  enterRule(_localctx, 130, poseidonParser::RuleNode_var);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(584);
    match(poseidonParser::LPAREN);
    setState(585);
    match(poseidonParser::Var);
    setState(586);
    match(poseidonParser::RPAREN);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

void poseidonParser::initialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  poseidonParserInitialize();
#else
  ::antlr4::internal::call_once(poseidonParserOnceFlag, poseidonParserInitialize);
#endif
}
