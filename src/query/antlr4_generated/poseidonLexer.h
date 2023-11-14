
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
    DetachNode_ = 26, Algorithm_ = 27, IntType_ = 28, Uint64Type_ = 29, 
    DoubleType_ = 30, StringType_ = 31, DateType_ = 32, ResultType_ = 33, 
    Count_ = 34, Sum_ = 35, Avg_ = 36, Min_ = 37, Max_ = 38, UDF_ = 39, 
    InExpandDir_ = 40, OutExpandDir_ = 41, TupleMode_ = 42, ResultSetMode_ = 43, 
    FromDir_ = 44, ToDir_ = 45, AllDir_ = 46, DescOrder_ = 47, AscOrder_ = 48, 
    INTEGER = 49, FLOAT = 50, Identifier_ = 51, Var = 52, NAME_ = 53, STRING_ = 54, 
    COLON_ = 55, DOUBLE_COLON = 56, COMMA_ = 57, LPAREN = 58, RPAREN = 59, 
    LBRACKET = 60, RBRACKET = 61, OR = 62, AND = 63, EQUALS = 64, NOTEQUALS = 65, 
    LT = 66, LTEQ = 67, GT = 68, GTEQ = 69, PLUS_ = 70, MULT = 71, DIV = 72, 
    MOD = 73, NOT = 74, WHITESPACE = 75
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

