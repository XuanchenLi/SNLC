#pragma once
#include <string>
#include <vector>
#include "token.h"

// AST节点类型枚举
#define NODETYPE_TABLE()\
DEF_TYPE(PRO_K) \
DEF_TYPE(PHEAD_K) \
DEF_TYPE(TYPE_K) \
DEF_TYPE(VAR_K) \
DEF_TYPE(PROC_K) \
DEF_TYPE(STM_L_K) \
DEF_TYPE(DEC_K) \
DEF_TYPE(STMT_K) \
DEF_TYPE(EXP_K)

enum class ASTNodeKind
{
#define DEF_TYPE(v) v,
    NODETYPE_TABLE()
#undef DEF_TYPE
};

const char* ASTNodeKindName(ASTNodeKind);

// DEC_K类型节点的具体类型枚举
#define DECTYPE_TABLE()\
DEF_TYPE(ARRAY_K) \
DEF_TYPE(CHAR_K) \
DEF_TYPE(INTEGER_K) \
DEF_TYPE(RECORD_K) \
DEF_TYPE(ID_K) \
DEF_TYPE(PROC_DEC_K) \
DEF_TYPE(PROC_V_K)

enum class ASTDecKind
{
#define DEF_TYPE(v) v,
    DECTYPE_TABLE()
#undef DEF_TYPE
};

const char* DecKindName(ASTDecKind);

// STMT_K类型节点的具体类型枚举
#define STMTTYPE_TABLE()\
DEF_TYPE(IF_K) \
DEF_TYPE(WHILE_K) \
DEF_TYPE(ASSIGN_K) \
DEF_TYPE(READ_K) \
DEF_TYPE(WRITE_K) \
DEF_TYPE(CALL_K) \
DEF_TYPE(RETURN_K)

enum class ASTStmtKind
{
#define DEF_TYPE(v) v,
    STMTTYPE_TABLE()
#undef DEF_TYPE
};


const char* StmtKindName(ASTStmtKind);

// EXP_K类型节点的具体类型枚举
#define EXPTYPE_TABLE()\
DEF_TYPE(OP_K) \
DEF_TYPE(CONST_K) \
DEF_TYPE(ID_K)

enum class ASTEXPKind
{
#define DEF_TYPE(v) v,
    EXPTYPE_TABLE()
#undef DEF_TYPE
};

const char* ExpKindName(ASTEXPKind);


// 语法树表达式节点的运算符枚举
#define OPTYPE_TABLE()\
DEF_TYPE(LT) \
DEF_TYPE(EQ) \
DEF_TYPE(PLUS) \
DEF_TYPE(MINUS) \
DEF_TYPE(TIMES) \
DEF_TYPE(OVER) \
DEF_TYPE(BRACKET_OPEN) \
DEF_TYPE(BRACKET_CLOSE) \
DEF_TYPE(STACK_END)


enum class ASTOpType
{
#define DEF_TYPE(v) v,
    OPTYPE_TABLE()
#undef DEF_TYPE
};

const char* ExpOpName(ASTOpType);

int GetOpPriority(ASTOpType);


// 语法树表达式节点的变量类型枚举
#define VARTYPE_TABLE()\
DEF_TYPE(ID_V) \
DEF_TYPE(ARRAY_MEMB_V) \
DEF_TYPE(FIELD_MEMB_V)


enum class ASTVarType
{
#define DEF_TYPE(v) v,
    VARTYPE_TABLE()
#undef DEF_TYPE
};
const char* ExpVarTypeName(ASTVarType);

// 语法树表达式节点的检查类型
#define EXP_CHECK_TYPE_TABLE()\
DEF_TYPE(VOID) \
DEF_TYPE(INTEGER) \
DEF_TYPE(BOOLEAN)


enum class ASTEXPCheckType
{
#define DEF_TYPE(v) v,
    EXP_CHECK_TYPE_TABLE()
#undef DEF_TYPE
};


// 语法树过程节点的参数类型，值参和变参
#define PARAM_TYPE_TABLE()\
DEF_TYPE(NOT_PARAM) \
DEF_TYPE(VAL_PARAM_TYPE) \
DEF_TYPE(VAR_PARAM_TYPE)


enum class ASTParamType
{
#define DEF_TYPE(v) v,
    PARAM_TYPE_TABLE()
#undef DEF_TYPE
};


//  表达式的属性
typedef struct ASTExpAttr
{
    ASTOpType op;   // 操作符类型
    int val;    // 数字节点有效，记录数值
    ASTVarType varType;     // 变量类型
    ASTEXPCheckType type;   // 表达式检查类型
}ASTExpAttr;

//  过程参数声明的属性
typedef struct ASTProcAttr
{
    ASTParamType paramType;     // 过程参数类型
}ASTProcAttr;


//  数组声明的属性
typedef struct ASTArrayAttr
{
    int low;
    int up;
    ASTDecKind itemType;    // 数组成员类型，只能是CHAR、INTEGER
}ASTArrayAttr;


#define MAXCHILDREN 3
//  语法树基节点，包含各种节点都有的属性
typedef struct ASTNodeBase
{
    ASTNodeBase() :lineNum(0), sibling(nullptr) { std::memset(child, 0, sizeof(child)); }
    int lineNum;
    ASTNodeKind nodeKind;
    ASTNodeBase* sibling;
    ASTNodeBase* child[MAXCHILDREN];
    std::vector<std::string> names;     //标识符名字
    std::vector<unsigned long int> tablePtrs;   //各个标识符符号表入口
    friend std::ostream& operator<<(std::ostream&, const ASTNodeBase&);
}ASTNodeBase;


//  语法树派生节点：声明节点
//  nodeKind应为DEC_K
typedef struct ASTDecNode
{
    ASTNodeBase nodeBase;
    ASTDecKind decKind;     //具体声明类型
    std::string typeName;   //类型为类型标识符表示时有效
    union
    {
        ASTArrayAttr arrayAttr;     //声明数组时有效（decKind为ARRAY_K）
        ASTProcAttr procAttr;   //声明过程参数时有效
    }decAttr;

}ASTDecNode;


//  语法树派生节点：语句节点
//  nodeKind应为STMT_K
typedef struct ASTStmtNode
{
    ASTNodeBase nodeBase;
    ASTStmtKind stmtKind;       //具体语句类型
}ASTStmtNode;


//  语法树派生节点：表达式节点
//  nodeKind应为EXP_K
typedef struct ASTExpNode
{
    ASTNodeBase nodeBase;
    ASTEXPKind expKind;     //具体表达式类型
    ASTExpAttr expAttr;
}ASTExpNode;


ASTNodeBase* GetASTLabelNode(ASTNodeKind);
ASTDecNode* GetASTDecNode(ASTDecKind);
ASTStmtNode* GetASTStmtNode(ASTStmtKind);
ASTExpNode* GetASTExpNode(ASTEXPKind);
ASTExpNode* GetASTExpOpNode(ASTOpType);
ASTDecNode* GetASTDecNode();
ASTStmtNode* GetASTStmtNode();
ASTExpNode* GetASTExpNode();
ASTExpNode* GetASTExpConstNode();
ASTExpNode* GetASTExpVarNode();
ASTOpType TokenType2OpType(TokenType);
