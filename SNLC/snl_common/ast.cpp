#include "ast.h"

const char* ASTNodeKindName(NodeKind c)
{
    switch (c)
    {

#define DEF_TYPE(v) case NodeKind::v : return #v;
        NODETYPE_TABLE()
#undef DEF_TYPE
    default:
        return "UNDEFINED";
    }
}


const char* DecKindName(DecKind c)
{
    switch (c)
    {

#define DEF_TYPE(v) case DecKind::v : return #v;
        DECTYPE_TABLE()
#undef DEF_TYPE
    default:
        return "UNDEFINED";
    }
}

const char* StmtKindName(StmtKind c)
{
    switch (c)
    {

#define DEF_TYPE(v) case StmtKind::v : return #v;
        STMTTYPE_TABLE()
#undef DEF_TYPE
    default:
        return "UNDEFINED";
    }
}

const char* ExpKindName(EXPKind c)
{
    switch (c)
    {

#define DEF_TYPE(v) case EXPKind::v : return #v;
        EXPTYPE_TABLE()
#undef DEF_TYPE
    default:
        return "UNDEFINED";
    }
}