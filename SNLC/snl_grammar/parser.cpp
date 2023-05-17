#include "parser.h"

Parser::Parser()
{
	initPredictTable();
	symbolStack.push(GetNonTerminal(NonTerminalType::PROGRAM));
	root = new ASTNodeBase;
	root->nodeKind = ASTNodeKind::PRO_K;
	ASTStack.push(&root->child[2]);
	ASTStack.push(&root->child[1]);
	ASTStack.push(&root->child[0]);
	currentP = root;
}

void Parser::initPredictTable()
{

}

void Parser::pushSymbolStack(std::vector<SymbolStackItem> vec)
{
	for (auto item : vec)
	{
		symbolStack.push(item);
	}
}
void Parser::storeTokenSem(TokenType t)
{
	symbolStack.push(GetTerminal(t));
	currentP->names.push_back(currentT.sem);
}
void Parser::linkStackTop(ASTNodeBase* p)
{
	if (ASTStack.empty())
	{
		throw std::exception("ERROR: AST Stack empty.");
	}
	ASTNodeBase** ptr = ASTStack.top();
	ASTStack.pop();
	*ptr = p;
}
/*
 当栈顶为ProgramHead,当前Token的词法信息为PROGRAM时，
选择这个产生式，处理程序头：将产生式右部压入符号栈；生成程序
头节点，弹语法树栈，并对弹出元素进行赋值，使得语法树根节点的
儿子节点指向程序头节点。
*/
void Parser::process2()
{
	pushSymbolStack({ GetNonTerminal(NonTerminalType::PROGRAM_NAME),
					GetTerminal(TokenType::PROGRAM)
					});
	
	currentP = GetASTLabelNode(ASTNodeKind::PHEAD_K);
	linkStackTop(currentP);
}

/*
处理程序名：ID压入符号栈；并将当前标识符的语义信息(标识符名)
写入程序头节点，标识符个数加1。
*/
void Parser::process3()
{
	storeTokenSem(TokenType::IDENTIFIER);
}


/*
处理类型声明，产生式右部压入符号栈；语法树部分，生成类型声明
标志节点，弹语法树栈，得到指针的地址，令指针指向此声明节点，
使得此节点作为根节点的儿子节点出现。当前类型声明节点的兄弟节
点应该指向变量声明标识节点，函数声明节点或语句序列节点，而子
节点则应指向具体的声明节点，故将当前节点的兄弟节点和第一个儿
子节点压入语法树栈，以待以后处理。
*/
void Parser::process7()
{
	pushSymbolStack({ GetNonTerminal(NonTerminalType::TYPE_DEC_LIST),
					GetTerminal(TokenType::TYPE)
					});
	currentP = GetASTLabelNode(ASTNodeKind::TYPE_K);
	linkStackTop(currentP);
	ASTStack.push(&currentP->sibling);
	ASTStack.push(&currentP->child[0]);
}
/*
* <TypeDecList>:=TypeId=TypeDef;TypeDecMore
* 进入具体的类型声明。语法树处理部分，生成一个声明类型节点，不
在此添加任何其他信息；弹语法树栈，得到指针的地址，令指针指向
此声明类型节点，若是第一个声明节点，则是Type类型的子节点指向
当前节点，否则，作为上一个类型声明的兄弟节点出现。并将此节点
的兄弟节点压入语法树栈，以便处理下一个类型声明。
*/
void Parser::process8()
{
	pushSymbolStack({ 
		GetNonTerminal(NonTerminalType::TYPE_DEC_MORE),
		GetTerminal(TokenType::SEMICOLON),
		GetNonTerminal(NonTerminalType::TYPE_DEF),
		GetTerminal(TokenType::EQUAL),
		GetNonTerminal(NonTerminalType::TYPE_ID)
					});
	currentP = (ASTNodeBase*)GetASTDecNode();
	linkStackTop(currentP);
	ASTStack.push(&currentP->sibling);
}
/*
没有其它的类型声明，这时语法树栈顶存放的是最后一个类型声明节
点的兄弟节点，弹出，完成类型部分的语法树节点生成。
*/
void Parser::process9()
{
	ASTStack.pop();
}

/*
处理声明节点的标识符的类型部分。基本类型可以是整型和字符型，
这里用变量temp记录节点上填写标识符类型信息的部分的地址，在下
面的产生式处理对temp里的内容进行赋值，就完成了类型部分的填
写。
*/
void Parser::process12()
{
	symbolStack.push(GetNonTerminal(NonTerminalType::BASE_TYPE));
	tempDecKindPtr = &((ASTDecNode*)currentP)->decKind;
}

/*
声明的类型标识符的类型是用已声明过的类型标识符给出的，在当前
节点存储此标识符的名字，节点上标识符的个数加1。
*/
void Parser::process14()
{
	*tempDecKindPtr = ASTDecKind::ID_K;
	storeTokenSem(TokenType::IDENTIFIER);
}
/*
声明的类型是整型，对temp这个地址的内容赋值，将整型信息存入声
明节点。
*/
void Parser::process15()
{
	*tempDecKindPtr = ASTDecKind::INTEGER_K;
	symbolStack.push(GetTerminal(TokenType::INTEGER));
}

/*
声明的类型是字符型，对temp这个地址的内容赋值，将字符型信息存
入声明节点。
*/
void Parser::process16()
{
	*tempDecKindPtr = ASTDecKind::CHAR_K;
	symbolStack.push(GetTerminal(TokenType::CHAR));
}

/*
声明的类型信息赋值为数组类型ArrayK,并用temp记录填写数组的
基类型部分的地址，以后对temp地址的内容赋值，就完成了对数组基
类型信息的填写。
*/
void Parser::process19()
{
	pushSymbolStack({
		GetNonTerminal(NonTerminalType::BASE_TYPE),
		GetTerminal(TokenType::OF),
		GetTerminal(TokenType::SQUARE_BRACKET_CLOSE),
		GetNonTerminal(NonTerminalType::TOP),
		GetTerminal(TokenType::DOT_DOT),
		GetNonTerminal(NonTerminalType::LOW),
		GetTerminal(TokenType::SQUARE_BRACKET_OPEN),
		GetTerminal(TokenType::ARRAY)
					});
	*tempDecKindPtr = ASTDecKind::ARRAY_K;
	tempDecKindPtr = &((ASTDecNode*)currentP)->decAttr.arrayAttr.itemType;
}

/*
右部终极符入符号栈，并将整数的值写入数组类型声明节点的下界。
*/
void Parser::process20()
{
	symbolStack.push(GetTerminal(TokenType::INT));
	((ASTDecNode*)currentP)->decAttr.arrayAttr.low = std::stoi(currentT.sem);
}

/*
右部终极符入符号栈，并将整数的值写入数组类型声明节点的上界。
*/
void Parser::process21()
{
	symbolStack.push(GetTerminal(TokenType::INT));
	((ASTDecNode*)currentP)->decAttr.arrayAttr.up = std::stoi(currentT.sem);
}


/*
明的类型部分赋值为记录类型RecordK,用变量saveP保存当前指向
记录类型声明节点的指针，以便处理完记录的各个域以后能够回到记
录类型节点处理没有完成的信息，并压入指向记录的第一个域的指针
进语法树栈，在后面对指针赋值。
*/
void Parser::process22()
{
	pushSymbolStack({
		GetTerminal(TokenType::END),
		GetNonTerminal(NonTerminalType::FIELD_DEC_LIST),
		GetTerminal(TokenType::RECORD)
					});
	*tempDecKindPtr = ASTDecKind::RECORD_K;
	saveP = (ASTDecNode*)currentP;
	ASTStack.push(&currentP->child[0]);
}
/*
生成记录类型的一个域，节点为声明类型的节点，不添加任何信息；
类型属于基类型，用temp记录填写类型信息的成员地址，以待以后填
写是整数类型还是字符类型。弹语法树栈，令指针指向这个节点。若
是第一个，则是record类型的子节点指向当前节点；否则，是上一个
记录域声明的兄弟节点。最后，压入指向记录类型下一个域的指针，
以处理多个域。
*/
void Parser::process23()
{
	pushSymbolStack({
		GetNonTerminal(NonTerminalType::FIELD_DEC_MORE),
		GetTerminal(TokenType::SEMICOLON),
		GetNonTerminal(NonTerminalType::ID_LIST),
		GetNonTerminal(NonTerminalType::BASE_TYPE),
					});
	currentP = (ASTNodeBase*)GetASTDecNode();
	linkStackTop(currentP);
	tempDecKindPtr = &((ASTDecNode*)currentP)->decKind;
	ASTStack.push(&currentP->sibling);
}

/*
* 生成记录类型的一个域，节点为声明节点，类型是数组类型，不添加
任何信息，弹语法树栈，令指针指向这个节点，若是第一个，则是record
类型的子结点指向当前结点，否则，是上一个记录域声明的兄弟结点，
最后，压入指向记录类型下一个域的指针，以处理多个域。
*/
void Parser::process24()
{
	pushSymbolStack({
		GetNonTerminal(NonTerminalType::FIELD_DEC_MORE),
		GetTerminal(TokenType::SEMICOLON),
		GetNonTerminal(NonTerminalType::ID_LIST),
		GetNonTerminal(NonTerminalType::ARRAY_TYPE),
					});
	currentP = (ASTNodeBase*)GetASTDecNode(ASTDecKind::ARRAY_K);
	linkStackTop(currentP);
	ASTStack.push(&currentP->sibling);
}
/*
* 没有记录类型的下一个域了，弹出栈顶保存的最后一个域的兄弟节点，
表示记录的域全部处理完；并利用saveP恢复当前记录类型节点的指
针。
*/
void Parser::process25()
{
	ASTStack.pop();
	currentP = (ASTNodeBase*)saveP;
}
/*
右部入符号栈，并将当前标识符的名字，存入节点中，标识符个数加1。
*/
void Parser::process27()
{
	pushSymbolStack({
		GetNonTerminal(NonTerminalType::ID_MORE),
		GetTerminal(TokenType::IDENTIFIER)
					});
	storeTokenSem(TokenType::IDENTIFIER);
}

/*
处理变量声明，产生式右部压入符号栈；语法树部分，生成变量声明
标志节点，弹语法树栈，得到指针的地址，令指针指向此声明节点，
使得此节点作为根节点的儿子节点或者类型标识节点的兄弟节点出
现。当前变量声明节点的兄弟节点应该指向函数声明节点或语句序列
节点，而子节点则应指向具体的声明节点，故将当前节点的兄弟节点
和第一个儿子节点压入语法树栈，以待以后处理。
*/
void Parser::process32()
{
	pushSymbolStack({
		GetNonTerminal(NonTerminalType::VAR_DEC_LIST),
		GetTerminal(TokenType::VAR)
					});
	currentP = GetASTLabelNode(ASTNodeKind::VAR_K);
	linkStackTop(currentP);
	ASTStack.push(&currentP->sibling);
	ASTStack.push(&currentP->child[0]);
}
/*
入具体的变量声明。语法树处理部分，生成一个声明类型节点，不
在此添加任何其他信息；弹语法树栈，得到指针的地址，令指针指向
此声明类型节点，若是第一个声明节点，则是VarK类型的子节点指向
当前节点，否则，作为上一个变量声明的兄弟节点出现。并将此节点
的兄弟节点压入语法树栈，以便处理下一个变量声明。
*/
void Parser::process33()
{
	pushSymbolStack({
		GetNonTerminal(NonTerminalType::VAR_DEC_MORE),
		GetTerminal(TokenType::SEMICOLON),
		GetNonTerminal(NonTerminalType::VAR_ID_LIST),
		GetNonTerminal(NonTerminalType::TYPE_DEF)
					});
	currentP = (ASTNodeBase*)GetASTDecNode();
	linkStackTop(currentP);
	ASTStack.push(&currentP->sibling);
}
/*
处理变量声明的若干个标识符，将当前标识符的名字，存入节点中，
标识符个数加1。
*/
void Parser::process36()
{
	pushSymbolStack({
		GetNonTerminal(NonTerminalType::VAR_ID_MORE),
		GetTerminal(TokenType::IDENTIFIER)
					});
	storeTokenSem(TokenType::IDENTIFIER);
}
/*
理过程声明，产生式右部压入符号栈；语法树部分，生成过程头结
点ProcK,弹语法树栈，得到指针的地址，令指针指向此声明节点，
使得此节点作为根节点的儿子节点或者类型标识节点，或者变量标识
节点的兄弟节点出现。当前过程声明节点的兄弟节点指向下一个过程
声明节点或语句序列节点，第三个儿子节点指向过程体部分；第二个
子节点指向过程中的声明部分，若没有声明，这个指针为空；第一个
子节点指向函数的形参部分。故将当前节点的兄弟节点和三个儿子节
点压入语法树栈，以待以后处理。
*/
void Parser::process41()
{
	pushSymbolStack({
		GetNonTerminal(NonTerminalType::PROC_DEC_MORE),
		GetNonTerminal(NonTerminalType::PROC_BODY),
		GetNonTerminal(NonTerminalType::PROC_DEC_PART),
		GetTerminal(TokenType::SEMICOLON),
		GetNonTerminal(NonTerminalType::BASE_TYPE),
		GetTerminal(TokenType::COLON),
		GetTerminal(TokenType::BRACKET_CLOSE),
		GetNonTerminal(NonTerminalType::PARAM_LIST),
		GetTerminal(TokenType::BRACKET_OPEN),
		GetNonTerminal(NonTerminalType::PROC_NAME),
		GetTerminal(TokenType::PROCEDURE)
					});
	currentP = GetASTLabelNode(ASTNodeKind::PRO_K);
	linkStackTop(currentP);
	ASTStack.push(&currentP->sibling);
	ASTStack.push(&currentP->child[2]);
	ASTStack.push(&currentP->child[1]);
	ASTStack.push(&currentP->child[0]);
}






























