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
    ASTNodeBase* rt = parser.parse(tokenList);
    try
    {
        ASTNodeBase* rt = parser.parse(tokenList);
        parser.printTree(rt, 0);
    }
    catch (std::exception e)
    {
        std::cout<<e.what()<<std::endl;
    }
    
}

void testTable(const char* path)
{
    FileReader fileReader(path);
    Lexer lexer;
    TokenList tokenList = lexer.genTokens(fileReader);
    Parser parser;
    ASTNodeBase* rt = parser.parse(tokenList);
    try
    {
        ASTNodeBase* rt = parser.parse(tokenList);
        parser.printTree(rt, 0);
    }
    catch (std::exception e)
    {
        std::cout << e.what() << std::endl;
    }

    try
    {
        Table table(true);
        table.Analyze(rt);
    }
    catch (std::exception e)
    {
        std::cout << "SEMANTIC ERROR:" << e.what() << std::endl;
    }

}
