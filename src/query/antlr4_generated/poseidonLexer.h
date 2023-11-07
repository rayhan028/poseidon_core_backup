
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
    Create_ = 22, Union_ = 23, RemoveNode_ = 24, RemoveRelationship_ = 25, 
    DetachNode_ = 26, IntType_ = 27, Uint64Type_ = 28, DoubleType_ = 29, 
    StringType_ = 30, DateType_ = 31, ResultType_ = 32, Count_ = 33, Sum_ = 34, 
    Avg_ = 35, Min_ = 36, Max_ = 37, UDF_ = 38, InExpandDir_ = 39, OutExpandDir_ = 40, 
    FromDir_ = 41, ToDir_ = 42, AllDir_ = 43, DescOrder_ = 44, AscOrder_ = 45, 
    INTEGER = 46, FLOAT = 47, Identifier_ = 48, Var = 49, NAME_ = 50, STRING_ = 51, 
    COLON_ = 52, DOUBLE_COLON = 53, COMMA_ = 54, LPAREN = 55, RPAREN = 56, 
    LBRACKET = 57, RBRACKET = 58, OR = 59, AND = 60, EQUALS = 61, NOTEQUALS = 62, 
    LT = 63, LTEQ = 64, GT = 65, GTEQ = 66, PLUS_ = 67, MULT = 68, DIV = 69, 
    MOD = 70, NOT = 71, WHITESPACE = 72
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

