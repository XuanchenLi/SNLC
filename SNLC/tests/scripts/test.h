#pragma once
#include <iostream>
#include "../../snl_lexer/lexer.h"
#include "../../snl_grammar/parser.h"
#include "../../snl_semantic/table.h"
void testLexer(const char* path);

void testParser(const char* path);

void testTable(const char* path);