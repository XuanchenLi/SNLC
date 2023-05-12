#include "token.h"


const char* TypeName(TokenType c)
{
    switch (c)
    {

#define DEF_TYPE(v) case TokenType::v : return #v;
        TYPE_TABLE()
#undef DEF_TYPE
    default:
        return "UNDEFINED";
    }
}
