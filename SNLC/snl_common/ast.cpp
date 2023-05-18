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

const char* ExpOpName(ASTOpType c)
{
    switch (c)
    {

#define DEF_TYPE(v) case ASTOpType::v : return #v;
        OPTYPE_TABLE()
#undef DEF_TYPE
    default:
        return "UNDEFINED";
    }
}


int GetOpPriority(ASTOpType op)
{
    switch (op)
    {
    case ASTOpType::LT:
        return 1;
        break;
    case ASTOpType::EQ:
        return 1;
        break;
    case ASTOpType::PLUS:
        return 2;
        break;
    case ASTOpType::MINUS:
        return 2;
        break;
    case ASTOpType::TIMES:
        return 3;
        break;
    case ASTOpType::OVER:
        return 3;
        break;
    case ASTOpType::STACK_END:
        return 0;
        break;
    default:
        throw std::exception("ERROR: Unknown AST Operator.");
        return -1;
        break;
    }
}


ASTNodeBase* GetASTLabelNode(ASTNodeKind k)
{
    ASTNodeBase* res = new ASTNodeBase();
    res->nodeKind = k;
    return res;
}
ASTDecNode* GetASTDecNode(ASTDecKind k)
{
    ASTDecNode* res = new ASTDecNode();
    res->nodeBase.nodeKind = ASTNodeKind::DEC_K;
    res->decKind = k;
    return res;
}
ASTStmtNode* GetASTStmtNode(ASTStmtKind k)
{
    ASTStmtNode* res = new ASTStmtNode();
    res->nodeBase.nodeKind = ASTNodeKind::STMT_K;
    res->stmtKind = k;
    return res;
}
ASTExpNode* GetASTExpNode(ASTEXPKind k)
{
    ASTExpNode* res = new ASTExpNode();
    res->nodeBase.nodeKind = ASTNodeKind::EXP_K;
    res->expKind = k;
    return res;
}
ASTDecNode* GetASTDecNode()
{
    ASTDecNode* res = new ASTDecNode();
    res->nodeBase.nodeKind = ASTNodeKind::DEC_K;
    return res;
}
ASTStmtNode* GetASTStmtNode()
{
    ASTStmtNode* res = new ASTStmtNode();
    res->nodeBase.nodeKind = ASTNodeKind::STMT_K;
    return res;
}
ASTExpNode* GetASTExpNode()
{
    ASTExpNode* res = new ASTExpNode();
    res->nodeBase.nodeKind = ASTNodeKind::EXP_K;
    return res;
}
ASTExpNode* GetASTExpOpNode(ASTOpType o)
{
    ASTExpNode* res = new ASTExpNode();
    res->nodeBase.nodeKind = ASTNodeKind::EXP_K;
    res->expKind = ASTEXPKind::OP_K;
    res->expAttr.op = o;
    return res;
}
ASTExpNode* GetASTExpConstNode()
{
    ASTExpNode* res = new ASTExpNode();
    res->nodeBase.nodeKind = ASTNodeKind::EXP_K;
    res->expKind = ASTEXPKind::CONST_K;
    return res;

}
ASTExpNode* GetASTExpVarNode()
{
    ASTExpNode* res = new ASTExpNode();
    res->nodeBase.nodeKind = ASTNodeKind::EXP_K;
    res->expKind = ASTEXPKind::ID_K;
    return res;

}
ASTOpType TokenType2OpType(TokenType t)
{
    switch (t)
    {
    case TokenType::LESS_THAN:
        return ASTOpType::LT;
    case TokenType::EQUAL:
        return ASTOpType::EQ;
    case TokenType::ADD:
        return ASTOpType::PLUS;
    case TokenType::MINUS:
        return ASTOpType::MINUS;
    case TokenType::MULTIPLY:
        return ASTOpType::TIMES;
    case TokenType::DIVIDE:
        return ASTOpType::OVER;
    case TokenType::BRACKET_OPEN:
        return ASTOpType::BRACKET_OPEN;
    case TokenType::BRACKET_CLOSE:
        return ASTOpType::BRACKET_CLOSE;
    default:
        throw std::exception(std::string("ERROR: Bad cast from: " 
                                         + std::string(TokenTypeName(t))
                                         + "to exp op type").c_str());
        break;
    }
}