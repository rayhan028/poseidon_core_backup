
// Generated from src/query/parser/poseidon.g4 by ANTLR 4.13.1

#pragma once


#include "antlr4-runtime.h"




class  poseidonLexer : public antlr4::Lexer {
public:
  enum {
    T__0 = 1, T__1 = 2, T__2 = 3, T__3 = 4, T__4 = 5, T__5 = 6, T__6 = 7, 
    Filter_ = 8, Nodescan_ = 9, Rshipscan_ = 10, Indexscan_ = 11, Match_ = 12, 
    Project_ = 13, Limit_ = 14, CrossJoin_ = 15, HashJoin_ = 16, NLJoin_ = 17, 
    LeftOuterJoin_ = 18, Expand_ = 19, ForeachRelationship_ = 20, Aggregate_ = 21, 
    GroupBy_ = 22, Sort_ = 23, Distinct_ = 24, Create_ = 25, Union_ = 26, 
    Except_ = 27, RemoveNode_ = 28, RemoveRelationship_ = 29, DetachNode_ = 30, 
    Algorithm_ = 31, Case_ = 32, IntType_ = 33, Uint64Type_ = 34, DoubleType_ = 35, 
    StringType_ = 36, DateType_ = 37, NodeResultType_ = 38, RshipResultType_ = 39, 
    Count_ = 40, Sum_ = 41, Avg_ = 42, Min_ = 43, Max_ = 44, UDF_ = 45, 
    BUILTIN_ = 46, InExpandDir_ = 47, OutExpandDir_ = 48, TupleMode_ = 49, 
    ResultSetMode_ = 50, FromDir_ = 51, ToDir_ = 52, AllDir_ = 53, DescOrder_ = 54, 
    AscOrder_ = 55, INTEGER = 56, FLOAT = 57, Identifier_ = 58, Var = 59, 
    NAME_ = 60, STRING_ = 61, COLON_ = 62, DOUBLE_COLON = 63, COMMA_ = 64, 
    LPAREN = 65, RPAREN = 66, LBRACKET = 67, RBRACKET = 68, OR = 69, AND = 70, 
    EQUALS = 71, NOTEQUALS = 72, LT = 73, LTEQ = 74, GT = 75, GTEQ = 76, 
    PLUS_ = 77, MULT = 78, DIV = 79, MOD = 80, REGEX = 81, NOT = 82, WHITESPACE = 83, 
    COMMENT = 84
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

