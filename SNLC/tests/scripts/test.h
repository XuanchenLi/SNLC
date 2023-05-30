#pragma once
#include <iostream>
#include "../../snl_lexer/lexer.h"
#include "../../snl_grammar/parser.h"
#include "../../snl_semantic/table.h"
#include"../../snl_mid/optimize.h"
void testLexer(const char* path);

void testParser(const char* path);

void testTable(const char* path);

void testMid(const char* path);