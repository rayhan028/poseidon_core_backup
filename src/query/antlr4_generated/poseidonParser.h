
// Generated from src/query/parser/poseidon.g4 by ANTLR 4.11.1

#pragma once


#include "antlr4-runtime.h"




class  poseidonParser : public antlr4::Parser {
public:
  enum {
    T__0 = 1, T__1 = 2, T__2 = 3, T__3 = 4, T__4 = 5, Filter_ = 6, Nodescan_ = 7, 
    Match_ = 8, Project_ = 9, Limit_ = 10, CrossJoin_ = 11, HashJoin_ = 12, 
    Expand_ = 13, ForeachRelationship_ = 14, Aggregate_ = 15, GroupBy_ = 16, 
    Sort_ = 17, Create_ = 18, IntType_ = 19, Uint64Type_ = 20, DoubleType_ = 21, 
    StringType_ = 22, DateType_ = 23, Count_ = 24, Sum_ = 25, Avg_ = 26, 
    Min_ = 27, Max_ = 28, UDF_ = 29, InExpandDir_ = 30, OutExpandDir_ = 31, 
    FromDir_ = 32, ToDir_ = 33, AllDir_ = 34, DescOrder_ = 35, AscOrder_ = 36, 
    INTEGER = 37, FLOAT = 38, Identifier_ = 39, Var = 40, NAME_ = 41, STRING_ = 42, 
    COLON_ = 43, DOUBLE_COLON = 44, COMMA_ = 45, LPAREN = 46, RPAREN = 47, 
    LBRACKET = 48, RBRACKET = 49, OR = 50, AND = 51, EQUALS = 52, NOTEQUALS = 53, 
    LT = 54, LTEQ = 55, GT = 56, GTEQ = 57, PLUS_ = 58, MULT = 59, DIV = 60, 
    MOD = 61, NOT = 62, WHITESPACE = 63
  };

  enum {
    RuleQuery = 0, RuleQuery_operator = 1, RuleNode_scan_op = 2, RuleScan_param = 3, 
    RuleScan_list = 4, RuleProject_op = 5, RuleProj_list = 6, RuleProj_expr = 7, 
    RuleType_spec = 8, RuleLimit_op = 9, RuleCrossjoin_op = 10, RuleHashjoin_op = 11, 
    RuleForeach_relationship_op = 12, RuleRship_dir = 13, RuleRship_cardinality = 14, 
    RuleRship_source_var = 15, RuleExpand_op = 16, RuleExpand_dir = 17, 
    RuleMatch_op = 18, RulePath_pattern = 19, RulePath_component = 20, RuleNode_pattern = 21, 
    RuleRship_pattern = 22, RuleAggregate_op = 23, RuleAggregate_list = 24, 
    RuleAggr_expr = 25, RuleAggr_func = 26, RuleGroup_by_op = 27, RuleGrouping_list = 28, 
    RuleFilter_op = 29, RuleLogical_expr = 30, RuleBoolean_expr = 31, RuleEquality_expr = 32, 
    RuleRelational_expr = 33, RuleAdditive_expr = 34, RuleMultiplicative_expr = 35, 
    RuleUnary_expr = 36, RulePrimary_expr = 37, RuleVariable = 38, RuleValue = 39, 
    RuleFunction_call = 40, RuleUdf_prefix = 41, RuleParam_list = 42, RuleParam = 43, 
    RuleSort_op = 44, RuleSort_list = 45, RuleSort_expr = 46, RuleSort_spec = 47, 
    RuleCreate_op = 48, RuleCreate_node = 49, RuleProperty_list = 50, RuleProperty = 51, 
    RuleCreate_rship = 52, RuleNode_var = 53
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
  class Project_opContext;
  class Proj_listContext;
  class Proj_exprContext;
  class Type_specContext;
  class Limit_opContext;
  class Crossjoin_opContext;
  class Hashjoin_opContext;
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
  class Aggregate_opContext;
  class Aggregate_listContext;
  class Aggr_exprContext;
  class Aggr_funcContext;
  class Group_by_opContext;
  class Grouping_listContext;
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
  class Udf_prefixContext;
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
    Match_opContext *match_op();
    Project_opContext *project_op();
    Limit_opContext *limit_op();
    Crossjoin_opContext *crossjoin_op();
    Hashjoin_opContext *hashjoin_op();
    Foreach_relationship_opContext *foreach_relationship_op();
    Expand_opContext *expand_op();
    Aggregate_opContext *aggregate_op();
    Group_by_opContext *group_by_op();
    Sort_opContext *sort_op();
    Create_opContext *create_op();


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
    antlr4::tree::TerminalNode *Var();
    antlr4::tree::TerminalNode *Identifier_();
    antlr4::tree::TerminalNode *COLON_();
    Type_specContext *type_spec();
    Function_callContext *function_call();


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
    antlr4::tree::TerminalNode *LBRACKET();
    antlr4::tree::TerminalNode *COLON_();
    std::vector<antlr4::tree::TerminalNode *> Identifier_();
    antlr4::tree::TerminalNode* Identifier_(size_t i);
    antlr4::tree::TerminalNode *RBRACKET();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Rship_patternContext* rship_pattern();

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
    Proj_listContext *proj_list();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Grouping_listContext* grouping_list();

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
    Udf_prefixContext *udf_prefix();
    antlr4::tree::TerminalNode *Identifier_();
    antlr4::tree::TerminalNode *LPAREN();
    antlr4::tree::TerminalNode *RPAREN();
    Param_listContext *param_list();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Function_callContext* function_call();

  class  Udf_prefixContext : public antlr4::ParserRuleContext {
  public:
    Udf_prefixContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *UDF_();
    antlr4::tree::TerminalNode *DOUBLE_COLON();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Udf_prefixContext* udf_prefix();

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
    VariableContext *variable();
    antlr4::tree::TerminalNode *COLON_();
    Type_specContext *type_spec();


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


  // By default the static state used to implement the parser is lazily initialized during the first
  // call to the constructor. You can call this function if you wish to initialize the static state
  // ahead of time.
  static void initialize();

private:
};

