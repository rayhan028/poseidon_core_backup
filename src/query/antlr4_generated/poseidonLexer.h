
// Generated from src/query/parser/poseidon.g4 by ANTLR 4.13.0

#pragma once


#include "antlr4-runtime.h"




class  poseidonLexer : public antlr4::Lexer {
public:
  enum {
    T__0 = 1, T__1 = 2, T__2 = 3, T__3 = 4, T__4 = 5, T__5 = 6, Filter_ = 7, 
    Nodescan_ = 8, Indexscan_ = 9, Match_ = 10, Project_ = 11, Limit_ = 12, 
    CrossJoin_ = 13, HashJoin_ = 14, LeftOuterJoin_ = 15, Expand_ = 16, 
    ForeachRelationship_ = 17, Aggregate_ = 18, GroupBy_ = 19, Sort_ = 20, 
    Create_ = 21, Union_ = 22, IntType_ = 23, Uint64Type_ = 24, DoubleType_ = 25, 
    StringType_ = 26, DateType_ = 27, ResultType_ = 28, Count_ = 29, Sum_ = 30, 
    Avg_ = 31, Min_ = 32, Max_ = 33, UDF_ = 34, InExpandDir_ = 35, OutExpandDir_ = 36, 
    FromDir_ = 37, ToDir_ = 38, AllDir_ = 39, DescOrder_ = 40, AscOrder_ = 41, 
    INTEGER = 42, FLOAT = 43, Identifier_ = 44, Var = 45, NAME_ = 46, STRING_ = 47, 
    COLON_ = 48, DOUBLE_COLON = 49, COMMA_ = 50, LPAREN = 51, RPAREN = 52, 
    LBRACKET = 53, RBRACKET = 54, OR = 55, AND = 56, EQUALS = 57, NOTEQUALS = 58, 
    LT = 59, LTEQ = 60, GT = 61, GTEQ = 62, PLUS_ = 63, MULT = 64, DIV = 65, 
    MOD = 66, NOT = 67, WHITESPACE = 68
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

