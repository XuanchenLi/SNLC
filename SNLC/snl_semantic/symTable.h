#pragma once
#define IDNAME_MAX_LEN 10
enum TypeKind{ intTy,charTy,arrayTy,recordTy,boolTy };
enum IdKind { varKind,procKind,typeKind };
enum AccessKind { didr,indir };

	/*
		语义分析符号表结构
		————————
		|name|name info|
		————————
		|name|name info|
		————————
		|name|name info|
		————————
		|name|name info|
		————————
		|name|name info|
		————————
	*/
//记录类型的链域
struct fieldChain {

	char idname[IDNAME_MAX_LEN];
	TypeKind unitType;
	unsigned int offset;
	struct fieldChain* next;
};

//类型内部表示：
typedef struct typeIR {
	int size;
	TypeKind kind;
	union {
		struct {
			struct typeIR* elemTy;
			struct typeIR* indexTy;
			
		}ArrayAttr;
		fieldChain* body;
	}More;

}TypeIR;

//形参信息表
struct ParamTable
{
	void* entry;				//形参在符号表中的位置
	struct ParamTable* next;		//指向下一个表项
};

//符号表信息项
typedef struct {


	struct typeIR* idtype;
	IdKind  kind;
	union {

		struct {
			AccessKind access;			//直接或间接变量
			int level;					//Level表示该变量声明所在主程序/过程的层数
			int off;					//Off表示该变量相对它所在主程序/过程的偏移量
		}VarAttr;						//变量标识符

		struct {
			int level;
			ParamTable* param;			//参数表
			int code;
			int size;

		}More;							//过程名标识符
	};

}AttributelR;



//符号表symbTable
typedef struct symTable {
	char idname[IDNAME_MAX_LEN];
	AttributelR attrIR;
	struct symTable* next;
}symTable;

typedef struct symTable* symTablePtr;

const char* praseTypeKind(TypeKind kind);
const char* praseIdKind(IdKind kind);
const char* praseAccess(AccessKind kind);