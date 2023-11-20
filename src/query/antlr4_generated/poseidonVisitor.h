
// Generated from src/query/parser/poseidon.g4 by ANTLR 4.13.1

#pragma once


#include "antlr4-runtime.h"
#include "poseidonParser.h"



/**
 * This class defines an abstract visitor for a parse tree
 * produced by poseidonParser.
 */
class  poseidonVisitor : public antlr4::tree::AbstractParseTreeVisitor {
public:

  /**
   * Visit parse trees produced by poseidonParser.
   */
    virtual std::any visitQuery(poseidonParser::QueryContext *context) = 0;

    virtual std::any visitQuery_operator(poseidonParser::Query_operatorContext *context) = 0;

    virtual std::any visitNode_scan_op(poseidonParser::Node_scan_opContext *context) = 0;

    virtual std::any visitScan_param(poseidonParser::Scan_paramContext *context) = 0;

    virtual std::any visitScan_list(poseidonParser::Scan_listContext *context) = 0;

    virtual std::any visitIndex_scan_op(poseidonParser::Index_scan_opContext *context) = 0;

    virtual std::any visitIndex_scan_param(poseidonParser::Index_scan_paramContext *context) = 0;

    virtual std::any visitProject_op(poseidonParser::Project_opContext *context) = 0;

    virtual std::any visitProj_list(poseidonParser::Proj_listContext *context) = 0;

    virtual std::any visitProj_expr(poseidonParser::Proj_exprContext *context) = 0;

    virtual std::any visitType_spec(poseidonParser::Type_specContext *context) = 0;

    virtual std::any visitLimit_op(poseidonParser::Limit_opContext *context) = 0;

    virtual std::any visitCrossjoin_op(poseidonParser::Crossjoin_opContext *context) = 0;

    virtual std::any visitHashjoin_op(poseidonParser::Hashjoin_opContext *context) = 0;

    virtual std::any visitLeftouterjoin_op(poseidonParser::Leftouterjoin_opContext *context) = 0;

    virtual std::any visitForeach_relationship_op(poseidonParser::Foreach_relationship_opContext *context) = 0;

    virtual std::any visitRship_dir(poseidonParser::Rship_dirContext *context) = 0;

    virtual std::any visitRship_cardinality(poseidonParser::Rship_cardinalityContext *context) = 0;

    virtual std::any visitRship_source_var(poseidonParser::Rship_source_varContext *context) = 0;

    virtual std::any visitExpand_op(poseidonParser::Expand_opContext *context) = 0;

    virtual std::any visitExpand_dir(poseidonParser::Expand_dirContext *context) = 0;

    virtual std::any visitMatch_op(poseidonParser::Match_opContext *context) = 0;

    virtual std::any visitPath_pattern(poseidonParser::Path_patternContext *context) = 0;

    virtual std::any visitPath_component(poseidonParser::Path_componentContext *context) = 0;

    virtual std::any visitNode_pattern(poseidonParser::Node_patternContext *context) = 0;

    virtual std::any visitRship_pattern(poseidonParser::Rship_patternContext *context) = 0;

    virtual std::any visitCardinality_spec(poseidonParser::Cardinality_specContext *context) = 0;

    virtual std::any visitMin_cardinality(poseidonParser::Min_cardinalityContext *context) = 0;

    virtual std::any visitMax_cardinality(poseidonParser::Max_cardinalityContext *context) = 0;

    virtual std::any visitDir_spec(poseidonParser::Dir_specContext *context) = 0;

    virtual std::any visitLeft_dir(poseidonParser::Left_dirContext *context) = 0;

    virtual std::any visitRight_dir(poseidonParser::Right_dirContext *context) = 0;

    virtual std::any visitNo_dir(poseidonParser::No_dirContext *context) = 0;

    virtual std::any visitAggregate_op(poseidonParser::Aggregate_opContext *context) = 0;

    virtual std::any visitAggregate_list(poseidonParser::Aggregate_listContext *context) = 0;

    virtual std::any visitAggr_expr(poseidonParser::Aggr_exprContext *context) = 0;

    virtual std::any visitAggr_func(poseidonParser::Aggr_funcContext *context) = 0;

    virtual std::any visitUnion_op(poseidonParser::Union_opContext *context) = 0;

    virtual std::any visitGroup_by_op(poseidonParser::Group_by_opContext *context) = 0;

    virtual std::any visitGrouping_list(poseidonParser::Grouping_listContext *context) = 0;

    virtual std::any visitGrouping_expr(poseidonParser::Grouping_exprContext *context) = 0;

    virtual std::any visitDistinct_op(poseidonParser::Distinct_opContext *context) = 0;

    virtual std::any visitFilter_op(poseidonParser::Filter_opContext *context) = 0;

    virtual std::any visitLogical_expr(poseidonParser::Logical_exprContext *context) = 0;

    virtual std::any visitBoolean_expr(poseidonParser::Boolean_exprContext *context) = 0;

    virtual std::any visitEquality_expr(poseidonParser::Equality_exprContext *context) = 0;

    virtual std::any visitRelational_expr(poseidonParser::Relational_exprContext *context) = 0;

    virtual std::any visitAdditive_expr(poseidonParser::Additive_exprContext *context) = 0;

    virtual std::any visitMultiplicative_expr(poseidonParser::Multiplicative_exprContext *context) = 0;

    virtual std::any visitUnary_expr(poseidonParser::Unary_exprContext *context) = 0;

    virtual std::any visitPrimary_expr(poseidonParser::Primary_exprContext *context) = 0;

    virtual std::any visitVariable(poseidonParser::VariableContext *context) = 0;

    virtual std::any visitValue(poseidonParser::ValueContext *context) = 0;

    virtual std::any visitFunction_call(poseidonParser::Function_callContext *context) = 0;

    virtual std::any visitPrefix(poseidonParser::PrefixContext *context) = 0;

    virtual std::any visitParam_list(poseidonParser::Param_listContext *context) = 0;

    virtual std::any visitParam(poseidonParser::ParamContext *context) = 0;

    virtual std::any visitSort_op(poseidonParser::Sort_opContext *context) = 0;

    virtual std::any visitSort_list(poseidonParser::Sort_listContext *context) = 0;

    virtual std::any visitSort_expr(poseidonParser::Sort_exprContext *context) = 0;

    virtual std::any visitSort_spec(poseidonParser::Sort_specContext *context) = 0;

    virtual std::any visitCreate_op(poseidonParser::Create_opContext *context) = 0;

    virtual std::any visitCreate_node(poseidonParser::Create_nodeContext *context) = 0;

    virtual std::any visitProperty_list(poseidonParser::Property_listContext *context) = 0;

    virtual std::any visitProperty(poseidonParser::PropertyContext *context) = 0;

    virtual std::any visitCreate_rship(poseidonParser::Create_rshipContext *context) = 0;

    virtual std::any visitNode_var(poseidonParser::Node_varContext *context) = 0;

    virtual std::any visitRemove_node_op(poseidonParser::Remove_node_opContext *context) = 0;

    virtual std::any visitRemove_relationship_op(poseidonParser::Remove_relationship_opContext *context) = 0;

    virtual std::any visitDetach_node_op(poseidonParser::Detach_node_opContext *context) = 0;

    virtual std::any visitAlgorithm_op(poseidonParser::Algorithm_opContext *context) = 0;

    virtual std::any visitCall_mode(poseidonParser::Call_modeContext *context) = 0;

    virtual std::any visitAlgo_param_list(poseidonParser::Algo_param_listContext *context) = 0;

    virtual std::any visitAlgo_param(poseidonParser::Algo_paramContext *context) = 0;


};

