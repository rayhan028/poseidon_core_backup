
// Generated from src/query/parser/poseidon.g4 by ANTLR 4.13.1


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
      "query", "query_operator", "node_scan_op", "rship_scan_op", "scan_param", 
      "scan_list", "index_scan_op", "index_scan_param", "project_op", "proj_list", 
      "proj_expr", "type_spec", "case_expr", "case_result", "limit_op", 
      "crossjoin_op", "hashjoin_op", "exists_op", "all_exists", "leftouterjoin_op", 
      "nljoin_op", "foreach_relationship_op", "rship_dir", "rship_cardinality", 
      "rship_source_var", "expand_op", "expand_dir", "match_op", "path_pattern", 
      "path_component", "node_pattern", "rship_pattern", "cardinality_spec", 
      "min_cardinality", "max_cardinality", "dir_spec", "left_dir", "right_dir", 
      "no_dir", "aggregate_op", "aggregate_list", "aggr_expr", "aggr_func", 
      "union_op", "except_op", "group_by_op", "grouping_list", "grouping_expr", 
      "distinct_op", "filter_op", "logical_expr", "boolean_expr", "equality_expr", 
      "relational_expr", "additive_expr", "multiplicative_expr", "unary_expr", 
      "primary_expr", "variable", "value", "function_call", "prefix", "param_list", 
      "param", "sort_op", "sort_list", "sort_expr", "sort_spec", "create_op", 
      "create_node", "property_list", "property", "create_rship", "node_var", 
      "remove_node_op", "remove_relationship_op", "detach_node_op", "algorithm_op", 
      "call_mode", "algo_param_list", "algo_param"
    },
    std::vector<std::string>{
      "", "'..'", "'<-'", "'->'", "'-'", "'.'", "'{'", "'}'", "'Filter'", 
      "'NodeScan'", "'RelationshipScan'", "'IndexScan'", "'Match'", "'Project'", 
      "'Limit'", "'CrossJoin'", "'HashJoin'", "'Exists'", "'NotExists'", 
      "'NLJoin'", "'LeftOuterJoin'", "'Expand'", "'ForeachRelationship'", 
      "'Aggregate'", "'GroupBy'", "'Sort'", "'Distinct'", "'Create'", "'Union'", 
      "'Except'", "'RemoveNode'", "'RemoveRelationship'", "'DetachNode'", 
      "'Algorithm'", "'Case'", "'int'", "'uint64'", "'double'", "'string'", 
      "'datetime'", "'node'", "'rship'", "'count'", "'sum'", "'avg'", "'min'", 
      "'max'", "'udf'", "'pb'", "'IN'", "'OUT'", "'TUPLE'", "'SET'", "'FROM'", 
      "'TO'", "'ALL'", "'DESC'", "'ASC'", "", "", "", "", "", "", "':'", 
      "'::'", "','", "'('", "')'", "'['", "']'", "", "", "", "", "'<'", 
      "'<='", "'>'", "'>='", "'+'", "'*'", "'/'", "'%'", "'=~'"
    },
    std::vector<std::string>{
      "", "", "", "", "", "", "", "", "Filter_", "Nodescan_", "Rshipscan_", 
      "Indexscan_", "Match_", "Project_", "Limit_", "CrossJoin_", "HashJoin_", 
      "Exists_", "NotExists_", "NLJoin_", "LeftOuterJoin_", "Expand_", "ForeachRelationship_", 
      "Aggregate_", "GroupBy_", "Sort_", "Distinct_", "Create_", "Union_", 
      "Except_", "RemoveNode_", "RemoveRelationship_", "DetachNode_", "Algorithm_", 
      "Case_", "IntType_", "Uint64Type_", "DoubleType_", "StringType_", 
      "DateType_", "NodeResultType_", "RshipResultType_", "Count_", "Sum_", 
      "Avg_", "Min_", "Max_", "UDF_", "BUILTIN_", "InExpandDir_", "OutExpandDir_", 
      "TupleMode_", "ResultSetMode_", "FromDir_", "ToDir_", "AllDir_", "DescOrder_", 
      "AscOrder_", "INTEGER", "FLOAT", "Identifier_", "Var", "NAME_", "STRING_", 
      "COLON_", "DOUBLE_COLON", "COMMA_", "LPAREN", "RPAREN", "LBRACKET", 
      "RBRACKET", "OR", "AND", "EQUALS", "NOTEQUALS", "LT", "LTEQ", "GT", 
      "GTEQ", "PLUS_", "MULT", "DIV", "MOD", "REGEX", "NOT", "WHITESPACE", 
      "COMMENT"
    }
  );
  static const int32_t serializedATNSegment[] = {
  	4,1,86,729,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,6,2,
  	7,7,7,2,8,7,8,2,9,7,9,2,10,7,10,2,11,7,11,2,12,7,12,2,13,7,13,2,14,7,
  	14,2,15,7,15,2,16,7,16,2,17,7,17,2,18,7,18,2,19,7,19,2,20,7,20,2,21,7,
  	21,2,22,7,22,2,23,7,23,2,24,7,24,2,25,7,25,2,26,7,26,2,27,7,27,2,28,7,
  	28,2,29,7,29,2,30,7,30,2,31,7,31,2,32,7,32,2,33,7,33,2,34,7,34,2,35,7,
  	35,2,36,7,36,2,37,7,37,2,38,7,38,2,39,7,39,2,40,7,40,2,41,7,41,2,42,7,
  	42,2,43,7,43,2,44,7,44,2,45,7,45,2,46,7,46,2,47,7,47,2,48,7,48,2,49,7,
  	49,2,50,7,50,2,51,7,51,2,52,7,52,2,53,7,53,2,54,7,54,2,55,7,55,2,56,7,
  	56,2,57,7,57,2,58,7,58,2,59,7,59,2,60,7,60,2,61,7,61,2,62,7,62,2,63,7,
  	63,2,64,7,64,2,65,7,65,2,66,7,66,2,67,7,67,2,68,7,68,2,69,7,69,2,70,7,
  	70,2,71,7,71,2,72,7,72,2,73,7,73,2,74,7,74,2,75,7,75,2,76,7,76,2,77,7,
  	77,2,78,7,78,2,79,7,79,2,80,7,80,1,0,1,0,1,0,1,1,1,1,1,1,1,1,1,1,1,1,
  	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  	1,3,1,190,8,1,1,2,1,2,1,2,3,2,195,8,2,1,2,1,2,1,3,1,3,1,3,3,3,202,8,3,
  	1,3,1,3,1,4,1,4,3,4,208,8,4,1,5,1,5,1,5,1,5,5,5,214,8,5,10,5,12,5,217,
  	9,5,1,5,1,5,1,6,1,6,1,6,1,6,1,6,1,7,1,7,1,7,1,7,1,7,1,7,1,8,1,8,1,8,1,
  	8,1,8,1,8,1,8,1,9,1,9,1,9,1,9,5,9,243,8,9,10,9,12,9,246,9,9,1,9,1,9,1,
  	10,1,10,1,10,1,10,3,10,254,8,10,1,11,1,11,1,12,1,12,1,12,1,12,1,12,1,
  	12,1,12,1,12,1,12,1,13,1,13,3,13,269,8,13,1,14,1,14,1,14,1,14,1,14,1,
  	14,1,14,1,15,1,15,1,15,1,15,1,15,1,15,1,15,1,16,1,16,1,16,1,16,1,16,1,
  	16,1,16,1,16,1,16,1,16,1,16,1,16,1,16,1,16,1,16,1,17,1,17,1,17,1,17,1,
  	17,1,17,1,17,1,18,1,18,1,19,1,19,1,19,1,19,1,19,1,19,1,19,1,19,1,19,1,
  	20,1,20,1,20,1,20,1,20,1,20,1,20,1,20,1,20,1,21,1,21,1,21,1,21,1,21,1,
  	21,1,21,3,21,334,8,21,1,21,1,21,3,21,338,8,21,1,21,1,21,3,21,342,8,21,
  	1,21,1,21,1,22,1,22,1,23,1,23,1,23,1,23,1,24,1,24,1,25,1,25,1,25,1,25,
  	1,25,1,25,1,25,1,25,1,25,1,26,1,26,1,27,1,27,1,27,1,27,1,27,1,28,1,28,
  	5,28,372,8,28,10,28,12,28,375,9,28,1,29,1,29,1,29,1,30,1,30,3,30,382,
  	8,30,1,30,1,30,1,30,3,30,387,8,30,1,30,1,30,1,31,1,31,1,31,3,31,394,8,
  	31,1,31,1,31,1,31,3,31,399,8,31,1,31,1,31,1,31,1,32,1,32,1,32,1,32,3,
  	32,408,8,32,1,33,1,33,1,34,1,34,1,35,1,35,1,35,3,35,417,8,35,1,36,1,36,
  	1,37,1,37,1,38,1,38,1,39,1,39,1,39,1,39,1,39,1,39,1,39,1,40,1,40,1,40,
  	1,40,5,40,436,8,40,10,40,12,40,439,9,40,1,40,1,40,1,41,1,41,1,41,1,41,
  	1,41,1,42,1,42,1,43,1,43,1,43,1,43,1,43,1,43,1,43,1,44,1,44,1,44,1,44,
  	1,44,1,44,1,44,1,44,1,44,1,44,1,44,1,44,1,44,1,44,1,44,1,45,1,45,1,45,
  	1,45,1,45,1,45,1,45,1,45,1,45,1,46,1,46,1,46,1,46,5,46,485,8,46,10,46,
  	12,46,488,9,46,1,46,1,46,1,47,1,47,1,48,1,48,1,48,1,48,1,48,1,49,1,49,
  	1,49,1,49,1,49,1,49,1,49,1,50,1,50,1,50,5,50,509,8,50,10,50,12,50,512,
  	9,50,1,51,1,51,1,51,5,51,517,8,51,10,51,12,51,520,9,51,1,52,1,52,1,52,
  	5,52,525,8,52,10,52,12,52,528,9,52,1,53,1,53,1,53,5,53,533,8,53,10,53,
  	12,53,536,9,53,1,54,1,54,1,54,5,54,541,8,54,10,54,12,54,544,9,54,1,55,
  	1,55,1,55,5,55,549,8,55,10,55,12,55,552,9,55,1,56,3,56,555,8,56,1,56,
  	1,56,1,57,1,57,1,57,1,57,1,57,1,57,1,57,3,57,566,8,57,1,58,1,58,1,58,
  	3,58,571,8,58,1,58,1,58,1,58,1,59,1,59,1,60,1,60,1,60,1,60,1,60,3,60,
  	583,8,60,1,60,1,60,1,61,1,61,1,62,1,62,1,62,5,62,592,8,62,10,62,12,62,
  	595,9,62,1,63,1,63,1,63,1,63,3,63,601,8,63,1,63,1,63,3,63,605,8,63,1,
  	64,1,64,1,64,1,64,1,64,1,64,1,64,1,65,1,65,1,65,1,65,5,65,618,8,65,10,
  	65,12,65,621,9,65,1,65,1,65,1,66,1,66,1,66,1,66,1,66,1,67,1,67,1,68,1,
  	68,1,68,1,68,3,68,636,8,68,1,68,1,68,3,68,640,8,68,1,68,1,68,1,69,1,69,
  	1,69,1,69,1,69,3,69,649,8,69,1,69,1,69,1,70,1,70,1,70,1,70,5,70,657,8,
  	70,10,70,12,70,660,9,70,1,70,1,70,1,71,1,71,1,71,1,71,1,72,1,72,1,72,
  	1,72,3,72,672,8,72,1,72,1,72,1,72,3,72,677,8,72,1,72,1,72,1,72,1,72,1,
  	73,1,73,1,73,1,73,1,74,1,74,1,74,1,74,1,74,1,75,1,75,1,75,1,75,1,75,1,
  	76,1,76,1,76,1,76,1,76,1,77,1,77,1,77,1,77,1,77,1,77,1,77,1,77,3,77,710,
  	8,77,1,77,1,77,1,77,1,77,1,77,1,78,1,78,1,79,1,79,1,79,5,79,722,8,79,
  	10,79,12,79,725,9,79,1,80,1,80,1,80,0,0,81,0,2,4,6,8,10,12,14,16,18,20,
  	22,24,26,28,30,32,34,36,38,40,42,44,46,48,50,52,54,56,58,60,62,64,66,
  	68,70,72,74,76,78,80,82,84,86,88,90,92,94,96,98,100,102,104,106,108,110,
  	112,114,116,118,120,122,124,126,128,130,132,134,136,138,140,142,144,146,
  	148,150,152,154,156,158,160,0,13,1,0,35,41,1,0,17,18,1,0,53,55,1,0,49,
  	50,1,0,42,46,1,0,73,74,2,0,75,78,83,83,2,0,4,4,79,79,1,0,80,82,2,0,58,
  	59,63,63,1,0,47,48,1,0,56,57,1,0,51,52,716,0,162,1,0,0,0,2,189,1,0,0,
  	0,4,191,1,0,0,0,6,198,1,0,0,0,8,207,1,0,0,0,10,209,1,0,0,0,12,220,1,0,
  	0,0,14,225,1,0,0,0,16,231,1,0,0,0,18,238,1,0,0,0,20,253,1,0,0,0,22,255,
  	1,0,0,0,24,257,1,0,0,0,26,268,1,0,0,0,28,270,1,0,0,0,30,277,1,0,0,0,32,
  	284,1,0,0,0,34,299,1,0,0,0,36,306,1,0,0,0,38,308,1,0,0,0,40,317,1,0,0,
  	0,42,326,1,0,0,0,44,345,1,0,0,0,46,347,1,0,0,0,48,351,1,0,0,0,50,353,
  	1,0,0,0,52,362,1,0,0,0,54,364,1,0,0,0,56,369,1,0,0,0,58,376,1,0,0,0,60,
  	379,1,0,0,0,62,390,1,0,0,0,64,403,1,0,0,0,66,409,1,0,0,0,68,411,1,0,0,
  	0,70,416,1,0,0,0,72,418,1,0,0,0,74,420,1,0,0,0,76,422,1,0,0,0,78,424,
  	1,0,0,0,80,431,1,0,0,0,82,442,1,0,0,0,84,447,1,0,0,0,86,449,1,0,0,0,88,
  	456,1,0,0,0,90,471,1,0,0,0,92,480,1,0,0,0,94,491,1,0,0,0,96,493,1,0,0,
  	0,98,498,1,0,0,0,100,505,1,0,0,0,102,513,1,0,0,0,104,521,1,0,0,0,106,
  	529,1,0,0,0,108,537,1,0,0,0,110,545,1,0,0,0,112,554,1,0,0,0,114,565,1,
  	0,0,0,116,567,1,0,0,0,118,575,1,0,0,0,120,577,1,0,0,0,122,586,1,0,0,0,
  	124,588,1,0,0,0,126,604,1,0,0,0,128,606,1,0,0,0,130,613,1,0,0,0,132,624,
  	1,0,0,0,134,629,1,0,0,0,136,631,1,0,0,0,138,643,1,0,0,0,140,652,1,0,0,
  	0,142,663,1,0,0,0,144,667,1,0,0,0,146,682,1,0,0,0,148,686,1,0,0,0,150,
  	691,1,0,0,0,152,696,1,0,0,0,154,701,1,0,0,0,156,716,1,0,0,0,158,718,1,
  	0,0,0,160,726,1,0,0,0,162,163,3,2,1,0,163,164,5,0,0,1,164,1,1,0,0,0,165,
  	190,3,98,49,0,166,190,3,4,2,0,167,190,3,6,3,0,168,190,3,12,6,0,169,190,
  	3,54,27,0,170,190,3,16,8,0,171,190,3,28,14,0,172,190,3,30,15,0,173,190,
  	3,32,16,0,174,190,3,38,19,0,175,190,3,42,21,0,176,190,3,50,25,0,177,190,
  	3,78,39,0,178,190,3,90,45,0,179,190,3,86,43,0,180,190,3,88,44,0,181,190,
  	3,34,17,0,182,190,3,128,64,0,183,190,3,96,48,0,184,190,3,136,68,0,185,
  	190,3,148,74,0,186,190,3,152,76,0,187,190,3,150,75,0,188,190,3,154,77,
  	0,189,165,1,0,0,0,189,166,1,0,0,0,189,167,1,0,0,0,189,168,1,0,0,0,189,
  	169,1,0,0,0,189,170,1,0,0,0,189,171,1,0,0,0,189,172,1,0,0,0,189,173,1,
  	0,0,0,189,174,1,0,0,0,189,175,1,0,0,0,189,176,1,0,0,0,189,177,1,0,0,0,
  	189,178,1,0,0,0,189,179,1,0,0,0,189,180,1,0,0,0,189,181,1,0,0,0,189,182,
  	1,0,0,0,189,183,1,0,0,0,189,184,1,0,0,0,189,185,1,0,0,0,189,186,1,0,0,
  	0,189,187,1,0,0,0,189,188,1,0,0,0,190,3,1,0,0,0,191,192,5,9,0,0,192,194,
  	5,67,0,0,193,195,3,8,4,0,194,193,1,0,0,0,194,195,1,0,0,0,195,196,1,0,
  	0,0,196,197,5,68,0,0,197,5,1,0,0,0,198,199,5,10,0,0,199,201,5,67,0,0,
  	200,202,3,8,4,0,201,200,1,0,0,0,201,202,1,0,0,0,202,203,1,0,0,0,203,204,
  	5,68,0,0,204,7,1,0,0,0,205,208,5,63,0,0,206,208,3,10,5,0,207,205,1,0,
  	0,0,207,206,1,0,0,0,208,9,1,0,0,0,209,210,5,69,0,0,210,215,5,63,0,0,211,
  	212,5,66,0,0,212,214,5,63,0,0,213,211,1,0,0,0,214,217,1,0,0,0,215,213,
  	1,0,0,0,215,216,1,0,0,0,216,218,1,0,0,0,217,215,1,0,0,0,218,219,5,70,
  	0,0,219,11,1,0,0,0,220,221,5,11,0,0,221,222,5,67,0,0,222,223,3,14,7,0,
  	223,224,5,68,0,0,224,13,1,0,0,0,225,226,5,63,0,0,226,227,5,66,0,0,227,
  	228,5,63,0,0,228,229,5,66,0,0,229,230,3,118,59,0,230,15,1,0,0,0,231,232,
  	5,13,0,0,232,233,5,67,0,0,233,234,3,18,9,0,234,235,5,66,0,0,235,236,3,
  	2,1,0,236,237,5,68,0,0,237,17,1,0,0,0,238,239,5,69,0,0,239,244,3,20,10,
  	0,240,241,5,66,0,0,241,243,3,20,10,0,242,240,1,0,0,0,243,246,1,0,0,0,
  	244,242,1,0,0,0,244,245,1,0,0,0,245,247,1,0,0,0,246,244,1,0,0,0,247,248,
  	5,70,0,0,248,19,1,0,0,0,249,254,3,120,60,0,250,254,3,116,58,0,251,254,
  	3,108,54,0,252,254,3,24,12,0,253,249,1,0,0,0,253,250,1,0,0,0,253,251,
  	1,0,0,0,253,252,1,0,0,0,254,21,1,0,0,0,255,256,7,0,0,0,256,23,1,0,0,0,
  	257,258,5,34,0,0,258,259,5,67,0,0,259,260,3,100,50,0,260,261,5,66,0,0,
  	261,262,3,26,13,0,262,263,5,66,0,0,263,264,3,26,13,0,264,265,5,68,0,0,
  	265,25,1,0,0,0,266,269,3,116,58,0,267,269,3,118,59,0,268,266,1,0,0,0,
  	268,267,1,0,0,0,269,27,1,0,0,0,270,271,5,14,0,0,271,272,5,67,0,0,272,
  	273,5,58,0,0,273,274,5,66,0,0,274,275,3,2,1,0,275,276,5,68,0,0,276,29,
  	1,0,0,0,277,278,5,15,0,0,278,279,5,67,0,0,279,280,3,2,1,0,280,281,5,66,
  	0,0,281,282,3,2,1,0,282,283,5,68,0,0,283,31,1,0,0,0,284,285,5,16,0,0,
  	285,286,5,67,0,0,286,287,5,69,0,0,287,288,3,116,58,0,288,289,5,70,0,0,
  	289,290,5,66,0,0,290,291,5,69,0,0,291,292,3,116,58,0,292,293,5,70,0,0,
  	293,294,5,66,0,0,294,295,3,2,1,0,295,296,5,66,0,0,296,297,3,2,1,0,297,
  	298,5,68,0,0,298,33,1,0,0,0,299,300,3,36,18,0,300,301,5,67,0,0,301,302,
  	3,2,1,0,302,303,5,66,0,0,303,304,3,2,1,0,304,305,5,68,0,0,305,35,1,0,
  	0,0,306,307,7,1,0,0,307,37,1,0,0,0,308,309,5,20,0,0,309,310,5,67,0,0,
  	310,311,3,100,50,0,311,312,5,66,0,0,312,313,3,2,1,0,313,314,5,66,0,0,
  	314,315,3,2,1,0,315,316,5,68,0,0,316,39,1,0,0,0,317,318,5,19,0,0,318,
  	319,5,67,0,0,319,320,3,100,50,0,320,321,5,66,0,0,321,322,3,2,1,0,322,
  	323,5,66,0,0,323,324,3,2,1,0,324,325,5,68,0,0,325,41,1,0,0,0,326,327,
  	5,22,0,0,327,328,5,67,0,0,328,329,3,44,22,0,329,330,5,66,0,0,330,333,
  	5,63,0,0,331,332,5,66,0,0,332,334,3,46,23,0,333,331,1,0,0,0,333,334,1,
  	0,0,0,334,337,1,0,0,0,335,336,5,66,0,0,336,338,3,48,24,0,337,335,1,0,
  	0,0,337,338,1,0,0,0,338,341,1,0,0,0,339,340,5,66,0,0,340,342,3,2,1,0,
  	341,339,1,0,0,0,341,342,1,0,0,0,342,343,1,0,0,0,343,344,5,68,0,0,344,
  	43,1,0,0,0,345,346,7,2,0,0,346,45,1,0,0,0,347,348,5,58,0,0,348,349,5,
  	66,0,0,349,350,5,58,0,0,350,47,1,0,0,0,351,352,5,61,0,0,352,49,1,0,0,
  	0,353,354,5,21,0,0,354,355,5,67,0,0,355,356,3,52,26,0,356,357,5,66,0,
  	0,357,358,5,63,0,0,358,359,5,66,0,0,359,360,3,2,1,0,360,361,5,68,0,0,
  	361,51,1,0,0,0,362,363,7,3,0,0,363,53,1,0,0,0,364,365,5,12,0,0,365,366,
  	5,67,0,0,366,367,3,56,28,0,367,368,5,68,0,0,368,55,1,0,0,0,369,373,3,
  	60,30,0,370,372,3,58,29,0,371,370,1,0,0,0,372,375,1,0,0,0,373,371,1,0,
  	0,0,373,374,1,0,0,0,374,57,1,0,0,0,375,373,1,0,0,0,376,377,3,62,31,0,
  	377,378,3,60,30,0,378,59,1,0,0,0,379,381,5,67,0,0,380,382,5,60,0,0,381,
  	380,1,0,0,0,381,382,1,0,0,0,382,383,1,0,0,0,383,384,5,64,0,0,384,386,
  	5,60,0,0,385,387,3,140,70,0,386,385,1,0,0,0,386,387,1,0,0,0,387,388,1,
  	0,0,0,388,389,5,68,0,0,389,61,1,0,0,0,390,391,3,70,35,0,391,393,5,69,
  	0,0,392,394,5,60,0,0,393,392,1,0,0,0,393,394,1,0,0,0,394,395,1,0,0,0,
  	395,396,5,64,0,0,396,398,5,60,0,0,397,399,3,64,32,0,398,397,1,0,0,0,398,
  	399,1,0,0,0,399,400,1,0,0,0,400,401,5,70,0,0,401,402,3,70,35,0,402,63,
  	1,0,0,0,403,404,5,80,0,0,404,405,3,66,33,0,405,407,5,1,0,0,406,408,3,
  	68,34,0,407,406,1,0,0,0,407,408,1,0,0,0,408,65,1,0,0,0,409,410,5,58,0,
  	0,410,67,1,0,0,0,411,412,5,58,0,0,412,69,1,0,0,0,413,417,3,72,36,0,414,
  	417,3,74,37,0,415,417,3,76,38,0,416,413,1,0,0,0,416,414,1,0,0,0,416,415,
  	1,0,0,0,417,71,1,0,0,0,418,419,5,2,0,0,419,73,1,0,0,0,420,421,5,3,0,0,
  	421,75,1,0,0,0,422,423,5,4,0,0,423,77,1,0,0,0,424,425,5,23,0,0,425,426,
  	5,67,0,0,426,427,3,80,40,0,427,428,5,66,0,0,428,429,3,2,1,0,429,430,5,
  	68,0,0,430,79,1,0,0,0,431,432,5,69,0,0,432,437,3,82,41,0,433,434,5,66,
  	0,0,434,436,3,82,41,0,435,433,1,0,0,0,436,439,1,0,0,0,437,435,1,0,0,0,
  	437,438,1,0,0,0,438,440,1,0,0,0,439,437,1,0,0,0,440,441,5,70,0,0,441,
  	81,1,0,0,0,442,443,3,84,42,0,443,444,5,67,0,0,444,445,3,116,58,0,445,
  	446,5,68,0,0,446,83,1,0,0,0,447,448,7,4,0,0,448,85,1,0,0,0,449,450,5,
  	28,0,0,450,451,5,67,0,0,451,452,3,2,1,0,452,453,5,66,0,0,453,454,3,2,
  	1,0,454,455,5,68,0,0,455,87,1,0,0,0,456,457,5,29,0,0,457,458,5,67,0,0,
  	458,459,5,69,0,0,459,460,3,116,58,0,460,461,5,70,0,0,461,462,5,66,0,0,
  	462,463,5,69,0,0,463,464,3,116,58,0,464,465,5,70,0,0,465,466,5,66,0,0,
  	466,467,3,2,1,0,467,468,5,66,0,0,468,469,3,2,1,0,469,470,5,68,0,0,470,
  	89,1,0,0,0,471,472,5,24,0,0,472,473,5,67,0,0,473,474,3,92,46,0,474,475,
  	5,66,0,0,475,476,3,80,40,0,476,477,5,66,0,0,477,478,3,2,1,0,478,479,5,
  	68,0,0,479,91,1,0,0,0,480,481,5,69,0,0,481,486,3,94,47,0,482,483,5,66,
  	0,0,483,485,3,94,47,0,484,482,1,0,0,0,485,488,1,0,0,0,486,484,1,0,0,0,
  	486,487,1,0,0,0,487,489,1,0,0,0,488,486,1,0,0,0,489,490,5,70,0,0,490,
  	93,1,0,0,0,491,492,3,116,58,0,492,95,1,0,0,0,493,494,5,26,0,0,494,495,
  	5,67,0,0,495,496,3,2,1,0,496,497,5,68,0,0,497,97,1,0,0,0,498,499,5,8,
  	0,0,499,500,5,67,0,0,500,501,3,100,50,0,501,502,5,66,0,0,502,503,3,2,
  	1,0,503,504,5,68,0,0,504,99,1,0,0,0,505,510,3,102,51,0,506,507,5,71,0,
  	0,507,509,3,102,51,0,508,506,1,0,0,0,509,512,1,0,0,0,510,508,1,0,0,0,
  	510,511,1,0,0,0,511,101,1,0,0,0,512,510,1,0,0,0,513,518,3,104,52,0,514,
  	515,5,72,0,0,515,517,3,104,52,0,516,514,1,0,0,0,517,520,1,0,0,0,518,516,
  	1,0,0,0,518,519,1,0,0,0,519,103,1,0,0,0,520,518,1,0,0,0,521,526,3,106,
  	53,0,522,523,7,5,0,0,523,525,3,106,53,0,524,522,1,0,0,0,525,528,1,0,0,
  	0,526,524,1,0,0,0,526,527,1,0,0,0,527,105,1,0,0,0,528,526,1,0,0,0,529,
  	534,3,108,54,0,530,531,7,6,0,0,531,533,3,108,54,0,532,530,1,0,0,0,533,
  	536,1,0,0,0,534,532,1,0,0,0,534,535,1,0,0,0,535,107,1,0,0,0,536,534,1,
  	0,0,0,537,542,3,110,55,0,538,539,7,7,0,0,539,541,3,110,55,0,540,538,1,
  	0,0,0,541,544,1,0,0,0,542,540,1,0,0,0,542,543,1,0,0,0,543,109,1,0,0,0,
  	544,542,1,0,0,0,545,550,3,112,56,0,546,547,7,8,0,0,547,549,3,112,56,0,
  	548,546,1,0,0,0,549,552,1,0,0,0,550,548,1,0,0,0,550,551,1,0,0,0,551,111,
  	1,0,0,0,552,550,1,0,0,0,553,555,5,84,0,0,554,553,1,0,0,0,554,555,1,0,
  	0,0,555,556,1,0,0,0,556,557,3,114,57,0,557,113,1,0,0,0,558,559,5,67,0,
  	0,559,560,3,100,50,0,560,561,5,68,0,0,561,566,1,0,0,0,562,566,3,120,60,
  	0,563,566,3,118,59,0,564,566,3,116,58,0,565,558,1,0,0,0,565,562,1,0,0,
  	0,565,563,1,0,0,0,565,564,1,0,0,0,566,115,1,0,0,0,567,570,5,61,0,0,568,
  	569,5,5,0,0,569,571,5,60,0,0,570,568,1,0,0,0,570,571,1,0,0,0,571,572,
  	1,0,0,0,572,573,5,64,0,0,573,574,3,22,11,0,574,117,1,0,0,0,575,576,7,
  	9,0,0,576,119,1,0,0,0,577,578,3,122,61,0,578,579,5,65,0,0,579,580,5,60,
  	0,0,580,582,5,67,0,0,581,583,3,124,62,0,582,581,1,0,0,0,582,583,1,0,0,
  	0,583,584,1,0,0,0,584,585,5,68,0,0,585,121,1,0,0,0,586,587,7,10,0,0,587,
  	123,1,0,0,0,588,593,3,126,63,0,589,590,5,66,0,0,590,592,3,126,63,0,591,
  	589,1,0,0,0,592,595,1,0,0,0,593,591,1,0,0,0,593,594,1,0,0,0,594,125,1,
  	0,0,0,595,593,1,0,0,0,596,605,3,118,59,0,597,600,5,61,0,0,598,599,5,5,
  	0,0,599,601,5,60,0,0,600,598,1,0,0,0,600,601,1,0,0,0,601,602,1,0,0,0,
  	602,603,5,64,0,0,603,605,3,22,11,0,604,596,1,0,0,0,604,597,1,0,0,0,605,
  	127,1,0,0,0,606,607,5,25,0,0,607,608,5,67,0,0,608,609,3,130,65,0,609,
  	610,5,66,0,0,610,611,3,2,1,0,611,612,5,68,0,0,612,129,1,0,0,0,613,614,
  	5,69,0,0,614,619,3,132,66,0,615,616,5,66,0,0,616,618,3,132,66,0,617,615,
  	1,0,0,0,618,621,1,0,0,0,619,617,1,0,0,0,619,620,1,0,0,0,620,622,1,0,0,
  	0,621,619,1,0,0,0,622,623,5,70,0,0,623,131,1,0,0,0,624,625,5,61,0,0,625,
  	626,5,64,0,0,626,627,3,22,11,0,627,628,3,134,67,0,628,133,1,0,0,0,629,
  	630,7,11,0,0,630,135,1,0,0,0,631,632,5,27,0,0,632,635,5,67,0,0,633,636,
  	3,144,72,0,634,636,3,138,69,0,635,633,1,0,0,0,635,634,1,0,0,0,636,639,
  	1,0,0,0,637,638,5,66,0,0,638,640,3,2,1,0,639,637,1,0,0,0,639,640,1,0,
  	0,0,640,641,1,0,0,0,641,642,5,68,0,0,642,137,1,0,0,0,643,644,5,67,0,0,
  	644,645,5,60,0,0,645,646,5,64,0,0,646,648,5,60,0,0,647,649,3,140,70,0,
  	648,647,1,0,0,0,648,649,1,0,0,0,649,650,1,0,0,0,650,651,5,68,0,0,651,
  	139,1,0,0,0,652,653,5,6,0,0,653,658,3,142,71,0,654,655,5,66,0,0,655,657,
  	3,142,71,0,656,654,1,0,0,0,657,660,1,0,0,0,658,656,1,0,0,0,658,659,1,
  	0,0,0,659,661,1,0,0,0,660,658,1,0,0,0,661,662,5,7,0,0,662,141,1,0,0,0,
  	663,664,5,60,0,0,664,665,5,64,0,0,665,666,3,118,59,0,666,143,1,0,0,0,
  	667,668,3,146,73,0,668,669,5,4,0,0,669,671,5,69,0,0,670,672,5,60,0,0,
  	671,670,1,0,0,0,671,672,1,0,0,0,672,673,1,0,0,0,673,674,5,64,0,0,674,
  	676,5,60,0,0,675,677,3,140,70,0,676,675,1,0,0,0,676,677,1,0,0,0,677,678,
  	1,0,0,0,678,679,5,70,0,0,679,680,5,3,0,0,680,681,3,146,73,0,681,145,1,
  	0,0,0,682,683,5,67,0,0,683,684,5,61,0,0,684,685,5,68,0,0,685,147,1,0,
  	0,0,686,687,5,30,0,0,687,688,5,67,0,0,688,689,3,2,1,0,689,690,5,68,0,
  	0,690,149,1,0,0,0,691,692,5,31,0,0,692,693,5,67,0,0,693,694,3,2,1,0,694,
  	695,5,68,0,0,695,151,1,0,0,0,696,697,5,32,0,0,697,698,5,67,0,0,698,699,
  	3,2,1,0,699,700,5,68,0,0,700,153,1,0,0,0,701,702,5,33,0,0,702,703,5,67,
  	0,0,703,704,5,69,0,0,704,705,5,60,0,0,705,706,5,66,0,0,706,709,3,156,
  	78,0,707,708,5,66,0,0,708,710,3,158,79,0,709,707,1,0,0,0,709,710,1,0,
  	0,0,710,711,1,0,0,0,711,712,5,70,0,0,712,713,5,66,0,0,713,714,3,2,1,0,
  	714,715,5,68,0,0,715,155,1,0,0,0,716,717,7,12,0,0,717,157,1,0,0,0,718,
  	723,3,160,80,0,719,720,5,66,0,0,720,722,3,160,80,0,721,719,1,0,0,0,722,
  	725,1,0,0,0,723,721,1,0,0,0,723,724,1,0,0,0,724,159,1,0,0,0,725,723,1,
  	0,0,0,726,727,3,118,59,0,727,161,1,0,0,0,42,189,194,201,207,215,244,253,
  	268,333,337,341,373,381,386,393,398,407,416,437,486,510,518,526,534,542,
  	550,554,565,570,582,593,600,604,619,635,639,648,658,671,676,709,723
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
    setState(162);
    query_operator();
    setState(163);
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

poseidonParser::Rship_scan_opContext* poseidonParser::Query_operatorContext::rship_scan_op() {
  return getRuleContext<poseidonParser::Rship_scan_opContext>(0);
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

poseidonParser::Except_opContext* poseidonParser::Query_operatorContext::except_op() {
  return getRuleContext<poseidonParser::Except_opContext>(0);
}

poseidonParser::Exists_opContext* poseidonParser::Query_operatorContext::exists_op() {
  return getRuleContext<poseidonParser::Exists_opContext>(0);
}

poseidonParser::Sort_opContext* poseidonParser::Query_operatorContext::sort_op() {
  return getRuleContext<poseidonParser::Sort_opContext>(0);
}

poseidonParser::Distinct_opContext* poseidonParser::Query_operatorContext::distinct_op() {
  return getRuleContext<poseidonParser::Distinct_opContext>(0);
}

poseidonParser::Create_opContext* poseidonParser::Query_operatorContext::create_op() {
  return getRuleContext<poseidonParser::Create_opContext>(0);
}

poseidonParser::Remove_node_opContext* poseidonParser::Query_operatorContext::remove_node_op() {
  return getRuleContext<poseidonParser::Remove_node_opContext>(0);
}

poseidonParser::Detach_node_opContext* poseidonParser::Query_operatorContext::detach_node_op() {
  return getRuleContext<poseidonParser::Detach_node_opContext>(0);
}

poseidonParser::Remove_relationship_opContext* poseidonParser::Query_operatorContext::remove_relationship_op() {
  return getRuleContext<poseidonParser::Remove_relationship_opContext>(0);
}

poseidonParser::Algorithm_opContext* poseidonParser::Query_operatorContext::algorithm_op() {
  return getRuleContext<poseidonParser::Algorithm_opContext>(0);
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
    setState(189);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case poseidonParser::Filter_: {
        enterOuterAlt(_localctx, 1);
        setState(165);
        filter_op();
        break;
      }

      case poseidonParser::Nodescan_: {
        enterOuterAlt(_localctx, 2);
        setState(166);
        node_scan_op();
        break;
      }

      case poseidonParser::Rshipscan_: {
        enterOuterAlt(_localctx, 3);
        setState(167);
        rship_scan_op();
        break;
      }

      case poseidonParser::Indexscan_: {
        enterOuterAlt(_localctx, 4);
        setState(168);
        index_scan_op();
        break;
      }

      case poseidonParser::Match_: {
        enterOuterAlt(_localctx, 5);
        setState(169);
        match_op();
        break;
      }

      case poseidonParser::Project_: {
        enterOuterAlt(_localctx, 6);
        setState(170);
        project_op();
        break;
      }

      case poseidonParser::Limit_: {
        enterOuterAlt(_localctx, 7);
        setState(171);
        limit_op();
        break;
      }

      case poseidonParser::CrossJoin_: {
        enterOuterAlt(_localctx, 8);
        setState(172);
        crossjoin_op();
        break;
      }

      case poseidonParser::HashJoin_: {
        enterOuterAlt(_localctx, 9);
        setState(173);
        hashjoin_op();
        break;
      }

      case poseidonParser::LeftOuterJoin_: {
        enterOuterAlt(_localctx, 10);
        setState(174);
        leftouterjoin_op();
        break;
      }

      case poseidonParser::ForeachRelationship_: {
        enterOuterAlt(_localctx, 11);
        setState(175);
        foreach_relationship_op();
        break;
      }

      case poseidonParser::Expand_: {
        enterOuterAlt(_localctx, 12);
        setState(176);
        expand_op();
        break;
      }

      case poseidonParser::Aggregate_: {
        enterOuterAlt(_localctx, 13);
        setState(177);
        aggregate_op();
        break;
      }

      case poseidonParser::GroupBy_: {
        enterOuterAlt(_localctx, 14);
        setState(178);
        group_by_op();
        break;
      }

      case poseidonParser::Union_: {
        enterOuterAlt(_localctx, 15);
        setState(179);
        union_op();
        break;
      }

      case poseidonParser::Except_: {
        enterOuterAlt(_localctx, 16);
        setState(180);
        except_op();
        break;
      }

      case poseidonParser::Exists_:
      case poseidonParser::NotExists_: {
        enterOuterAlt(_localctx, 17);
        setState(181);
        exists_op();
        break;
      }

      case poseidonParser::Sort_: {
        enterOuterAlt(_localctx, 18);
        setState(182);
        sort_op();
        break;
      }

      case poseidonParser::Distinct_: {
        enterOuterAlt(_localctx, 19);
        setState(183);
        distinct_op();
        break;
      }

      case poseidonParser::Create_: {
        enterOuterAlt(_localctx, 20);
        setState(184);
        create_op();
        break;
      }

      case poseidonParser::RemoveNode_: {
        enterOuterAlt(_localctx, 21);
        setState(185);
        remove_node_op();
        break;
      }

      case poseidonParser::DetachNode_: {
        enterOuterAlt(_localctx, 22);
        setState(186);
        detach_node_op();
        break;
      }

      case poseidonParser::RemoveRelationship_: {
        enterOuterAlt(_localctx, 23);
        setState(187);
        remove_relationship_op();
        break;
      }

      case poseidonParser::Algorithm_: {
        enterOuterAlt(_localctx, 24);
        setState(188);
        algorithm_op();
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
    setState(191);
    match(poseidonParser::Nodescan_);
    setState(192);
    match(poseidonParser::LPAREN);
    setState(194);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == poseidonParser::STRING_

    || _la == poseidonParser::LBRACKET) {
      setState(193);
      scan_param();
    }
    setState(196);
    match(poseidonParser::RPAREN);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Rship_scan_opContext ------------------------------------------------------------------

poseidonParser::Rship_scan_opContext::Rship_scan_opContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* poseidonParser::Rship_scan_opContext::Rshipscan_() {
  return getToken(poseidonParser::Rshipscan_, 0);
}

tree::TerminalNode* poseidonParser::Rship_scan_opContext::LPAREN() {
  return getToken(poseidonParser::LPAREN, 0);
}

tree::TerminalNode* poseidonParser::Rship_scan_opContext::RPAREN() {
  return getToken(poseidonParser::RPAREN, 0);
}

poseidonParser::Scan_paramContext* poseidonParser::Rship_scan_opContext::scan_param() {
  return getRuleContext<poseidonParser::Scan_paramContext>(0);
}


size_t poseidonParser::Rship_scan_opContext::getRuleIndex() const {
  return poseidonParser::RuleRship_scan_op;
}


std::any poseidonParser::Rship_scan_opContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<poseidonVisitor*>(visitor))
    return parserVisitor->visitRship_scan_op(this);
  else
    return visitor->visitChildren(this);
}

poseidonParser::Rship_scan_opContext* poseidonParser::rship_scan_op() {
  Rship_scan_opContext *_localctx = _tracker.createInstance<Rship_scan_opContext>(_ctx, getState());
  enterRule(_localctx, 6, poseidonParser::RuleRship_scan_op);
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
    setState(198);
    match(poseidonParser::Rshipscan_);
    setState(199);
    match(poseidonParser::LPAREN);
    setState(201);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == poseidonParser::STRING_

    || _la == poseidonParser::LBRACKET) {
      setState(200);
      scan_param();
    }
    setState(203);
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
  enterRule(_localctx, 8, poseidonParser::RuleScan_param);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(207);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case poseidonParser::STRING_: {
        enterOuterAlt(_localctx, 1);
        setState(205);
        match(poseidonParser::STRING_);
        break;
      }

      case poseidonParser::LBRACKET: {
        enterOuterAlt(_localctx, 2);
        setState(206);
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
  enterRule(_localctx, 10, poseidonParser::RuleScan_list);
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
    setState(209);
    match(poseidonParser::LBRACKET);
    setState(210);
    match(poseidonParser::STRING_);
    setState(215);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == poseidonParser::COMMA_) {
      setState(211);
      match(poseidonParser::COMMA_);
      setState(212);
      match(poseidonParser::STRING_);
      setState(217);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(218);
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
  enterRule(_localctx, 12, poseidonParser::RuleIndex_scan_op);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(220);
    match(poseidonParser::Indexscan_);
    setState(221);
    match(poseidonParser::LPAREN);
    setState(222);
    index_scan_param();
    setState(223);
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
  enterRule(_localctx, 14, poseidonParser::RuleIndex_scan_param);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(225);
    match(poseidonParser::STRING_);
    setState(226);
    match(poseidonParser::COMMA_);
    setState(227);
    match(poseidonParser::STRING_);
    setState(228);
    match(poseidonParser::COMMA_);
    setState(229);
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
  enterRule(_localctx, 16, poseidonParser::RuleProject_op);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(231);
    match(poseidonParser::Project_);
    setState(232);
    match(poseidonParser::LPAREN);
    setState(233);
    proj_list();
    setState(234);
    match(poseidonParser::COMMA_);
    setState(235);
    query_operator();
    setState(236);
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
  enterRule(_localctx, 18, poseidonParser::RuleProj_list);
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
    setState(238);
    match(poseidonParser::LBRACKET);
    setState(239);
    proj_expr();
    setState(244);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == poseidonParser::COMMA_) {
      setState(240);
      match(poseidonParser::COMMA_);
      setState(241);
      proj_expr();
      setState(246);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(247);
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

poseidonParser::VariableContext* poseidonParser::Proj_exprContext::variable() {
  return getRuleContext<poseidonParser::VariableContext>(0);
}

poseidonParser::Additive_exprContext* poseidonParser::Proj_exprContext::additive_expr() {
  return getRuleContext<poseidonParser::Additive_exprContext>(0);
}

poseidonParser::Case_exprContext* poseidonParser::Proj_exprContext::case_expr() {
  return getRuleContext<poseidonParser::Case_exprContext>(0);
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
  enterRule(_localctx, 20, poseidonParser::RuleProj_expr);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(253);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 6, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(249);
      function_call();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(250);
      variable();
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(251);
      additive_expr();
      break;
    }

    case 4: {
      enterOuterAlt(_localctx, 4);
      setState(252);
      case_expr();
      break;
    }

    default:
      break;
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

tree::TerminalNode* poseidonParser::Type_specContext::NodeResultType_() {
  return getToken(poseidonParser::NodeResultType_, 0);
}

tree::TerminalNode* poseidonParser::Type_specContext::RshipResultType_() {
  return getToken(poseidonParser::RshipResultType_, 0);
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
  enterRule(_localctx, 22, poseidonParser::RuleType_spec);
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
    setState(255);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 4363686772736) != 0))) {
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

//----------------- Case_exprContext ------------------------------------------------------------------

poseidonParser::Case_exprContext::Case_exprContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* poseidonParser::Case_exprContext::Case_() {
  return getToken(poseidonParser::Case_, 0);
}

tree::TerminalNode* poseidonParser::Case_exprContext::LPAREN() {
  return getToken(poseidonParser::LPAREN, 0);
}

poseidonParser::Logical_exprContext* poseidonParser::Case_exprContext::logical_expr() {
  return getRuleContext<poseidonParser::Logical_exprContext>(0);
}

std::vector<tree::TerminalNode *> poseidonParser::Case_exprContext::COMMA_() {
  return getTokens(poseidonParser::COMMA_);
}

tree::TerminalNode* poseidonParser::Case_exprContext::COMMA_(size_t i) {
  return getToken(poseidonParser::COMMA_, i);
}

std::vector<poseidonParser::Case_resultContext *> poseidonParser::Case_exprContext::case_result() {
  return getRuleContexts<poseidonParser::Case_resultContext>();
}

poseidonParser::Case_resultContext* poseidonParser::Case_exprContext::case_result(size_t i) {
  return getRuleContext<poseidonParser::Case_resultContext>(i);
}

tree::TerminalNode* poseidonParser::Case_exprContext::RPAREN() {
  return getToken(poseidonParser::RPAREN, 0);
}


size_t poseidonParser::Case_exprContext::getRuleIndex() const {
  return poseidonParser::RuleCase_expr;
}


std::any poseidonParser::Case_exprContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<poseidonVisitor*>(visitor))
    return parserVisitor->visitCase_expr(this);
  else
    return visitor->visitChildren(this);
}

poseidonParser::Case_exprContext* poseidonParser::case_expr() {
  Case_exprContext *_localctx = _tracker.createInstance<Case_exprContext>(_ctx, getState());
  enterRule(_localctx, 24, poseidonParser::RuleCase_expr);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(257);
    match(poseidonParser::Case_);
    setState(258);
    match(poseidonParser::LPAREN);
    setState(259);
    logical_expr();
    setState(260);
    match(poseidonParser::COMMA_);
    setState(261);
    case_result();
    setState(262);
    match(poseidonParser::COMMA_);
    setState(263);
    case_result();
    setState(264);
    match(poseidonParser::RPAREN);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Case_resultContext ------------------------------------------------------------------

poseidonParser::Case_resultContext::Case_resultContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

poseidonParser::VariableContext* poseidonParser::Case_resultContext::variable() {
  return getRuleContext<poseidonParser::VariableContext>(0);
}

poseidonParser::ValueContext* poseidonParser::Case_resultContext::value() {
  return getRuleContext<poseidonParser::ValueContext>(0);
}


size_t poseidonParser::Case_resultContext::getRuleIndex() const {
  return poseidonParser::RuleCase_result;
}


std::any poseidonParser::Case_resultContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<poseidonVisitor*>(visitor))
    return parserVisitor->visitCase_result(this);
  else
    return visitor->visitChildren(this);
}

poseidonParser::Case_resultContext* poseidonParser::case_result() {
  Case_resultContext *_localctx = _tracker.createInstance<Case_resultContext>(_ctx, getState());
  enterRule(_localctx, 26, poseidonParser::RuleCase_result);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(268);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case poseidonParser::Var: {
        enterOuterAlt(_localctx, 1);
        setState(266);
        variable();
        break;
      }

      case poseidonParser::INTEGER:
      case poseidonParser::FLOAT:
      case poseidonParser::STRING_: {
        enterOuterAlt(_localctx, 2);
        setState(267);
        value();
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
  enterRule(_localctx, 28, poseidonParser::RuleLimit_op);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(270);
    match(poseidonParser::Limit_);
    setState(271);
    match(poseidonParser::LPAREN);
    setState(272);
    match(poseidonParser::INTEGER);
    setState(273);
    match(poseidonParser::COMMA_);
    setState(274);
    query_operator();
    setState(275);
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
  enterRule(_localctx, 30, poseidonParser::RuleCrossjoin_op);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(277);
    match(poseidonParser::CrossJoin_);
    setState(278);
    match(poseidonParser::LPAREN);
    setState(279);
    query_operator();
    setState(280);
    match(poseidonParser::COMMA_);
    setState(281);
    query_operator();
    setState(282);
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

std::vector<tree::TerminalNode *> poseidonParser::Hashjoin_opContext::LBRACKET() {
  return getTokens(poseidonParser::LBRACKET);
}

tree::TerminalNode* poseidonParser::Hashjoin_opContext::LBRACKET(size_t i) {
  return getToken(poseidonParser::LBRACKET, i);
}

std::vector<poseidonParser::VariableContext *> poseidonParser::Hashjoin_opContext::variable() {
  return getRuleContexts<poseidonParser::VariableContext>();
}

poseidonParser::VariableContext* poseidonParser::Hashjoin_opContext::variable(size_t i) {
  return getRuleContext<poseidonParser::VariableContext>(i);
}

std::vector<tree::TerminalNode *> poseidonParser::Hashjoin_opContext::RBRACKET() {
  return getTokens(poseidonParser::RBRACKET);
}

tree::TerminalNode* poseidonParser::Hashjoin_opContext::RBRACKET(size_t i) {
  return getToken(poseidonParser::RBRACKET, i);
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
  enterRule(_localctx, 32, poseidonParser::RuleHashjoin_op);

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
    match(poseidonParser::HashJoin_);
    setState(285);
    match(poseidonParser::LPAREN);
    setState(286);
    match(poseidonParser::LBRACKET);
    setState(287);
    variable();
    setState(288);
    match(poseidonParser::RBRACKET);
    setState(289);
    match(poseidonParser::COMMA_);
    setState(290);
    match(poseidonParser::LBRACKET);
    setState(291);
    variable();
    setState(292);
    match(poseidonParser::RBRACKET);
    setState(293);
    match(poseidonParser::COMMA_);
    setState(294);
    query_operator();
    setState(295);
    match(poseidonParser::COMMA_);
    setState(296);
    query_operator();
    setState(297);
    match(poseidonParser::RPAREN);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Exists_opContext ------------------------------------------------------------------

poseidonParser::Exists_opContext::Exists_opContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

poseidonParser::All_existsContext* poseidonParser::Exists_opContext::all_exists() {
  return getRuleContext<poseidonParser::All_existsContext>(0);
}

tree::TerminalNode* poseidonParser::Exists_opContext::LPAREN() {
  return getToken(poseidonParser::LPAREN, 0);
}

std::vector<poseidonParser::Query_operatorContext *> poseidonParser::Exists_opContext::query_operator() {
  return getRuleContexts<poseidonParser::Query_operatorContext>();
}

poseidonParser::Query_operatorContext* poseidonParser::Exists_opContext::query_operator(size_t i) {
  return getRuleContext<poseidonParser::Query_operatorContext>(i);
}

tree::TerminalNode* poseidonParser::Exists_opContext::COMMA_() {
  return getToken(poseidonParser::COMMA_, 0);
}

tree::TerminalNode* poseidonParser::Exists_opContext::RPAREN() {
  return getToken(poseidonParser::RPAREN, 0);
}


size_t poseidonParser::Exists_opContext::getRuleIndex() const {
  return poseidonParser::RuleExists_op;
}


std::any poseidonParser::Exists_opContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<poseidonVisitor*>(visitor))
    return parserVisitor->visitExists_op(this);
  else
    return visitor->visitChildren(this);
}

poseidonParser::Exists_opContext* poseidonParser::exists_op() {
  Exists_opContext *_localctx = _tracker.createInstance<Exists_opContext>(_ctx, getState());
  enterRule(_localctx, 34, poseidonParser::RuleExists_op);

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
    all_exists();
    setState(300);
    match(poseidonParser::LPAREN);
    setState(301);
    query_operator();
    setState(302);
    match(poseidonParser::COMMA_);
    setState(303);
    query_operator();
    setState(304);
    match(poseidonParser::RPAREN);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- All_existsContext ------------------------------------------------------------------

poseidonParser::All_existsContext::All_existsContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* poseidonParser::All_existsContext::Exists_() {
  return getToken(poseidonParser::Exists_, 0);
}

tree::TerminalNode* poseidonParser::All_existsContext::NotExists_() {
  return getToken(poseidonParser::NotExists_, 0);
}


size_t poseidonParser::All_existsContext::getRuleIndex() const {
  return poseidonParser::RuleAll_exists;
}


std::any poseidonParser::All_existsContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<poseidonVisitor*>(visitor))
    return parserVisitor->visitAll_exists(this);
  else
    return visitor->visitChildren(this);
}

poseidonParser::All_existsContext* poseidonParser::all_exists() {
  All_existsContext *_localctx = _tracker.createInstance<All_existsContext>(_ctx, getState());
  enterRule(_localctx, 36, poseidonParser::RuleAll_exists);
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
    setState(306);
    _la = _input->LA(1);
    if (!(_la == poseidonParser::Exists_

    || _la == poseidonParser::NotExists_)) {
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
  enterRule(_localctx, 38, poseidonParser::RuleLeftouterjoin_op);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(308);
    match(poseidonParser::LeftOuterJoin_);
    setState(309);
    match(poseidonParser::LPAREN);
    setState(310);
    logical_expr();
    setState(311);
    match(poseidonParser::COMMA_);
    setState(312);
    query_operator();
    setState(313);
    match(poseidonParser::COMMA_);
    setState(314);
    query_operator();
    setState(315);
    match(poseidonParser::RPAREN);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Nljoin_opContext ------------------------------------------------------------------

poseidonParser::Nljoin_opContext::Nljoin_opContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* poseidonParser::Nljoin_opContext::NLJoin_() {
  return getToken(poseidonParser::NLJoin_, 0);
}

tree::TerminalNode* poseidonParser::Nljoin_opContext::LPAREN() {
  return getToken(poseidonParser::LPAREN, 0);
}

poseidonParser::Logical_exprContext* poseidonParser::Nljoin_opContext::logical_expr() {
  return getRuleContext<poseidonParser::Logical_exprContext>(0);
}

std::vector<tree::TerminalNode *> poseidonParser::Nljoin_opContext::COMMA_() {
  return getTokens(poseidonParser::COMMA_);
}

tree::TerminalNode* poseidonParser::Nljoin_opContext::COMMA_(size_t i) {
  return getToken(poseidonParser::COMMA_, i);
}

std::vector<poseidonParser::Query_operatorContext *> poseidonParser::Nljoin_opContext::query_operator() {
  return getRuleContexts<poseidonParser::Query_operatorContext>();
}

poseidonParser::Query_operatorContext* poseidonParser::Nljoin_opContext::query_operator(size_t i) {
  return getRuleContext<poseidonParser::Query_operatorContext>(i);
}

tree::TerminalNode* poseidonParser::Nljoin_opContext::RPAREN() {
  return getToken(poseidonParser::RPAREN, 0);
}


size_t poseidonParser::Nljoin_opContext::getRuleIndex() const {
  return poseidonParser::RuleNljoin_op;
}


std::any poseidonParser::Nljoin_opContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<poseidonVisitor*>(visitor))
    return parserVisitor->visitNljoin_op(this);
  else
    return visitor->visitChildren(this);
}

poseidonParser::Nljoin_opContext* poseidonParser::nljoin_op() {
  Nljoin_opContext *_localctx = _tracker.createInstance<Nljoin_opContext>(_ctx, getState());
  enterRule(_localctx, 40, poseidonParser::RuleNljoin_op);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(317);
    match(poseidonParser::NLJoin_);
    setState(318);
    match(poseidonParser::LPAREN);
    setState(319);
    logical_expr();
    setState(320);
    match(poseidonParser::COMMA_);
    setState(321);
    query_operator();
    setState(322);
    match(poseidonParser::COMMA_);
    setState(323);
    query_operator();
    setState(324);
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

tree::TerminalNode* poseidonParser::Foreach_relationship_opContext::RPAREN() {
  return getToken(poseidonParser::RPAREN, 0);
}

poseidonParser::Rship_cardinalityContext* poseidonParser::Foreach_relationship_opContext::rship_cardinality() {
  return getRuleContext<poseidonParser::Rship_cardinalityContext>(0);
}

poseidonParser::Rship_source_varContext* poseidonParser::Foreach_relationship_opContext::rship_source_var() {
  return getRuleContext<poseidonParser::Rship_source_varContext>(0);
}

poseidonParser::Query_operatorContext* poseidonParser::Foreach_relationship_opContext::query_operator() {
  return getRuleContext<poseidonParser::Query_operatorContext>(0);
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
  enterRule(_localctx, 42, poseidonParser::RuleForeach_relationship_op);
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
    setState(326);
    match(poseidonParser::ForeachRelationship_);
    setState(327);
    match(poseidonParser::LPAREN);
    setState(328);
    rship_dir();
    setState(329);
    match(poseidonParser::COMMA_);
    setState(330);
    match(poseidonParser::STRING_);
    setState(333);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 8, _ctx)) {
    case 1: {
      setState(331);
      match(poseidonParser::COMMA_);
      setState(332);
      rship_cardinality();
      break;
    }

    default:
      break;
    }
    setState(337);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 9, _ctx)) {
    case 1: {
      setState(335);
      match(poseidonParser::COMMA_);
      setState(336);
      rship_source_var();
      break;
    }

    default:
      break;
    }
    setState(341);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == poseidonParser::COMMA_) {
      setState(339);
      match(poseidonParser::COMMA_);
      setState(340);
      query_operator();
    }
    setState(343);
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
  enterRule(_localctx, 44, poseidonParser::RuleRship_dir);
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
    setState(345);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 63050394783186944) != 0))) {
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
  enterRule(_localctx, 46, poseidonParser::RuleRship_cardinality);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(347);
    match(poseidonParser::INTEGER);
    setState(348);
    match(poseidonParser::COMMA_);
    setState(349);
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
  enterRule(_localctx, 48, poseidonParser::RuleRship_source_var);

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
  enterRule(_localctx, 50, poseidonParser::RuleExpand_op);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(353);
    match(poseidonParser::Expand_);
    setState(354);
    match(poseidonParser::LPAREN);
    setState(355);
    expand_dir();
    setState(356);
    match(poseidonParser::COMMA_);
    setState(357);
    match(poseidonParser::STRING_);
    setState(358);
    match(poseidonParser::COMMA_);
    setState(359);
    query_operator();
    setState(360);
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
  enterRule(_localctx, 52, poseidonParser::RuleExpand_dir);
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
    setState(362);
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
  enterRule(_localctx, 54, poseidonParser::RuleMatch_op);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(364);
    match(poseidonParser::Match_);
    setState(365);
    match(poseidonParser::LPAREN);
    setState(366);
    path_pattern();
    setState(367);
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
  enterRule(_localctx, 56, poseidonParser::RulePath_pattern);
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
    setState(369);
    node_pattern();
    setState(373);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 28) != 0)) {
      setState(370);
      path_component();
      setState(375);
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
  enterRule(_localctx, 58, poseidonParser::RulePath_component);

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
    rship_pattern();
    setState(377);
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
  enterRule(_localctx, 60, poseidonParser::RuleNode_pattern);
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
    setState(379);
    match(poseidonParser::LPAREN);
    setState(381);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == poseidonParser::Identifier_) {
      setState(380);
      match(poseidonParser::Identifier_);
    }
    setState(383);
    match(poseidonParser::COLON_);
    setState(384);
    match(poseidonParser::Identifier_);
    setState(386);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == poseidonParser::T__5) {
      setState(385);
      property_list();
    }
    setState(388);
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
  enterRule(_localctx, 62, poseidonParser::RuleRship_pattern);
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
    setState(390);
    dir_spec();
    setState(391);
    match(poseidonParser::LBRACKET);
    setState(393);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == poseidonParser::Identifier_) {
      setState(392);
      match(poseidonParser::Identifier_);
    }
    setState(395);
    match(poseidonParser::COLON_);
    setState(396);
    match(poseidonParser::Identifier_);
    setState(398);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == poseidonParser::MULT) {
      setState(397);
      cardinality_spec();
    }
    setState(400);
    match(poseidonParser::RBRACKET);
    setState(401);
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
  enterRule(_localctx, 64, poseidonParser::RuleCardinality_spec);
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
    setState(403);
    match(poseidonParser::MULT);
    setState(404);
    min_cardinality();
    setState(405);
    match(poseidonParser::T__0);
    setState(407);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == poseidonParser::INTEGER) {
      setState(406);
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
  enterRule(_localctx, 66, poseidonParser::RuleMin_cardinality);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(409);
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
  enterRule(_localctx, 68, poseidonParser::RuleMax_cardinality);

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
  enterRule(_localctx, 70, poseidonParser::RuleDir_spec);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(416);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case poseidonParser::T__1: {
        enterOuterAlt(_localctx, 1);
        setState(413);
        left_dir();
        break;
      }

      case poseidonParser::T__2: {
        enterOuterAlt(_localctx, 2);
        setState(414);
        right_dir();
        break;
      }

      case poseidonParser::T__3: {
        enterOuterAlt(_localctx, 3);
        setState(415);
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
  enterRule(_localctx, 72, poseidonParser::RuleLeft_dir);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(418);
    match(poseidonParser::T__1);
   
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
  enterRule(_localctx, 74, poseidonParser::RuleRight_dir);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(420);
    match(poseidonParser::T__2);
   
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
  enterRule(_localctx, 76, poseidonParser::RuleNo_dir);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(422);
    match(poseidonParser::T__3);
   
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
  enterRule(_localctx, 78, poseidonParser::RuleAggregate_op);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(424);
    match(poseidonParser::Aggregate_);
    setState(425);
    match(poseidonParser::LPAREN);
    setState(426);
    aggregate_list();
    setState(427);
    match(poseidonParser::COMMA_);
    setState(428);
    query_operator();
    setState(429);
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
  enterRule(_localctx, 80, poseidonParser::RuleAggregate_list);
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
    setState(431);
    match(poseidonParser::LBRACKET);
    setState(432);
    aggr_expr();
    setState(437);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == poseidonParser::COMMA_) {
      setState(433);
      match(poseidonParser::COMMA_);
      setState(434);
      aggr_expr();
      setState(439);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(440);
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

poseidonParser::VariableContext* poseidonParser::Aggr_exprContext::variable() {
  return getRuleContext<poseidonParser::VariableContext>(0);
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
  enterRule(_localctx, 82, poseidonParser::RuleAggr_expr);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(442);
    aggr_func();
    setState(443);
    match(poseidonParser::LPAREN);
    setState(444);
    variable();
    setState(445);
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
  enterRule(_localctx, 84, poseidonParser::RuleAggr_func);
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
    setState(447);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 136339441844224) != 0))) {
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
  enterRule(_localctx, 86, poseidonParser::RuleUnion_op);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(449);
    match(poseidonParser::Union_);
    setState(450);
    match(poseidonParser::LPAREN);
    setState(451);
    query_operator();
    setState(452);
    match(poseidonParser::COMMA_);
    setState(453);
    query_operator();
    setState(454);
    match(poseidonParser::RPAREN);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Except_opContext ------------------------------------------------------------------

poseidonParser::Except_opContext::Except_opContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* poseidonParser::Except_opContext::Except_() {
  return getToken(poseidonParser::Except_, 0);
}

tree::TerminalNode* poseidonParser::Except_opContext::LPAREN() {
  return getToken(poseidonParser::LPAREN, 0);
}

std::vector<tree::TerminalNode *> poseidonParser::Except_opContext::LBRACKET() {
  return getTokens(poseidonParser::LBRACKET);
}

tree::TerminalNode* poseidonParser::Except_opContext::LBRACKET(size_t i) {
  return getToken(poseidonParser::LBRACKET, i);
}

std::vector<poseidonParser::VariableContext *> poseidonParser::Except_opContext::variable() {
  return getRuleContexts<poseidonParser::VariableContext>();
}

poseidonParser::VariableContext* poseidonParser::Except_opContext::variable(size_t i) {
  return getRuleContext<poseidonParser::VariableContext>(i);
}

std::vector<tree::TerminalNode *> poseidonParser::Except_opContext::RBRACKET() {
  return getTokens(poseidonParser::RBRACKET);
}

tree::TerminalNode* poseidonParser::Except_opContext::RBRACKET(size_t i) {
  return getToken(poseidonParser::RBRACKET, i);
}

std::vector<tree::TerminalNode *> poseidonParser::Except_opContext::COMMA_() {
  return getTokens(poseidonParser::COMMA_);
}

tree::TerminalNode* poseidonParser::Except_opContext::COMMA_(size_t i) {
  return getToken(poseidonParser::COMMA_, i);
}

std::vector<poseidonParser::Query_operatorContext *> poseidonParser::Except_opContext::query_operator() {
  return getRuleContexts<poseidonParser::Query_operatorContext>();
}

poseidonParser::Query_operatorContext* poseidonParser::Except_opContext::query_operator(size_t i) {
  return getRuleContext<poseidonParser::Query_operatorContext>(i);
}

tree::TerminalNode* poseidonParser::Except_opContext::RPAREN() {
  return getToken(poseidonParser::RPAREN, 0);
}


size_t poseidonParser::Except_opContext::getRuleIndex() const {
  return poseidonParser::RuleExcept_op;
}


std::any poseidonParser::Except_opContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<poseidonVisitor*>(visitor))
    return parserVisitor->visitExcept_op(this);
  else
    return visitor->visitChildren(this);
}

poseidonParser::Except_opContext* poseidonParser::except_op() {
  Except_opContext *_localctx = _tracker.createInstance<Except_opContext>(_ctx, getState());
  enterRule(_localctx, 88, poseidonParser::RuleExcept_op);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(456);
    match(poseidonParser::Except_);
    setState(457);
    match(poseidonParser::LPAREN);
    setState(458);
    match(poseidonParser::LBRACKET);
    setState(459);
    variable();
    setState(460);
    match(poseidonParser::RBRACKET);
    setState(461);
    match(poseidonParser::COMMA_);
    setState(462);
    match(poseidonParser::LBRACKET);
    setState(463);
    variable();
    setState(464);
    match(poseidonParser::RBRACKET);
    setState(465);
    match(poseidonParser::COMMA_);
    setState(466);
    query_operator();
    setState(467);
    match(poseidonParser::COMMA_);
    setState(468);
    query_operator();
    setState(469);
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
  enterRule(_localctx, 90, poseidonParser::RuleGroup_by_op);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(471);
    match(poseidonParser::GroupBy_);
    setState(472);
    match(poseidonParser::LPAREN);
    setState(473);
    grouping_list();
    setState(474);
    match(poseidonParser::COMMA_);
    setState(475);
    aggregate_list();
    setState(476);
    match(poseidonParser::COMMA_);
    setState(477);
    query_operator();
    setState(478);
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
  enterRule(_localctx, 92, poseidonParser::RuleGrouping_list);
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
    setState(480);
    match(poseidonParser::LBRACKET);
    setState(481);
    grouping_expr();
    setState(486);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == poseidonParser::COMMA_) {
      setState(482);
      match(poseidonParser::COMMA_);
      setState(483);
      grouping_expr();
      setState(488);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(489);
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

poseidonParser::VariableContext* poseidonParser::Grouping_exprContext::variable() {
  return getRuleContext<poseidonParser::VariableContext>(0);
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
  enterRule(_localctx, 94, poseidonParser::RuleGrouping_expr);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(491);
    variable();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Distinct_opContext ------------------------------------------------------------------

poseidonParser::Distinct_opContext::Distinct_opContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* poseidonParser::Distinct_opContext::Distinct_() {
  return getToken(poseidonParser::Distinct_, 0);
}

tree::TerminalNode* poseidonParser::Distinct_opContext::LPAREN() {
  return getToken(poseidonParser::LPAREN, 0);
}

poseidonParser::Query_operatorContext* poseidonParser::Distinct_opContext::query_operator() {
  return getRuleContext<poseidonParser::Query_operatorContext>(0);
}

tree::TerminalNode* poseidonParser::Distinct_opContext::RPAREN() {
  return getToken(poseidonParser::RPAREN, 0);
}


size_t poseidonParser::Distinct_opContext::getRuleIndex() const {
  return poseidonParser::RuleDistinct_op;
}


std::any poseidonParser::Distinct_opContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<poseidonVisitor*>(visitor))
    return parserVisitor->visitDistinct_op(this);
  else
    return visitor->visitChildren(this);
}

poseidonParser::Distinct_opContext* poseidonParser::distinct_op() {
  Distinct_opContext *_localctx = _tracker.createInstance<Distinct_opContext>(_ctx, getState());
  enterRule(_localctx, 96, poseidonParser::RuleDistinct_op);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(493);
    match(poseidonParser::Distinct_);
    setState(494);
    match(poseidonParser::LPAREN);
    setState(495);
    query_operator();
    setState(496);
    match(poseidonParser::RPAREN);
   
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
  enterRule(_localctx, 98, poseidonParser::RuleFilter_op);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(498);
    match(poseidonParser::Filter_);
    setState(499);
    match(poseidonParser::LPAREN);
    setState(500);
    logical_expr();
    setState(501);
    match(poseidonParser::COMMA_);
    setState(502);
    query_operator();
    setState(503);
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
  enterRule(_localctx, 100, poseidonParser::RuleLogical_expr);
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
    setState(505);
    boolean_expr();
    setState(510);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == poseidonParser::OR) {
      setState(506);
      match(poseidonParser::OR);
      setState(507);
      boolean_expr();
      setState(512);
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
  enterRule(_localctx, 102, poseidonParser::RuleBoolean_expr);
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
    setState(513);
    equality_expr();
    setState(518);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == poseidonParser::AND) {
      setState(514);
      match(poseidonParser::AND);
      setState(515);
      equality_expr();
      setState(520);
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
  enterRule(_localctx, 104, poseidonParser::RuleEquality_expr);
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
    setState(521);
    relational_expr();
    setState(526);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == poseidonParser::EQUALS

    || _la == poseidonParser::NOTEQUALS) {
      setState(522);
      _la = _input->LA(1);
      if (!(_la == poseidonParser::EQUALS

      || _la == poseidonParser::NOTEQUALS)) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(523);
      relational_expr();
      setState(528);
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

std::vector<tree::TerminalNode *> poseidonParser::Relational_exprContext::REGEX() {
  return getTokens(poseidonParser::REGEX);
}

tree::TerminalNode* poseidonParser::Relational_exprContext::REGEX(size_t i) {
  return getToken(poseidonParser::REGEX, i);
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
  enterRule(_localctx, 106, poseidonParser::RuleRelational_expr);
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
    setState(529);
    additive_expr();
    setState(534);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (((((_la - 75) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 75)) & 271) != 0)) {
      setState(530);
      _la = _input->LA(1);
      if (!(((((_la - 75) & ~ 0x3fULL) == 0) &&
        ((1ULL << (_la - 75)) & 271) != 0))) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(531);
      additive_expr();
      setState(536);
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
  enterRule(_localctx, 108, poseidonParser::RuleAdditive_expr);
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
    setState(537);
    multiplicative_expr();
    setState(542);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == poseidonParser::T__3 || _la == poseidonParser::PLUS_) {
      setState(538);
      _la = _input->LA(1);
      if (!(_la == poseidonParser::T__3 || _la == poseidonParser::PLUS_)) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(539);
      multiplicative_expr();
      setState(544);
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
  enterRule(_localctx, 110, poseidonParser::RuleMultiplicative_expr);
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
    unary_expr();
    setState(550);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (((((_la - 80) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 80)) & 7) != 0)) {
      setState(546);
      _la = _input->LA(1);
      if (!(((((_la - 80) & ~ 0x3fULL) == 0) &&
        ((1ULL << (_la - 80)) & 7) != 0))) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(547);
      unary_expr();
      setState(552);
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
  enterRule(_localctx, 112, poseidonParser::RuleUnary_expr);
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
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == poseidonParser::NOT) {
      setState(553);
      match(poseidonParser::NOT);
    }
    setState(556);
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
  enterRule(_localctx, 114, poseidonParser::RulePrimary_expr);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(565);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case poseidonParser::LPAREN: {
        enterOuterAlt(_localctx, 1);
        setState(558);
        match(poseidonParser::LPAREN);
        setState(559);
        logical_expr();
        setState(560);
        match(poseidonParser::RPAREN);
        break;
      }

      case poseidonParser::UDF_:
      case poseidonParser::BUILTIN_: {
        enterOuterAlt(_localctx, 2);
        setState(562);
        function_call();
        break;
      }

      case poseidonParser::INTEGER:
      case poseidonParser::FLOAT:
      case poseidonParser::STRING_: {
        enterOuterAlt(_localctx, 3);
        setState(563);
        value();
        break;
      }

      case poseidonParser::Var: {
        enterOuterAlt(_localctx, 4);
        setState(564);
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

tree::TerminalNode* poseidonParser::VariableContext::COLON_() {
  return getToken(poseidonParser::COLON_, 0);
}

poseidonParser::Type_specContext* poseidonParser::VariableContext::type_spec() {
  return getRuleContext<poseidonParser::Type_specContext>(0);
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
  enterRule(_localctx, 116, poseidonParser::RuleVariable);
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
    setState(567);
    match(poseidonParser::Var);
    setState(570);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == poseidonParser::T__4) {
      setState(568);
      match(poseidonParser::T__4);
      setState(569);
      match(poseidonParser::Identifier_);
    }
    setState(572);
    match(poseidonParser::COLON_);
    setState(573);
    type_spec();
   
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
  enterRule(_localctx, 118, poseidonParser::RuleValue);
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
    setState(575);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & -8358680908399640576) != 0))) {
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

poseidonParser::PrefixContext* poseidonParser::Function_callContext::prefix() {
  return getRuleContext<poseidonParser::PrefixContext>(0);
}

tree::TerminalNode* poseidonParser::Function_callContext::DOUBLE_COLON() {
  return getToken(poseidonParser::DOUBLE_COLON, 0);
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
  enterRule(_localctx, 120, poseidonParser::RuleFunction_call);
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
    setState(577);
    prefix();
    setState(578);
    match(poseidonParser::DOUBLE_COLON);
    setState(579);
    match(poseidonParser::Identifier_);
    setState(580);
    match(poseidonParser::LPAREN);
    setState(582);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & -6052837899185946624) != 0)) {
      setState(581);
      param_list();
    }
    setState(584);
    match(poseidonParser::RPAREN);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- PrefixContext ------------------------------------------------------------------

poseidonParser::PrefixContext::PrefixContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* poseidonParser::PrefixContext::BUILTIN_() {
  return getToken(poseidonParser::BUILTIN_, 0);
}

tree::TerminalNode* poseidonParser::PrefixContext::UDF_() {
  return getToken(poseidonParser::UDF_, 0);
}


size_t poseidonParser::PrefixContext::getRuleIndex() const {
  return poseidonParser::RulePrefix;
}


std::any poseidonParser::PrefixContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<poseidonVisitor*>(visitor))
    return parserVisitor->visitPrefix(this);
  else
    return visitor->visitChildren(this);
}

poseidonParser::PrefixContext* poseidonParser::prefix() {
  PrefixContext *_localctx = _tracker.createInstance<PrefixContext>(_ctx, getState());
  enterRule(_localctx, 122, poseidonParser::RulePrefix);
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
    setState(586);
    _la = _input->LA(1);
    if (!(_la == poseidonParser::UDF_

    || _la == poseidonParser::BUILTIN_)) {
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
  enterRule(_localctx, 124, poseidonParser::RuleParam_list);
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
    setState(588);
    param();
    setState(593);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == poseidonParser::COMMA_) {
      setState(589);
      match(poseidonParser::COMMA_);
      setState(590);
      param();
      setState(595);
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
  enterRule(_localctx, 126, poseidonParser::RuleParam);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(604);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case poseidonParser::INTEGER:
      case poseidonParser::FLOAT:
      case poseidonParser::STRING_: {
        enterOuterAlt(_localctx, 1);
        setState(596);
        value();
        break;
      }

      case poseidonParser::Var: {
        enterOuterAlt(_localctx, 2);
        setState(597);
        match(poseidonParser::Var);
        setState(600);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (_la == poseidonParser::T__4) {
          setState(598);
          match(poseidonParser::T__4);
          setState(599);
          match(poseidonParser::Identifier_);
        }
        setState(602);
        match(poseidonParser::COLON_);
        setState(603);
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
  enterRule(_localctx, 128, poseidonParser::RuleSort_op);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(606);
    match(poseidonParser::Sort_);
    setState(607);
    match(poseidonParser::LPAREN);
    setState(608);
    sort_list();
    setState(609);
    match(poseidonParser::COMMA_);
    setState(610);
    query_operator();
    setState(611);
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
  enterRule(_localctx, 130, poseidonParser::RuleSort_list);
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
    setState(613);
    match(poseidonParser::LBRACKET);
    setState(614);
    sort_expr();
    setState(619);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == poseidonParser::COMMA_) {
      setState(615);
      match(poseidonParser::COMMA_);
      setState(616);
      sort_expr();
      setState(621);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(622);
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
  enterRule(_localctx, 132, poseidonParser::RuleSort_expr);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(624);
    match(poseidonParser::Var);
    setState(625);
    match(poseidonParser::COLON_);
    setState(626);
    type_spec();
    setState(627);
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
  enterRule(_localctx, 134, poseidonParser::RuleSort_spec);
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
    setState(629);
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
  enterRule(_localctx, 136, poseidonParser::RuleCreate_op);
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
    setState(631);
    match(poseidonParser::Create_);
    setState(632);
    match(poseidonParser::LPAREN);
    setState(635);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 34, _ctx)) {
    case 1: {
      setState(633);
      create_rship();
      break;
    }

    case 2: {
      setState(634);
      create_node();
      break;
    }

    default:
      break;
    }
    setState(639);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == poseidonParser::COMMA_) {
      setState(637);
      match(poseidonParser::COMMA_);
      setState(638);
      query_operator();
    }
    setState(641);
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
  enterRule(_localctx, 138, poseidonParser::RuleCreate_node);
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
    setState(643);
    match(poseidonParser::LPAREN);
    setState(644);
    match(poseidonParser::Identifier_);
    setState(645);
    match(poseidonParser::COLON_);
    setState(646);
    match(poseidonParser::Identifier_);
    setState(648);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == poseidonParser::T__5) {
      setState(647);
      property_list();
    }
    setState(650);
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
  enterRule(_localctx, 140, poseidonParser::RuleProperty_list);
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
    setState(652);
    match(poseidonParser::T__5);
    setState(653);
    property();
    setState(658);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == poseidonParser::COMMA_) {
      setState(654);
      match(poseidonParser::COMMA_);
      setState(655);
      property();
      setState(660);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(661);
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
  enterRule(_localctx, 142, poseidonParser::RuleProperty);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(663);
    match(poseidonParser::Identifier_);
    setState(664);
    match(poseidonParser::COLON_);
    setState(665);
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
  enterRule(_localctx, 144, poseidonParser::RuleCreate_rship);
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
    setState(667);
    node_var();
    setState(668);
    match(poseidonParser::T__3);
    setState(669);
    match(poseidonParser::LBRACKET);
    setState(671);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == poseidonParser::Identifier_) {
      setState(670);
      match(poseidonParser::Identifier_);
    }
    setState(673);
    match(poseidonParser::COLON_);
    setState(674);
    match(poseidonParser::Identifier_);
    setState(676);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == poseidonParser::T__5) {
      setState(675);
      property_list();
    }
    setState(678);
    match(poseidonParser::RBRACKET);
    setState(679);
    match(poseidonParser::T__2);
    setState(680);
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
  enterRule(_localctx, 146, poseidonParser::RuleNode_var);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(682);
    match(poseidonParser::LPAREN);
    setState(683);
    match(poseidonParser::Var);
    setState(684);
    match(poseidonParser::RPAREN);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Remove_node_opContext ------------------------------------------------------------------

poseidonParser::Remove_node_opContext::Remove_node_opContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* poseidonParser::Remove_node_opContext::RemoveNode_() {
  return getToken(poseidonParser::RemoveNode_, 0);
}

tree::TerminalNode* poseidonParser::Remove_node_opContext::LPAREN() {
  return getToken(poseidonParser::LPAREN, 0);
}

poseidonParser::Query_operatorContext* poseidonParser::Remove_node_opContext::query_operator() {
  return getRuleContext<poseidonParser::Query_operatorContext>(0);
}

tree::TerminalNode* poseidonParser::Remove_node_opContext::RPAREN() {
  return getToken(poseidonParser::RPAREN, 0);
}


size_t poseidonParser::Remove_node_opContext::getRuleIndex() const {
  return poseidonParser::RuleRemove_node_op;
}


std::any poseidonParser::Remove_node_opContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<poseidonVisitor*>(visitor))
    return parserVisitor->visitRemove_node_op(this);
  else
    return visitor->visitChildren(this);
}

poseidonParser::Remove_node_opContext* poseidonParser::remove_node_op() {
  Remove_node_opContext *_localctx = _tracker.createInstance<Remove_node_opContext>(_ctx, getState());
  enterRule(_localctx, 148, poseidonParser::RuleRemove_node_op);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(686);
    match(poseidonParser::RemoveNode_);
    setState(687);
    match(poseidonParser::LPAREN);
    setState(688);
    query_operator();
    setState(689);
    match(poseidonParser::RPAREN);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Remove_relationship_opContext ------------------------------------------------------------------

poseidonParser::Remove_relationship_opContext::Remove_relationship_opContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* poseidonParser::Remove_relationship_opContext::RemoveRelationship_() {
  return getToken(poseidonParser::RemoveRelationship_, 0);
}

tree::TerminalNode* poseidonParser::Remove_relationship_opContext::LPAREN() {
  return getToken(poseidonParser::LPAREN, 0);
}

poseidonParser::Query_operatorContext* poseidonParser::Remove_relationship_opContext::query_operator() {
  return getRuleContext<poseidonParser::Query_operatorContext>(0);
}

tree::TerminalNode* poseidonParser::Remove_relationship_opContext::RPAREN() {
  return getToken(poseidonParser::RPAREN, 0);
}


size_t poseidonParser::Remove_relationship_opContext::getRuleIndex() const {
  return poseidonParser::RuleRemove_relationship_op;
}


std::any poseidonParser::Remove_relationship_opContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<poseidonVisitor*>(visitor))
    return parserVisitor->visitRemove_relationship_op(this);
  else
    return visitor->visitChildren(this);
}

poseidonParser::Remove_relationship_opContext* poseidonParser::remove_relationship_op() {
  Remove_relationship_opContext *_localctx = _tracker.createInstance<Remove_relationship_opContext>(_ctx, getState());
  enterRule(_localctx, 150, poseidonParser::RuleRemove_relationship_op);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(691);
    match(poseidonParser::RemoveRelationship_);
    setState(692);
    match(poseidonParser::LPAREN);
    setState(693);
    query_operator();
    setState(694);
    match(poseidonParser::RPAREN);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Detach_node_opContext ------------------------------------------------------------------

poseidonParser::Detach_node_opContext::Detach_node_opContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* poseidonParser::Detach_node_opContext::DetachNode_() {
  return getToken(poseidonParser::DetachNode_, 0);
}

tree::TerminalNode* poseidonParser::Detach_node_opContext::LPAREN() {
  return getToken(poseidonParser::LPAREN, 0);
}

poseidonParser::Query_operatorContext* poseidonParser::Detach_node_opContext::query_operator() {
  return getRuleContext<poseidonParser::Query_operatorContext>(0);
}

tree::TerminalNode* poseidonParser::Detach_node_opContext::RPAREN() {
  return getToken(poseidonParser::RPAREN, 0);
}


size_t poseidonParser::Detach_node_opContext::getRuleIndex() const {
  return poseidonParser::RuleDetach_node_op;
}


std::any poseidonParser::Detach_node_opContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<poseidonVisitor*>(visitor))
    return parserVisitor->visitDetach_node_op(this);
  else
    return visitor->visitChildren(this);
}

poseidonParser::Detach_node_opContext* poseidonParser::detach_node_op() {
  Detach_node_opContext *_localctx = _tracker.createInstance<Detach_node_opContext>(_ctx, getState());
  enterRule(_localctx, 152, poseidonParser::RuleDetach_node_op);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(696);
    match(poseidonParser::DetachNode_);
    setState(697);
    match(poseidonParser::LPAREN);
    setState(698);
    query_operator();
    setState(699);
    match(poseidonParser::RPAREN);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Algorithm_opContext ------------------------------------------------------------------

poseidonParser::Algorithm_opContext::Algorithm_opContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* poseidonParser::Algorithm_opContext::Algorithm_() {
  return getToken(poseidonParser::Algorithm_, 0);
}

tree::TerminalNode* poseidonParser::Algorithm_opContext::LPAREN() {
  return getToken(poseidonParser::LPAREN, 0);
}

tree::TerminalNode* poseidonParser::Algorithm_opContext::LBRACKET() {
  return getToken(poseidonParser::LBRACKET, 0);
}

tree::TerminalNode* poseidonParser::Algorithm_opContext::Identifier_() {
  return getToken(poseidonParser::Identifier_, 0);
}

std::vector<tree::TerminalNode *> poseidonParser::Algorithm_opContext::COMMA_() {
  return getTokens(poseidonParser::COMMA_);
}

tree::TerminalNode* poseidonParser::Algorithm_opContext::COMMA_(size_t i) {
  return getToken(poseidonParser::COMMA_, i);
}

poseidonParser::Call_modeContext* poseidonParser::Algorithm_opContext::call_mode() {
  return getRuleContext<poseidonParser::Call_modeContext>(0);
}

tree::TerminalNode* poseidonParser::Algorithm_opContext::RBRACKET() {
  return getToken(poseidonParser::RBRACKET, 0);
}

poseidonParser::Query_operatorContext* poseidonParser::Algorithm_opContext::query_operator() {
  return getRuleContext<poseidonParser::Query_operatorContext>(0);
}

tree::TerminalNode* poseidonParser::Algorithm_opContext::RPAREN() {
  return getToken(poseidonParser::RPAREN, 0);
}

poseidonParser::Algo_param_listContext* poseidonParser::Algorithm_opContext::algo_param_list() {
  return getRuleContext<poseidonParser::Algo_param_listContext>(0);
}


size_t poseidonParser::Algorithm_opContext::getRuleIndex() const {
  return poseidonParser::RuleAlgorithm_op;
}


std::any poseidonParser::Algorithm_opContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<poseidonVisitor*>(visitor))
    return parserVisitor->visitAlgorithm_op(this);
  else
    return visitor->visitChildren(this);
}

poseidonParser::Algorithm_opContext* poseidonParser::algorithm_op() {
  Algorithm_opContext *_localctx = _tracker.createInstance<Algorithm_opContext>(_ctx, getState());
  enterRule(_localctx, 154, poseidonParser::RuleAlgorithm_op);
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
    setState(701);
    match(poseidonParser::Algorithm_);
    setState(702);
    match(poseidonParser::LPAREN);
    setState(703);
    match(poseidonParser::LBRACKET);
    setState(704);
    match(poseidonParser::Identifier_);
    setState(705);
    match(poseidonParser::COMMA_);
    setState(706);
    call_mode();
    setState(709);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == poseidonParser::COMMA_) {
      setState(707);
      match(poseidonParser::COMMA_);
      setState(708);
      algo_param_list();
    }
    setState(711);
    match(poseidonParser::RBRACKET);
    setState(712);
    match(poseidonParser::COMMA_);
    setState(713);
    query_operator();
    setState(714);
    match(poseidonParser::RPAREN);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Call_modeContext ------------------------------------------------------------------

poseidonParser::Call_modeContext::Call_modeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* poseidonParser::Call_modeContext::TupleMode_() {
  return getToken(poseidonParser::TupleMode_, 0);
}

tree::TerminalNode* poseidonParser::Call_modeContext::ResultSetMode_() {
  return getToken(poseidonParser::ResultSetMode_, 0);
}


size_t poseidonParser::Call_modeContext::getRuleIndex() const {
  return poseidonParser::RuleCall_mode;
}


std::any poseidonParser::Call_modeContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<poseidonVisitor*>(visitor))
    return parserVisitor->visitCall_mode(this);
  else
    return visitor->visitChildren(this);
}

poseidonParser::Call_modeContext* poseidonParser::call_mode() {
  Call_modeContext *_localctx = _tracker.createInstance<Call_modeContext>(_ctx, getState());
  enterRule(_localctx, 156, poseidonParser::RuleCall_mode);
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
    setState(716);
    _la = _input->LA(1);
    if (!(_la == poseidonParser::TupleMode_

    || _la == poseidonParser::ResultSetMode_)) {
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

//----------------- Algo_param_listContext ------------------------------------------------------------------

poseidonParser::Algo_param_listContext::Algo_param_listContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<poseidonParser::Algo_paramContext *> poseidonParser::Algo_param_listContext::algo_param() {
  return getRuleContexts<poseidonParser::Algo_paramContext>();
}

poseidonParser::Algo_paramContext* poseidonParser::Algo_param_listContext::algo_param(size_t i) {
  return getRuleContext<poseidonParser::Algo_paramContext>(i);
}

std::vector<tree::TerminalNode *> poseidonParser::Algo_param_listContext::COMMA_() {
  return getTokens(poseidonParser::COMMA_);
}

tree::TerminalNode* poseidonParser::Algo_param_listContext::COMMA_(size_t i) {
  return getToken(poseidonParser::COMMA_, i);
}


size_t poseidonParser::Algo_param_listContext::getRuleIndex() const {
  return poseidonParser::RuleAlgo_param_list;
}


std::any poseidonParser::Algo_param_listContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<poseidonVisitor*>(visitor))
    return parserVisitor->visitAlgo_param_list(this);
  else
    return visitor->visitChildren(this);
}

poseidonParser::Algo_param_listContext* poseidonParser::algo_param_list() {
  Algo_param_listContext *_localctx = _tracker.createInstance<Algo_param_listContext>(_ctx, getState());
  enterRule(_localctx, 158, poseidonParser::RuleAlgo_param_list);
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
    setState(718);
    algo_param();
    setState(723);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == poseidonParser::COMMA_) {
      setState(719);
      match(poseidonParser::COMMA_);
      setState(720);
      algo_param();
      setState(725);
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

//----------------- Algo_paramContext ------------------------------------------------------------------

poseidonParser::Algo_paramContext::Algo_paramContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

poseidonParser::ValueContext* poseidonParser::Algo_paramContext::value() {
  return getRuleContext<poseidonParser::ValueContext>(0);
}


size_t poseidonParser::Algo_paramContext::getRuleIndex() const {
  return poseidonParser::RuleAlgo_param;
}


std::any poseidonParser::Algo_paramContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<poseidonVisitor*>(visitor))
    return parserVisitor->visitAlgo_param(this);
  else
    return visitor->visitChildren(this);
}

poseidonParser::Algo_paramContext* poseidonParser::algo_param() {
  Algo_paramContext *_localctx = _tracker.createInstance<Algo_paramContext>(_ctx, getState());
  enterRule(_localctx, 160, poseidonParser::RuleAlgo_param);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(726);
    value();
   
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
