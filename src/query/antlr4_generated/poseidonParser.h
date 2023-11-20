
// Generated from src/query/parser/poseidon.g4 by ANTLR 4.13.1

#pragma once


#include "antlr4-runtime.h"




class  poseidonParser : public antlr4::Parser {
public:
  enum {
    T__0 = 1, T__1 = 2, T__2 = 3, T__3 = 4, T__4 = 5, T__5 = 6, T__6 = 7, 
    Filter_ = 8, Nodescan_ = 9, Indexscan_ = 10, Match_ = 11, Project_ = 12, 
    Limit_ = 13, CrossJoin_ = 14, HashJoin_ = 15, LeftOuterJoin_ = 16, Expand_ = 17, 
    ForeachRelationship_ = 18, Aggregate_ = 19, GroupBy_ = 20, Sort_ = 21, 
    Distinct_ = 22, Create_ = 23, Union_ = 24, RemoveNode_ = 25, RemoveRelationship_ = 26, 
    DetachNode_ = 27, Algorithm_ = 28, IntType_ = 29, Uint64Type_ = 30, 
    DoubleType_ = 31, StringType_ = 32, DateType_ = 33, ResultType_ = 34, 
    Count_ = 35, Sum_ = 36, Avg_ = 37, Min_ = 38, Max_ = 39, UDF_ = 40, 
    BUILTIN_ = 41, InExpandDir_ = 42, OutExpandDir_ = 43, TupleMode_ = 44, 
    ResultSetMode_ = 45, FromDir_ = 46, ToDir_ = 47, AllDir_ = 48, DescOrder_ = 49, 
    AscOrder_ = 50, INTEGER = 51, FLOAT = 52, Identifier_ = 53, Var = 54, 
    NAME_ = 55, STRING_ = 56, COLON_ = 57, DOUBLE_COLON = 58, COMMA_ = 59, 
    LPAREN = 60, RPAREN = 61, LBRACKET = 62, RBRACKET = 63, OR = 64, AND = 65, 
    EQUALS = 66, NOTEQUALS = 67, LT = 68, LTEQ = 69, GT = 70, GTEQ = 71, 
    PLUS_ = 72, MULT = 73, DIV = 74, MOD = 75, NOT = 76, WHITESPACE = 77
  };

  enum {
    RuleQuery = 0, RuleQuery_operator = 1, RuleNode_scan_op = 2, RuleScan_param = 3, 
    RuleScan_list = 4, RuleIndex_scan_op = 5, RuleIndex_scan_param = 6, 
    RuleProject_op = 7, RuleProj_list = 8, RuleProj_expr = 9, RuleType_spec = 10, 
    RuleLimit_op = 11, RuleCrossjoin_op = 12, RuleHashjoin_op = 13, RuleLeftouterjoin_op = 14, 
    RuleForeach_relationship_op = 15, RuleRship_dir = 16, RuleRship_cardinality = 17, 
    RuleRship_source_var = 18, RuleExpand_op = 19, RuleExpand_dir = 20, 
    RuleMatch_op = 21, RulePath_pattern = 22, RulePath_component = 23, RuleNode_pattern = 24, 
    RuleRship_pattern = 25, RuleCardinality_spec = 26, RuleMin_cardinality = 27, 
    RuleMax_cardinality = 28, RuleDir_spec = 29, RuleLeft_dir = 30, RuleRight_dir = 31, 
    RuleNo_dir = 32, RuleAggregate_op = 33, RuleAggregate_list = 34, RuleAggr_expr = 35, 
    RuleAggr_func = 36, RuleUnion_op = 37, RuleGroup_by_op = 38, RuleGrouping_list = 39, 
    RuleGrouping_expr = 40, RuleDistinct_op = 41, RuleFilter_op = 42, RuleLogical_expr = 43, 
    RuleBoolean_expr = 44, RuleEquality_expr = 45, RuleRelational_expr = 46, 
    RuleAdditive_expr = 47, RuleMultiplicative_expr = 48, RuleUnary_expr = 49, 
    RulePrimary_expr = 50, RuleVariable = 51, RuleValue = 52, RuleFunction_call = 53, 
    RulePrefix = 54, RuleParam_list = 55, RuleParam = 56, RuleSort_op = 57, 
    RuleSort_list = 58, RuleSort_expr = 59, RuleSort_spec = 60, RuleCreate_op = 61, 
    RuleCreate_node = 62, RuleProperty_list = 63, RuleProperty = 64, RuleCreate_rship = 65, 
    RuleNode_var = 66, RuleRemove_node_op = 67, RuleRemove_relationship_op = 68, 
    RuleDetach_node_op = 69, RuleAlgorithm_op = 70, RuleCall_mode = 71, 
    RuleAlgo_param_list = 72, RuleAlgo_param = 73
  };

  explicit poseidonParser(antlr4::TokenStream *input);

  poseidonParser(antlr4::TokenStream *input, const antlr4::atn::ParserATNSimulatorOptions &options);

  ~poseidonParser() override;

  std::string getGrammarFileName() const override;

  const antlr4::atn::ATN& getATN() const override;

  const std::vector<std::string>& getRuleNames() const override;

  const antlr4::dfa::Vocabulary& getVocabulary() const override;

  antlr4::atn::SerializedATNView getSerializedATN() const override;


  class QueryContext;
  class Query_operatorContext;
  class Node_scan_opContext;
  class Scan_paramContext;
  class Scan_listContext;
  class Index_scan_opContext;
  class Index_scan_paramContext;
  class Project_opContext;
  class Proj_listContext;
  class Proj_exprContext;
  class Type_specContext;
  class Limit_opContext;
  class Crossjoin_opContext;
  class Hashjoin_opContext;
  class Leftouterjoin_opContext;
  class Foreach_relationship_opContext;
  class Rship_dirContext;
  class Rship_cardinalityContext;
  class Rship_source_varContext;
  class Expand_opContext;
  class Expand_dirContext;
  class Match_opContext;
  class Path_patternContext;
  class Path_componentContext;
  class Node_patternContext;
  class Rship_patternContext;
  class Cardinality_specContext;
  class Min_cardinalityContext;
  class Max_cardinalityContext;
  class Dir_specContext;
  class Left_dirContext;
  class Right_dirContext;
  class No_dirContext;
  class Aggregate_opContext;
  class Aggregate_listContext;
  class Aggr_exprContext;
  class Aggr_funcContext;
  class Union_opContext;
  class Group_by_opContext;
  class Grouping_listContext;
  class Grouping_exprContext;
  class Distinct_opContext;
  class Filter_opContext;
  class Logical_exprContext;
  class Boolean_exprContext;
  class Equality_exprContext;
  class Relational_exprContext;
  class Additive_exprContext;
  class Multiplicative_exprContext;
  class Unary_exprContext;
  class Primary_exprContext;
  class VariableContext;
  class ValueContext;
  class Function_callContext;
  class PrefixContext;
  class Param_listContext;
  class ParamContext;
  class Sort_opContext;
  class Sort_listContext;
  class Sort_exprContext;
  class Sort_specContext;
  class Create_opContext;
  class Create_nodeContext;
  class Property_listContext;
  class PropertyContext;
  class Create_rshipContext;
  class Node_varContext;
  class Remove_node_opContext;
  class Remove_relationship_opContext;
  class Detach_node_opContext;
  class Algorithm_opContext;
  class Call_modeContext;
  class Algo_param_listContext;
  class Algo_paramContext; 

  class  QueryContext : public antlr4::ParserRuleContext {
  public:
    QueryContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Query_operatorContext *query_operator();
    antlr4::tree::TerminalNode *EOF();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  QueryContext* query();

  class  Query_operatorContext : public antlr4::ParserRuleContext {
  public:
    Query_operatorContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Filter_opContext *filter_op();
    Node_scan_opContext *node_scan_op();
    Index_scan_opContext *index_scan_op();
    Match_opContext *match_op();
    Project_opContext *project_op();
    Limit_opContext *limit_op();
    Crossjoin_opContext *crossjoin_op();
    Hashjoin_opContext *hashjoin_op();
    Leftouterjoin_opContext *leftouterjoin_op();
    Foreach_relationship_opContext *foreach_relationship_op();
    Expand_opContext *expand_op();
    Aggregate_opContext *aggregate_op();
    Group_by_opContext *group_by_op();
    Union_opContext *union_op();
    Sort_opContext *sort_op();
    Distinct_opContext *distinct_op();
    Create_opContext *create_op();
    Remove_node_opContext *remove_node_op();
    Detach_node_opContext *detach_node_op();
    Remove_relationship_opContext *remove_relationship_op();
    Algorithm_opContext *algorithm_op();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Query_operatorContext* query_operator();

  class  Node_scan_opContext : public antlr4::ParserRuleContext {
  public:
    Node_scan_opContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *Nodescan_();
    antlr4::tree::TerminalNode *LPAREN();
    antlr4::tree::TerminalNode *RPAREN();
    Scan_paramContext *scan_param();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Node_scan_opContext* node_scan_op();

  class  Scan_paramContext : public antlr4::ParserRuleContext {
  public:
    Scan_paramContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *STRING_();
    Scan_listContext *scan_list();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Scan_paramContext* scan_param();

  class  Scan_listContext : public antlr4::ParserRuleContext {
  public:
    Scan_listContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LBRACKET();
    std::vector<antlr4::tree::TerminalNode *> STRING_();
    antlr4::tree::TerminalNode* STRING_(size_t i);
    antlr4::tree::TerminalNode *RBRACKET();
    std::vector<antlr4::tree::TerminalNode *> COMMA_();
    antlr4::tree::TerminalNode* COMMA_(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Scan_listContext* scan_list();

  class  Index_scan_opContext : public antlr4::ParserRuleContext {
  public:
    Index_scan_opContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *Indexscan_();
    antlr4::tree::TerminalNode *LPAREN();
    Index_scan_paramContext *index_scan_param();
    antlr4::tree::TerminalNode *RPAREN();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Index_scan_opContext* index_scan_op();

  class  Index_scan_paramContext : public antlr4::ParserRuleContext {
  public:
    Index_scan_paramContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<antlr4::tree::TerminalNode *> STRING_();
    antlr4::tree::TerminalNode* STRING_(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA_();
    antlr4::tree::TerminalNode* COMMA_(size_t i);
    ValueContext *value();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Index_scan_paramContext* index_scan_param();

  class  Project_opContext : public antlr4::ParserRuleContext {
  public:
    Project_opContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *Project_();
    antlr4::tree::TerminalNode *LPAREN();
    Proj_listContext *proj_list();
    antlr4::tree::TerminalNode *COMMA_();
    Query_operatorContext *query_operator();
    antlr4::tree::TerminalNode *RPAREN();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Project_opContext* project_op();

  class  Proj_listContext : public antlr4::ParserRuleContext {
  public:
    Proj_listContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LBRACKET();
    std::vector<Proj_exprContext *> proj_expr();
    Proj_exprContext* proj_expr(size_t i);
    antlr4::tree::TerminalNode *RBRACKET();
    std::vector<antlr4::tree::TerminalNode *> COMMA_();
    antlr4::tree::TerminalNode* COMMA_(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Proj_listContext* proj_list();

  class  Proj_exprContext : public antlr4::ParserRuleContext {
  public:
    Proj_exprContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Function_callContext *function_call();
    antlr4::tree::TerminalNode *Var();
    antlr4::tree::TerminalNode *COLON_();
    Type_specContext *type_spec();
    antlr4::tree::TerminalNode *Identifier_();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Proj_exprContext* proj_expr();

  class  Type_specContext : public antlr4::ParserRuleContext {
  public:
    Type_specContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IntType_();
    antlr4::tree::TerminalNode *DoubleType_();
    antlr4::tree::TerminalNode *Uint64Type_();
    antlr4::tree::TerminalNode *StringType_();
    antlr4::tree::TerminalNode *DateType_();
    antlr4::tree::TerminalNode *ResultType_();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Type_specContext* type_spec();

  class  Limit_opContext : public antlr4::ParserRuleContext {
  public:
    Limit_opContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *Limit_();
    antlr4::tree::TerminalNode *LPAREN();
    antlr4::tree::TerminalNode *INTEGER();
    antlr4::tree::TerminalNode *COMMA_();
    Query_operatorContext *query_operator();
    antlr4::tree::TerminalNode *RPAREN();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Limit_opContext* limit_op();

  class  Crossjoin_opContext : public antlr4::ParserRuleContext {
  public:
    Crossjoin_opContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *CrossJoin_();
    antlr4::tree::TerminalNode *LPAREN();
    std::vector<Query_operatorContext *> query_operator();
    Query_operatorContext* query_operator(size_t i);
    antlr4::tree::TerminalNode *COMMA_();
    antlr4::tree::TerminalNode *RPAREN();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Crossjoin_opContext* crossjoin_op();

  class  Hashjoin_opContext : public antlr4::ParserRuleContext {
  public:
    Hashjoin_opContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *HashJoin_();
    antlr4::tree::TerminalNode *LPAREN();
    Logical_exprContext *logical_expr();
    std::vector<antlr4::tree::TerminalNode *> COMMA_();
    antlr4::tree::TerminalNode* COMMA_(size_t i);
    std::vector<Query_operatorContext *> query_operator();
    Query_operatorContext* query_operator(size_t i);
    antlr4::tree::TerminalNode *RPAREN();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Hashjoin_opContext* hashjoin_op();

  class  Leftouterjoin_opContext : public antlr4::ParserRuleContext {
  public:
    Leftouterjoin_opContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LeftOuterJoin_();
    antlr4::tree::TerminalNode *LPAREN();
    Logical_exprContext *logical_expr();
    std::vector<antlr4::tree::TerminalNode *> COMMA_();
    antlr4::tree::TerminalNode* COMMA_(size_t i);
    std::vector<Query_operatorContext *> query_operator();
    Query_operatorContext* query_operator(size_t i);
    antlr4::tree::TerminalNode *RPAREN();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Leftouterjoin_opContext* leftouterjoin_op();

  class  Foreach_relationship_opContext : public antlr4::ParserRuleContext {
  public:
    Foreach_relationship_opContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *ForeachRelationship_();
    antlr4::tree::TerminalNode *LPAREN();
    Rship_dirContext *rship_dir();
    std::vector<antlr4::tree::TerminalNode *> COMMA_();
    antlr4::tree::TerminalNode* COMMA_(size_t i);
    antlr4::tree::TerminalNode *STRING_();
    Query_operatorContext *query_operator();
    antlr4::tree::TerminalNode *RPAREN();
    Rship_cardinalityContext *rship_cardinality();
    Rship_source_varContext *rship_source_var();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Foreach_relationship_opContext* foreach_relationship_op();

  class  Rship_dirContext : public antlr4::ParserRuleContext {
  public:
    Rship_dirContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *FromDir_();
    antlr4::tree::TerminalNode *ToDir_();
    antlr4::tree::TerminalNode *AllDir_();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Rship_dirContext* rship_dir();

  class  Rship_cardinalityContext : public antlr4::ParserRuleContext {
  public:
    Rship_cardinalityContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<antlr4::tree::TerminalNode *> INTEGER();
    antlr4::tree::TerminalNode* INTEGER(size_t i);
    antlr4::tree::TerminalNode *COMMA_();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Rship_cardinalityContext* rship_cardinality();

  class  Rship_source_varContext : public antlr4::ParserRuleContext {
  public:
    Rship_source_varContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *Var();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Rship_source_varContext* rship_source_var();

  class  Expand_opContext : public antlr4::ParserRuleContext {
  public:
    Expand_opContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *Expand_();
    antlr4::tree::TerminalNode *LPAREN();
    Expand_dirContext *expand_dir();
    std::vector<antlr4::tree::TerminalNode *> COMMA_();
    antlr4::tree::TerminalNode* COMMA_(size_t i);
    antlr4::tree::TerminalNode *STRING_();
    Query_operatorContext *query_operator();
    antlr4::tree::TerminalNode *RPAREN();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Expand_opContext* expand_op();

  class  Expand_dirContext : public antlr4::ParserRuleContext {
  public:
    Expand_dirContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *InExpandDir_();
    antlr4::tree::TerminalNode *OutExpandDir_();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Expand_dirContext* expand_dir();

  class  Match_opContext : public antlr4::ParserRuleContext {
  public:
    Match_opContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *Match_();
    antlr4::tree::TerminalNode *LPAREN();
    Path_patternContext *path_pattern();
    antlr4::tree::TerminalNode *RPAREN();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Match_opContext* match_op();

  class  Path_patternContext : public antlr4::ParserRuleContext {
  public:
    Path_patternContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Node_patternContext *node_pattern();
    std::vector<Path_componentContext *> path_component();
    Path_componentContext* path_component(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Path_patternContext* path_pattern();

  class  Path_componentContext : public antlr4::ParserRuleContext {
  public:
    Path_componentContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Rship_patternContext *rship_pattern();
    Node_patternContext *node_pattern();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Path_componentContext* path_component();

  class  Node_patternContext : public antlr4::ParserRuleContext {
  public:
    Node_patternContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LPAREN();
    antlr4::tree::TerminalNode *COLON_();
    std::vector<antlr4::tree::TerminalNode *> Identifier_();
    antlr4::tree::TerminalNode* Identifier_(size_t i);
    antlr4::tree::TerminalNode *RPAREN();
    Property_listContext *property_list();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Node_patternContext* node_pattern();

  class  Rship_patternContext : public antlr4::ParserRuleContext {
  public:
    Rship_patternContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Dir_specContext *> dir_spec();
    Dir_specContext* dir_spec(size_t i);
    antlr4::tree::TerminalNode *LBRACKET();
    antlr4::tree::TerminalNode *COLON_();
    std::vector<antlr4::tree::TerminalNode *> Identifier_();
    antlr4::tree::TerminalNode* Identifier_(size_t i);
    antlr4::tree::TerminalNode *RBRACKET();
    Cardinality_specContext *cardinality_spec();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Rship_patternContext* rship_pattern();

  class  Cardinality_specContext : public antlr4::ParserRuleContext {
  public:
    Cardinality_specContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *MULT();
    Min_cardinalityContext *min_cardinality();
    Max_cardinalityContext *max_cardinality();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Cardinality_specContext* cardinality_spec();

  class  Min_cardinalityContext : public antlr4::ParserRuleContext {
  public:
    Min_cardinalityContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *INTEGER();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Min_cardinalityContext* min_cardinality();

  class  Max_cardinalityContext : public antlr4::ParserRuleContext {
  public:
    Max_cardinalityContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *INTEGER();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Max_cardinalityContext* max_cardinality();

  class  Dir_specContext : public antlr4::ParserRuleContext {
  public:
    Dir_specContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Left_dirContext *left_dir();
    Right_dirContext *right_dir();
    No_dirContext *no_dir();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Dir_specContext* dir_spec();

  class  Left_dirContext : public antlr4::ParserRuleContext {
  public:
    Left_dirContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Left_dirContext* left_dir();

  class  Right_dirContext : public antlr4::ParserRuleContext {
  public:
    Right_dirContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Right_dirContext* right_dir();

  class  No_dirContext : public antlr4::ParserRuleContext {
  public:
    No_dirContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  No_dirContext* no_dir();

  class  Aggregate_opContext : public antlr4::ParserRuleContext {
  public:
    Aggregate_opContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *Aggregate_();
    antlr4::tree::TerminalNode *LPAREN();
    Aggregate_listContext *aggregate_list();
    antlr4::tree::TerminalNode *COMMA_();
    Query_operatorContext *query_operator();
    antlr4::tree::TerminalNode *RPAREN();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Aggregate_opContext* aggregate_op();

  class  Aggregate_listContext : public antlr4::ParserRuleContext {
  public:
    Aggregate_listContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LBRACKET();
    std::vector<Aggr_exprContext *> aggr_expr();
    Aggr_exprContext* aggr_expr(size_t i);
    antlr4::tree::TerminalNode *RBRACKET();
    std::vector<antlr4::tree::TerminalNode *> COMMA_();
    antlr4::tree::TerminalNode* COMMA_(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Aggregate_listContext* aggregate_list();

  class  Aggr_exprContext : public antlr4::ParserRuleContext {
  public:
    Aggr_exprContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Aggr_funcContext *aggr_func();
    antlr4::tree::TerminalNode *LPAREN();
    Proj_exprContext *proj_expr();
    antlr4::tree::TerminalNode *RPAREN();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Aggr_exprContext* aggr_expr();

  class  Aggr_funcContext : public antlr4::ParserRuleContext {
  public:
    Aggr_funcContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *Count_();
    antlr4::tree::TerminalNode *Sum_();
    antlr4::tree::TerminalNode *Avg_();
    antlr4::tree::TerminalNode *Min_();
    antlr4::tree::TerminalNode *Max_();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Aggr_funcContext* aggr_func();

  class  Union_opContext : public antlr4::ParserRuleContext {
  public:
    Union_opContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *Union_();
    antlr4::tree::TerminalNode *LPAREN();
    std::vector<Query_operatorContext *> query_operator();
    Query_operatorContext* query_operator(size_t i);
    antlr4::tree::TerminalNode *COMMA_();
    antlr4::tree::TerminalNode *RPAREN();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Union_opContext* union_op();

  class  Group_by_opContext : public antlr4::ParserRuleContext {
  public:
    Group_by_opContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *GroupBy_();
    antlr4::tree::TerminalNode *LPAREN();
    Grouping_listContext *grouping_list();
    std::vector<antlr4::tree::TerminalNode *> COMMA_();
    antlr4::tree::TerminalNode* COMMA_(size_t i);
    Aggregate_listContext *aggregate_list();
    Query_operatorContext *query_operator();
    antlr4::tree::TerminalNode *RPAREN();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Group_by_opContext* group_by_op();

  class  Grouping_listContext : public antlr4::ParserRuleContext {
  public:
    Grouping_listContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LBRACKET();
    std::vector<Grouping_exprContext *> grouping_expr();
    Grouping_exprContext* grouping_expr(size_t i);
    antlr4::tree::TerminalNode *RBRACKET();
    std::vector<antlr4::tree::TerminalNode *> COMMA_();
    antlr4::tree::TerminalNode* COMMA_(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Grouping_listContext* grouping_list();

  class  Grouping_exprContext : public antlr4::ParserRuleContext {
  public:
    Grouping_exprContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *Var();
    antlr4::tree::TerminalNode *COLON_();
    Type_specContext *type_spec();
    antlr4::tree::TerminalNode *Identifier_();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Grouping_exprContext* grouping_expr();

  class  Distinct_opContext : public antlr4::ParserRuleContext {
  public:
    Distinct_opContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *Distinct_();
    antlr4::tree::TerminalNode *LPAREN();
    Query_operatorContext *query_operator();
    antlr4::tree::TerminalNode *RPAREN();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Distinct_opContext* distinct_op();

  class  Filter_opContext : public antlr4::ParserRuleContext {
  public:
    Filter_opContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *Filter_();
    antlr4::tree::TerminalNode *LPAREN();
    Logical_exprContext *logical_expr();
    antlr4::tree::TerminalNode *COMMA_();
    Query_operatorContext *query_operator();
    antlr4::tree::TerminalNode *RPAREN();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Filter_opContext* filter_op();

  class  Logical_exprContext : public antlr4::ParserRuleContext {
  public:
    Logical_exprContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Boolean_exprContext *> boolean_expr();
    Boolean_exprContext* boolean_expr(size_t i);
    std::vector<antlr4::tree::TerminalNode *> OR();
    antlr4::tree::TerminalNode* OR(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Logical_exprContext* logical_expr();

  class  Boolean_exprContext : public antlr4::ParserRuleContext {
  public:
    Boolean_exprContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Equality_exprContext *> equality_expr();
    Equality_exprContext* equality_expr(size_t i);
    std::vector<antlr4::tree::TerminalNode *> AND();
    antlr4::tree::TerminalNode* AND(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Boolean_exprContext* boolean_expr();

  class  Equality_exprContext : public antlr4::ParserRuleContext {
  public:
    Equality_exprContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Relational_exprContext *> relational_expr();
    Relational_exprContext* relational_expr(size_t i);
    std::vector<antlr4::tree::TerminalNode *> EQUALS();
    antlr4::tree::TerminalNode* EQUALS(size_t i);
    std::vector<antlr4::tree::TerminalNode *> NOTEQUALS();
    antlr4::tree::TerminalNode* NOTEQUALS(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Equality_exprContext* equality_expr();

  class  Relational_exprContext : public antlr4::ParserRuleContext {
  public:
    Relational_exprContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Additive_exprContext *> additive_expr();
    Additive_exprContext* additive_expr(size_t i);
    std::vector<antlr4::tree::TerminalNode *> LT();
    antlr4::tree::TerminalNode* LT(size_t i);
    std::vector<antlr4::tree::TerminalNode *> LTEQ();
    antlr4::tree::TerminalNode* LTEQ(size_t i);
    std::vector<antlr4::tree::TerminalNode *> GT();
    antlr4::tree::TerminalNode* GT(size_t i);
    std::vector<antlr4::tree::TerminalNode *> GTEQ();
    antlr4::tree::TerminalNode* GTEQ(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Relational_exprContext* relational_expr();

  class  Additive_exprContext : public antlr4::ParserRuleContext {
  public:
    Additive_exprContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Multiplicative_exprContext *> multiplicative_expr();
    Multiplicative_exprContext* multiplicative_expr(size_t i);
    std::vector<antlr4::tree::TerminalNode *> PLUS_();
    antlr4::tree::TerminalNode* PLUS_(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Additive_exprContext* additive_expr();

  class  Multiplicative_exprContext : public antlr4::ParserRuleContext {
  public:
    Multiplicative_exprContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Unary_exprContext *> unary_expr();
    Unary_exprContext* unary_expr(size_t i);
    std::vector<antlr4::tree::TerminalNode *> MULT();
    antlr4::tree::TerminalNode* MULT(size_t i);
    std::vector<antlr4::tree::TerminalNode *> DIV();
    antlr4::tree::TerminalNode* DIV(size_t i);
    std::vector<antlr4::tree::TerminalNode *> MOD();
    antlr4::tree::TerminalNode* MOD(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Multiplicative_exprContext* multiplicative_expr();

  class  Unary_exprContext : public antlr4::ParserRuleContext {
  public:
    Unary_exprContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Primary_exprContext *primary_expr();
    antlr4::tree::TerminalNode *NOT();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Unary_exprContext* unary_expr();

  class  Primary_exprContext : public antlr4::ParserRuleContext {
  public:
    Primary_exprContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LPAREN();
    Logical_exprContext *logical_expr();
    antlr4::tree::TerminalNode *RPAREN();
    Function_callContext *function_call();
    ValueContext *value();
    VariableContext *variable();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Primary_exprContext* primary_expr();

  class  VariableContext : public antlr4::ParserRuleContext {
  public:
    VariableContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *Var();
    antlr4::tree::TerminalNode *Identifier_();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  VariableContext* variable();

  class  ValueContext : public antlr4::ParserRuleContext {
  public:
    ValueContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *INTEGER();
    antlr4::tree::TerminalNode *FLOAT();
    antlr4::tree::TerminalNode *STRING_();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ValueContext* value();

  class  Function_callContext : public antlr4::ParserRuleContext {
  public:
    Function_callContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    PrefixContext *prefix();
    antlr4::tree::TerminalNode *DOUBLE_COLON();
    antlr4::tree::TerminalNode *Identifier_();
    antlr4::tree::TerminalNode *LPAREN();
    antlr4::tree::TerminalNode *RPAREN();
    Param_listContext *param_list();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Function_callContext* function_call();

  class  PrefixContext : public antlr4::ParserRuleContext {
  public:
    PrefixContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *BUILTIN_();
    antlr4::tree::TerminalNode *UDF_();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  PrefixContext* prefix();

  class  Param_listContext : public antlr4::ParserRuleContext {
  public:
    Param_listContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<ParamContext *> param();
    ParamContext* param(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA_();
    antlr4::tree::TerminalNode* COMMA_(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Param_listContext* param_list();

  class  ParamContext : public antlr4::ParserRuleContext {
  public:
    ParamContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ValueContext *value();
    antlr4::tree::TerminalNode *Var();
    antlr4::tree::TerminalNode *COLON_();
    Type_specContext *type_spec();
    antlr4::tree::TerminalNode *Identifier_();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ParamContext* param();

  class  Sort_opContext : public antlr4::ParserRuleContext {
  public:
    Sort_opContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *Sort_();
    antlr4::tree::TerminalNode *LPAREN();
    Sort_listContext *sort_list();
    antlr4::tree::TerminalNode *COMMA_();
    Query_operatorContext *query_operator();
    antlr4::tree::TerminalNode *RPAREN();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Sort_opContext* sort_op();

  class  Sort_listContext : public antlr4::ParserRuleContext {
  public:
    Sort_listContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LBRACKET();
    std::vector<Sort_exprContext *> sort_expr();
    Sort_exprContext* sort_expr(size_t i);
    antlr4::tree::TerminalNode *RBRACKET();
    std::vector<antlr4::tree::TerminalNode *> COMMA_();
    antlr4::tree::TerminalNode* COMMA_(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Sort_listContext* sort_list();

  class  Sort_exprContext : public antlr4::ParserRuleContext {
  public:
    Sort_exprContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *Var();
    antlr4::tree::TerminalNode *COLON_();
    Type_specContext *type_spec();
    Sort_specContext *sort_spec();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Sort_exprContext* sort_expr();

  class  Sort_specContext : public antlr4::ParserRuleContext {
  public:
    Sort_specContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *DescOrder_();
    antlr4::tree::TerminalNode *AscOrder_();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Sort_specContext* sort_spec();

  class  Create_opContext : public antlr4::ParserRuleContext {
  public:
    Create_opContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *Create_();
    antlr4::tree::TerminalNode *LPAREN();
    antlr4::tree::TerminalNode *RPAREN();
    Create_rshipContext *create_rship();
    Create_nodeContext *create_node();
    antlr4::tree::TerminalNode *COMMA_();
    Query_operatorContext *query_operator();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Create_opContext* create_op();

  class  Create_nodeContext : public antlr4::ParserRuleContext {
  public:
    Create_nodeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LPAREN();
    std::vector<antlr4::tree::TerminalNode *> Identifier_();
    antlr4::tree::TerminalNode* Identifier_(size_t i);
    antlr4::tree::TerminalNode *COLON_();
    antlr4::tree::TerminalNode *RPAREN();
    Property_listContext *property_list();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Create_nodeContext* create_node();

  class  Property_listContext : public antlr4::ParserRuleContext {
  public:
    Property_listContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<PropertyContext *> property();
    PropertyContext* property(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA_();
    antlr4::tree::TerminalNode* COMMA_(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Property_listContext* property_list();

  class  PropertyContext : public antlr4::ParserRuleContext {
  public:
    PropertyContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *Identifier_();
    antlr4::tree::TerminalNode *COLON_();
    ValueContext *value();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  PropertyContext* property();

  class  Create_rshipContext : public antlr4::ParserRuleContext {
  public:
    Create_rshipContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Node_varContext *> node_var();
    Node_varContext* node_var(size_t i);
    antlr4::tree::TerminalNode *LBRACKET();
    antlr4::tree::TerminalNode *COLON_();
    std::vector<antlr4::tree::TerminalNode *> Identifier_();
    antlr4::tree::TerminalNode* Identifier_(size_t i);
    antlr4::tree::TerminalNode *RBRACKET();
    Property_listContext *property_list();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Create_rshipContext* create_rship();

  class  Node_varContext : public antlr4::ParserRuleContext {
  public:
    Node_varContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LPAREN();
    antlr4::tree::TerminalNode *Var();
    antlr4::tree::TerminalNode *RPAREN();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Node_varContext* node_var();

  class  Remove_node_opContext : public antlr4::ParserRuleContext {
  public:
    Remove_node_opContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *RemoveNode_();
    antlr4::tree::TerminalNode *LPAREN();
    Query_operatorContext *query_operator();
    antlr4::tree::TerminalNode *RPAREN();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Remove_node_opContext* remove_node_op();

  class  Remove_relationship_opContext : public antlr4::ParserRuleContext {
  public:
    Remove_relationship_opContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *RemoveRelationship_();
    antlr4::tree::TerminalNode *LPAREN();
    Query_operatorContext *query_operator();
    antlr4::tree::TerminalNode *RPAREN();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Remove_relationship_opContext* remove_relationship_op();

  class  Detach_node_opContext : public antlr4::ParserRuleContext {
  public:
    Detach_node_opContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *DetachNode_();
    antlr4::tree::TerminalNode *LPAREN();
    Query_operatorContext *query_operator();
    antlr4::tree::TerminalNode *RPAREN();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Detach_node_opContext* detach_node_op();

  class  Algorithm_opContext : public antlr4::ParserRuleContext {
  public:
    Algorithm_opContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *Algorithm_();
    antlr4::tree::TerminalNode *LPAREN();
    antlr4::tree::TerminalNode *LBRACKET();
    antlr4::tree::TerminalNode *Identifier_();
    std::vector<antlr4::tree::TerminalNode *> COMMA_();
    antlr4::tree::TerminalNode* COMMA_(size_t i);
    Call_modeContext *call_mode();
    antlr4::tree::TerminalNode *RBRACKET();
    Query_operatorContext *query_operator();
    antlr4::tree::TerminalNode *RPAREN();
    Algo_param_listContext *algo_param_list();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Algorithm_opContext* algorithm_op();

  class  Call_modeContext : public antlr4::ParserRuleContext {
  public:
    Call_modeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *TupleMode_();
    antlr4::tree::TerminalNode *ResultSetMode_();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Call_modeContext* call_mode();

  class  Algo_param_listContext : public antlr4::ParserRuleContext {
  public:
    Algo_param_listContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Algo_paramContext *> algo_param();
    Algo_paramContext* algo_param(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA_();
    antlr4::tree::TerminalNode* COMMA_(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Algo_param_listContext* algo_param_list();

  class  Algo_paramContext : public antlr4::ParserRuleContext {
  public:
    Algo_paramContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ValueContext *value();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Algo_paramContext* algo_param();


  // By default the static state used to implement the parser is lazily initialized during the first
  // call to the constructor. You can call this function if you wish to initialize the static state
  // ahead of time.
  static void initialize();

private:
};

