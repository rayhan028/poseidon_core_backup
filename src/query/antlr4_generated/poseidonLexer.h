
// Generated from src/query/parser/poseidon.g4 by ANTLR 4.13.1

#pragma once


#include "antlr4-runtime.h"




class  poseidonLexer : public antlr4::Lexer {
public:
  enum {
    T__0 = 1, T__1 = 2, T__2 = 3, T__3 = 4, T__4 = 5, T__5 = 6, T__6 = 7, 
    Filter_ = 8, Nodescan_ = 9, Rshipscan_ = 10, Indexscan_ = 11, NodeById_ = 12, 
    Match_ = 13, Project_ = 14, Limit_ = 15, CrossJoin_ = 16, HashJoin_ = 17, 
    Exists_ = 18, NotExists_ = 19, NLJoin_ = 20, LeftOuterJoin_ = 21, Expand_ = 22, 
    ForeachRelationship_ = 23, Aggregate_ = 24, GroupBy_ = 25, Sort_ = 26, 
    Distinct_ = 27, Create_ = 28, Update_ = 29, Union_ = 30, Except_ = 31, 
    RemoveNode_ = 32, RemoveRelationship_ = 33, DetachNode_ = 34, Algorithm_ = 35, 
    Case_ = 36, IntType_ = 37, Uint64Type_ = 38, DoubleType_ = 39, StringType_ = 40, 
    DateType_ = 41, NodeResultType_ = 42, RshipResultType_ = 43, Count_ = 44, 
    Sum_ = 45, Avg_ = 46, Min_ = 47, Max_ = 48, UDF_ = 49, BUILTIN_ = 50, 
    InExpandDir_ = 51, OutExpandDir_ = 52, TupleMode_ = 53, ResultSetMode_ = 54, 
    FromDir_ = 55, ToDir_ = 56, AllDir_ = 57, DescOrder_ = 58, AscOrder_ = 59, 
    INTEGER = 60, LONG_INTEGER = 61, FLOAT = 62, Identifier_ = 63, Var = 64, 
    NAME_ = 65, STRING_ = 66, COLON_ = 67, DOUBLE_COLON = 68, COMMA_ = 69, 
    LPAREN = 70, RPAREN = 71, LBRACKET = 72, RBRACKET = 73, OR = 74, AND = 75, 
    EQUALS = 76, NOTEQUALS = 77, LT = 78, LTEQ = 79, GT = 80, GTEQ = 81, 
    PLUS_ = 82, MULT = 83, DIV = 84, MOD = 85, REGEX = 86, NOT = 87, WHITESPACE = 88, 
    COMMENT = 89
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

