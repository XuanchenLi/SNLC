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