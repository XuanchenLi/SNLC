#pragma once
#include "symTable.h"
#include"../snl_grammar/parser.h"
#include"../snl_common/ast.h"
#include <vector>
#include<map>
#include<string>

class Table {
	std::map<std::string,TypeIR*> typetabel;      //类型表
	std::vector<std::vector<symTablePtr>> scope;		//分程序表
	std::vector<std::vector<symTablePtr>> tables;
	unsigned int Level;					//子过程深度
	unsigned long int Access;                //符号表入口
	bool TraceTable;
public:
	Table() : Level(0),Access(0),TraceTable(true) {};
	~Table();
public:
	void Analyze(ASTNodeBase* currentP);
protected:
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
	void CreatTable();												//建一个符号表
	void DestroyTable();											//撤销一个符号表
	bool Enter(char* Id, AttributelR* AttribP, symTablePtr Entry);	//登记标识符和属性到符号表
	bool FindEntry(char* id, bool diraction, symTablePtr Entry);				//号表中查找标识符
	void PrintSymbTabl();														//打印符号表
private:
	bool SearchoneTable(char* id, int currentLevel, symTablePtr Entry);			//号表中查找标识符


};
