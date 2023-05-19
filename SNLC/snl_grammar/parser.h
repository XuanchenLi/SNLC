#pragma once
#include "predict.h"
#include "../snl_common/token_list.h"
#include "../snl_common/ast.h"
#include <map>
#include <unordered_map>
#include <functional>
#include <stack>



class Parser
{
private:
	std::map<PredictTableKey, std::function<void()>> predictTable;	//LL1分析表
	std::stack<SymbolStackItem> symbolStack;//符号栈
	std::stack<ASTNodeBase**> ASTStack;		//语法树栈
	std::stack<ASTExpNode*> operandStack;	//操作数栈
	std::stack<ASTExpNode*> operatorStack; //操作符栈
	ASTNodeBase* root;
	void initPredictTable();
	void insertPredictTable(NonTerminalType, std::vector<TokenType>, std::function<void()>);
	void initOpStack();
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
	void process40();  //教材有误？
	void process41();
	void process48();
	void process50();
	void process51();
	void process52();
	void process57();
	void process61();
	void process62();
	void process63();
	void process64();
	void process65();
	void process66();
	void process67();
	void process68();
	void process69();
	void process70();
	void process71();
	void process74();
	void process76(); //教材有误？
	void process78();
	void process80();
	void process81();
	void process82();
	void process84();
	void process85(); //教材有误？
	void process88();
	void process89();
	void process90();
	void process92();
	void process93();
	void process94();
	void process95();
	void process96();
	void process97();
	ASTNodeBase* currentP;
	Token currentT;
	ASTDecKind* tempDecKindPtr;
	ASTDecNode* saveP;
	bool getExpResult;
	bool getExpResult2;
	int expFlag;
public:
	Parser();
	ASTNodeBase* parse(TokenList&);
	void printTree(ASTNodeBase* t, int layer=0);
};