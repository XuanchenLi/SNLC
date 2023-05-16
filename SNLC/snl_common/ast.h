#pragma once
#include <string>
#include <vector>

// AST节点类型枚举
#define NODETYPE_TABLE()\
DEF_TYPE(PRO_K) \
DEF_TYPE(PHEAD_K) \
DEF_TYPE(TYPE_K) \
DEF_TYPE(VAR_K) \
DEF_TYPE(PROC_DEC_K) \
DEF_TYPE(STM_L_K) \
DEF_TYPE(DEC_K) \
DEF_TYPE(STMT_K) \
DEF_TYPE(EXP_K)

enum class NodeKind
{
#define DEF_TYPE(v) v,
    NODETYPE_TABLE()
#undef DEF_TYPE
};

const char* ASTNodeKindName(NodeKind);

// DEC_K类型节点的具体类型枚举
#define DECTYPE_TABLE()\
DEF_TYPE(ARRAY_K) \
DEF_TYPE(CHAR_K) \
DEF_TYPE(INTEGER_K) \
DEF_TYPE(RECORD_K) \
DEF_TYPE(ID_K)

enum class DecKind
{
#define DEF_TYPE(v) v,
    DECTYPE_TABLE()
#undef DEF_TYPE
};

const char* DecKindName(DecKind);

// STMT_K类型节点的具体类型枚举
#define STMTTYPE_TABLE()\
DEF_TYPE(IF_K) \
DEF_TYPE(WHILE_K) \
DEF_TYPE(ASSIGN_K) \
DEF_TYPE(READ_K) \
DEF_TYPE(WRITE_K) \
DEF_TYPE(CALL_K) \
DEF_TYPE(RETURN_K)

enum class StmtKind
{
#define DEF_TYPE(v) v,
    STMTTYPE_TABLE()
#undef DEF_TYPE
};

const char* StmtKindName(StmtKind);

// EXP_K类型节点的具体类型枚举
#define EXPTYPE_TABLE()\
DEF_TYPE(OP_K) \
DEF_TYPE(CONST_K) \
DEF_TYPE(ID_K)

enum class EXPKind
{
#define DEF_TYPE(v) v,
    EXPTYPE_TABLE()
#undef DEF_TYPE
};

const char* ExpKindName(EXPKind);


typedef struct ASTNodeBase
{
    int lineNum;
    NodeKind nodeKind;
}ASTNodeBase;

