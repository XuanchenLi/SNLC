#pragma once
#include "token.h"
#include "exceptions/"
#include <vector>


class TokenList
{
private:
	std::vector<Token> tokens;
	int curIdx;

public:
	TokenList(std::vector<Token>& ts, int idx=0): tokens(ts), curIdx(idx) {}
	TokenList(): curIdx(0) {}
	Token getCurToken();
	Token getNextToken();
	Token testAndGetCurToken();
	bool hasNext();
	void moveNext();
};