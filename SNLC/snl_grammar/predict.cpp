#include "predict.h"


const char* NonTerminalTypeName(NonTerminalType c)
{
    switch (c)
    {

#define DEF_TYPE(v) case NonTerminalType::v : return #v;
        NONTERMINAL_TABLE()
#undef DEF_TYPE
    default:
        return "UNDEFINED";
    }
}

std::pair<PredictTableKey, std::function<void()>> GetPredictItem(PredictTableKey key, std::function<void()> f)
{
    return std::pair<PredictTableKey, std::function<void()>>(key, f);
}


const char* SymbolTypeName(SymbolType c)
{
    switch (c)
    {

#define DEF_TYPE(v) case SymbolType::v : return #v;
        SYMBOLTYPE_TABLE()
#undef DEF_TYPE
    default:
        return "UNDEFINED";
    }
}


SymbolStackItem GetTerminal(TokenType t)
{
    SymbolStackItem res;
    res.symbolType = SymbolType::TERMINAL;
    res.symbolName.tokenType = t;
    return res;
}
SymbolStackItem GetNonTerminal(NonTerminalType t)
{
    SymbolStackItem res;
    res.symbolType = SymbolType::NON_TERMINAL;
    res.symbolName.nonTerminalType= t;
    return res;
}