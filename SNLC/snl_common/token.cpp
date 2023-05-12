#include "token.h"


const char* TypeName(TokenType c)
{
    switch (c)
    {

#define DEF_TYPE(v) case TokenType::v : return #v;
        TYPE_TABLE()
#undef DEF_TYPE
    default:
        return "UNDEFINED";
    }
}

TokenType getTokenType(const std::string& str)
{
    if (str == "program")   return TokenType::PROGRAM;
    if (str == "type")   return TokenType::TYPE;
    if (str == "array")   return TokenType::ARRAY;
    if (str == "integer")   return TokenType::INTEGER;
    if (str == "char")   return TokenType::CHAR;
    if (str == "record")   return TokenType::RECORD;
    if (str == "of")   return TokenType::OF;
    if (str == "var")   return TokenType::VAR;
    if (str == "procedure")   return TokenType::PROCEDURE;
    if (str == "begin")   return TokenType::BEGIN;
    if (str == "end")   return TokenType::END;
    if (str == "if")   return TokenType::IF;
    if (str == "then")   return TokenType::THEN;
    if (str == "else")   return TokenType::ELSE;
    if (str == "fi")   return TokenType::FI;
    if (str == "while")   return TokenType::WHILE;
    if (str == "do")   return TokenType::DO;
    if (str == "endwh")   return TokenType::END_WHILE;
    if (str == "read")   return TokenType::READ;
    if (str == "write")   return TokenType::WRITE;
    if (str == "return")   return TokenType::RETURN;
    if (str == "+")   return TokenType::ADD;
    if (str == "-")   return TokenType::MINUS;
    if (str == "*")   return TokenType::MULTIPLY;
    if (str == "/")   return TokenType::DIVIDE;
    if (str == "=")   return TokenType::EQUAL;
    if (str == ":=")   return TokenType::ASSIGN;
    if (str == ",")   return TokenType::COMMA;
    if (str == ";")   return TokenType::SEMICOLON;
    if (str == "<")   return TokenType::LESS_THAN;
    if (str == "[")   return TokenType::SQUARE_BRACKET_OPEN;
    if (str == "]")   return TokenType::SQUARE_BRACKET_CLOSE;
    if (str == "(")   return TokenType::BRACKET_OPEN;
    if (str == ")")   return TokenType::BRACKET_CLOSE;
    if (str == ".")   return TokenType::DOT;
    if (str == "..")   return TokenType::DOT_DOT;
    return TokenType::IDENTIFIER;
}