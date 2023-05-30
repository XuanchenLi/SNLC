#pragma once
#define IDNAME_MAX_LEN 10
enum TypeKind{ intTy,charTy,arrayTy,recordTy,boolTy };
enum IdKind { varKind,procKind,typeKind };
enum AccessKind { didr,indir };

	/*
		����������ű�ṹ
		����������������
		|name|name info|
		����������������
		|name|name info|
		����������������
		|name|name info|
		����������������
		|name|name info|
		����������������
		|name|name info|
		����������������
	*/
//��¼���͵�����
struct fieldChain {

	char idname[IDNAME_MAX_LEN];
	TypeKind unitType;
	unsigned int offset;
	struct fieldChain* next;
};

//�����ڲ���ʾ��
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

//�β���Ϣ��
struct ParamTable
{
	void* entry;				//�β��ڷ��ű��е�λ��
	struct ParamTable* next;		//ָ����һ������
};

//���ű���Ϣ��
typedef struct {


	struct typeIR* idtype;
	IdKind  kind;
	union {

		struct {
			AccessKind access;			//ֱ�ӻ��ӱ���
			int level;					//Level��ʾ�ñ�����������������/���̵Ĳ���
			int off;					//Off��ʾ�ñ������������������/���̵�ƫ����
		}VarAttr;						//������ʶ��

		struct {
			int level;
			ParamTable* param;			//������
			int code;
			int size;

		}More;							//��������ʶ��
	};

}AttributelR;

//���ű�symbTable
typedef struct symTable {
	char idname[IDNAME_MAX_LEN];
	AttributelR attrIR;
	struct symTable* next;
}symTable;
typedef struct symTable* symTablePtr;

const char* praseTypeKind(TypeKind kind);
const char* praseIdKind(IdKind kind);
const char* praseAccess(AccessKind kind);