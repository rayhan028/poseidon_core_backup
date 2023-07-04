
// Generated from src/query/parser/poseidon.g4 by ANTLR 4.13.0

#pragma once


#include "antlr4-runtime.h"




class  poseidonLexer : public antlr4::Lexer {
public:
  enum {
    T__0 = 1, T__1 = 2, T__2 = 3, T__3 = 4, T__4 = 5, Filter_ = 6, Nodescan_ = 7, 
    Match_ = 8, Project_ = 9, Limit_ = 10, CrossJoin_ = 11, HashJoin_ = 12, 
    LeftOuterJoin_ = 13, Expand_ = 14, ForeachRelationship_ = 15, Aggregate_ = 16, 
    GroupBy_ = 17, Sort_ = 18, Create_ = 19, Union_ = 20, IntType_ = 21, 
    Uint64Type_ = 22, DoubleType_ = 23, StringType_ = 24, DateType_ = 25, 
    ResultType_ = 26, Count_ = 27, Sum_ = 28, Avg_ = 29, Min_ = 30, Max_ = 31, 
    UDF_ = 32, InExpandDir_ = 33, OutExpandDir_ = 34, FromDir_ = 35, ToDir_ = 36, 
    AllDir_ = 37, DescOrder_ = 38, AscOrder_ = 39, INTEGER = 40, FLOAT = 41, 
    Identifier_ = 42, Var = 43, NAME_ = 44, STRING_ = 45, COLON_ = 46, DOUBLE_COLON = 47, 
    COMMA_ = 48, LPAREN = 49, RPAREN = 50, LBRACKET = 51, RBRACKET = 52, 
    OR = 53, AND = 54, EQUALS = 55, NOTEQUALS = 56, LT = 57, LTEQ = 58, 
    GT = 59, GTEQ = 60, PLUS_ = 61, MULT = 62, DIV = 63, MOD = 64, NOT = 65, 
    WHITESPACE = 66
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

