
// Generated from src/query/parser/poseidon.g4 by ANTLR 4.13.1

#pragma once


#include "antlr4-runtime.h"




class  poseidonLexer : public antlr4::Lexer {
public:
  enum {
    T__0 = 1, T__1 = 2, T__2 = 3, T__3 = 4, T__4 = 5, T__5 = 6, T__6 = 7, 
    Filter_ = 8, Nodescan_ = 9, Rshipscan_ = 10, Indexscan_ = 11, Match_ = 12, 
    Project_ = 13, Limit_ = 14, CrossJoin_ = 15, HashJoin_ = 16, Exists_ = 17, 
    NotExists_ = 18, NLJoin_ = 19, LeftOuterJoin_ = 20, Expand_ = 21, ForeachRelationship_ = 22, 
    Aggregate_ = 23, GroupBy_ = 24, Sort_ = 25, Distinct_ = 26, Create_ = 27, 
    Update_ = 28, Union_ = 29, Except_ = 30, RemoveNode_ = 31, RemoveRelationship_ = 32, 
    DetachNode_ = 33, Algorithm_ = 34, Case_ = 35, IntType_ = 36, Uint64Type_ = 37, 
    DoubleType_ = 38, StringType_ = 39, DateType_ = 40, NodeResultType_ = 41, 
    RshipResultType_ = 42, Count_ = 43, Sum_ = 44, Avg_ = 45, Min_ = 46, 
    Max_ = 47, UDF_ = 48, BUILTIN_ = 49, InExpandDir_ = 50, OutExpandDir_ = 51, 
    TupleMode_ = 52, ResultSetMode_ = 53, FromDir_ = 54, ToDir_ = 55, AllDir_ = 56, 
    DescOrder_ = 57, AscOrder_ = 58, INTEGER = 59, FLOAT = 60, Identifier_ = 61, 
    Var = 62, NAME_ = 63, STRING_ = 64, COLON_ = 65, DOUBLE_COLON = 66, 
    COMMA_ = 67, LPAREN = 68, RPAREN = 69, LBRACKET = 70, RBRACKET = 71, 
    OR = 72, AND = 73, EQUALS = 74, NOTEQUALS = 75, LT = 76, LTEQ = 77, 
    GT = 78, GTEQ = 79, PLUS_ = 80, MULT = 81, DIV = 82, MOD = 83, REGEX = 84, 
    NOT = 85, WHITESPACE = 86, COMMENT = 87
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

