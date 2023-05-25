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
    case ASTOpType::BRACKET_OPEN:
        return 0;
        break;
    default:
        throw std::exception("ERROR: Unknown AST Operator.");
        return -1;
        break;
    }
}

const char* ExpVarTypeName(ASTVarType c)
{
    switch (c)
    {

#define DEF_TYPE(v) case ASTVarType::v : return #v;
        VARTYPE_TABLE()
#undef DEF_TYPE
    default:
        return "UNDEFINED";
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
    res->decAttr.procAttr.paramType = ASTParamType::NOT_PARAM;
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

void printDecK(std::ostream& os, const ASTNodeBase& t)
{
    const ASTDecNode* tmp = (const ASTDecNode*)&t;
    if (tmp->decAttr.procAttr.paramType != ASTParamType::NOT_PARAM)
    {
        if (tmp->decAttr.procAttr.paramType == ASTParamType::VAL_PARAM_TYPE)
        {
            os << "value param: ";
        }
        else
        {
            os << "variable param: ";
        }
    }
    if (tmp->decKind != ASTDecKind::ARRAY_K && tmp->decKind != ASTDecKind::PROC_DEC_K)
    {
        os << DecKindName(tmp->decKind) << " ";
        for (auto i : tmp->nodeBase.names)
        {
            os << i << " ";
        }
    }
    else if (tmp->decKind == ASTDecKind::PROC_DEC_K)
    {
        os << DecKindName(tmp->decKind) << " ";
        os << t.names[0];
    }
    else
    {
        os << DecKindName(tmp->decKind) << " ";
        os << tmp->nodeBase.names[0] + "[" + std::to_string(tmp->decAttr.arrayAttr.low) + ".."
            + std::to_string(tmp->decAttr.arrayAttr.up) + "] of " + DecKindName(tmp->decAttr.arrayAttr.itemType);
    }
}
void printStmtK(std::ostream& os, const ASTNodeBase& t)
{
    const ASTStmtNode* tmp = (const ASTStmtNode*)&t;
    os << StmtKindName(tmp->stmtKind) << " ";
    for (auto i : tmp->nodeBase.names)
    {
        os << i << " ";
    }
}
void printExpK(std::ostream& os, const ASTNodeBase& t)
{
    const ASTExpNode* tmp = (const ASTExpNode*)&t;
    os << ExpKindName(tmp->expKind)<<" ";
    if (tmp->expKind == ASTEXPKind::CONST_K)
    {
        os << std::to_string(tmp->expAttr.val) << " ";
    }
    else if (tmp->expKind == ASTEXPKind::ID_K)
    {
        os << ExpVarTypeName(tmp->expAttr.varType) << " ";
        for (auto i : tmp->nodeBase.names)
        {
            os << i << " ";
        }
    }
    else
    {
        os << ExpOpName(tmp->expAttr.op) << " ";

    }
}
std::ostream& operator<<(std::ostream& os, const ASTNodeBase& t)
{
    os << ASTNodeKindName(t.nodeKind) << " ";
    switch (t.nodeKind)
    {
    case ASTNodeKind::PRO_K:
        break;
    case ASTNodeKind::PHEAD_K:
        os <<t.names[0];
        break;
    case ASTNodeKind::TYPE_K:
        break;
    case ASTNodeKind::VAR_K:
        break;
    case ASTNodeKind::PROC_K:
        break;
    case ASTNodeKind::STM_L_K:
        break;
    case ASTNodeKind::DEC_K:
        printDecK(os, t);
        break;
    case ASTNodeKind::STMT_K:
        printStmtK(os, t);
        break;
    case ASTNodeKind::EXP_K:
        printExpK(os, t);
        break;
    default:
        break;
    }
    os << "\n";
    return os;
}