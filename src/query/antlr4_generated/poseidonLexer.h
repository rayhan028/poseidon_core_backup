
// Generated from src/query/parser/poseidon.g4 by ANTLR 4.13.0

#pragma once


#include "antlr4-runtime.h"




class  poseidonLexer : public antlr4::Lexer {
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

