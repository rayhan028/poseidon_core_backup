
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
    Union_ = 28, Except_ = 29, RemoveNode_ = 30, RemoveRelationship_ = 31, 
    DetachNode_ = 32, Algorithm_ = 33, Case_ = 34, IntType_ = 35, Uint64Type_ = 36, 
    DoubleType_ = 37, StringType_ = 38, DateType_ = 39, NodeResultType_ = 40, 
    RshipResultType_ = 41, Count_ = 42, Sum_ = 43, Avg_ = 44, Min_ = 45, 
    Max_ = 46, UDF_ = 47, BUILTIN_ = 48, InExpandDir_ = 49, OutExpandDir_ = 50, 
    TupleMode_ = 51, ResultSetMode_ = 52, FromDir_ = 53, ToDir_ = 54, AllDir_ = 55, 
    DescOrder_ = 56, AscOrder_ = 57, INTEGER = 58, FLOAT = 59, Identifier_ = 60, 
    Var = 61, NAME_ = 62, STRING_ = 63, COLON_ = 64, DOUBLE_COLON = 65, 
    COMMA_ = 66, LPAREN = 67, RPAREN = 68, LBRACKET = 69, RBRACKET = 70, 
    OR = 71, AND = 72, EQUALS = 73, NOTEQUALS = 74, LT = 75, LTEQ = 76, 
    GT = 77, GTEQ = 78, PLUS_ = 79, MULT = 80, DIV = 81, MOD = 82, REGEX = 83, 
    NOT = 84, WHITESPACE = 85, COMMENT = 86
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

