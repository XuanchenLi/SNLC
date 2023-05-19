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
	unsigned int Level;					//子过程深度
public:
	Table() : Level(0) {};
	~Table();
public:
	void Analyze(ASTNodeBase* currentP);
	void VarDecList(ASTNodeBase* currentP);
protected:
	TypeIR* TypeProcess(ASTNodeBase* currentP);
	TypeIR* arrayType(ASTDecNode*);
	TypeIR* nameType(ASTNodeBase* currentP);
	TypeIR* recordType(ASTNodeBase* currentP);
	void initialize(void);
	void TypeDecPart(ASTNodeBase* currentP);
	void VarDecList(ASTNodeBase* currentP);
	void ProcDecPart(ASTNodeBase* currentP);
public:
	void CreatTable();												//建一个符号表
	void DestroyTable();											//撤销一个符号表
	bool Enter(char* Id, AttributelR* AttribP, symTablePtr* Entry);	//登记标识符和属性到符号表
	bool FindEntry(char* id, bool diraction, symTablePtr* Entry);				//号表中查找标识符
	void PrintSymbTabl() {};														//打印符号表
private:
	bool SearchoneTable(char* id, int currentLevel, symTablePtr* Entry);			//号表中查找标识符


};