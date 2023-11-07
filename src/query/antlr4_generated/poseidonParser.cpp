
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
      "group_by_op", "grouping_list", "grouping_expr", "filter_op", "logical_expr", 
      "boolean_expr", "equality_expr", "relational_expr", "additive_expr", 
      "multiplicative_expr", "unary_expr", "primary_expr", "variable", "value", 
      "function_call", "udf_prefix", "param_list", "param", "sort_op", "sort_list", 
      "sort_expr", "sort_spec", "create_op", "create_node", "property_list", 
      "property", "create_rship", "node_var", "remove_node_op", "remove_relationship_op", 
      "detach_node_op"
    },
    std::vector<std::string>{
      "", "'.'", "'..'", "'<-'", "'->'", "'-'", "'{'", "'}'", "'Filter'", 
      "'NodeScan'", "'IndexScan'", "'Match'", "'Project'", "'Limit'", "'CrossJoin'", 
      "'HashJoin'", "'LeftOuterJoin'", "'Expand'", "'ForeachRelationship'", 
      "'Aggregate'", "'GroupBy'", "'Sort'", "'Create'", "'Union'", "'RemoveNode'", 
      "'RemoveRelationship'", "'DetachNode'", "'int'", "'uint64'", "'double'", 
      "'string'", "'datetime'", "'qresult'", "'count'", "'sum'", "'avg'", 
      "'min'", "'max'", "'udf'", "'IN'", "'OUT'", "'FROM'", "'TO'", "'ALL'", 
      "'DESC'", "'ASC'", "", "", "", "", "", "", "':'", "'::'", "','", "'('", 
      "')'", "'['", "']'", "", "", "", "", "'<'", "'<='", "'>'", "'>='", 
      "'+'", "'*'", "'/'", "'%'"
    },
    std::vector<std::string>{
      "", "", "", "", "", "", "", "", "Filter_", "Nodescan_", "Indexscan_", 
      "Match_", "Project_", "Limit_", "CrossJoin_", "HashJoin_", "LeftOuterJoin_", 
      "Expand_", "ForeachRelationship_", "Aggregate_", "GroupBy_", "Sort_", 
      "Create_", "Union_", "RemoveNode_", "RemoveRelationship_", "DetachNode_", 
      "IntType_", "Uint64Type_", "DoubleType_", "StringType_", "DateType_", 
      "ResultType_", "Count_", "Sum_", "Avg_", "Min_", "Max_", "UDF_", "InExpandDir_", 
      "OutExpandDir_", "FromDir_", "ToDir_", "AllDir_", "DescOrder_", "AscOrder_", 
      "INTEGER", "FLOAT", "Identifier_", "Var", "NAME_", "STRING_", "COLON_", 
      "DOUBLE_COLON", "COMMA_", "LPAREN", "RPAREN", "LBRACKET", "RBRACKET", 
      "OR", "AND", "EQUALS", "NOTEQUALS", "LT", "LTEQ", "GT", "GTEQ", "PLUS_", 
      "MULT", "DIV", "MOD", "NOT", "WHITESPACE"
    }
  );
  static const int32_t serializedATNSegment[] = {
  	4,1,72,613,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,6,2,
  	7,7,7,2,8,7,8,2,9,7,9,2,10,7,10,2,11,7,11,2,12,7,12,2,13,7,13,2,14,7,
  	14,2,15,7,15,2,16,7,16,2,17,7,17,2,18,7,18,2,19,7,19,2,20,7,20,2,21,7,
  	21,2,22,7,22,2,23,7,23,2,24,7,24,2,25,7,25,2,26,7,26,2,27,7,27,2,28,7,
  	28,2,29,7,29,2,30,7,30,2,31,7,31,2,32,7,32,2,33,7,33,2,34,7,34,2,35,7,
  	35,2,36,7,36,2,37,7,37,2,38,7,38,2,39,7,39,2,40,7,40,2,41,7,41,2,42,7,
  	42,2,43,7,43,2,44,7,44,2,45,7,45,2,46,7,46,2,47,7,47,2,48,7,48,2,49,7,
  	49,2,50,7,50,2,51,7,51,2,52,7,52,2,53,7,53,2,54,7,54,2,55,7,55,2,56,7,
  	56,2,57,7,57,2,58,7,58,2,59,7,59,2,60,7,60,2,61,7,61,2,62,7,62,2,63,7,
  	63,2,64,7,64,2,65,7,65,2,66,7,66,2,67,7,67,2,68,7,68,1,0,1,0,1,0,1,1,
  	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  	1,3,1,161,8,1,1,2,1,2,1,2,3,2,166,8,2,1,2,1,2,1,3,1,3,3,3,172,8,3,1,4,
  	1,4,1,4,1,4,5,4,178,8,4,10,4,12,4,181,9,4,1,4,1,4,1,5,1,5,1,5,1,5,1,5,
  	1,6,1,6,1,6,1,6,1,6,1,6,1,7,1,7,1,7,1,7,1,7,1,7,1,7,1,8,1,8,1,8,1,8,5,
  	8,207,8,8,10,8,12,8,210,9,8,1,8,1,8,1,9,1,9,1,9,1,9,3,9,218,8,9,1,9,1,
  	9,3,9,222,8,9,1,10,1,10,1,11,1,11,1,11,1,11,1,11,1,11,1,11,1,12,1,12,
  	1,12,1,12,1,12,1,12,1,12,1,13,1,13,1,13,1,13,1,13,1,13,1,13,1,13,1,13,
  	1,14,1,14,1,14,1,14,1,14,1,14,1,14,1,14,1,14,1,15,1,15,1,15,1,15,1,15,
  	1,15,1,15,3,15,265,8,15,1,15,1,15,3,15,269,8,15,1,15,1,15,1,15,1,15,1,
  	16,1,16,1,17,1,17,1,17,1,17,1,18,1,18,1,19,1,19,1,19,1,19,1,19,1,19,1,
  	19,1,19,1,19,1,20,1,20,1,21,1,21,1,21,1,21,1,21,1,22,1,22,5,22,301,8,
  	22,10,22,12,22,304,9,22,1,23,1,23,1,23,1,24,1,24,3,24,311,8,24,1,24,1,
  	24,1,24,3,24,316,8,24,1,24,1,24,1,25,1,25,1,25,3,25,323,8,25,1,25,1,25,
  	1,25,3,25,328,8,25,1,25,1,25,1,25,1,26,1,26,1,26,1,26,3,26,337,8,26,1,
  	27,1,27,1,28,1,28,1,29,1,29,1,29,3,29,346,8,29,1,30,1,30,1,31,1,31,1,
  	32,1,32,1,33,1,33,1,33,1,33,1,33,1,33,1,33,1,34,1,34,1,34,1,34,5,34,365,
  	8,34,10,34,12,34,368,9,34,1,34,1,34,1,35,1,35,1,35,1,35,1,35,1,36,1,36,
  	1,37,1,37,1,37,1,37,1,37,1,37,1,37,1,38,1,38,1,38,1,38,1,38,1,38,1,38,
  	1,38,1,38,1,39,1,39,1,39,1,39,5,39,399,8,39,10,39,12,39,402,9,39,1,39,
  	1,39,1,40,1,40,1,40,3,40,409,8,40,1,40,1,40,1,40,1,41,1,41,1,41,1,41,
  	1,41,1,41,1,41,1,42,1,42,1,42,5,42,424,8,42,10,42,12,42,427,9,42,1,43,
  	1,43,1,43,5,43,432,8,43,10,43,12,43,435,9,43,1,44,1,44,1,44,5,44,440,
  	8,44,10,44,12,44,443,9,44,1,45,1,45,1,45,5,45,448,8,45,10,45,12,45,451,
  	9,45,1,46,1,46,1,46,5,46,456,8,46,10,46,12,46,459,9,46,1,47,1,47,1,47,
  	5,47,464,8,47,10,47,12,47,467,9,47,1,48,3,48,470,8,48,1,48,1,48,1,49,
  	1,49,1,49,1,49,1,49,1,49,1,49,3,49,481,8,49,1,50,1,50,1,50,1,50,1,51,
  	1,51,1,52,1,52,1,52,1,52,3,52,493,8,52,1,52,1,52,1,53,1,53,1,53,1,54,
  	1,54,1,54,5,54,503,8,54,10,54,12,54,506,9,54,1,55,1,55,1,55,1,55,3,55,
  	512,8,55,1,55,1,55,3,55,516,8,55,1,56,1,56,1,56,1,56,1,56,1,56,1,56,1,
  	57,1,57,1,57,1,57,5,57,529,8,57,10,57,12,57,532,9,57,1,57,1,57,1,58,1,
  	58,1,58,1,58,1,58,1,59,1,59,1,60,1,60,1,60,1,60,3,60,547,8,60,1,60,1,
  	60,3,60,551,8,60,1,60,1,60,1,61,1,61,1,61,1,61,1,61,3,61,560,8,61,1,61,
  	1,61,1,62,1,62,1,62,1,62,5,62,568,8,62,10,62,12,62,571,9,62,1,62,1,62,
  	1,63,1,63,1,63,1,63,1,64,1,64,1,64,1,64,3,64,583,8,64,1,64,1,64,1,64,
  	3,64,588,8,64,1,64,1,64,1,64,1,64,1,65,1,65,1,65,1,65,1,66,1,66,1,66,
  	1,66,1,66,1,67,1,67,1,67,1,67,1,67,1,68,1,68,1,68,1,68,1,68,1,68,0,0,
  	69,0,2,4,6,8,10,12,14,16,18,20,22,24,26,28,30,32,34,36,38,40,42,44,46,
  	48,50,52,54,56,58,60,62,64,66,68,70,72,74,76,78,80,82,84,86,88,90,92,
  	94,96,98,100,102,104,106,108,110,112,114,116,118,120,122,124,126,128,
  	130,132,134,136,0,10,1,0,27,32,1,0,41,43,1,0,39,40,1,0,33,37,1,0,61,62,
  	1,0,63,66,2,0,5,5,67,67,1,0,68,70,2,0,46,47,51,51,1,0,44,45,601,0,138,
  	1,0,0,0,2,160,1,0,0,0,4,162,1,0,0,0,6,171,1,0,0,0,8,173,1,0,0,0,10,184,
  	1,0,0,0,12,189,1,0,0,0,14,195,1,0,0,0,16,202,1,0,0,0,18,221,1,0,0,0,20,
  	223,1,0,0,0,22,225,1,0,0,0,24,232,1,0,0,0,26,239,1,0,0,0,28,248,1,0,0,
  	0,30,257,1,0,0,0,32,274,1,0,0,0,34,276,1,0,0,0,36,280,1,0,0,0,38,282,
  	1,0,0,0,40,291,1,0,0,0,42,293,1,0,0,0,44,298,1,0,0,0,46,305,1,0,0,0,48,
  	308,1,0,0,0,50,319,1,0,0,0,52,332,1,0,0,0,54,338,1,0,0,0,56,340,1,0,0,
  	0,58,345,1,0,0,0,60,347,1,0,0,0,62,349,1,0,0,0,64,351,1,0,0,0,66,353,
  	1,0,0,0,68,360,1,0,0,0,70,371,1,0,0,0,72,376,1,0,0,0,74,378,1,0,0,0,76,
  	385,1,0,0,0,78,394,1,0,0,0,80,405,1,0,0,0,82,413,1,0,0,0,84,420,1,0,0,
  	0,86,428,1,0,0,0,88,436,1,0,0,0,90,444,1,0,0,0,92,452,1,0,0,0,94,460,
  	1,0,0,0,96,469,1,0,0,0,98,480,1,0,0,0,100,482,1,0,0,0,102,486,1,0,0,0,
  	104,488,1,0,0,0,106,496,1,0,0,0,108,499,1,0,0,0,110,515,1,0,0,0,112,517,
  	1,0,0,0,114,524,1,0,0,0,116,535,1,0,0,0,118,540,1,0,0,0,120,542,1,0,0,
  	0,122,554,1,0,0,0,124,563,1,0,0,0,126,574,1,0,0,0,128,578,1,0,0,0,130,
  	593,1,0,0,0,132,597,1,0,0,0,134,602,1,0,0,0,136,607,1,0,0,0,138,139,3,
  	2,1,0,139,140,5,0,0,1,140,1,1,0,0,0,141,161,3,82,41,0,142,161,3,4,2,0,
  	143,161,3,10,5,0,144,161,3,42,21,0,145,161,3,14,7,0,146,161,3,22,11,0,
  	147,161,3,24,12,0,148,161,3,26,13,0,149,161,3,28,14,0,150,161,3,30,15,
  	0,151,161,3,38,19,0,152,161,3,66,33,0,153,161,3,76,38,0,154,161,3,74,
  	37,0,155,161,3,112,56,0,156,161,3,120,60,0,157,161,3,132,66,0,158,161,
  	3,136,68,0,159,161,3,134,67,0,160,141,1,0,0,0,160,142,1,0,0,0,160,143,
  	1,0,0,0,160,144,1,0,0,0,160,145,1,0,0,0,160,146,1,0,0,0,160,147,1,0,0,
  	0,160,148,1,0,0,0,160,149,1,0,0,0,160,150,1,0,0,0,160,151,1,0,0,0,160,
  	152,1,0,0,0,160,153,1,0,0,0,160,154,1,0,0,0,160,155,1,0,0,0,160,156,1,
  	0,0,0,160,157,1,0,0,0,160,158,1,0,0,0,160,159,1,0,0,0,161,3,1,0,0,0,162,
  	163,5,9,0,0,163,165,5,55,0,0,164,166,3,6,3,0,165,164,1,0,0,0,165,166,
  	1,0,0,0,166,167,1,0,0,0,167,168,5,56,0,0,168,5,1,0,0,0,169,172,5,51,0,
  	0,170,172,3,8,4,0,171,169,1,0,0,0,171,170,1,0,0,0,172,7,1,0,0,0,173,174,
  	5,57,0,0,174,179,5,51,0,0,175,176,5,54,0,0,176,178,5,51,0,0,177,175,1,
  	0,0,0,178,181,1,0,0,0,179,177,1,0,0,0,179,180,1,0,0,0,180,182,1,0,0,0,
  	181,179,1,0,0,0,182,183,5,58,0,0,183,9,1,0,0,0,184,185,5,10,0,0,185,186,
  	5,55,0,0,186,187,3,12,6,0,187,188,5,56,0,0,188,11,1,0,0,0,189,190,5,51,
  	0,0,190,191,5,54,0,0,191,192,5,51,0,0,192,193,5,54,0,0,193,194,3,102,
  	51,0,194,13,1,0,0,0,195,196,5,12,0,0,196,197,5,55,0,0,197,198,3,16,8,
  	0,198,199,5,54,0,0,199,200,3,2,1,0,200,201,5,56,0,0,201,15,1,0,0,0,202,
  	203,5,57,0,0,203,208,3,18,9,0,204,205,5,54,0,0,205,207,3,18,9,0,206,204,
  	1,0,0,0,207,210,1,0,0,0,208,206,1,0,0,0,208,209,1,0,0,0,209,211,1,0,0,
  	0,210,208,1,0,0,0,211,212,5,58,0,0,212,17,1,0,0,0,213,222,3,104,52,0,
  	214,217,5,49,0,0,215,216,5,1,0,0,216,218,5,48,0,0,217,215,1,0,0,0,217,
  	218,1,0,0,0,218,219,1,0,0,0,219,220,5,52,0,0,220,222,3,20,10,0,221,213,
  	1,0,0,0,221,214,1,0,0,0,222,19,1,0,0,0,223,224,7,0,0,0,224,21,1,0,0,0,
  	225,226,5,13,0,0,226,227,5,55,0,0,227,228,5,46,0,0,228,229,5,54,0,0,229,
  	230,3,2,1,0,230,231,5,56,0,0,231,23,1,0,0,0,232,233,5,14,0,0,233,234,
  	5,55,0,0,234,235,3,2,1,0,235,236,5,54,0,0,236,237,3,2,1,0,237,238,5,56,
  	0,0,238,25,1,0,0,0,239,240,5,15,0,0,240,241,5,55,0,0,241,242,3,84,42,
  	0,242,243,5,54,0,0,243,244,3,2,1,0,244,245,5,54,0,0,245,246,3,2,1,0,246,
  	247,5,56,0,0,247,27,1,0,0,0,248,249,5,16,0,0,249,250,5,55,0,0,250,251,
  	3,84,42,0,251,252,5,54,0,0,252,253,3,2,1,0,253,254,5,54,0,0,254,255,3,
  	2,1,0,255,256,5,56,0,0,256,29,1,0,0,0,257,258,5,18,0,0,258,259,5,55,0,
  	0,259,260,3,32,16,0,260,261,5,54,0,0,261,264,5,51,0,0,262,263,5,54,0,
  	0,263,265,3,34,17,0,264,262,1,0,0,0,264,265,1,0,0,0,265,268,1,0,0,0,266,
  	267,5,54,0,0,267,269,3,36,18,0,268,266,1,0,0,0,268,269,1,0,0,0,269,270,
  	1,0,0,0,270,271,5,54,0,0,271,272,3,2,1,0,272,273,5,56,0,0,273,31,1,0,
  	0,0,274,275,7,1,0,0,275,33,1,0,0,0,276,277,5,46,0,0,277,278,5,54,0,0,
  	278,279,5,46,0,0,279,35,1,0,0,0,280,281,5,49,0,0,281,37,1,0,0,0,282,283,
  	5,17,0,0,283,284,5,55,0,0,284,285,3,40,20,0,285,286,5,54,0,0,286,287,
  	5,51,0,0,287,288,5,54,0,0,288,289,3,2,1,0,289,290,5,56,0,0,290,39,1,0,
  	0,0,291,292,7,2,0,0,292,41,1,0,0,0,293,294,5,11,0,0,294,295,5,55,0,0,
  	295,296,3,44,22,0,296,297,5,56,0,0,297,43,1,0,0,0,298,302,3,48,24,0,299,
  	301,3,46,23,0,300,299,1,0,0,0,301,304,1,0,0,0,302,300,1,0,0,0,302,303,
  	1,0,0,0,303,45,1,0,0,0,304,302,1,0,0,0,305,306,3,50,25,0,306,307,3,48,
  	24,0,307,47,1,0,0,0,308,310,5,55,0,0,309,311,5,48,0,0,310,309,1,0,0,0,
  	310,311,1,0,0,0,311,312,1,0,0,0,312,313,5,52,0,0,313,315,5,48,0,0,314,
  	316,3,124,62,0,315,314,1,0,0,0,315,316,1,0,0,0,316,317,1,0,0,0,317,318,
  	5,56,0,0,318,49,1,0,0,0,319,320,3,58,29,0,320,322,5,57,0,0,321,323,5,
  	48,0,0,322,321,1,0,0,0,322,323,1,0,0,0,323,324,1,0,0,0,324,325,5,52,0,
  	0,325,327,5,48,0,0,326,328,3,52,26,0,327,326,1,0,0,0,327,328,1,0,0,0,
  	328,329,1,0,0,0,329,330,5,58,0,0,330,331,3,58,29,0,331,51,1,0,0,0,332,
  	333,5,68,0,0,333,334,3,54,27,0,334,336,5,2,0,0,335,337,3,56,28,0,336,
  	335,1,0,0,0,336,337,1,0,0,0,337,53,1,0,0,0,338,339,5,46,0,0,339,55,1,
  	0,0,0,340,341,5,46,0,0,341,57,1,0,0,0,342,346,3,60,30,0,343,346,3,62,
  	31,0,344,346,3,64,32,0,345,342,1,0,0,0,345,343,1,0,0,0,345,344,1,0,0,
  	0,346,59,1,0,0,0,347,348,5,3,0,0,348,61,1,0,0,0,349,350,5,4,0,0,350,63,
  	1,0,0,0,351,352,5,5,0,0,352,65,1,0,0,0,353,354,5,19,0,0,354,355,5,55,
  	0,0,355,356,3,68,34,0,356,357,5,54,0,0,357,358,3,2,1,0,358,359,5,56,0,
  	0,359,67,1,0,0,0,360,361,5,57,0,0,361,366,3,70,35,0,362,363,5,54,0,0,
  	363,365,3,70,35,0,364,362,1,0,0,0,365,368,1,0,0,0,366,364,1,0,0,0,366,
  	367,1,0,0,0,367,369,1,0,0,0,368,366,1,0,0,0,369,370,5,58,0,0,370,69,1,
  	0,0,0,371,372,3,72,36,0,372,373,5,55,0,0,373,374,3,18,9,0,374,375,5,56,
  	0,0,375,71,1,0,0,0,376,377,7,3,0,0,377,73,1,0,0,0,378,379,5,23,0,0,379,
  	380,5,55,0,0,380,381,3,2,1,0,381,382,5,54,0,0,382,383,3,2,1,0,383,384,
  	5,56,0,0,384,75,1,0,0,0,385,386,5,20,0,0,386,387,5,55,0,0,387,388,3,78,
  	39,0,388,389,5,54,0,0,389,390,3,68,34,0,390,391,5,54,0,0,391,392,3,2,
  	1,0,392,393,5,56,0,0,393,77,1,0,0,0,394,395,5,57,0,0,395,400,3,80,40,
  	0,396,397,5,54,0,0,397,399,3,80,40,0,398,396,1,0,0,0,399,402,1,0,0,0,
  	400,398,1,0,0,0,400,401,1,0,0,0,401,403,1,0,0,0,402,400,1,0,0,0,403,404,
  	5,58,0,0,404,79,1,0,0,0,405,408,5,49,0,0,406,407,5,1,0,0,407,409,5,48,
  	0,0,408,406,1,0,0,0,408,409,1,0,0,0,409,410,1,0,0,0,410,411,5,52,0,0,
  	411,412,3,20,10,0,412,81,1,0,0,0,413,414,5,8,0,0,414,415,5,55,0,0,415,
  	416,3,84,42,0,416,417,5,54,0,0,417,418,3,2,1,0,418,419,5,56,0,0,419,83,
  	1,0,0,0,420,425,3,86,43,0,421,422,5,59,0,0,422,424,3,86,43,0,423,421,
  	1,0,0,0,424,427,1,0,0,0,425,423,1,0,0,0,425,426,1,0,0,0,426,85,1,0,0,
  	0,427,425,1,0,0,0,428,433,3,88,44,0,429,430,5,60,0,0,430,432,3,88,44,
  	0,431,429,1,0,0,0,432,435,1,0,0,0,433,431,1,0,0,0,433,434,1,0,0,0,434,
  	87,1,0,0,0,435,433,1,0,0,0,436,441,3,90,45,0,437,438,7,4,0,0,438,440,
  	3,90,45,0,439,437,1,0,0,0,440,443,1,0,0,0,441,439,1,0,0,0,441,442,1,0,
  	0,0,442,89,1,0,0,0,443,441,1,0,0,0,444,449,3,92,46,0,445,446,7,5,0,0,
  	446,448,3,92,46,0,447,445,1,0,0,0,448,451,1,0,0,0,449,447,1,0,0,0,449,
  	450,1,0,0,0,450,91,1,0,0,0,451,449,1,0,0,0,452,457,3,94,47,0,453,454,
  	7,6,0,0,454,456,3,94,47,0,455,453,1,0,0,0,456,459,1,0,0,0,457,455,1,0,
  	0,0,457,458,1,0,0,0,458,93,1,0,0,0,459,457,1,0,0,0,460,465,3,96,48,0,
  	461,462,7,7,0,0,462,464,3,96,48,0,463,461,1,0,0,0,464,467,1,0,0,0,465,
  	463,1,0,0,0,465,466,1,0,0,0,466,95,1,0,0,0,467,465,1,0,0,0,468,470,5,
  	71,0,0,469,468,1,0,0,0,469,470,1,0,0,0,470,471,1,0,0,0,471,472,3,98,49,
  	0,472,97,1,0,0,0,473,474,5,55,0,0,474,475,3,84,42,0,475,476,5,56,0,0,
  	476,481,1,0,0,0,477,481,3,104,52,0,478,481,3,102,51,0,479,481,3,100,50,
  	0,480,473,1,0,0,0,480,477,1,0,0,0,480,478,1,0,0,0,480,479,1,0,0,0,481,
  	99,1,0,0,0,482,483,5,49,0,0,483,484,5,1,0,0,484,485,5,48,0,0,485,101,
  	1,0,0,0,486,487,7,8,0,0,487,103,1,0,0,0,488,489,3,106,53,0,489,490,5,
  	48,0,0,490,492,5,55,0,0,491,493,3,108,54,0,492,491,1,0,0,0,492,493,1,
  	0,0,0,493,494,1,0,0,0,494,495,5,56,0,0,495,105,1,0,0,0,496,497,5,38,0,
  	0,497,498,5,53,0,0,498,107,1,0,0,0,499,504,3,110,55,0,500,501,5,54,0,
  	0,501,503,3,110,55,0,502,500,1,0,0,0,503,506,1,0,0,0,504,502,1,0,0,0,
  	504,505,1,0,0,0,505,109,1,0,0,0,506,504,1,0,0,0,507,516,3,102,51,0,508,
  	511,5,49,0,0,509,510,5,1,0,0,510,512,5,48,0,0,511,509,1,0,0,0,511,512,
  	1,0,0,0,512,513,1,0,0,0,513,514,5,52,0,0,514,516,3,20,10,0,515,507,1,
  	0,0,0,515,508,1,0,0,0,516,111,1,0,0,0,517,518,5,21,0,0,518,519,5,55,0,
  	0,519,520,3,114,57,0,520,521,5,54,0,0,521,522,3,2,1,0,522,523,5,56,0,
  	0,523,113,1,0,0,0,524,525,5,57,0,0,525,530,3,116,58,0,526,527,5,54,0,
  	0,527,529,3,116,58,0,528,526,1,0,0,0,529,532,1,0,0,0,530,528,1,0,0,0,
  	530,531,1,0,0,0,531,533,1,0,0,0,532,530,1,0,0,0,533,534,5,58,0,0,534,
  	115,1,0,0,0,535,536,5,49,0,0,536,537,5,52,0,0,537,538,3,20,10,0,538,539,
  	3,118,59,0,539,117,1,0,0,0,540,541,7,9,0,0,541,119,1,0,0,0,542,543,5,
  	22,0,0,543,546,5,55,0,0,544,547,3,128,64,0,545,547,3,122,61,0,546,544,
  	1,0,0,0,546,545,1,0,0,0,547,550,1,0,0,0,548,549,5,54,0,0,549,551,3,2,
  	1,0,550,548,1,0,0,0,550,551,1,0,0,0,551,552,1,0,0,0,552,553,5,56,0,0,
  	553,121,1,0,0,0,554,555,5,55,0,0,555,556,5,48,0,0,556,557,5,52,0,0,557,
  	559,5,48,0,0,558,560,3,124,62,0,559,558,1,0,0,0,559,560,1,0,0,0,560,561,
  	1,0,0,0,561,562,5,56,0,0,562,123,1,0,0,0,563,564,5,6,0,0,564,569,3,126,
  	63,0,565,566,5,54,0,0,566,568,3,126,63,0,567,565,1,0,0,0,568,571,1,0,
  	0,0,569,567,1,0,0,0,569,570,1,0,0,0,570,572,1,0,0,0,571,569,1,0,0,0,572,
  	573,5,7,0,0,573,125,1,0,0,0,574,575,5,48,0,0,575,576,5,52,0,0,576,577,
  	3,102,51,0,577,127,1,0,0,0,578,579,3,130,65,0,579,580,5,5,0,0,580,582,
  	5,57,0,0,581,583,5,48,0,0,582,581,1,0,0,0,582,583,1,0,0,0,583,584,1,0,
  	0,0,584,585,5,52,0,0,585,587,5,48,0,0,586,588,3,124,62,0,587,586,1,0,
  	0,0,587,588,1,0,0,0,588,589,1,0,0,0,589,590,5,58,0,0,590,591,5,4,0,0,
  	591,592,3,130,65,0,592,129,1,0,0,0,593,594,5,55,0,0,594,595,5,49,0,0,
  	595,596,5,56,0,0,596,131,1,0,0,0,597,598,5,24,0,0,598,599,5,55,0,0,599,
  	600,3,2,1,0,600,601,5,56,0,0,601,133,1,0,0,0,602,603,5,25,0,0,603,604,
  	5,55,0,0,604,605,3,2,1,0,605,606,5,56,0,0,606,135,1,0,0,0,607,608,5,26,
  	0,0,608,609,5,55,0,0,609,610,3,2,1,0,610,611,5,56,0,0,611,137,1,0,0,0,
  	38,160,165,171,179,208,217,221,264,268,302,310,315,322,327,336,345,366,
  	400,408,425,433,441,449,457,465,469,480,492,504,511,515,530,546,550,559,
  	569,582,587
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
    setState(138);
    query_operator();
    setState(139);
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

poseidonParser::Remove_node_opContext* poseidonParser::Query_operatorContext::remove_node_op() {
  return getRuleContext<poseidonParser::Remove_node_opContext>(0);
}

poseidonParser::Detach_node_opContext* poseidonParser::Query_operatorContext::detach_node_op() {
  return getRuleContext<poseidonParser::Detach_node_opContext>(0);
}

poseidonParser::Remove_relationship_opContext* poseidonParser::Query_operatorContext::remove_relationship_op() {
  return getRuleContext<poseidonParser::Remove_relationship_opContext>(0);
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
    setState(160);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case poseidonParser::Filter_: {
        enterOuterAlt(_localctx, 1);
        setState(141);
        filter_op();
        break;
      }

      case poseidonParser::Nodescan_: {
        enterOuterAlt(_localctx, 2);
        setState(142);
        node_scan_op();
        break;
      }

      case poseidonParser::Indexscan_: {
        enterOuterAlt(_localctx, 3);
        setState(143);
        index_scan_op();
        break;
      }

      case poseidonParser::Match_: {
        enterOuterAlt(_localctx, 4);
        setState(144);
        match_op();
        break;
      }

      case poseidonParser::Project_: {
        enterOuterAlt(_localctx, 5);
        setState(145);
        project_op();
        break;
      }

      case poseidonParser::Limit_: {
        enterOuterAlt(_localctx, 6);
        setState(146);
        limit_op();
        break;
      }

      case poseidonParser::CrossJoin_: {
        enterOuterAlt(_localctx, 7);
        setState(147);
        crossjoin_op();
        break;
      }

      case poseidonParser::HashJoin_: {
        enterOuterAlt(_localctx, 8);
        setState(148);
        hashjoin_op();
        break;
      }

      case poseidonParser::LeftOuterJoin_: {
        enterOuterAlt(_localctx, 9);
        setState(149);
        leftouterjoin_op();
        break;
      }

      case poseidonParser::ForeachRelationship_: {
        enterOuterAlt(_localctx, 10);
        setState(150);
        foreach_relationship_op();
        break;
      }

      case poseidonParser::Expand_: {
        enterOuterAlt(_localctx, 11);
        setState(151);
        expand_op();
        break;
      }

      case poseidonParser::Aggregate_: {
        enterOuterAlt(_localctx, 12);
        setState(152);
        aggregate_op();
        break;
      }

      case poseidonParser::GroupBy_: {
        enterOuterAlt(_localctx, 13);
        setState(153);
        group_by_op();
        break;
      }

      case poseidonParser::Union_: {
        enterOuterAlt(_localctx, 14);
        setState(154);
        union_op();
        break;
      }

      case poseidonParser::Sort_: {
        enterOuterAlt(_localctx, 15);
        setState(155);
        sort_op();
        break;
      }

      case poseidonParser::Create_: {
        enterOuterAlt(_localctx, 16);
        setState(156);
        create_op();
        break;
      }

      case poseidonParser::RemoveNode_: {
        enterOuterAlt(_localctx, 17);
        setState(157);
        remove_node_op();
        break;
      }

      case poseidonParser::DetachNode_: {
        enterOuterAlt(_localctx, 18);
        setState(158);
        detach_node_op();
        break;
      }

      case poseidonParser::RemoveRelationship_: {
        enterOuterAlt(_localctx, 19);
        setState(159);
        remove_relationship_op();
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
    setState(162);
    match(poseidonParser::Nodescan_);
    setState(163);
    match(poseidonParser::LPAREN);
    setState(165);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == poseidonParser::STRING_

    || _la == poseidonParser::LBRACKET) {
      setState(164);
      scan_param();
    }
    setState(167);
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
    setState(171);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case poseidonParser::STRING_: {
        enterOuterAlt(_localctx, 1);
        setState(169);
        match(poseidonParser::STRING_);
        break;
      }

      case poseidonParser::LBRACKET: {
        enterOuterAlt(_localctx, 2);
        setState(170);
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
    setState(173);
    match(poseidonParser::LBRACKET);
    setState(174);
    match(poseidonParser::STRING_);
    setState(179);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == poseidonParser::COMMA_) {
      setState(175);
      match(poseidonParser::COMMA_);
      setState(176);
      match(poseidonParser::STRING_);
      setState(181);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(182);
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
    setState(184);
    match(poseidonParser::Indexscan_);
    setState(185);
    match(poseidonParser::LPAREN);
    setState(186);
    index_scan_param();
    setState(187);
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
    setState(189);
    match(poseidonParser::STRING_);
    setState(190);
    match(poseidonParser::COMMA_);
    setState(191);
    match(poseidonParser::STRING_);
    setState(192);
    match(poseidonParser::COMMA_);
    setState(193);
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
    setState(195);
    match(poseidonParser::Project_);
    setState(196);
    match(poseidonParser::LPAREN);
    setState(197);
    proj_list();
    setState(198);
    match(poseidonParser::COMMA_);
    setState(199);
    query_operator();
    setState(200);
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
    setState(202);
    match(poseidonParser::LBRACKET);
    setState(203);
    proj_expr();
    setState(208);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == poseidonParser::COMMA_) {
      setState(204);
      match(poseidonParser::COMMA_);
      setState(205);
      proj_expr();
      setState(210);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(211);
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
    setState(221);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case poseidonParser::UDF_: {
        enterOuterAlt(_localctx, 1);
        setState(213);
        function_call();
        break;
      }

      case poseidonParser::Var: {
        enterOuterAlt(_localctx, 2);
        setState(214);
        match(poseidonParser::Var);
        setState(217);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (_la == poseidonParser::T__0) {
          setState(215);
          match(poseidonParser::T__0);
          setState(216);
          match(poseidonParser::Identifier_);
        }
        setState(219);
        match(poseidonParser::COLON_);
        setState(220);
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
    setState(223);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 8455716864) != 0))) {
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
    setState(225);
    match(poseidonParser::Limit_);
    setState(226);
    match(poseidonParser::LPAREN);
    setState(227);
    match(poseidonParser::INTEGER);
    setState(228);
    match(poseidonParser::COMMA_);
    setState(229);
    query_operator();
    setState(230);
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
    setState(232);
    match(poseidonParser::CrossJoin_);
    setState(233);
    match(poseidonParser::LPAREN);
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
    setState(239);
    match(poseidonParser::HashJoin_);
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
    setState(248);
    match(poseidonParser::LeftOuterJoin_);
    setState(249);
    match(poseidonParser::LPAREN);
    setState(250);
    logical_expr();
    setState(251);
    match(poseidonParser::COMMA_);
    setState(252);
    query_operator();
    setState(253);
    match(poseidonParser::COMMA_);
    setState(254);
    query_operator();
    setState(255);
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
    setState(257);
    match(poseidonParser::ForeachRelationship_);
    setState(258);
    match(poseidonParser::LPAREN);
    setState(259);
    rship_dir();
    setState(260);
    match(poseidonParser::COMMA_);
    setState(261);
    match(poseidonParser::STRING_);
    setState(264);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 7, _ctx)) {
    case 1: {
      setState(262);
      match(poseidonParser::COMMA_);
      setState(263);
      rship_cardinality();
      break;
    }

    default:
      break;
    }
    setState(268);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 8, _ctx)) {
    case 1: {
      setState(266);
      match(poseidonParser::COMMA_);
      setState(267);
      rship_source_var();
      break;
    }

    default:
      break;
    }
    setState(270);
    match(poseidonParser::COMMA_);
    setState(271);
    query_operator();
    setState(272);
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
    setState(274);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 15393162788864) != 0))) {
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
    setState(276);
    match(poseidonParser::INTEGER);
    setState(277);
    match(poseidonParser::COMMA_);
    setState(278);
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
    setState(280);
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
    setState(282);
    match(poseidonParser::Expand_);
    setState(283);
    match(poseidonParser::LPAREN);
    setState(284);
    expand_dir();
    setState(285);
    match(poseidonParser::COMMA_);
    setState(286);
    match(poseidonParser::STRING_);
    setState(287);
    match(poseidonParser::COMMA_);
    setState(288);
    query_operator();
    setState(289);
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
    setState(291);
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
    setState(293);
    match(poseidonParser::Match_);
    setState(294);
    match(poseidonParser::LPAREN);
    setState(295);
    path_pattern();
    setState(296);
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
    setState(298);
    node_pattern();
    setState(302);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 56) != 0)) {
      setState(299);
      path_component();
      setState(304);
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
    setState(305);
    rship_pattern();
    setState(306);
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
    setState(308);
    match(poseidonParser::LPAREN);
    setState(310);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == poseidonParser::Identifier_) {
      setState(309);
      match(poseidonParser::Identifier_);
    }
    setState(312);
    match(poseidonParser::COLON_);
    setState(313);
    match(poseidonParser::Identifier_);
    setState(315);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == poseidonParser::T__5) {
      setState(314);
      property_list();
    }
    setState(317);
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
    setState(319);
    dir_spec();
    setState(320);
    match(poseidonParser::LBRACKET);
    setState(322);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == poseidonParser::Identifier_) {
      setState(321);
      match(poseidonParser::Identifier_);
    }
    setState(324);
    match(poseidonParser::COLON_);
    setState(325);
    match(poseidonParser::Identifier_);
    setState(327);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == poseidonParser::MULT) {
      setState(326);
      cardinality_spec();
    }
    setState(329);
    match(poseidonParser::RBRACKET);
    setState(330);
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
    setState(332);
    match(poseidonParser::MULT);
    setState(333);
    min_cardinality();
    setState(334);
    match(poseidonParser::T__1);
    setState(336);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == poseidonParser::INTEGER) {
      setState(335);
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
    setState(338);
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
    setState(340);
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
    setState(345);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case poseidonParser::T__2: {
        enterOuterAlt(_localctx, 1);
        setState(342);
        left_dir();
        break;
      }

      case poseidonParser::T__3: {
        enterOuterAlt(_localctx, 2);
        setState(343);
        right_dir();
        break;
      }

      case poseidonParser::T__4: {
        enterOuterAlt(_localctx, 3);
        setState(344);
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
    setState(347);
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
    setState(349);
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
    setState(351);
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
    setState(353);
    match(poseidonParser::Aggregate_);
    setState(354);
    match(poseidonParser::LPAREN);
    setState(355);
    aggregate_list();
    setState(356);
    match(poseidonParser::COMMA_);
    setState(357);
    query_operator();
    setState(358);
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
    setState(360);
    match(poseidonParser::LBRACKET);
    setState(361);
    aggr_expr();
    setState(366);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == poseidonParser::COMMA_) {
      setState(362);
      match(poseidonParser::COMMA_);
      setState(363);
      aggr_expr();
      setState(368);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(369);
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
    setState(371);
    aggr_func();
    setState(372);
    match(poseidonParser::LPAREN);
    setState(373);
    proj_expr();
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
    setState(376);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 266287972352) != 0))) {
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
    setState(378);
    match(poseidonParser::Union_);
    setState(379);
    match(poseidonParser::LPAREN);
    setState(380);
    query_operator();
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
    setState(385);
    match(poseidonParser::GroupBy_);
    setState(386);
    match(poseidonParser::LPAREN);
    setState(387);
    grouping_list();
    setState(388);
    match(poseidonParser::COMMA_);
    setState(389);
    aggregate_list();
    setState(390);
    match(poseidonParser::COMMA_);
    setState(391);
    query_operator();
    setState(392);
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
    setState(394);
    match(poseidonParser::LBRACKET);
    setState(395);
    grouping_expr();
    setState(400);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == poseidonParser::COMMA_) {
      setState(396);
      match(poseidonParser::COMMA_);
      setState(397);
      grouping_expr();
      setState(402);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(403);
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
    setState(405);
    match(poseidonParser::Var);
    setState(408);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == poseidonParser::T__0) {
      setState(406);
      match(poseidonParser::T__0);
      setState(407);
      match(poseidonParser::Identifier_);
    }
    setState(410);
    match(poseidonParser::COLON_);
    setState(411);
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
    setState(413);
    match(poseidonParser::Filter_);
    setState(414);
    match(poseidonParser::LPAREN);
    setState(415);
    logical_expr();
    setState(416);
    match(poseidonParser::COMMA_);
    setState(417);
    query_operator();
    setState(418);
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
    setState(420);
    boolean_expr();
    setState(425);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == poseidonParser::OR) {
      setState(421);
      match(poseidonParser::OR);
      setState(422);
      boolean_expr();
      setState(427);
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
    setState(428);
    equality_expr();
    setState(433);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == poseidonParser::AND) {
      setState(429);
      match(poseidonParser::AND);
      setState(430);
      equality_expr();
      setState(435);
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
    setState(436);
    relational_expr();
    setState(441);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == poseidonParser::EQUALS

    || _la == poseidonParser::NOTEQUALS) {
      setState(437);
      _la = _input->LA(1);
      if (!(_la == poseidonParser::EQUALS

      || _la == poseidonParser::NOTEQUALS)) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(438);
      relational_expr();
      setState(443);
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
    setState(444);
    additive_expr();
    setState(449);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (((((_la - 63) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 63)) & 15) != 0)) {
      setState(445);
      _la = _input->LA(1);
      if (!(((((_la - 63) & ~ 0x3fULL) == 0) &&
        ((1ULL << (_la - 63)) & 15) != 0))) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(446);
      additive_expr();
      setState(451);
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
    setState(452);
    multiplicative_expr();
    setState(457);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == poseidonParser::T__4

    || _la == poseidonParser::PLUS_) {
      setState(453);
      _la = _input->LA(1);
      if (!(_la == poseidonParser::T__4

      || _la == poseidonParser::PLUS_)) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(454);
      multiplicative_expr();
      setState(459);
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
    setState(460);
    unary_expr();
    setState(465);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (((((_la - 68) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 68)) & 7) != 0)) {
      setState(461);
      _la = _input->LA(1);
      if (!(((((_la - 68) & ~ 0x3fULL) == 0) &&
        ((1ULL << (_la - 68)) & 7) != 0))) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(462);
      unary_expr();
      setState(467);
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
    setState(469);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == poseidonParser::NOT) {
      setState(468);
      match(poseidonParser::NOT);
    }
    setState(471);
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
    setState(480);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case poseidonParser::LPAREN: {
        enterOuterAlt(_localctx, 1);
        setState(473);
        match(poseidonParser::LPAREN);
        setState(474);
        logical_expr();
        setState(475);
        match(poseidonParser::RPAREN);
        break;
      }

      case poseidonParser::UDF_: {
        enterOuterAlt(_localctx, 2);
        setState(477);
        function_call();
        break;
      }

      case poseidonParser::INTEGER:
      case poseidonParser::FLOAT:
      case poseidonParser::STRING_: {
        enterOuterAlt(_localctx, 3);
        setState(478);
        value();
        break;
      }

      case poseidonParser::Var: {
        enterOuterAlt(_localctx, 4);
        setState(479);
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
    setState(482);
    match(poseidonParser::Var);
    setState(483);
    match(poseidonParser::T__0);
    setState(484);
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
    setState(486);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 2462906046218240) != 0))) {
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
    setState(488);
    udf_prefix();
    setState(489);
    match(poseidonParser::Identifier_);
    setState(490);
    match(poseidonParser::LPAREN);
    setState(492);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 3025855999639552) != 0)) {
      setState(491);
      param_list();
    }
    setState(494);
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
    setState(496);
    match(poseidonParser::UDF_);
    setState(497);
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
    setState(499);
    param();
    setState(504);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == poseidonParser::COMMA_) {
      setState(500);
      match(poseidonParser::COMMA_);
      setState(501);
      param();
      setState(506);
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
    setState(515);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case poseidonParser::INTEGER:
      case poseidonParser::FLOAT:
      case poseidonParser::STRING_: {
        enterOuterAlt(_localctx, 1);
        setState(507);
        value();
        break;
      }

      case poseidonParser::Var: {
        enterOuterAlt(_localctx, 2);
        setState(508);
        match(poseidonParser::Var);
        setState(511);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (_la == poseidonParser::T__0) {
          setState(509);
          match(poseidonParser::T__0);
          setState(510);
          match(poseidonParser::Identifier_);
        }
        setState(513);
        match(poseidonParser::COLON_);
        setState(514);
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
    setState(517);
    match(poseidonParser::Sort_);
    setState(518);
    match(poseidonParser::LPAREN);
    setState(519);
    sort_list();
    setState(520);
    match(poseidonParser::COMMA_);
    setState(521);
    query_operator();
    setState(522);
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
    setState(524);
    match(poseidonParser::LBRACKET);
    setState(525);
    sort_expr();
    setState(530);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == poseidonParser::COMMA_) {
      setState(526);
      match(poseidonParser::COMMA_);
      setState(527);
      sort_expr();
      setState(532);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(533);
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
    setState(535);
    match(poseidonParser::Var);
    setState(536);
    match(poseidonParser::COLON_);
    setState(537);
    type_spec();
    setState(538);
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
    setState(540);
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
    setState(542);
    match(poseidonParser::Create_);
    setState(543);
    match(poseidonParser::LPAREN);
    setState(546);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 32, _ctx)) {
    case 1: {
      setState(544);
      create_rship();
      break;
    }

    case 2: {
      setState(545);
      create_node();
      break;
    }

    default:
      break;
    }
    setState(550);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == poseidonParser::COMMA_) {
      setState(548);
      match(poseidonParser::COMMA_);
      setState(549);
      query_operator();
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
    setState(554);
    match(poseidonParser::LPAREN);
    setState(555);
    match(poseidonParser::Identifier_);
    setState(556);
    match(poseidonParser::COLON_);
    setState(557);
    match(poseidonParser::Identifier_);
    setState(559);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == poseidonParser::T__5) {
      setState(558);
      property_list();
    }
    setState(561);
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
    setState(563);
    match(poseidonParser::T__5);
    setState(564);
    property();
    setState(569);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == poseidonParser::COMMA_) {
      setState(565);
      match(poseidonParser::COMMA_);
      setState(566);
      property();
      setState(571);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(572);
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
    setState(574);
    match(poseidonParser::Identifier_);
    setState(575);
    match(poseidonParser::COLON_);
    setState(576);
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
    setState(578);
    node_var();
    setState(579);
    match(poseidonParser::T__4);
    setState(580);
    match(poseidonParser::LBRACKET);
    setState(582);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == poseidonParser::Identifier_) {
      setState(581);
      match(poseidonParser::Identifier_);
    }
    setState(584);
    match(poseidonParser::COLON_);
    setState(585);
    match(poseidonParser::Identifier_);
    setState(587);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == poseidonParser::T__5) {
      setState(586);
      property_list();
    }
    setState(589);
    match(poseidonParser::RBRACKET);
    setState(590);
    match(poseidonParser::T__3);
    setState(591);
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
    setState(593);
    match(poseidonParser::LPAREN);
    setState(594);
    match(poseidonParser::Var);
    setState(595);
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
  enterRule(_localctx, 132, poseidonParser::RuleRemove_node_op);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(597);
    match(poseidonParser::RemoveNode_);
    setState(598);
    match(poseidonParser::LPAREN);
    setState(599);
    query_operator();
    setState(600);
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
  enterRule(_localctx, 134, poseidonParser::RuleRemove_relationship_op);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(602);
    match(poseidonParser::RemoveRelationship_);
    setState(603);
    match(poseidonParser::LPAREN);
    setState(604);
    query_operator();
    setState(605);
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
  enterRule(_localctx, 136, poseidonParser::RuleDetach_node_op);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(607);
    match(poseidonParser::DetachNode_);
    setState(608);
    match(poseidonParser::LPAREN);
    setState(609);
    query_operator();
    setState(610);
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
