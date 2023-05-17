#pragma once
#include "predict.h"
#include "../snl_common/ast.h"
#include <map>
#include <functional>
#include <stack>



class Parser
{
private:
	std::map<PredictTableKey, std::function<void()>> predictTable;	//LL1·ÖÎö±í
	std::stack<SymbolStackItem> symbolStack;//·ûºÅÕ»
	std::stack<ASTNodeBase**> ASTStack;		//Óï·¨Ê÷Õ»
	std::stack<ASTNodeBase*> operandStack;	//²Ù×÷ÊýÕ»
	std::stack<ASTNodeBase*> operatorStack; //²Ù×÷·ûÕ»
	ASTNodeBase* root;
	void initPredictTable();
	void pushSymbolStack(std::vector<SymbolStackItem>);
	void storeTokenSem(TokenType);
	void linkStackTop(ASTNodeBase*);
	void process2();
	void process3();
	void process7();
	void process8();
	void process9();
	void process12();
	void process14();
	void process15();
	void process16();
	void process19();
	void process20();
	void process21();
	void process22();
	void process23();
	void process24();
	void process25();
	void process27();
	void process32();
	void process33();
	void process36();
	void process41();
	ASTNodeBase* currentP;
	Token currentT;
	ASTDecKind* tempDecKindPtr;
	ASTDecNode* saveP;
public:
	Parser();
};