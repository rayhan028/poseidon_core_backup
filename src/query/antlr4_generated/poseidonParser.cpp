
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
      "query", "query_operator", "node_scan_op", "scan_param", "scan_list", 
      "index_scan_op", "index_scan_param", "project_op", "proj_list", "proj_expr", 
      "type_spec", "limit_op", "crossjoin_op", "hashjoin_op", "leftouterjoin_op", 
      "foreach_relationship_op", "rship_dir", "rship_cardinality", "rship_source_var", 
      "expand_op", "expand_dir", "match_op", "path_pattern", "path_component", 
      "node_pattern", "rship_pattern", "cardinality_spec", "min_cardinality", 
      "max_cardinality", "dir_spec", "left_dir", "right_dir", "no_dir", 
      "aggregate_op", "aggregate_list", "aggr_expr", "aggr_func", "union_op", 
      "group_by_op", "grouping_list", "grouping_expr", "distinct_op", "filter_op", 
      "logical_expr", "boolean_expr", "equality_expr", "relational_expr", 
      "additive_expr", "multiplicative_expr", "unary_expr", "primary_expr", 
      "variable", "value", "function_call", "prefix", "param_list", "param", 
      "sort_op", "sort_list", "sort_expr", "sort_spec", "create_op", "create_node", 
      "property_list", "property", "create_rship", "node_var", "remove_node_op", 
      "remove_relationship_op", "detach_node_op", "algorithm_op", "call_mode", 
      "algo_param_list", "algo_param"
    },
    std::vector<std::string>{
      "", "'.'", "'..'", "'<-'", "'->'", "'-'", "'{'", "'}'", "'Filter'", 
      "'NodeScan'", "'IndexScan'", "'Match'", "'Project'", "'Limit'", "'CrossJoin'", 
      "'HashJoin'", "'LeftOuterJoin'", "'Expand'", "'ForeachRelationship'", 
      "'Aggregate'", "'GroupBy'", "'Sort'", "'Distinct'", "'Create'", "'Union'", 
      "'RemoveNode'", "'RemoveRelationship'", "'DetachNode'", "'Algorithm'", 
      "'int'", "'uint64'", "'double'", "'string'", "'datetime'", "'node'", 
      "'rship'", "'count'", "'sum'", "'avg'", "'min'", "'max'", "'udf'", 
      "'pb'", "'IN'", "'OUT'", "'TUPLE'", "'SET'", "'FROM'", "'TO'", "'ALL'", 
      "'DESC'", "'ASC'", "", "", "", "", "", "", "':'", "'::'", "','", "'('", 
      "')'", "'['", "']'", "", "", "", "", "'<'", "'<='", "'>'", "'>='", 
      "'+'", "'*'", "'/'", "'%'", "'=~'"
    },
    std::vector<std::string>{
      "", "", "", "", "", "", "", "", "Filter_", "Nodescan_", "Indexscan_", 
      "Match_", "Project_", "Limit_", "CrossJoin_", "HashJoin_", "LeftOuterJoin_", 
      "Expand_", "ForeachRelationship_", "Aggregate_", "GroupBy_", "Sort_", 
      "Distinct_", "Create_", "Union_", "RemoveNode_", "RemoveRelationship_", 
      "DetachNode_", "Algorithm_", "IntType_", "Uint64Type_", "DoubleType_", 
      "StringType_", "DateType_", "NodeResultType_", "RshipResultType_", 
      "Count_", "Sum_", "Avg_", "Min_", "Max_", "UDF_", "BUILTIN_", "InExpandDir_", 
      "OutExpandDir_", "TupleMode_", "ResultSetMode_", "FromDir_", "ToDir_", 
      "AllDir_", "DescOrder_", "AscOrder_", "INTEGER", "FLOAT", "Identifier_", 
      "Var", "NAME_", "STRING_", "COLON_", "DOUBLE_COLON", "COMMA_", "LPAREN", 
      "RPAREN", "LBRACKET", "RBRACKET", "OR", "AND", "EQUALS", "NOTEQUALS", 
      "LT", "LTEQ", "GT", "GTEQ", "PLUS_", "MULT", "DIV", "MOD", "REGEX", 
      "NOT", "WHITESPACE"
    }
  );
  static const int32_t serializedATNSegment[] = {
  	4,1,79,665,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,6,2,
  	7,7,7,2,8,7,8,2,9,7,9,2,10,7,10,2,11,7,11,2,12,7,12,2,13,7,13,2,14,7,
  	14,2,15,7,15,2,16,7,16,2,17,7,17,2,18,7,18,2,19,7,19,2,20,7,20,2,21,7,
  	21,2,22,7,22,2,23,7,23,2,24,7,24,2,25,7,25,2,26,7,26,2,27,7,27,2,28,7,
  	28,2,29,7,29,2,30,7,30,2,31,7,31,2,32,7,32,2,33,7,33,2,34,7,34,2,35,7,
  	35,2,36,7,36,2,37,7,37,2,38,7,38,2,39,7,39,2,40,7,40,2,41,7,41,2,42,7,
  	42,2,43,7,43,2,44,7,44,2,45,7,45,2,46,7,46,2,47,7,47,2,48,7,48,2,49,7,
  	49,2,50,7,50,2,51,7,51,2,52,7,52,2,53,7,53,2,54,7,54,2,55,7,55,2,56,7,
  	56,2,57,7,57,2,58,7,58,2,59,7,59,2,60,7,60,2,61,7,61,2,62,7,62,2,63,7,
  	63,2,64,7,64,2,65,7,65,2,66,7,66,2,67,7,67,2,68,7,68,2,69,7,69,2,70,7,
  	70,2,71,7,71,2,72,7,72,2,73,7,73,1,0,1,0,1,0,1,1,1,1,1,1,1,1,1,1,1,1,
  	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,3,1,173,8,
  	1,1,2,1,2,1,2,3,2,178,8,2,1,2,1,2,1,3,1,3,3,3,184,8,3,1,4,1,4,1,4,1,4,
  	5,4,190,8,4,10,4,12,4,193,9,4,1,4,1,4,1,5,1,5,1,5,1,5,1,5,1,6,1,6,1,6,
  	1,6,1,6,1,6,1,7,1,7,1,7,1,7,1,7,1,7,1,7,1,8,1,8,1,8,1,8,5,8,219,8,8,10,
  	8,12,8,222,9,8,1,8,1,8,1,9,1,9,1,9,1,9,3,9,230,8,9,1,9,1,9,3,9,234,8,
  	9,1,10,1,10,1,11,1,11,1,11,1,11,1,11,1,11,1,11,1,12,1,12,1,12,1,12,1,
  	12,1,12,1,12,1,13,1,13,1,13,1,13,1,13,1,13,1,13,1,13,1,13,1,13,1,13,1,
  	13,1,13,1,14,1,14,1,14,1,14,1,14,1,14,1,14,1,14,1,14,1,15,1,15,1,15,1,
  	15,1,15,1,15,1,15,3,15,281,8,15,1,15,1,15,3,15,285,8,15,1,15,1,15,1,15,
  	1,15,1,16,1,16,1,17,1,17,1,17,1,17,1,18,1,18,1,19,1,19,1,19,1,19,1,19,
  	1,19,1,19,1,19,1,19,1,20,1,20,1,21,1,21,1,21,1,21,1,21,1,22,1,22,5,22,
  	317,8,22,10,22,12,22,320,9,22,1,23,1,23,1,23,1,24,1,24,3,24,327,8,24,
  	1,24,1,24,1,24,3,24,332,8,24,1,24,1,24,1,25,1,25,1,25,3,25,339,8,25,1,
  	25,1,25,1,25,3,25,344,8,25,1,25,1,25,1,25,1,26,1,26,1,26,1,26,3,26,353,
  	8,26,1,27,1,27,1,28,1,28,1,29,1,29,1,29,3,29,362,8,29,1,30,1,30,1,31,
  	1,31,1,32,1,32,1,33,1,33,1,33,1,33,1,33,1,33,1,33,1,34,1,34,1,34,1,34,
  	5,34,381,8,34,10,34,12,34,384,9,34,1,34,1,34,1,35,1,35,1,35,1,35,1,35,
  	1,36,1,36,1,37,1,37,1,37,1,37,1,37,1,37,1,37,1,38,1,38,1,38,1,38,1,38,
  	1,38,1,38,1,38,1,38,1,39,1,39,1,39,1,39,5,39,415,8,39,10,39,12,39,418,
  	9,39,1,39,1,39,1,40,1,40,1,40,3,40,425,8,40,1,40,1,40,1,40,1,41,1,41,
  	1,41,1,41,1,41,1,42,1,42,1,42,1,42,1,42,1,42,1,42,1,43,1,43,1,43,5,43,
  	445,8,43,10,43,12,43,448,9,43,1,44,1,44,1,44,5,44,453,8,44,10,44,12,44,
  	456,9,44,1,45,1,45,1,45,5,45,461,8,45,10,45,12,45,464,9,45,1,46,1,46,
  	1,46,5,46,469,8,46,10,46,12,46,472,9,46,1,47,1,47,1,47,5,47,477,8,47,
  	10,47,12,47,480,9,47,1,48,1,48,1,48,5,48,485,8,48,10,48,12,48,488,9,48,
  	1,49,3,49,491,8,49,1,49,1,49,1,50,1,50,1,50,1,50,1,50,1,50,1,50,3,50,
  	502,8,50,1,51,1,51,1,51,3,51,507,8,51,1,51,1,51,1,51,1,52,1,52,1,53,1,
  	53,1,53,1,53,1,53,3,53,519,8,53,1,53,1,53,1,54,1,54,1,55,1,55,1,55,5,
  	55,528,8,55,10,55,12,55,531,9,55,1,56,1,56,1,56,1,56,3,56,537,8,56,1,
  	56,1,56,3,56,541,8,56,1,57,1,57,1,57,1,57,1,57,1,57,1,57,1,58,1,58,1,
  	58,1,58,5,58,554,8,58,10,58,12,58,557,9,58,1,58,1,58,1,59,1,59,1,59,1,
  	59,1,59,1,60,1,60,1,61,1,61,1,61,1,61,3,61,572,8,61,1,61,1,61,3,61,576,
  	8,61,1,61,1,61,1,62,1,62,1,62,1,62,1,62,3,62,585,8,62,1,62,1,62,1,63,
  	1,63,1,63,1,63,5,63,593,8,63,10,63,12,63,596,9,63,1,63,1,63,1,64,1,64,
  	1,64,1,64,1,65,1,65,1,65,1,65,3,65,608,8,65,1,65,1,65,1,65,3,65,613,8,
  	65,1,65,1,65,1,65,1,65,1,66,1,66,1,66,1,66,1,67,1,67,1,67,1,67,1,67,1,
  	68,1,68,1,68,1,68,1,68,1,69,1,69,1,69,1,69,1,69,1,70,1,70,1,70,1,70,1,
  	70,1,70,1,70,1,70,3,70,646,8,70,1,70,1,70,1,70,1,70,1,70,1,71,1,71,1,
  	72,1,72,1,72,5,72,658,8,72,10,72,12,72,661,9,72,1,73,1,73,1,73,0,0,74,
  	0,2,4,6,8,10,12,14,16,18,20,22,24,26,28,30,32,34,36,38,40,42,44,46,48,
  	50,52,54,56,58,60,62,64,66,68,70,72,74,76,78,80,82,84,86,88,90,92,94,
  	96,98,100,102,104,106,108,110,112,114,116,118,120,122,124,126,128,130,
  	132,134,136,138,140,142,144,146,0,12,1,0,29,35,1,0,47,49,1,0,43,44,1,
  	0,36,40,1,0,67,68,2,0,69,72,77,77,2,0,5,5,73,73,1,0,74,76,2,0,52,53,57,
  	57,1,0,41,42,1,0,50,51,1,0,45,46,653,0,148,1,0,0,0,2,172,1,0,0,0,4,174,
  	1,0,0,0,6,183,1,0,0,0,8,185,1,0,0,0,10,196,1,0,0,0,12,201,1,0,0,0,14,
  	207,1,0,0,0,16,214,1,0,0,0,18,233,1,0,0,0,20,235,1,0,0,0,22,237,1,0,0,
  	0,24,244,1,0,0,0,26,251,1,0,0,0,28,264,1,0,0,0,30,273,1,0,0,0,32,290,
  	1,0,0,0,34,292,1,0,0,0,36,296,1,0,0,0,38,298,1,0,0,0,40,307,1,0,0,0,42,
  	309,1,0,0,0,44,314,1,0,0,0,46,321,1,0,0,0,48,324,1,0,0,0,50,335,1,0,0,
  	0,52,348,1,0,0,0,54,354,1,0,0,0,56,356,1,0,0,0,58,361,1,0,0,0,60,363,
  	1,0,0,0,62,365,1,0,0,0,64,367,1,0,0,0,66,369,1,0,0,0,68,376,1,0,0,0,70,
  	387,1,0,0,0,72,392,1,0,0,0,74,394,1,0,0,0,76,401,1,0,0,0,78,410,1,0,0,
  	0,80,421,1,0,0,0,82,429,1,0,0,0,84,434,1,0,0,0,86,441,1,0,0,0,88,449,
  	1,0,0,0,90,457,1,0,0,0,92,465,1,0,0,0,94,473,1,0,0,0,96,481,1,0,0,0,98,
  	490,1,0,0,0,100,501,1,0,0,0,102,503,1,0,0,0,104,511,1,0,0,0,106,513,1,
  	0,0,0,108,522,1,0,0,0,110,524,1,0,0,0,112,540,1,0,0,0,114,542,1,0,0,0,
  	116,549,1,0,0,0,118,560,1,0,0,0,120,565,1,0,0,0,122,567,1,0,0,0,124,579,
  	1,0,0,0,126,588,1,0,0,0,128,599,1,0,0,0,130,603,1,0,0,0,132,618,1,0,0,
  	0,134,622,1,0,0,0,136,627,1,0,0,0,138,632,1,0,0,0,140,637,1,0,0,0,142,
  	652,1,0,0,0,144,654,1,0,0,0,146,662,1,0,0,0,148,149,3,2,1,0,149,150,5,
  	0,0,1,150,1,1,0,0,0,151,173,3,84,42,0,152,173,3,4,2,0,153,173,3,10,5,
  	0,154,173,3,42,21,0,155,173,3,14,7,0,156,173,3,22,11,0,157,173,3,24,12,
  	0,158,173,3,26,13,0,159,173,3,28,14,0,160,173,3,30,15,0,161,173,3,38,
  	19,0,162,173,3,66,33,0,163,173,3,76,38,0,164,173,3,74,37,0,165,173,3,
  	114,57,0,166,173,3,82,41,0,167,173,3,122,61,0,168,173,3,134,67,0,169,
  	173,3,138,69,0,170,173,3,136,68,0,171,173,3,140,70,0,172,151,1,0,0,0,
  	172,152,1,0,0,0,172,153,1,0,0,0,172,154,1,0,0,0,172,155,1,0,0,0,172,156,
  	1,0,0,0,172,157,1,0,0,0,172,158,1,0,0,0,172,159,1,0,0,0,172,160,1,0,0,
  	0,172,161,1,0,0,0,172,162,1,0,0,0,172,163,1,0,0,0,172,164,1,0,0,0,172,
  	165,1,0,0,0,172,166,1,0,0,0,172,167,1,0,0,0,172,168,1,0,0,0,172,169,1,
  	0,0,0,172,170,1,0,0,0,172,171,1,0,0,0,173,3,1,0,0,0,174,175,5,9,0,0,175,
  	177,5,61,0,0,176,178,3,6,3,0,177,176,1,0,0,0,177,178,1,0,0,0,178,179,
  	1,0,0,0,179,180,5,62,0,0,180,5,1,0,0,0,181,184,5,57,0,0,182,184,3,8,4,
  	0,183,181,1,0,0,0,183,182,1,0,0,0,184,7,1,0,0,0,185,186,5,63,0,0,186,
  	191,5,57,0,0,187,188,5,60,0,0,188,190,5,57,0,0,189,187,1,0,0,0,190,193,
  	1,0,0,0,191,189,1,0,0,0,191,192,1,0,0,0,192,194,1,0,0,0,193,191,1,0,0,
  	0,194,195,5,64,0,0,195,9,1,0,0,0,196,197,5,10,0,0,197,198,5,61,0,0,198,
  	199,3,12,6,0,199,200,5,62,0,0,200,11,1,0,0,0,201,202,5,57,0,0,202,203,
  	5,60,0,0,203,204,5,57,0,0,204,205,5,60,0,0,205,206,3,104,52,0,206,13,
  	1,0,0,0,207,208,5,12,0,0,208,209,5,61,0,0,209,210,3,16,8,0,210,211,5,
  	60,0,0,211,212,3,2,1,0,212,213,5,62,0,0,213,15,1,0,0,0,214,215,5,63,0,
  	0,215,220,3,18,9,0,216,217,5,60,0,0,217,219,3,18,9,0,218,216,1,0,0,0,
  	219,222,1,0,0,0,220,218,1,0,0,0,220,221,1,0,0,0,221,223,1,0,0,0,222,220,
  	1,0,0,0,223,224,5,64,0,0,224,17,1,0,0,0,225,234,3,106,53,0,226,229,5,
  	55,0,0,227,228,5,1,0,0,228,230,5,54,0,0,229,227,1,0,0,0,229,230,1,0,0,
  	0,230,231,1,0,0,0,231,232,5,58,0,0,232,234,3,20,10,0,233,225,1,0,0,0,
  	233,226,1,0,0,0,234,19,1,0,0,0,235,236,7,0,0,0,236,21,1,0,0,0,237,238,
  	5,13,0,0,238,239,5,61,0,0,239,240,5,52,0,0,240,241,5,60,0,0,241,242,3,
  	2,1,0,242,243,5,62,0,0,243,23,1,0,0,0,244,245,5,14,0,0,245,246,5,61,0,
  	0,246,247,3,2,1,0,247,248,5,60,0,0,248,249,3,2,1,0,249,250,5,62,0,0,250,
  	25,1,0,0,0,251,252,5,15,0,0,252,253,5,61,0,0,253,254,5,63,0,0,254,255,
  	3,102,51,0,255,256,5,60,0,0,256,257,3,102,51,0,257,258,5,64,0,0,258,259,
  	5,60,0,0,259,260,3,2,1,0,260,261,5,60,0,0,261,262,3,2,1,0,262,263,5,62,
  	0,0,263,27,1,0,0,0,264,265,5,16,0,0,265,266,5,61,0,0,266,267,3,86,43,
  	0,267,268,5,60,0,0,268,269,3,2,1,0,269,270,5,60,0,0,270,271,3,2,1,0,271,
  	272,5,62,0,0,272,29,1,0,0,0,273,274,5,18,0,0,274,275,5,61,0,0,275,276,
  	3,32,16,0,276,277,5,60,0,0,277,280,5,57,0,0,278,279,5,60,0,0,279,281,
  	3,34,17,0,280,278,1,0,0,0,280,281,1,0,0,0,281,284,1,0,0,0,282,283,5,60,
  	0,0,283,285,3,36,18,0,284,282,1,0,0,0,284,285,1,0,0,0,285,286,1,0,0,0,
  	286,287,5,60,0,0,287,288,3,2,1,0,288,289,5,62,0,0,289,31,1,0,0,0,290,
  	291,7,1,0,0,291,33,1,0,0,0,292,293,5,52,0,0,293,294,5,60,0,0,294,295,
  	5,52,0,0,295,35,1,0,0,0,296,297,5,55,0,0,297,37,1,0,0,0,298,299,5,17,
  	0,0,299,300,5,61,0,0,300,301,3,40,20,0,301,302,5,60,0,0,302,303,5,57,
  	0,0,303,304,5,60,0,0,304,305,3,2,1,0,305,306,5,62,0,0,306,39,1,0,0,0,
  	307,308,7,2,0,0,308,41,1,0,0,0,309,310,5,11,0,0,310,311,5,61,0,0,311,
  	312,3,44,22,0,312,313,5,62,0,0,313,43,1,0,0,0,314,318,3,48,24,0,315,317,
  	3,46,23,0,316,315,1,0,0,0,317,320,1,0,0,0,318,316,1,0,0,0,318,319,1,0,
  	0,0,319,45,1,0,0,0,320,318,1,0,0,0,321,322,3,50,25,0,322,323,3,48,24,
  	0,323,47,1,0,0,0,324,326,5,61,0,0,325,327,5,54,0,0,326,325,1,0,0,0,326,
  	327,1,0,0,0,327,328,1,0,0,0,328,329,5,58,0,0,329,331,5,54,0,0,330,332,
  	3,126,63,0,331,330,1,0,0,0,331,332,1,0,0,0,332,333,1,0,0,0,333,334,5,
  	62,0,0,334,49,1,0,0,0,335,336,3,58,29,0,336,338,5,63,0,0,337,339,5,54,
  	0,0,338,337,1,0,0,0,338,339,1,0,0,0,339,340,1,0,0,0,340,341,5,58,0,0,
  	341,343,5,54,0,0,342,344,3,52,26,0,343,342,1,0,0,0,343,344,1,0,0,0,344,
  	345,1,0,0,0,345,346,5,64,0,0,346,347,3,58,29,0,347,51,1,0,0,0,348,349,
  	5,74,0,0,349,350,3,54,27,0,350,352,5,2,0,0,351,353,3,56,28,0,352,351,
  	1,0,0,0,352,353,1,0,0,0,353,53,1,0,0,0,354,355,5,52,0,0,355,55,1,0,0,
  	0,356,357,5,52,0,0,357,57,1,0,0,0,358,362,3,60,30,0,359,362,3,62,31,0,
  	360,362,3,64,32,0,361,358,1,0,0,0,361,359,1,0,0,0,361,360,1,0,0,0,362,
  	59,1,0,0,0,363,364,5,3,0,0,364,61,1,0,0,0,365,366,5,4,0,0,366,63,1,0,
  	0,0,367,368,5,5,0,0,368,65,1,0,0,0,369,370,5,19,0,0,370,371,5,61,0,0,
  	371,372,3,68,34,0,372,373,5,60,0,0,373,374,3,2,1,0,374,375,5,62,0,0,375,
  	67,1,0,0,0,376,377,5,63,0,0,377,382,3,70,35,0,378,379,5,60,0,0,379,381,
  	3,70,35,0,380,378,1,0,0,0,381,384,1,0,0,0,382,380,1,0,0,0,382,383,1,0,
  	0,0,383,385,1,0,0,0,384,382,1,0,0,0,385,386,5,64,0,0,386,69,1,0,0,0,387,
  	388,3,72,36,0,388,389,5,61,0,0,389,390,3,18,9,0,390,391,5,62,0,0,391,
  	71,1,0,0,0,392,393,7,3,0,0,393,73,1,0,0,0,394,395,5,24,0,0,395,396,5,
  	61,0,0,396,397,3,2,1,0,397,398,5,60,0,0,398,399,3,2,1,0,399,400,5,62,
  	0,0,400,75,1,0,0,0,401,402,5,20,0,0,402,403,5,61,0,0,403,404,3,78,39,
  	0,404,405,5,60,0,0,405,406,3,68,34,0,406,407,5,60,0,0,407,408,3,2,1,0,
  	408,409,5,62,0,0,409,77,1,0,0,0,410,411,5,63,0,0,411,416,3,80,40,0,412,
  	413,5,60,0,0,413,415,3,80,40,0,414,412,1,0,0,0,415,418,1,0,0,0,416,414,
  	1,0,0,0,416,417,1,0,0,0,417,419,1,0,0,0,418,416,1,0,0,0,419,420,5,64,
  	0,0,420,79,1,0,0,0,421,424,5,55,0,0,422,423,5,1,0,0,423,425,5,54,0,0,
  	424,422,1,0,0,0,424,425,1,0,0,0,425,426,1,0,0,0,426,427,5,58,0,0,427,
  	428,3,20,10,0,428,81,1,0,0,0,429,430,5,22,0,0,430,431,5,61,0,0,431,432,
  	3,2,1,0,432,433,5,62,0,0,433,83,1,0,0,0,434,435,5,8,0,0,435,436,5,61,
  	0,0,436,437,3,86,43,0,437,438,5,60,0,0,438,439,3,2,1,0,439,440,5,62,0,
  	0,440,85,1,0,0,0,441,446,3,88,44,0,442,443,5,65,0,0,443,445,3,88,44,0,
  	444,442,1,0,0,0,445,448,1,0,0,0,446,444,1,0,0,0,446,447,1,0,0,0,447,87,
  	1,0,0,0,448,446,1,0,0,0,449,454,3,90,45,0,450,451,5,66,0,0,451,453,3,
  	90,45,0,452,450,1,0,0,0,453,456,1,0,0,0,454,452,1,0,0,0,454,455,1,0,0,
  	0,455,89,1,0,0,0,456,454,1,0,0,0,457,462,3,92,46,0,458,459,7,4,0,0,459,
  	461,3,92,46,0,460,458,1,0,0,0,461,464,1,0,0,0,462,460,1,0,0,0,462,463,
  	1,0,0,0,463,91,1,0,0,0,464,462,1,0,0,0,465,470,3,94,47,0,466,467,7,5,
  	0,0,467,469,3,94,47,0,468,466,1,0,0,0,469,472,1,0,0,0,470,468,1,0,0,0,
  	470,471,1,0,0,0,471,93,1,0,0,0,472,470,1,0,0,0,473,478,3,96,48,0,474,
  	475,7,6,0,0,475,477,3,96,48,0,476,474,1,0,0,0,477,480,1,0,0,0,478,476,
  	1,0,0,0,478,479,1,0,0,0,479,95,1,0,0,0,480,478,1,0,0,0,481,486,3,98,49,
  	0,482,483,7,7,0,0,483,485,3,98,49,0,484,482,1,0,0,0,485,488,1,0,0,0,486,
  	484,1,0,0,0,486,487,1,0,0,0,487,97,1,0,0,0,488,486,1,0,0,0,489,491,5,
  	78,0,0,490,489,1,0,0,0,490,491,1,0,0,0,491,492,1,0,0,0,492,493,3,100,
  	50,0,493,99,1,0,0,0,494,495,5,61,0,0,495,496,3,86,43,0,496,497,5,62,0,
  	0,497,502,1,0,0,0,498,502,3,106,53,0,499,502,3,104,52,0,500,502,3,102,
  	51,0,501,494,1,0,0,0,501,498,1,0,0,0,501,499,1,0,0,0,501,500,1,0,0,0,
  	502,101,1,0,0,0,503,506,5,55,0,0,504,505,5,1,0,0,505,507,5,54,0,0,506,
  	504,1,0,0,0,506,507,1,0,0,0,507,508,1,0,0,0,508,509,5,58,0,0,509,510,
  	3,20,10,0,510,103,1,0,0,0,511,512,7,8,0,0,512,105,1,0,0,0,513,514,3,108,
  	54,0,514,515,5,59,0,0,515,516,5,54,0,0,516,518,5,61,0,0,517,519,3,110,
  	55,0,518,517,1,0,0,0,518,519,1,0,0,0,519,520,1,0,0,0,520,521,5,62,0,0,
  	521,107,1,0,0,0,522,523,7,9,0,0,523,109,1,0,0,0,524,529,3,112,56,0,525,
  	526,5,60,0,0,526,528,3,112,56,0,527,525,1,0,0,0,528,531,1,0,0,0,529,527,
  	1,0,0,0,529,530,1,0,0,0,530,111,1,0,0,0,531,529,1,0,0,0,532,541,3,104,
  	52,0,533,536,5,55,0,0,534,535,5,1,0,0,535,537,5,54,0,0,536,534,1,0,0,
  	0,536,537,1,0,0,0,537,538,1,0,0,0,538,539,5,58,0,0,539,541,3,20,10,0,
  	540,532,1,0,0,0,540,533,1,0,0,0,541,113,1,0,0,0,542,543,5,21,0,0,543,
  	544,5,61,0,0,544,545,3,116,58,0,545,546,5,60,0,0,546,547,3,2,1,0,547,
  	548,5,62,0,0,548,115,1,0,0,0,549,550,5,63,0,0,550,555,3,118,59,0,551,
  	552,5,60,0,0,552,554,3,118,59,0,553,551,1,0,0,0,554,557,1,0,0,0,555,553,
  	1,0,0,0,555,556,1,0,0,0,556,558,1,0,0,0,557,555,1,0,0,0,558,559,5,64,
  	0,0,559,117,1,0,0,0,560,561,5,55,0,0,561,562,5,58,0,0,562,563,3,20,10,
  	0,563,564,3,120,60,0,564,119,1,0,0,0,565,566,7,10,0,0,566,121,1,0,0,0,
  	567,568,5,23,0,0,568,571,5,61,0,0,569,572,3,130,65,0,570,572,3,124,62,
  	0,571,569,1,0,0,0,571,570,1,0,0,0,572,575,1,0,0,0,573,574,5,60,0,0,574,
  	576,3,2,1,0,575,573,1,0,0,0,575,576,1,0,0,0,576,577,1,0,0,0,577,578,5,
  	62,0,0,578,123,1,0,0,0,579,580,5,61,0,0,580,581,5,54,0,0,581,582,5,58,
  	0,0,582,584,5,54,0,0,583,585,3,126,63,0,584,583,1,0,0,0,584,585,1,0,0,
  	0,585,586,1,0,0,0,586,587,5,62,0,0,587,125,1,0,0,0,588,589,5,6,0,0,589,
  	594,3,128,64,0,590,591,5,60,0,0,591,593,3,128,64,0,592,590,1,0,0,0,593,
  	596,1,0,0,0,594,592,1,0,0,0,594,595,1,0,0,0,595,597,1,0,0,0,596,594,1,
  	0,0,0,597,598,5,7,0,0,598,127,1,0,0,0,599,600,5,54,0,0,600,601,5,58,0,
  	0,601,602,3,104,52,0,602,129,1,0,0,0,603,604,3,132,66,0,604,605,5,5,0,
  	0,605,607,5,63,0,0,606,608,5,54,0,0,607,606,1,0,0,0,607,608,1,0,0,0,608,
  	609,1,0,0,0,609,610,5,58,0,0,610,612,5,54,0,0,611,613,3,126,63,0,612,
  	611,1,0,0,0,612,613,1,0,0,0,613,614,1,0,0,0,614,615,5,64,0,0,615,616,
  	5,4,0,0,616,617,3,132,66,0,617,131,1,0,0,0,618,619,5,61,0,0,619,620,5,
  	55,0,0,620,621,5,62,0,0,621,133,1,0,0,0,622,623,5,25,0,0,623,624,5,61,
  	0,0,624,625,3,2,1,0,625,626,5,62,0,0,626,135,1,0,0,0,627,628,5,26,0,0,
  	628,629,5,61,0,0,629,630,3,2,1,0,630,631,5,62,0,0,631,137,1,0,0,0,632,
  	633,5,27,0,0,633,634,5,61,0,0,634,635,3,2,1,0,635,636,5,62,0,0,636,139,
  	1,0,0,0,637,638,5,28,0,0,638,639,5,61,0,0,639,640,5,63,0,0,640,641,5,
  	54,0,0,641,642,5,60,0,0,642,645,3,142,71,0,643,644,5,60,0,0,644,646,3,
  	144,72,0,645,643,1,0,0,0,645,646,1,0,0,0,646,647,1,0,0,0,647,648,5,64,
  	0,0,648,649,5,60,0,0,649,650,3,2,1,0,650,651,5,62,0,0,651,141,1,0,0,0,
  	652,653,7,11,0,0,653,143,1,0,0,0,654,659,3,146,73,0,655,656,5,60,0,0,
  	656,658,3,146,73,0,657,655,1,0,0,0,658,661,1,0,0,0,659,657,1,0,0,0,659,
  	660,1,0,0,0,660,145,1,0,0,0,661,659,1,0,0,0,662,663,3,104,52,0,663,147,
  	1,0,0,0,41,172,177,183,191,220,229,233,280,284,318,326,331,338,343,352,
  	361,382,416,424,446,454,462,470,478,486,490,501,506,518,529,536,540,555,
  	571,575,584,594,607,612,645,659
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
    setState(148);
    query_operator();
    setState(149);
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
    setState(172);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case poseidonParser::Filter_: {
        enterOuterAlt(_localctx, 1);
        setState(151);
        filter_op();
        break;
      }

      case poseidonParser::Nodescan_: {
        enterOuterAlt(_localctx, 2);
        setState(152);
        node_scan_op();
        break;
      }

      case poseidonParser::Indexscan_: {
        enterOuterAlt(_localctx, 3);
        setState(153);
        index_scan_op();
        break;
      }

      case poseidonParser::Match_: {
        enterOuterAlt(_localctx, 4);
        setState(154);
        match_op();
        break;
      }

      case poseidonParser::Project_: {
        enterOuterAlt(_localctx, 5);
        setState(155);
        project_op();
        break;
      }

      case poseidonParser::Limit_: {
        enterOuterAlt(_localctx, 6);
        setState(156);
        limit_op();
        break;
      }

      case poseidonParser::CrossJoin_: {
        enterOuterAlt(_localctx, 7);
        setState(157);
        crossjoin_op();
        break;
      }

      case poseidonParser::HashJoin_: {
        enterOuterAlt(_localctx, 8);
        setState(158);
        hashjoin_op();
        break;
      }

      case poseidonParser::LeftOuterJoin_: {
        enterOuterAlt(_localctx, 9);
        setState(159);
        leftouterjoin_op();
        break;
      }

      case poseidonParser::ForeachRelationship_: {
        enterOuterAlt(_localctx, 10);
        setState(160);
        foreach_relationship_op();
        break;
      }

      case poseidonParser::Expand_: {
        enterOuterAlt(_localctx, 11);
        setState(161);
        expand_op();
        break;
      }

      case poseidonParser::Aggregate_: {
        enterOuterAlt(_localctx, 12);
        setState(162);
        aggregate_op();
        break;
      }

      case poseidonParser::GroupBy_: {
        enterOuterAlt(_localctx, 13);
        setState(163);
        group_by_op();
        break;
      }

      case poseidonParser::Union_: {
        enterOuterAlt(_localctx, 14);
        setState(164);
        union_op();
        break;
      }

      case poseidonParser::Sort_: {
        enterOuterAlt(_localctx, 15);
        setState(165);
        sort_op();
        break;
      }

      case poseidonParser::Distinct_: {
        enterOuterAlt(_localctx, 16);
        setState(166);
        distinct_op();
        break;
      }

      case poseidonParser::Create_: {
        enterOuterAlt(_localctx, 17);
        setState(167);
        create_op();
        break;
      }

      case poseidonParser::RemoveNode_: {
        enterOuterAlt(_localctx, 18);
        setState(168);
        remove_node_op();
        break;
      }

      case poseidonParser::DetachNode_: {
        enterOuterAlt(_localctx, 19);
        setState(169);
        detach_node_op();
        break;
      }

      case poseidonParser::RemoveRelationship_: {
        enterOuterAlt(_localctx, 20);
        setState(170);
        remove_relationship_op();
        break;
      }

      case poseidonParser::Algorithm_: {
        enterOuterAlt(_localctx, 21);
        setState(171);
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
    setState(174);
    match(poseidonParser::Nodescan_);
    setState(175);
    match(poseidonParser::LPAREN);
    setState(177);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == poseidonParser::STRING_

    || _la == poseidonParser::LBRACKET) {
      setState(176);
      scan_param();
    }
    setState(179);
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
    setState(183);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case poseidonParser::STRING_: {
        enterOuterAlt(_localctx, 1);
        setState(181);
        match(poseidonParser::STRING_);
        break;
      }

      case poseidonParser::LBRACKET: {
        enterOuterAlt(_localctx, 2);
        setState(182);
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
    setState(185);
    match(poseidonParser::LBRACKET);
    setState(186);
    match(poseidonParser::STRING_);
    setState(191);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == poseidonParser::COMMA_) {
      setState(187);
      match(poseidonParser::COMMA_);
      setState(188);
      match(poseidonParser::STRING_);
      setState(193);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(194);
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
    setState(196);
    match(poseidonParser::Indexscan_);
    setState(197);
    match(poseidonParser::LPAREN);
    setState(198);
    index_scan_param();
    setState(199);
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
    setState(201);
    match(poseidonParser::STRING_);
    setState(202);
    match(poseidonParser::COMMA_);
    setState(203);
    match(poseidonParser::STRING_);
    setState(204);
    match(poseidonParser::COMMA_);
    setState(205);
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
    setState(207);
    match(poseidonParser::Project_);
    setState(208);
    match(poseidonParser::LPAREN);
    setState(209);
    proj_list();
    setState(210);
    match(poseidonParser::COMMA_);
    setState(211);
    query_operator();
    setState(212);
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
    setState(214);
    match(poseidonParser::LBRACKET);
    setState(215);
    proj_expr();
    setState(220);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == poseidonParser::COMMA_) {
      setState(216);
      match(poseidonParser::COMMA_);
      setState(217);
      proj_expr();
      setState(222);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(223);
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
    setState(233);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case poseidonParser::UDF_:
      case poseidonParser::BUILTIN_: {
        enterOuterAlt(_localctx, 1);
        setState(225);
        function_call();
        break;
      }

      case poseidonParser::Var: {
        enterOuterAlt(_localctx, 2);
        setState(226);
        match(poseidonParser::Var);
        setState(229);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (_la == poseidonParser::T__0) {
          setState(227);
          match(poseidonParser::T__0);
          setState(228);
          match(poseidonParser::Identifier_);
        }
        setState(231);
        match(poseidonParser::COLON_);
        setState(232);
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
    setState(235);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 68182605824) != 0))) {
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
    setState(237);
    match(poseidonParser::Limit_);
    setState(238);
    match(poseidonParser::LPAREN);
    setState(239);
    match(poseidonParser::INTEGER);
    setState(240);
    match(poseidonParser::COMMA_);
    setState(241);
    query_operator();
    setState(242);
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
    setState(244);
    match(poseidonParser::CrossJoin_);
    setState(245);
    match(poseidonParser::LPAREN);
    setState(246);
    query_operator();
    setState(247);
    match(poseidonParser::COMMA_);
    setState(248);
    query_operator();
    setState(249);
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

tree::TerminalNode* poseidonParser::Hashjoin_opContext::LBRACKET() {
  return getToken(poseidonParser::LBRACKET, 0);
}

std::vector<poseidonParser::VariableContext *> poseidonParser::Hashjoin_opContext::variable() {
  return getRuleContexts<poseidonParser::VariableContext>();
}

poseidonParser::VariableContext* poseidonParser::Hashjoin_opContext::variable(size_t i) {
  return getRuleContext<poseidonParser::VariableContext>(i);
}

std::vector<tree::TerminalNode *> poseidonParser::Hashjoin_opContext::COMMA_() {
  return getTokens(poseidonParser::COMMA_);
}

tree::TerminalNode* poseidonParser::Hashjoin_opContext::COMMA_(size_t i) {
  return getToken(poseidonParser::COMMA_, i);
}

tree::TerminalNode* poseidonParser::Hashjoin_opContext::RBRACKET() {
  return getToken(poseidonParser::RBRACKET, 0);
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
    setState(251);
    match(poseidonParser::HashJoin_);
    setState(252);
    match(poseidonParser::LPAREN);
    setState(253);
    match(poseidonParser::LBRACKET);
    setState(254);
    variable();
    setState(255);
    match(poseidonParser::COMMA_);
    setState(256);
    variable();
    setState(257);
    match(poseidonParser::RBRACKET);
    setState(258);
    match(poseidonParser::COMMA_);
    setState(259);
    query_operator();
    setState(260);
    match(poseidonParser::COMMA_);
    setState(261);
    query_operator();
    setState(262);
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
    setState(264);
    match(poseidonParser::LeftOuterJoin_);
    setState(265);
    match(poseidonParser::LPAREN);
    setState(266);
    logical_expr();
    setState(267);
    match(poseidonParser::COMMA_);
    setState(268);
    query_operator();
    setState(269);
    match(poseidonParser::COMMA_);
    setState(270);
    query_operator();
    setState(271);
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
    setState(273);
    match(poseidonParser::ForeachRelationship_);
    setState(274);
    match(poseidonParser::LPAREN);
    setState(275);
    rship_dir();
    setState(276);
    match(poseidonParser::COMMA_);
    setState(277);
    match(poseidonParser::STRING_);
    setState(280);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 7, _ctx)) {
    case 1: {
      setState(278);
      match(poseidonParser::COMMA_);
      setState(279);
      rship_cardinality();
      break;
    }

    default:
      break;
    }
    setState(284);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 8, _ctx)) {
    case 1: {
      setState(282);
      match(poseidonParser::COMMA_);
      setState(283);
      rship_source_var();
      break;
    }

    default:
      break;
    }
    setState(286);
    match(poseidonParser::COMMA_);
    setState(287);
    query_operator();
    setState(288);
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
    setState(290);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 985162418487296) != 0))) {
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
    setState(292);
    match(poseidonParser::INTEGER);
    setState(293);
    match(poseidonParser::COMMA_);
    setState(294);
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
    setState(296);
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
    setState(298);
    match(poseidonParser::Expand_);
    setState(299);
    match(poseidonParser::LPAREN);
    setState(300);
    expand_dir();
    setState(301);
    match(poseidonParser::COMMA_);
    setState(302);
    match(poseidonParser::STRING_);
    setState(303);
    match(poseidonParser::COMMA_);
    setState(304);
    query_operator();
    setState(305);
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
    setState(307);
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
    setState(309);
    match(poseidonParser::Match_);
    setState(310);
    match(poseidonParser::LPAREN);
    setState(311);
    path_pattern();
    setState(312);
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
    setState(314);
    node_pattern();
    setState(318);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 56) != 0)) {
      setState(315);
      path_component();
      setState(320);
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
    setState(321);
    rship_pattern();
    setState(322);
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
    setState(324);
    match(poseidonParser::LPAREN);
    setState(326);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == poseidonParser::Identifier_) {
      setState(325);
      match(poseidonParser::Identifier_);
    }
    setState(328);
    match(poseidonParser::COLON_);
    setState(329);
    match(poseidonParser::Identifier_);
    setState(331);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == poseidonParser::T__5) {
      setState(330);
      property_list();
    }
    setState(333);
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
    setState(335);
    dir_spec();
    setState(336);
    match(poseidonParser::LBRACKET);
    setState(338);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == poseidonParser::Identifier_) {
      setState(337);
      match(poseidonParser::Identifier_);
    }
    setState(340);
    match(poseidonParser::COLON_);
    setState(341);
    match(poseidonParser::Identifier_);
    setState(343);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == poseidonParser::MULT) {
      setState(342);
      cardinality_spec();
    }
    setState(345);
    match(poseidonParser::RBRACKET);
    setState(346);
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
    setState(348);
    match(poseidonParser::MULT);
    setState(349);
    min_cardinality();
    setState(350);
    match(poseidonParser::T__1);
    setState(352);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == poseidonParser::INTEGER) {
      setState(351);
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
    setState(354);
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
    setState(356);
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
    setState(361);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case poseidonParser::T__2: {
        enterOuterAlt(_localctx, 1);
        setState(358);
        left_dir();
        break;
      }

      case poseidonParser::T__3: {
        enterOuterAlt(_localctx, 2);
        setState(359);
        right_dir();
        break;
      }

      case poseidonParser::T__4: {
        enterOuterAlt(_localctx, 3);
        setState(360);
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
    setState(363);
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
    setState(365);
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
    setState(367);
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
    setState(369);
    match(poseidonParser::Aggregate_);
    setState(370);
    match(poseidonParser::LPAREN);
    setState(371);
    aggregate_list();
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
    setState(376);
    match(poseidonParser::LBRACKET);
    setState(377);
    aggr_expr();
    setState(382);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == poseidonParser::COMMA_) {
      setState(378);
      match(poseidonParser::COMMA_);
      setState(379);
      aggr_expr();
      setState(384);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(385);
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
    setState(387);
    aggr_func();
    setState(388);
    match(poseidonParser::LPAREN);
    setState(389);
    proj_expr();
    setState(390);
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
    setState(392);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 2130303778816) != 0))) {
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
    setState(394);
    match(poseidonParser::Union_);
    setState(395);
    match(poseidonParser::LPAREN);
    setState(396);
    query_operator();
    setState(397);
    match(poseidonParser::COMMA_);
    setState(398);
    query_operator();
    setState(399);
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
    setState(401);
    match(poseidonParser::GroupBy_);
    setState(402);
    match(poseidonParser::LPAREN);
    setState(403);
    grouping_list();
    setState(404);
    match(poseidonParser::COMMA_);
    setState(405);
    aggregate_list();
    setState(406);
    match(poseidonParser::COMMA_);
    setState(407);
    query_operator();
    setState(408);
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
    setState(410);
    match(poseidonParser::LBRACKET);
    setState(411);
    grouping_expr();
    setState(416);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == poseidonParser::COMMA_) {
      setState(412);
      match(poseidonParser::COMMA_);
      setState(413);
      grouping_expr();
      setState(418);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(419);
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
    setState(421);
    match(poseidonParser::Var);
    setState(424);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == poseidonParser::T__0) {
      setState(422);
      match(poseidonParser::T__0);
      setState(423);
      match(poseidonParser::Identifier_);
    }
    setState(426);
    match(poseidonParser::COLON_);
    setState(427);
    type_spec();
   
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
  enterRule(_localctx, 82, poseidonParser::RuleDistinct_op);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(429);
    match(poseidonParser::Distinct_);
    setState(430);
    match(poseidonParser::LPAREN);
    setState(431);
    query_operator();
    setState(432);
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
  enterRule(_localctx, 84, poseidonParser::RuleFilter_op);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(434);
    match(poseidonParser::Filter_);
    setState(435);
    match(poseidonParser::LPAREN);
    setState(436);
    logical_expr();
    setState(437);
    match(poseidonParser::COMMA_);
    setState(438);
    query_operator();
    setState(439);
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
  enterRule(_localctx, 86, poseidonParser::RuleLogical_expr);
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
    setState(441);
    boolean_expr();
    setState(446);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == poseidonParser::OR) {
      setState(442);
      match(poseidonParser::OR);
      setState(443);
      boolean_expr();
      setState(448);
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
  enterRule(_localctx, 88, poseidonParser::RuleBoolean_expr);
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
    setState(449);
    equality_expr();
    setState(454);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == poseidonParser::AND) {
      setState(450);
      match(poseidonParser::AND);
      setState(451);
      equality_expr();
      setState(456);
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
  enterRule(_localctx, 90, poseidonParser::RuleEquality_expr);
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
    setState(457);
    relational_expr();
    setState(462);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == poseidonParser::EQUALS

    || _la == poseidonParser::NOTEQUALS) {
      setState(458);
      _la = _input->LA(1);
      if (!(_la == poseidonParser::EQUALS

      || _la == poseidonParser::NOTEQUALS)) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(459);
      relational_expr();
      setState(464);
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
  enterRule(_localctx, 92, poseidonParser::RuleRelational_expr);
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
    setState(465);
    additive_expr();
    setState(470);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (((((_la - 69) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 69)) & 271) != 0)) {
      setState(466);
      _la = _input->LA(1);
      if (!(((((_la - 69) & ~ 0x3fULL) == 0) &&
        ((1ULL << (_la - 69)) & 271) != 0))) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(467);
      additive_expr();
      setState(472);
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
  enterRule(_localctx, 94, poseidonParser::RuleAdditive_expr);
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
    setState(473);
    multiplicative_expr();
    setState(478);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == poseidonParser::T__4 || _la == poseidonParser::PLUS_) {
      setState(474);
      _la = _input->LA(1);
      if (!(_la == poseidonParser::T__4 || _la == poseidonParser::PLUS_)) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(475);
      multiplicative_expr();
      setState(480);
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
  enterRule(_localctx, 96, poseidonParser::RuleMultiplicative_expr);
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
    setState(481);
    unary_expr();
    setState(486);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (((((_la - 74) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 74)) & 7) != 0)) {
      setState(482);
      _la = _input->LA(1);
      if (!(((((_la - 74) & ~ 0x3fULL) == 0) &&
        ((1ULL << (_la - 74)) & 7) != 0))) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(483);
      unary_expr();
      setState(488);
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
  enterRule(_localctx, 98, poseidonParser::RuleUnary_expr);
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
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == poseidonParser::NOT) {
      setState(489);
      match(poseidonParser::NOT);
    }
    setState(492);
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
  enterRule(_localctx, 100, poseidonParser::RulePrimary_expr);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(501);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case poseidonParser::LPAREN: {
        enterOuterAlt(_localctx, 1);
        setState(494);
        match(poseidonParser::LPAREN);
        setState(495);
        logical_expr();
        setState(496);
        match(poseidonParser::RPAREN);
        break;
      }

      case poseidonParser::UDF_:
      case poseidonParser::BUILTIN_: {
        enterOuterAlt(_localctx, 2);
        setState(498);
        function_call();
        break;
      }

      case poseidonParser::INTEGER:
      case poseidonParser::FLOAT:
      case poseidonParser::STRING_: {
        enterOuterAlt(_localctx, 3);
        setState(499);
        value();
        break;
      }

      case poseidonParser::Var: {
        enterOuterAlt(_localctx, 4);
        setState(500);
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
  enterRule(_localctx, 102, poseidonParser::RuleVariable);
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
    setState(503);
    match(poseidonParser::Var);
    setState(506);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == poseidonParser::T__0) {
      setState(504);
      match(poseidonParser::T__0);
      setState(505);
      match(poseidonParser::Identifier_);
    }
    setState(508);
    match(poseidonParser::COLON_);
    setState(509);
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
  enterRule(_localctx, 104, poseidonParser::RuleValue);
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
    setState(511);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 157625986957967360) != 0))) {
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
  enterRule(_localctx, 106, poseidonParser::RuleFunction_call);
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
    prefix();
    setState(514);
    match(poseidonParser::DOUBLE_COLON);
    setState(515);
    match(poseidonParser::Identifier_);
    setState(516);
    match(poseidonParser::LPAREN);
    setState(518);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 193654783976931328) != 0)) {
      setState(517);
      param_list();
    }
    setState(520);
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
  enterRule(_localctx, 108, poseidonParser::RulePrefix);
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
    setState(522);
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
  enterRule(_localctx, 110, poseidonParser::RuleParam_list);
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
    setState(524);
    param();
    setState(529);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == poseidonParser::COMMA_) {
      setState(525);
      match(poseidonParser::COMMA_);
      setState(526);
      param();
      setState(531);
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
  enterRule(_localctx, 112, poseidonParser::RuleParam);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(540);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case poseidonParser::INTEGER:
      case poseidonParser::FLOAT:
      case poseidonParser::STRING_: {
        enterOuterAlt(_localctx, 1);
        setState(532);
        value();
        break;
      }

      case poseidonParser::Var: {
        enterOuterAlt(_localctx, 2);
        setState(533);
        match(poseidonParser::Var);
        setState(536);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (_la == poseidonParser::T__0) {
          setState(534);
          match(poseidonParser::T__0);
          setState(535);
          match(poseidonParser::Identifier_);
        }
        setState(538);
        match(poseidonParser::COLON_);
        setState(539);
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
  enterRule(_localctx, 114, poseidonParser::RuleSort_op);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(542);
    match(poseidonParser::Sort_);
    setState(543);
    match(poseidonParser::LPAREN);
    setState(544);
    sort_list();
    setState(545);
    match(poseidonParser::COMMA_);
    setState(546);
    query_operator();
    setState(547);
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
  enterRule(_localctx, 116, poseidonParser::RuleSort_list);
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
    setState(549);
    match(poseidonParser::LBRACKET);
    setState(550);
    sort_expr();
    setState(555);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == poseidonParser::COMMA_) {
      setState(551);
      match(poseidonParser::COMMA_);
      setState(552);
      sort_expr();
      setState(557);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(558);
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
  enterRule(_localctx, 118, poseidonParser::RuleSort_expr);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(560);
    match(poseidonParser::Var);
    setState(561);
    match(poseidonParser::COLON_);
    setState(562);
    type_spec();
    setState(563);
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
  enterRule(_localctx, 120, poseidonParser::RuleSort_spec);
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
    setState(565);
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
  enterRule(_localctx, 122, poseidonParser::RuleCreate_op);
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
    match(poseidonParser::Create_);
    setState(568);
    match(poseidonParser::LPAREN);
    setState(571);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 33, _ctx)) {
    case 1: {
      setState(569);
      create_rship();
      break;
    }

    case 2: {
      setState(570);
      create_node();
      break;
    }

    default:
      break;
    }
    setState(575);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == poseidonParser::COMMA_) {
      setState(573);
      match(poseidonParser::COMMA_);
      setState(574);
      query_operator();
    }
    setState(577);
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
  enterRule(_localctx, 124, poseidonParser::RuleCreate_node);
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
    setState(579);
    match(poseidonParser::LPAREN);
    setState(580);
    match(poseidonParser::Identifier_);
    setState(581);
    match(poseidonParser::COLON_);
    setState(582);
    match(poseidonParser::Identifier_);
    setState(584);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == poseidonParser::T__5) {
      setState(583);
      property_list();
    }
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
  enterRule(_localctx, 126, poseidonParser::RuleProperty_list);
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
    match(poseidonParser::T__5);
    setState(589);
    property();
    setState(594);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == poseidonParser::COMMA_) {
      setState(590);
      match(poseidonParser::COMMA_);
      setState(591);
      property();
      setState(596);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(597);
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
  enterRule(_localctx, 128, poseidonParser::RuleProperty);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(599);
    match(poseidonParser::Identifier_);
    setState(600);
    match(poseidonParser::COLON_);
    setState(601);
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
  enterRule(_localctx, 130, poseidonParser::RuleCreate_rship);
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
    setState(603);
    node_var();
    setState(604);
    match(poseidonParser::T__4);
    setState(605);
    match(poseidonParser::LBRACKET);
    setState(607);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == poseidonParser::Identifier_) {
      setState(606);
      match(poseidonParser::Identifier_);
    }
    setState(609);
    match(poseidonParser::COLON_);
    setState(610);
    match(poseidonParser::Identifier_);
    setState(612);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == poseidonParser::T__5) {
      setState(611);
      property_list();
    }
    setState(614);
    match(poseidonParser::RBRACKET);
    setState(615);
    match(poseidonParser::T__3);
    setState(616);
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
  enterRule(_localctx, 132, poseidonParser::RuleNode_var);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(618);
    match(poseidonParser::LPAREN);
    setState(619);
    match(poseidonParser::Var);
    setState(620);
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
  enterRule(_localctx, 134, poseidonParser::RuleRemove_node_op);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(622);
    match(poseidonParser::RemoveNode_);
    setState(623);
    match(poseidonParser::LPAREN);
    setState(624);
    query_operator();
    setState(625);
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
  enterRule(_localctx, 136, poseidonParser::RuleRemove_relationship_op);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(627);
    match(poseidonParser::RemoveRelationship_);
    setState(628);
    match(poseidonParser::LPAREN);
    setState(629);
    query_operator();
    setState(630);
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
  enterRule(_localctx, 138, poseidonParser::RuleDetach_node_op);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(632);
    match(poseidonParser::DetachNode_);
    setState(633);
    match(poseidonParser::LPAREN);
    setState(634);
    query_operator();
    setState(635);
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
  enterRule(_localctx, 140, poseidonParser::RuleAlgorithm_op);
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
    setState(637);
    match(poseidonParser::Algorithm_);
    setState(638);
    match(poseidonParser::LPAREN);
    setState(639);
    match(poseidonParser::LBRACKET);
    setState(640);
    match(poseidonParser::Identifier_);
    setState(641);
    match(poseidonParser::COMMA_);
    setState(642);
    call_mode();
    setState(645);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == poseidonParser::COMMA_) {
      setState(643);
      match(poseidonParser::COMMA_);
      setState(644);
      algo_param_list();
    }
    setState(647);
    match(poseidonParser::RBRACKET);
    setState(648);
    match(poseidonParser::COMMA_);
    setState(649);
    query_operator();
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
  enterRule(_localctx, 142, poseidonParser::RuleCall_mode);
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
  enterRule(_localctx, 144, poseidonParser::RuleAlgo_param_list);
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
    setState(654);
    algo_param();
    setState(659);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == poseidonParser::COMMA_) {
      setState(655);
      match(poseidonParser::COMMA_);
      setState(656);
      algo_param();
      setState(661);
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
  enterRule(_localctx, 146, poseidonParser::RuleAlgo_param);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(662);
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
