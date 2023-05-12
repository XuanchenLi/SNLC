#pragma once
#include "../snl_common/token_list.h"
#include "../snl_common/file_reader.h"
#include <vector>


enum LexState
{
	START,
	IN_ID,
	IN_NUM,
	IN_COMMENT,
	IN_ASSIGN,
	IN_DOT
};



class Lexer
{
public:
	std::vector<Token> genTokens(FileReader& fScanner);
};