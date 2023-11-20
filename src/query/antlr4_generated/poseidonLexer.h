
// Generated from src/query/parser/poseidon.g4 by ANTLR 4.13.1

#pragma once


#include "antlr4-runtime.h"




class  poseidonLexer : public antlr4::Lexer {
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

  explicit poseidonLexer(antlr4::CharStream *input);

  ~poseidonLexer() override;


  std::string getGrammarFileName() const override;

  const std::vector<std::string>& getRuleNames() const override;

  const std::vector<std::string>& getChannelNames() const override;

  const std::vector<std::string>& getModeNames() const override;

  const antlr4::dfa::Vocabulary& getVocabulary() const override;

  antlr4::atn::SerializedATNView getSerializedATN() const override;

  const antlr4::atn::ATN& getATN() const override;

  // By default the static state used to implement the lexer is lazily initialized during the first
  // call to the constructor. You can call this function if you wish to initialize the static state
  // ahead of time.
  static void initialize();

private:

  // Individual action functions triggered by action() above.

  // Individual semantic predicate functions triggered by sempred() above.

};

