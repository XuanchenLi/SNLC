#include <iostream>
#include "snl_lexer/lexer.h"
#include "snl_grammar/parser.h"


int main()
{
    FileReader fileReader("./tests/test1.snl");
    Lexer lexer;
    TokenList tokenList = lexer.genTokens(fileReader);

    std::cout << tokenList.getCurToken() << std::endl;
    while (tokenList.hasNext())
    {
        std::cout << tokenList.getNextToken() << std::endl;
        tokenList.moveNext();
    }
}

