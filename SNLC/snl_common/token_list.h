#pragma once
#include "token.h"
#include "exceptions/type_error.h"
#include <vector>


class TokenList
{
private:
	std::vector<Token> tokens;
	int curIdx;

public:
	TokenList(std::vector<Token>& ts, int idx=0): tokens(ts), curIdx(idx) {}
	TokenList(): curIdx(0) {}
	inline void appendToken(const Token& t) { tokens.emplace_back(t); }
	inline bool hasNext() { return curIdx < tokens.size() - 1; }
	inline void moveNext() { curIdx++; }
	Token getCurToken();
	Token getNextToken();
	Token testAndGetCurToken(TokenType targetType);
};