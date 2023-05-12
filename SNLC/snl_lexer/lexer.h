#pragma once
#include "../snl_common/token_list.h"


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

};