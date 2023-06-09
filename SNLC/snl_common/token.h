#pragma once
#include <string>
#include <iostream>

#define TYPE_TABLE()\
DEF_TYPE(IDENTIFIER) \
DEF_TYPE(INT) \
DEF_TYPE(PROGRAM) \
DEF_TYPE(TYPE) \
DEF_TYPE(ARRAY) \
DEF_TYPE(INTEGER) \
DEF_TYPE(CHAR) \
DEF_TYPE(RECORD) \
DEF_TYPE(OF) \
DEF_TYPE(VAR) \
DEF_TYPE(PROCEDURE) \
DEF_TYPE(BEGIN) \
DEF_TYPE(END) \
DEF_TYPE(IF) \
DEF_TYPE(THEN) \
DEF_TYPE(ELSE) \
DEF_TYPE(FI) \
DEF_TYPE(WHILE) \
DEF_TYPE(DO) \
DEF_TYPE(END_WHILE) \
DEF_TYPE(READ) \
DEF_TYPE(WRITE) \
DEF_TYPE(RETURN) \
DEF_TYPE(ADD) \
DEF_TYPE(MINUS) \
DEF_TYPE(MULTIPLY) \
DEF_TYPE(DIVIDE) \
DEF_TYPE(EQUAL) \
DEF_TYPE(ASSIGN) \
DEF_TYPE(COMMA) \
DEF_TYPE(SEMICOLON) \
DEF_TYPE(COLON) \
DEF_TYPE(LESS_THAN) \
DEF_TYPE(BRACKET_OPEN) \
DEF_TYPE(BRACKET_CLOSE) \
DEF_TYPE(SQUARE_BRACKET_OPEN) \
DEF_TYPE(SQUARE_BRACKET_CLOSE) \
DEF_TYPE(DOT) \
DEF_TYPE(DOT_DOT) \
DEF_TYPE(EOF_) \
DEF_TYPE(NOT_ID)


enum class TokenType
{
#define DEF_TYPE(v) v,
    TYPE_TABLE()
#undef DEF_TYPE
};


typedef struct Token
{
    Token() {};
    Token(int ln, int cn, std::string se, TokenType t):lineNum(ln), colNum(cn), sem(se), type(t){}
    int lineNum;
    int colNum;
    std::string sem;
    TokenType type;
public:
    friend std::ostream& operator<<(std::ostream&, Token&&);
}Token;


const char* TokenTypeName(TokenType);
TokenType getID(const std::string& str);