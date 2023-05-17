#include "ast.h"

const char* ASTNodeKindName(ASTNodeKind c)
{
    switch (c)
    {

#define DEF_TYPE(v) case ASTNodeKind::v : return #v;
        NODETYPE_TABLE()
#undef DEF_TYPE
    default:
        return "UNDEFINED";
    }
}


const char* DecKindName(ASTDecKind c)
{
    switch (c)
    {

#define DEF_TYPE(v) case ASTDecKind::v : return #v;
        DECTYPE_TABLE()
#undef DEF_TYPE
    default:
        return "UNDEFINED";
    }
}

const char* StmtKindName(ASTStmtKind c)
{
    switch (c)
    {

#define DEF_TYPE(v) case ASTStmtKind::v : return #v;
        STMTTYPE_TABLE()
#undef DEF_TYPE
    default:
        return "UNDEFINED";
    }
}

const char* ExpKindName(ASTEXPKind c)
{
    switch (c)
    {

#define DEF_TYPE(v) case ASTEXPKind::v : return #v;
        EXPTYPE_TABLE()
#undef DEF_TYPE
    default:
        return "UNDEFINED";
    }
}