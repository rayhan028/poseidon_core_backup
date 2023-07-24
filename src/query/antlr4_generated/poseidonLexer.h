
// Generated from src/query/parser/poseidon.g4 by ANTLR 4.13.0

#pragma once


#include "antlr4-runtime.h"




class  poseidonLexer : public antlr4::Lexer {
public:
  enum {
    T__0 = 1, T__1 = 2, T__2 = 3, T__3 = 4, T__4 = 5, T__5 = 6, T__6 = 7, 
    Filter_ = 8, Nodescan_ = 9, Indexscan_ = 10, Match_ = 11, Project_ = 12, 
    Limit_ = 13, CrossJoin_ = 14, HashJoin_ = 15, LeftOuterJoin_ = 16, Expand_ = 17, 
    ForeachRelationship_ = 18, Aggregate_ = 19, GroupBy_ = 20, Sort_ = 21, 
    Create_ = 22, Union_ = 23, IntType_ = 24, Uint64Type_ = 25, DoubleType_ = 26, 
    StringType_ = 27, DateType_ = 28, ResultType_ = 29, Count_ = 30, Sum_ = 31, 
    Avg_ = 32, Min_ = 33, Max_ = 34, UDF_ = 35, InExpandDir_ = 36, OutExpandDir_ = 37, 
    FromDir_ = 38, ToDir_ = 39, AllDir_ = 40, DescOrder_ = 41, AscOrder_ = 42, 
    INTEGER = 43, FLOAT = 44, Identifier_ = 45, Var = 46, NAME_ = 47, STRING_ = 48, 
    COLON_ = 49, DOUBLE_COLON = 50, COMMA_ = 51, LPAREN = 52, RPAREN = 53, 
    LBRACKET = 54, RBRACKET = 55, OR = 56, AND = 57, EQUALS = 58, NOTEQUALS = 59, 
    LT = 60, LTEQ = 61, GT = 62, GTEQ = 63, PLUS_ = 64, MULT = 65, DIV = 66, 
    MOD = 67, NOT = 68, WHITESPACE = 69
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

