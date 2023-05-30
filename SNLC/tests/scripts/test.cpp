#include "test.h"

void testLexer(const char* path)
{
    FileReader fileReader(path);
    Lexer lexer;
    TokenList tokenList = lexer.genTokens(fileReader);

    std::cout << tokenList.getCurToken() << std::endl;
    while (tokenList.hasNext())
    {
        std::cout << tokenList.getNextToken() << std::endl;
        tokenList.moveNext();
    }
}

void testParser(const char* path)
{
    FileReader fileReader(path);
    Lexer lexer;
    TokenList tokenList = lexer.genTokens(fileReader);
    Parser parser;
    try
    {
        ASTNodeBase* rt = parser.parse(tokenList);
        parser.printTree(rt, 0);
    }
    catch (std::runtime_error e)
    {
        std::cout << e.what() << std::endl;
    }
    catch (std::exception e)
    {
        std::cout<<e.what()<<std::endl;
    }
    
}
