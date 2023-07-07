
// Generated from src/query/parser/poseidon.g4 by ANTLR 4.13.0

#pragma once


#include "antlr4-runtime.h"




class  poseidonLexer : public antlr4::Lexer {
public:
  enum {
    T__0 = 1, T__1 = 2, T__2 = 3, T__3 = 4, T__4 = 5, T__5 = 6, Filter_ = 7, 
    Nodescan_ = 8, Match_ = 9, Project_ = 10, Limit_ = 11, CrossJoin_ = 12, 
    HashJoin_ = 13, LeftOuterJoin_ = 14, Expand_ = 15, ForeachRelationship_ = 16, 
    Aggregate_ = 17, GroupBy_ = 18, Sort_ = 19, Create_ = 20, Union_ = 21, 
    IntType_ = 22, Uint64Type_ = 23, DoubleType_ = 24, StringType_ = 25, 
    DateType_ = 26, ResultType_ = 27, Count_ = 28, Sum_ = 29, Avg_ = 30, 
    Min_ = 31, Max_ = 32, UDF_ = 33, InExpandDir_ = 34, OutExpandDir_ = 35, 
    FromDir_ = 36, ToDir_ = 37, AllDir_ = 38, DescOrder_ = 39, AscOrder_ = 40, 
    INTEGER = 41, FLOAT = 42, Identifier_ = 43, Var = 44, NAME_ = 45, STRING_ = 46, 
    COLON_ = 47, DOUBLE_COLON = 48, COMMA_ = 49, LPAREN = 50, RPAREN = 51, 
    LBRACKET = 52, RBRACKET = 53, OR = 54, AND = 55, EQUALS = 56, NOTEQUALS = 57, 
    LT = 58, LTEQ = 59, GT = 60, GTEQ = 61, PLUS_ = 62, MULT = 63, DIV = 64, 
    MOD = 65, NOT = 66, WHITESPACE = 67
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

