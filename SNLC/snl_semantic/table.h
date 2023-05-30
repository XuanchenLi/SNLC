#pragma once
#include "symTable.h"
#include"../snl_grammar/parser.h"
#include"../snl_common/ast.h"
#include <vector>
#include<map>
#include<string>
#include <stdexcept>
#define INIT_OFF 7;
class Table {
	std::map<std::string,TypeIR*> typetabel;      //���ͱ�
	std::vector<std::vector<symTablePtr>> scope;		//�ֳ����
	std::vector<std::vector<symTablePtr>> tables;
	unsigned int Level;					//�ӹ������
	unsigned long int Access;                //���ű����
	bool TraceTable;
public:
	Table(bool access=true) : Level(0),Access(0),TraceTable(access) {this->initialize();};
	~Table();
public:
	void Analyze(ASTNodeBase* currentP);
protected:
	void analyze(ASTNodeBase* currentP);
	TypeIR* TypeProcess(ASTNodeBase* currentP);
	TypeIR* arrayType(ASTDecNode*);
	TypeIR* nameType(ASTNodeBase* currentP) { return nullptr; };
	TypeIR* recordType(ASTNodeBase* currentP) { return nullptr; };
	void initialize(void);
	void TypeDecPart(ASTNodeBase* currentP);
	void VarDecList(ASTNodeBase* currentP);
	void ProcDecPart(ASTNodeBase* currentP);
	symTablePtr HeadProcess(ASTNodeBase* currentP);
	ParamTable *ParaDecList(ASTNodeBase* currentP);
	void Body(ASTNodeBase* currentP);
	void statement(ASTNodeBase* currentP);
	TypeIR* arrayVar(ASTNodeBase* currentP);
	TypeIR* Expr(ASTNodeBase* currentP);			/*, AccessKind* Ekind*/
	void assignstatement(ASTStmtNode* currentP);
	void ifstatment(ASTStmtNode* currentP);
	void whilestatement(ASTStmtNode* currentP);
	void callstatement(ASTStmtNode* currentP);
	void paramstatemnt(ParamTable*,int level,ASTNodeBase* currentP);
	void writestatemen(ASTNodeBase* currentP);
	void readstatemen(ASTNodeBase* currentP);

public:
	void CreatTable();												//��һ�����ű�
	void DestroyTable();											//����һ�����ű�
	bool Enter(char* Id, AttributelR* AttribP, symTablePtr& Entry);	//�ǼǱ�ʶ�������Ե����ű�
	bool FindEntry(char* id, bool diraction, symTablePtr& Entry);				//�ű��в��ұ�ʶ��												//��ӡ���ű�
private:
	void PrintSymbTabl();
	bool SearchoneTable(char* id, int currentLevel, symTablePtr& Entry);			//�ű��в��ұ�ʶ��
	void PrintProcDecSym(symTablePtr& Entry);
	void PrintVarDecSym(symTablePtr& Entry);
	void PrintHead();

};
