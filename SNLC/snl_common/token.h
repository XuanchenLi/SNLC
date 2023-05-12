#pragma once
#include <string>

#define TYPE_TABLE()\
DEF_TYPE(Identifier) \
DEF_TYPE(Int) \
DEF_TYPE(Program) \
DEF_TYPE(Type) \
DEF_TYPE(Array) \
DEF_TYPE(Integer) \
DEF_TYPE(Char) \
DEF_TYPE(Record) \
DEF_TYPE(Of) \
DEF_TYPE(Var) \
DEF_TYPE(Procedure) \
DEF_TYPE(Begin) \
DEF_TYPE(End) \
DEF_TYPE(If) \
DEF_TYPE(Then) \
DEF_TYPE(Else) \
DEF_TYPE(Fi) \
DEF_TYPE(While) \
DEF_TYPE(Do) \
DEF_TYPE(EndWhile) \
DEF_TYPE(Read) \
DEF_TYPE(Write) \
DEF_TYPE(Return) \
DEF_TYPE(Add) \
DEF_TYPE(Minus) \
DEF_TYPE(Multiply) \
DEF_TYPE(Divide) \
DEF_TYPE(Equal) \
DEF_TYPE(Assign) \
DEF_TYPE(Comma) \
DEF_TYPE(Semicolon) \
DEF_TYPE(LessThan) \
DEF_TYPE(BracketOpen) \
DEF_TYPE(BrackerClose) \
DEF_TYPE(SquareBracketOpen) \
DEF_TYPE(SquareBracketClose) \
DEF_TYPE(Dot) \
DEF_TYPE(DotDot) \
DEF_TYPE(Eof)


enum TokenType
{
#define DEF_TYPE(v) v,
    TYPE_TABLE()
#undef DEF_TYPE
};


typedef struct Token
{
    int lineNum;
    int colNum;
    std::string sem;
    TokenType type;
}Token;


const char* TypeName(TokenType);