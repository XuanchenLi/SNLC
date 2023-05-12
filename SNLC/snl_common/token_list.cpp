#include "token_list.h"


Token TokenList::getCurToken()
{
	if (tokens.empty() || curIdx >= tokens.size())
	{
		throw std::out_of_range("Token序列越界！");
	}
	return tokens[curIdx];
}

Token TokenList::getNextToken()
{
	int nextIdx = curIdx + 1;
	if (nextIdx >= tokens.size())
	{
		throw std::out_of_range("Token序列越界！");
	}
	return tokens[nextIdx];
}

Token TokenList::testAndGetCurToken(TokenType targetType)
{
	Token curToken = getCurToken();
	if (curToken.type == targetType) return curToken;
	else throw TypeError(curToken.lineNum, curToken.colNum, targetType, curToken.type);
}

