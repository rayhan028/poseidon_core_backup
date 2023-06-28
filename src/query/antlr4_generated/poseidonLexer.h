
// Generated from src/query/parser/poseidon.g4 by ANTLR 4.13.0

#pragma once


#include "antlr4-runtime.h"




class  poseidonLexer : public antlr4::Lexer {
public:
  enum {
    T__0 = 1, T__1 = 2, T__2 = 3, T__3 = 4, T__4 = 5, Filter_ = 6, Nodescan_ = 7, 
    Match_ = 8, Project_ = 9, Limit_ = 10, CrossJoin_ = 11, HashJoin_ = 12, 
    Expand_ = 13, ForeachRelationship_ = 14, Aggregate_ = 15, GroupBy_ = 16, 
    Sort_ = 17, Create_ = 18, Union_ = 19, IntType_ = 20, Uint64Type_ = 21, 
    DoubleType_ = 22, StringType_ = 23, DateType_ = 24, Count_ = 25, Sum_ = 26, 
    Avg_ = 27, Min_ = 28, Max_ = 29, UDF_ = 30, InExpandDir_ = 31, OutExpandDir_ = 32, 
    FromDir_ = 33, ToDir_ = 34, AllDir_ = 35, DescOrder_ = 36, AscOrder_ = 37, 
    INTEGER = 38, FLOAT = 39, Identifier_ = 40, Var = 41, NAME_ = 42, STRING_ = 43, 
    COLON_ = 44, DOUBLE_COLON = 45, COMMA_ = 46, LPAREN = 47, RPAREN = 48, 
    LBRACKET = 49, RBRACKET = 50, OR = 51, AND = 52, EQUALS = 53, NOTEQUALS = 54, 
    LT = 55, LTEQ = 56, GT = 57, GTEQ = 58, PLUS_ = 59, MULT = 60, DIV = 61, 
    MOD = 62, NOT = 63, WHITESPACE = 64
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

