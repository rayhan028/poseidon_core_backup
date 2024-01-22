
// Generated from src/query/parser/poseidon.g4 by ANTLR 4.13.1

#pragma once


#include "antlr4-runtime.h"




class  poseidonLexer : public antlr4::Lexer {
public:
  enum {
    T__0 = 1, T__1 = 2, T__2 = 3, T__3 = 4, T__4 = 5, T__5 = 6, T__6 = 7, 
    Filter_ = 8, Nodescan_ = 9, Indexscan_ = 10, Match_ = 11, Project_ = 12, 
    Limit_ = 13, CrossJoin_ = 14, HashJoin_ = 15, NLJoin_ = 16, LeftOuterJoin_ = 17, 
    Expand_ = 18, ForeachRelationship_ = 19, Aggregate_ = 20, GroupBy_ = 21, 
    Sort_ = 22, Distinct_ = 23, Create_ = 24, Union_ = 25, RemoveNode_ = 26, 
    RemoveRelationship_ = 27, DetachNode_ = 28, Algorithm_ = 29, Case_ = 30, 
    IntType_ = 31, Uint64Type_ = 32, DoubleType_ = 33, StringType_ = 34, 
    DateType_ = 35, NodeResultType_ = 36, RshipResultType_ = 37, Count_ = 38, 
    Sum_ = 39, Avg_ = 40, Min_ = 41, Max_ = 42, UDF_ = 43, BUILTIN_ = 44, 
    InExpandDir_ = 45, OutExpandDir_ = 46, TupleMode_ = 47, ResultSetMode_ = 48, 
    FromDir_ = 49, ToDir_ = 50, AllDir_ = 51, DescOrder_ = 52, AscOrder_ = 53, 
    INTEGER = 54, FLOAT = 55, Identifier_ = 56, Var = 57, NAME_ = 58, STRING_ = 59, 
    COLON_ = 60, DOUBLE_COLON = 61, COMMA_ = 62, LPAREN = 63, RPAREN = 64, 
    LBRACKET = 65, RBRACKET = 66, OR = 67, AND = 68, EQUALS = 69, NOTEQUALS = 70, 
    LT = 71, LTEQ = 72, GT = 73, GTEQ = 74, PLUS_ = 75, MULT = 76, DIV = 77, 
    MOD = 78, REGEX = 79, NOT = 80, WHITESPACE = 81
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

