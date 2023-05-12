#pragma once
#include <iostream>
#include <string>
#include "../token.h"

class TypeError :public std::exception
{
public:
	virtual ~TypeError() noexcept{}
	virtual const char* what();
	TypeError(const int line, 
			  const int col,
			  TokenType e, 
			  TokenType g):lineNum(line), colNum(col), expect(e), got(g) {}
private:
	int lineNum, colNum;
	TokenType expect, got;
};