
// Generated from src/query/parser/poseidon.g4 by ANTLR 4.11.1

#pragma once


#include "antlr4-runtime.h"
#include "poseidonVisitor.h"


/**
 * This class provides an empty implementation of poseidonVisitor, which can be
 * extended to create a visitor which only needs to handle a subset of the available methods.
 */
class  poseidonBaseVisitor : public poseidonVisitor {
public:

  virtual std::any visitQuery(poseidonParser::QueryContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitQuery_operator(poseidonParser::Query_operatorContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitNode_scan_op(poseidonParser::Node_scan_opContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitScan_param(poseidonParser::Scan_paramContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitScan_list(poseidonParser::Scan_listContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitProject_op(poseidonParser::Project_opContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitProj_list(poseidonParser::Proj_listContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitProj_expr(poseidonParser::Proj_exprContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitType_spec(poseidonParser::Type_specContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLimit_op(poseidonParser::Limit_opContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCrossjoin_op(poseidonParser::Crossjoin_opContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitHashjoin_op(poseidonParser::Hashjoin_opContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitForeach_relationship_op(poseidonParser::Foreach_relationship_opContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitRship_dir(poseidonParser::Rship_dirContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitRship_cardinality(poseidonParser::Rship_cardinalityContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitRship_source_var(poseidonParser::Rship_source_varContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExpand_op(poseidonParser::Expand_opContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExpand_dir(poseidonParser::Expand_dirContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitMatch_op(poseidonParser::Match_opContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPath_pattern(poseidonParser::Path_patternContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPath_component(poseidonParser::Path_componentContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitNode_pattern(poseidonParser::Node_patternContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitRship_pattern(poseidonParser::Rship_patternContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAggregate_op(poseidonParser::Aggregate_opContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAggregate_list(poseidonParser::Aggregate_listContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAggr_expr(poseidonParser::Aggr_exprContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAggr_func(poseidonParser::Aggr_funcContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitGroup_by_op(poseidonParser::Group_by_opContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitGrouping_list(poseidonParser::Grouping_listContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFilter_op(poseidonParser::Filter_opContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLogical_expr(poseidonParser::Logical_exprContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBoolean_expr(poseidonParser::Boolean_exprContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitEquality_expr(poseidonParser::Equality_exprContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitRelational_expr(poseidonParser::Relational_exprContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAdditive_expr(poseidonParser::Additive_exprContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitMultiplicative_expr(poseidonParser::Multiplicative_exprContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitUnary_expr(poseidonParser::Unary_exprContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPrimary_expr(poseidonParser::Primary_exprContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitVariable(poseidonParser::VariableContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitValue(poseidonParser::ValueContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFunction_call(poseidonParser::Function_callContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitUdf_prefix(poseidonParser::Udf_prefixContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitParam_list(poseidonParser::Param_listContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitParam(poseidonParser::ParamContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSort_op(poseidonParser::Sort_opContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSort_list(poseidonParser::Sort_listContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSort_expr(poseidonParser::Sort_exprContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSort_spec(poseidonParser::Sort_specContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCreate_op(poseidonParser::Create_opContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCreate_node(poseidonParser::Create_nodeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitProperty_list(poseidonParser::Property_listContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitProperty(poseidonParser::PropertyContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCreate_rship(poseidonParser::Create_rshipContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitNode_var(poseidonParser::Node_varContext *ctx) override {
    return visitChildren(ctx);
  }


};

