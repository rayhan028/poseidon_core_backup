
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
    DoubleType_ = 31, StringType_ = 32, DateType_ = 33, NodeResultType_ = 34, 
    RshipResultType_ = 35, Count_ = 36, Sum_ = 37, Avg_ = 38, Min_ = 39, 
    Max_ = 40, UDF_ = 41, BUILTIN_ = 42, InExpandDir_ = 43, OutExpandDir_ = 44, 
    TupleMode_ = 45, ResultSetMode_ = 46, FromDir_ = 47, ToDir_ = 48, AllDir_ = 49, 
    DescOrder_ = 50, AscOrder_ = 51, INTEGER = 52, FLOAT = 53, Identifier_ = 54, 
    Var = 55, NAME_ = 56, STRING_ = 57, COLON_ = 58, DOUBLE_COLON = 59, 
    COMMA_ = 60, LPAREN = 61, RPAREN = 62, LBRACKET = 63, RBRACKET = 64, 
    OR = 65, AND = 66, EQUALS = 67, NOTEQUALS = 68, LT = 69, LTEQ = 70, 
    GT = 71, GTEQ = 72, PLUS_ = 73, MULT = 74, DIV = 75, MOD = 76, REGEX = 77, 
    NOT = 78, WHITESPACE = 79
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

