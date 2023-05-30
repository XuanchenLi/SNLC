#include "parser.h"
#include <iostream>

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

ASTNodeBase* Parser::parse(TokenList& tokenList)
{
	if (!tokenList.hasNext())
	{
		return root;
	}
	currentT = tokenList.getCurToken();
	currentP->lineNum = currentT.lineNum;
	while (!symbolStack.empty())
	{
		if (symbolStack.top().symbolType == SymbolType::TERMINAL)
		{
			if (currentT.type == symbolStack.top().symbolName.tokenType)
			{
				symbolStack.pop();
				if (!tokenList.hasNext())
				{
					throw std::runtime_error(
						"ERROR: Unexcept end of tokens."
					);
				}
				tokenList.moveNext();
				currentT = tokenList.getCurToken();
				currentP->lineNum = currentT.lineNum;
			}
			else
			{
				throw std::runtime_error(
					("ERROR: Except to get: " + std::string(TokenTypeName(symbolStack.top().symbolName.tokenType))
					 + " but got: " + TokenTypeName(currentT.type) + " at line " + std::to_string(currentT.lineNum)
					 + " column " + std::to_string(currentT.colNum)).c_str()
				);
			}
		}
		else
		{
			NonTerminalType line = symbolStack.top().symbolName.nonTerminalType;
			TokenType col = currentT.type;
			std::function<void()> preFun = predictTable[PredictTableKey(line, col)];
			symbolStack.pop();
			//std::cout << "Call predictable table line: " + std::string(NonTerminalTypeName(line)) + " colomn: " + TokenTypeName(col) + "\n";
			try
			{
				preFun();
			}
			catch (std::runtime_error e)
			{
				std::cout << e.what() << std::endl;
				return nullptr;
			}
			catch (std::exception e)
			{
				std::cout << std::string(TokenTypeName(col)) + " should not after " + NonTerminalTypeName(line) << std::endl;
				return nullptr;
			}
		}

	}
	if (currentT.type != TokenType::EOF_) {
		throw std::runtime_error(("ERROR: Unexcept end of file at line " + std::to_string(currentT.lineNum)).c_str());
	}
	return root;
}

void Parser::printTree(ASTNodeBase* t, int layer)
{
	if (!t) return;
	std::cout << std::string(layer, '\t');
	std::cout << *t;
	for (int i = 0; i < 3; ++i)
	{
		if (t->child[i]) printTree(t->child[i], layer + 1);
	}
	if (t->sibling) printTree(t->sibling, layer);
}

void Parser::initPredictTable()
{
	predictTable.clear();
	insertPredictTable(
		NonTerminalType::PROGRAM,		   
		{ TokenType::PROGRAM },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 1\n";
				pushSymbolStack({ 
								GetTerminal(TokenType::DOT),
								GetNonTerminal(NonTerminalType::PROGRAM_BODY),
								GetNonTerminal(NonTerminalType::DECLARE_PART),
								GetNonTerminal(NonTerminalType::PROGRAM_HEAD)
							});
			}
				)
	);
	insertPredictTable(
		NonTerminalType::PROGRAM_HEAD,
		{ TokenType::PROGRAM },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 2\n";
				this->process2();
			}
				)
	);
	insertPredictTable(
		NonTerminalType::PROGRAM_NAME,
		{ TokenType::IDENTIFIER },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 3\n";
				this->process3();
			}
				)
	);
	insertPredictTable(
		NonTerminalType::DECLARE_PART,
		{ TokenType::TYPE, TokenType::VAR, TokenType::PROCEDURE, TokenType::BEGIN },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 4\n";
				pushSymbolStack({ 
					GetNonTerminal(NonTerminalType::PROC_DECPART),
					GetNonTerminal(NonTerminalType::VAR_DEC),
					GetNonTerminal(NonTerminalType::TYPE_DEC)
								});
			}
				)
	);
	insertPredictTable(
		NonTerminalType::TYPE_DEC,
		{ TokenType::VAR, TokenType::PROCEDURE, TokenType::BEGIN },
		std::function<void()>(
			[]
			{
				std::cout << "Call process 5\n";
			}
				)
	);
	insertPredictTable(
		NonTerminalType::TYPE_DEC,
		{ TokenType::TYPE},
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 6\n";
				pushSymbolStack({
					GetNonTerminal(NonTerminalType::TYPE_DECLATRATION)
								});
			}
				)
	);
	insertPredictTable(
		NonTerminalType::TYPE_DECLATRATION,
		{ TokenType::TYPE },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 7\n";
				this->process7();
			}
				)
	);
	insertPredictTable(
		NonTerminalType::TYPE_DEC_LIST,
		{ TokenType::IDENTIFIER },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 8\n";
				this->process8();
			}
				)
	);
	insertPredictTable(
		NonTerminalType::TYPE_DEC_MORE,
		{ TokenType::VAR, TokenType::PROCEDURE, TokenType::BEGIN },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 9\n";
				this->process9();
			}
				)
	);
	insertPredictTable(
		NonTerminalType::TYPE_DEC_MORE,
		{ TokenType::IDENTIFIER },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 10\n";
				pushSymbolStack({
					GetNonTerminal(NonTerminalType::TYPE_DEC_LIST)
								});
			}
				)
	);
	insertPredictTable(
		NonTerminalType::TYPE_ID,
		{ TokenType::IDENTIFIER },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 11\n";
				storeTokenSem(TokenType::IDENTIFIER);
			}
				)
	);
	insertPredictTable(
		NonTerminalType::TYPE_DEF,
		{ TokenType::INTEGER, TokenType::CHAR},
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 12\n";
				this->process12();
			}
				)
	);
	insertPredictTable(
		NonTerminalType::TYPE_DEF,
		{ TokenType::ARRAY,  TokenType::RECORD },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 13\n";
				pushSymbolStack({
					GetNonTerminal(NonTerminalType::STRUCT_TYPE)
								});
			}
				)
	);
	insertPredictTable(
		NonTerminalType::TYPE_DEF,
		{ TokenType::IDENTIFIER},
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 14\n";
				this->process14();
			}
				)
	);
	insertPredictTable(
		NonTerminalType::BASE_TYPE,
		{ TokenType::INTEGER },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 15\n";
				this->process15();
			}
				)
	);
	insertPredictTable(
		NonTerminalType::BASE_TYPE,
		{ TokenType::CHAR },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 16\n";
				this->process16();
			}
				)
	);
	insertPredictTable(
		NonTerminalType::STRUCT_TYPE,
		{ TokenType::ARRAY },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 17\n";
				pushSymbolStack({
					GetNonTerminal(NonTerminalType::ARRAY_TYPE)
								});
			}
				)
	);
	insertPredictTable(
		NonTerminalType::STRUCT_TYPE,
		{ TokenType::RECORD },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 18\n";
				pushSymbolStack({
					GetNonTerminal(NonTerminalType::REC_TYPE)
								});
			}
				)
	);
	insertPredictTable(
		NonTerminalType::ARRAY_TYPE,
		{ TokenType::ARRAY },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 19\n";
				this->process19();
			}
				)
	);
	insertPredictTable(
		NonTerminalType::LOW,
		{ TokenType::INT },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 20\n";
				this->process20();
			}
				)
	);
	insertPredictTable(
		NonTerminalType::TOP,
		{ TokenType::INT },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 21\n";
				this->process21();
			}
				)
	);
	insertPredictTable(
		NonTerminalType::REC_TYPE,
		{ TokenType::RECORD },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 22\n";
				this->process22();
			}
				)
	);
	insertPredictTable(
		NonTerminalType::FIELD_DEC_LIST,
		{ TokenType::INTEGER,  TokenType::CHAR },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 23\n";
				this->process23();
			}
				)
	);
	insertPredictTable(
		NonTerminalType::FIELD_DEC_LIST,
		{ TokenType::ARRAY },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 24\n";
				this->process24();
			}
				)
	);
	insertPredictTable(
		NonTerminalType::FIELD_DEC_MORE,
		{ TokenType::END },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 25\n";
				this->process25();
			}
				)
	);
	insertPredictTable(
		NonTerminalType::FIELD_DEC_MORE,
		{ TokenType::INTEGER,  TokenType::CHAR, TokenType::ARRAY },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 26\n";
				pushSymbolStack({
					GetNonTerminal(NonTerminalType::FIELD_DEC_LIST)
								});
			}
				)
	);
	insertPredictTable(
		NonTerminalType::ID_LIST,
		{ TokenType::IDENTIFIER },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 27\n";
				this->process27();
			}
				)
	);
	insertPredictTable(
		NonTerminalType::ID_MORE,
		{ TokenType::SEMICOLON },
		std::function<void()>(
			[]
			{
				std::cout << "Call process 28\n";
			}
				)
	);
	insertPredictTable(
		NonTerminalType::ID_MORE,
		{ TokenType::COMMA },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 29\n";
				pushSymbolStack({
					GetNonTerminal(NonTerminalType::ID_LIST),
					GetTerminal(TokenType::COMMA)
								});
			}
				)
	);
	insertPredictTable(
		NonTerminalType::VAR_DEC,
		{ TokenType::PROCEDURE, TokenType::BEGIN },
		std::function<void()>(
			[]
			{
				std::cout << "Call process 30\n";
			}
				)
	);
	insertPredictTable(
		NonTerminalType::VAR_DEC,
		{ TokenType::VAR },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 31\n";
				pushSymbolStack({
					GetNonTerminal(NonTerminalType::VAR_DECLARATION)
								});
			}
				)
	);
	insertPredictTable(
		NonTerminalType::VAR_DECLARATION,
		{ TokenType::VAR },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 32\n";
				this->process32();
			}
				)
	);
	insertPredictTable(
		NonTerminalType::VAR_DEC_LIST,
		{ TokenType::INTEGER, TokenType::CHAR, TokenType::ARRAY, TokenType::RECORD, TokenType::IDENTIFIER },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 33\n";
				this->process33();
			}
				)
	);
	insertPredictTable(
		NonTerminalType::VAR_DEC_MORE,
		{ TokenType::PROCEDURE, TokenType::BEGIN },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 34\n";
				ASTStack.pop();
			}
				)
	);
	insertPredictTable(
		NonTerminalType::VAR_DEC_MORE,
		{ TokenType::INTEGER, TokenType::CHAR, TokenType::ARRAY, TokenType::RECORD, TokenType::IDENTIFIER },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 35\n";
				pushSymbolStack({
					GetNonTerminal(NonTerminalType::VAR_DEC_LIST)
								});
			}
				)
	);
	insertPredictTable(
		NonTerminalType::VAR_ID_LIST,
		{ TokenType::IDENTIFIER },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 36\n";
				this->process36();
			}
				)
	);
	insertPredictTable(
		NonTerminalType::VAR_ID_MORE,
		{ TokenType::SEMICOLON },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 37\n";
				//printTree(root, 0);
			}
				)
	);
	insertPredictTable(
		NonTerminalType::VAR_ID_MORE,
		{ TokenType::COMMA},
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 38\n";
				pushSymbolStack({
					GetNonTerminal(NonTerminalType::VAR_ID_LIST),
					GetTerminal(TokenType::COMMA)
								});
			}
				)
	);
	insertPredictTable(
		NonTerminalType::PROC_DECPART,
		{ TokenType::BEGIN },
		std::function<void()>(
			[]
			{
				std::cout << "Call process 39\n";
			}
				)
	);
	insertPredictTable(
		NonTerminalType::PROC_DECPART,
		{ TokenType::PROCEDURE },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 40\n";
				process40();
			}
				)
	);
	insertPredictTable(
		NonTerminalType::PROC_DEC,
		{ TokenType::PROCEDURE },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 41\n";
				process41();
			}
				)
	);
	insertPredictTable(
		NonTerminalType::PROC_DEC_MORE,
		{ TokenType::BEGIN },
		std::function<void()>(
			[]
			{
				std::cout << "Call process 42\n";
			}
				)
	);
	insertPredictTable(
		NonTerminalType::PROC_DEC_MORE,
		{ TokenType::PROCEDURE },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 43\n";
				pushSymbolStack({
					GetNonTerminal(NonTerminalType::PROC_DEC)
								});
			}
				)
	);
	insertPredictTable(
		NonTerminalType::PROC_NAME,
		{ TokenType::IDENTIFIER },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 44\n";
				storeTokenSem(TokenType::IDENTIFIER);
			}
				)
	);
	insertPredictTable(
		NonTerminalType::PARAM_LIST,
		{ TokenType::BRACKET_CLOSE },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 45\n";
				ASTStack.pop();
			}
				)
	);
	insertPredictTable(
		NonTerminalType::PARAM_LIST,
		{ TokenType::INTEGER, TokenType::CHAR, TokenType::ARRAY, TokenType::RECORD, TokenType::IDENTIFIER, TokenType::VAR },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 46\n";
				pushSymbolStack({
					GetNonTerminal(NonTerminalType::PARAM_DEC_LIST)
								});
			}
				)
	);
	insertPredictTable(
		NonTerminalType::PARAM_DEC_LIST,
		{ TokenType::INTEGER, TokenType::CHAR, TokenType::ARRAY, TokenType::RECORD, TokenType::IDENTIFIER, TokenType::VAR },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 47\n";
				pushSymbolStack({
					GetNonTerminal(NonTerminalType::PARAM_MORE),
					GetNonTerminal(NonTerminalType::PARAM)
								});
			}
				)
	);
	insertPredictTable(
		NonTerminalType::PARAM_MORE,
		{ TokenType::BRACKET_CLOSE },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 48\n";
				process48();
			}
				)
	);
	insertPredictTable(
		NonTerminalType::PARAM_MORE,
		{ TokenType::SEMICOLON },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 49\n";
				pushSymbolStack({
					GetNonTerminal(NonTerminalType::PARAM_DEC_LIST),
					GetTerminal(TokenType::SEMICOLON)
								});
			}
				)
	);
	insertPredictTable(
		NonTerminalType::PARAM,
		{ TokenType::INTEGER, TokenType::CHAR, TokenType::ARRAY, TokenType::RECORD, TokenType::IDENTIFIER },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 50\n";
				process50();
			}
				)
	);
	insertPredictTable(
		NonTerminalType::PARAM,
		{ TokenType::VAR },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 51\n";
				process51();
			}
				)
	);
	insertPredictTable(
		NonTerminalType::FORM_LIST,
		{ TokenType::IDENTIFIER },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 52\n";
				process52();
			}
				)
	);
	insertPredictTable(
		NonTerminalType::FID_MORE,
		{ TokenType::SEMICOLON, TokenType::BRACKET_CLOSE },
		std::function<void()>(
			[]
			{
				std::cout << "Call process 53\n";
			}
				)
	);
	insertPredictTable(
		NonTerminalType::FID_MORE,
		{ TokenType::COMMA },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 54\n";
				pushSymbolStack({
					GetNonTerminal(NonTerminalType::FORM_LIST),
					GetTerminal(TokenType::COMMA)
								});
			}
				)
	);
	insertPredictTable(
		NonTerminalType::PROC_DEC_PART,
		{ TokenType::TYPE, TokenType::VAR, TokenType::PROCEDURE, TokenType::BEGIN },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 55\n";
				pushSymbolStack({
					GetNonTerminal(NonTerminalType::DECLARE_PART)
								});
			}
				)
	);
	insertPredictTable(
		NonTerminalType::PROC_BODY,
		{ TokenType::BEGIN },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 56\n";
				pushSymbolStack({
					GetNonTerminal(NonTerminalType::PROGRAM_BODY)
								});
			}
				)
	);
	insertPredictTable(
		NonTerminalType::PROGRAM_BODY,
		{ TokenType::BEGIN },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 57\n";
				process57();
			}
				)
	);
	insertPredictTable(
		NonTerminalType::STM_LIST,
		{ TokenType::IDENTIFIER, TokenType::IF, TokenType::WHILE, TokenType::RETURN, TokenType::READ, TokenType::WRITE },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 58\n";
				pushSymbolStack({
					GetNonTerminal(NonTerminalType::STM_MORE),
					GetNonTerminal(NonTerminalType::STM)
								});
			}
				)
	);
	insertPredictTable(
		NonTerminalType::STM_MORE,
		{ TokenType::ELSE, TokenType::FI, TokenType::END, TokenType::END_WHILE },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 59\n";
				ASTStack.pop();
			}
				)
	);
	insertPredictTable(
		NonTerminalType::STM_MORE,
		{ TokenType::SEMICOLON },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 60\n";
				pushSymbolStack({
					GetNonTerminal(NonTerminalType::STM_LIST),
					GetTerminal(TokenType::SEMICOLON)
								});
			}
				)
	);
	insertPredictTable(
		NonTerminalType::STM,
		{ TokenType::IF },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 61\n";
				process61();
			}
				)
	);
	insertPredictTable(
		NonTerminalType::STM,
		{ TokenType::WHILE },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 62\n";
				process62();
			}
				)
	);
	insertPredictTable(
		NonTerminalType::STM,
		{ TokenType::READ },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 63\n";
				process63();
			}
				)
	);
	insertPredictTable(
		NonTerminalType::STM,
		{ TokenType::WRITE },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 64\n";
				process64();
			}
				)
	);
	insertPredictTable(
		NonTerminalType::STM,
		{ TokenType::RETURN },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 65\n";
				process65();
			}
				)
	);
	insertPredictTable(
		NonTerminalType::STM,
		{ TokenType::IDENTIFIER},
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 66\n";
				process66();
			}
				)
	);
	insertPredictTable(
		NonTerminalType::ASS_CALL,
		{ TokenType::ASSIGN, TokenType::SQUARE_BRACKET_OPEN},
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 67\n";
				process67();
			}
				)
	);
	insertPredictTable(
		NonTerminalType::ASS_CALL,
		{ TokenType::BRACKET_OPEN },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 68\n";
				process68();
			}
				)
	);
	insertPredictTable(
		NonTerminalType::ASSIGNMENT_REST,
		{ TokenType::ASSIGN, TokenType::DOT, TokenType::SQUARE_BRACKET_OPEN },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 69\n";
				process69();
			}
				)
	);
	insertPredictTable(
		NonTerminalType::CONDITIONAL_STM,
		{ TokenType::IF },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 70\n";
				process70();
			}
				)
	);
	insertPredictTable(
		NonTerminalType::LOOP_STM,
		{ TokenType::WHILE },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 71\n";
				process71();
			}
				)
	);
	insertPredictTable(
		NonTerminalType::INPUT_STM,
		{ TokenType::READ },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 72\n";
				pushSymbolStack({
					GetTerminal(TokenType::BRACKET_CLOSE),
					GetNonTerminal(NonTerminalType::IN_VAR),
					GetTerminal(TokenType::BRACKET_OPEN),
					GetTerminal(TokenType::READ)
								});
			}
				)
	);
	insertPredictTable(
		NonTerminalType::IN_VAR,
		{ TokenType::IDENTIFIER },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 73\n";
				storeTokenSem(TokenType::IDENTIFIER);
			}
				)
	);
	insertPredictTable(
		NonTerminalType::OUTPUT_STM,
		{ TokenType::WRITE },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 74\n";
				process74();
			}
				)
	);

	insertPredictTable(
		NonTerminalType::RETURN_STM,
		{ TokenType::RETURN },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 75\n";
				pushSymbolStack({
					GetTerminal(TokenType::RETURN)
								});
			}
				)
	);
	insertPredictTable(
		NonTerminalType::CALL_STM_REST,
		{ TokenType::BRACKET_OPEN },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 76\n";
				process76();
			}
				)
	);
	insertPredictTable(
		NonTerminalType::ACT_PARAM_LIST,
		{ TokenType::BRACKET_CLOSE },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 77\n";
				ASTStack.pop();
			}
				)
	);
	insertPredictTable(
		NonTerminalType::ACT_PARAM_LIST,
		{ TokenType::BRACKET_OPEN, TokenType::IDENTIFIER, TokenType::INT },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 78\n";
				process78();
			}
				)
	);
	insertPredictTable(
		NonTerminalType::ACT_PARAM_MORE,
		{ TokenType::BRACKET_CLOSE },
		std::function<void()>(
			[]
			{
				std::cout << "Call process 79\n";
			}
				)
	);
	insertPredictTable(
		NonTerminalType::ACT_PARAM_MORE,
		{ TokenType::COMMA },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 80\n";
				process80();
			}
				)
	);
	insertPredictTable(
		NonTerminalType::REL_EXP,
		{ TokenType::BRACKET_OPEN, TokenType::IDENTIFIER, TokenType::INT },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 81\n";
				process81();
			}
				)
	);
	insertPredictTable(
		NonTerminalType::OTHER_REL_E,
		{ TokenType::EQUAL, TokenType::LESS_THAN },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 82\n";
				process82();
			}
				)
	);
	insertPredictTable(
		NonTerminalType::EXP,
		{ TokenType::BRACKET_OPEN, TokenType::IDENTIFIER, TokenType::INT },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 83\n";
				pushSymbolStack({
					GetNonTerminal(NonTerminalType::OTHER_TERM),
					GetNonTerminal(NonTerminalType::TERM)
								});
			}
				)
	);
	insertPredictTable(
		NonTerminalType::OTHER_TERM,
		{ TokenType::EQUAL, TokenType::LESS_THAN, TokenType::SQUARE_BRACKET_CLOSE,
		  TokenType::THEN, TokenType::ELSE, TokenType::FI,
		TokenType::DO, TokenType::END_WHILE, TokenType::BRACKET_CLOSE,
		TokenType::END, TokenType::SEMICOLON, TokenType::COMMA
		},
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 84\n";
				process84();
			}
				)
	);
	insertPredictTable(
		NonTerminalType::OTHER_TERM,
		{ TokenType::ADD, TokenType::MINUS },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 85\n";
				process85();
			}
				)
	);
	insertPredictTable(
		NonTerminalType::TERM,
		{ TokenType::BRACKET_OPEN, TokenType::IDENTIFIER, TokenType::INT },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 86\n";
				pushSymbolStack({
					GetNonTerminal(NonTerminalType::OTHER_FACTOR),
					GetNonTerminal(NonTerminalType::FACTOR)
								});
			}
				)
	);
	insertPredictTable(
		NonTerminalType::OTHER_FACTOR,
		{ TokenType::EQUAL, TokenType::LESS_THAN, TokenType::SQUARE_BRACKET_CLOSE,
		  TokenType::THEN, TokenType::ELSE, TokenType::FI,
		TokenType::DO, TokenType::END_WHILE, TokenType::BRACKET_CLOSE,
		TokenType::END, TokenType::SEMICOLON, TokenType::COMMA,
		TokenType::ADD, TokenType::MINUS
		},
		std::function<void()>(
			[]
			{
				std::cout << "Call process 87\n";
			}
				)
	);
	insertPredictTable(
		NonTerminalType::OTHER_FACTOR,
		{ TokenType::MULTIPLY, TokenType::DIVIDE },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 88\n";
				process88();
			}
				)
	);
	insertPredictTable(
		NonTerminalType::FACTOR,
		{ TokenType::BRACKET_OPEN},
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 89\n";
				process89();
			}
				)
	);
	insertPredictTable(
		NonTerminalType::FACTOR,
		{ TokenType::INT },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 90\n";
				process90();
			}
				)
	);
	insertPredictTable(
		NonTerminalType::FACTOR,
		{ TokenType::IDENTIFIER },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 91\n";
				pushSymbolStack({
					GetNonTerminal(NonTerminalType::VARIABLE)
								});
			}
				)
	);
	insertPredictTable(
		NonTerminalType::VARIABLE,
		{ TokenType::IDENTIFIER },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 92\n";
				process92();
			}
				)
	);
	insertPredictTable(
		NonTerminalType::VARI_MORE,
		{ TokenType::EQUAL, TokenType::LESS_THAN, TokenType::SQUARE_BRACKET_CLOSE,
		  TokenType::THEN, TokenType::ELSE, TokenType::FI,
		TokenType::DO, TokenType::END_WHILE, TokenType::BRACKET_CLOSE,
		TokenType::END, TokenType::SEMICOLON, TokenType::COMMA,
		TokenType::ADD, TokenType::MINUS, TokenType::MULTIPLY, TokenType::DIVIDE, TokenType::ASSIGN
		},
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 93\n";
				process93();
			}
				)
	);
	insertPredictTable(
		NonTerminalType::VARI_MORE,
		{ TokenType::SQUARE_BRACKET_OPEN },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 94\n";
				process94();
			}
				)
	);
	insertPredictTable(
		NonTerminalType::VARI_MORE,
		{ TokenType::DOT },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 95\n";
				process95();
			}
				)
	);
	insertPredictTable(
		NonTerminalType::FIELD_VAR,
		{ TokenType::IDENTIFIER },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 96\n";
				process96();
			}
				)
	);
	insertPredictTable(
		NonTerminalType::FIELD_VAR_MORE,
		{ TokenType::EQUAL, TokenType::LESS_THAN, TokenType::SQUARE_BRACKET_CLOSE,
		  TokenType::THEN, TokenType::ELSE, TokenType::FI,
		TokenType::DO, TokenType::END_WHILE, TokenType::BRACKET_CLOSE,
		TokenType::END, TokenType::SEMICOLON, TokenType::COMMA,
		TokenType::ADD, TokenType::MINUS, TokenType::MULTIPLY, TokenType::DIVIDE, TokenType::ASSIGN
		},
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 97\n";
				process97();
			}
				)
	);

	insertPredictTable(
		NonTerminalType::FIELD_VAR_MORE,
		{ TokenType::SQUARE_BRACKET_OPEN },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 98\n";
				process94();
			}
				)
	);
	insertPredictTable(
		NonTerminalType::CMP_OP,
		{ TokenType::LESS_THAN },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 99\n";
				pushSymbolStack({
					GetTerminal(TokenType::LESS_THAN)
								});
			}
				)
	);
	insertPredictTable(
		NonTerminalType::CMP_OP,
		{ TokenType::EQUAL },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 100\n";
				pushSymbolStack({
					GetTerminal(TokenType::EQUAL)
								});
			}
				)
	);
	insertPredictTable(
		NonTerminalType::ADD_OP,
		{ TokenType::ADD },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 101\n";
				pushSymbolStack({
					GetTerminal(TokenType::ADD)
								});
			}
				)
	);
	insertPredictTable(
		NonTerminalType::ADD_OP,
		{ TokenType::MINUS },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 102\n";
				pushSymbolStack({
					GetTerminal(TokenType::MINUS)
								});
			}
				)
	);
	insertPredictTable(
		NonTerminalType::MULTI_OP,
		{ TokenType::MULTIPLY },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 103\n";
				pushSymbolStack({
					GetTerminal(TokenType::MULTIPLY)
								});
			}
				)
	);
	insertPredictTable(
		NonTerminalType::MULTI_OP,
		{ TokenType::DIVIDE },
		std::function<void()>(
			[this]
			{
				std::cout << "Call process 104\n";
				pushSymbolStack({
					GetTerminal(TokenType::DIVIDE)
								});
			}
				)
	);
}
void Parser::insertPredictTable(NonTerminalType nt, std::vector<TokenType> ts, std::function<void()>f)
{
	for (auto t : ts) {
		predictTable.insert(GetPredictItem(
			PredictTableKey(nt, t), f
		));
	}
}
void Parser::initOpStack()
{
	while (!operandStack.empty())
	{
		delete operandStack.top();
		operandStack.pop();
	}
	while (!operatorStack.empty())
	{
		delete operatorStack.top();
		operatorStack.pop();
	}
	operatorStack.push(GetASTExpOpNode(ASTOpType::STACK_END));
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
		throw std::runtime_error("ERROR: AST Stack empty.");
	}
	ASTNodeBase** ptr = ASTStack.top();
	ASTStack.pop();
	*ptr = p;
}
/*
 ��ջ��ΪProgramHead,��ǰToken�Ĵʷ���ϢΪPROGRAMʱ��
ѡ���������ʽ���������ͷ��������ʽ�Ҳ�ѹ�����ջ�����ɳ���
ͷ�ڵ㣬���﷨��ջ�����Ե���Ԫ�ؽ��и�ֵ��ʹ���﷨�����ڵ��
���ӽڵ�ָ�����ͷ�ڵ㡣
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
�����������IDѹ�����ջ��������ǰ��ʶ����������Ϣ(��ʶ����)
д�����ͷ�ڵ㣬��ʶ��������1��
*/
void Parser::process3()
{
	/*
	pushSymbolStack({
					GetTerminal(TokenType::SEMICOLON)
					});
	*/
	storeTokenSem(TokenType::IDENTIFIER);
}


/*
������������������ʽ�Ҳ�ѹ�����ջ���﷨�����֣�������������
��־�ڵ㣬���﷨��ջ���õ�ָ��ĵ�ַ����ָ��ָ��������ڵ㣬
ʹ�ô˽ڵ���Ϊ���ڵ�Ķ��ӽڵ���֡���ǰ���������ڵ���ֵܽ�
��Ӧ��ָ�����������ʶ�ڵ㣬���������ڵ��������нڵ㣬����
�ڵ���Ӧָ�����������ڵ㣬�ʽ���ǰ�ڵ���ֵܽڵ�͵�һ����
�ӽڵ�ѹ���﷨��ջ���Դ��Ժ���
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
* �������������������﷨�������֣�����һ���������ͽڵ㣬��
�ڴ�����κ�������Ϣ�����﷨��ջ���õ�ָ��ĵ�ַ����ָ��ָ��
���������ͽڵ㣬���ǵ�һ�������ڵ㣬����Type���͵��ӽڵ�ָ��
��ǰ�ڵ㣬������Ϊ��һ�������������ֵܽڵ���֡������˽ڵ�
���ֵܽڵ�ѹ���﷨��ջ���Ա㴦����һ������������
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
û��������������������ʱ�﷨��ջ����ŵ������һ������������
����ֵܽڵ㣬������������Ͳ��ֵ��﷨���ڵ����ɡ�
*/
void Parser::process9()
{
	ASTStack.pop();
}

/*
���������ڵ�ı�ʶ�������Ͳ��֡��������Ϳ��������ͺ��ַ��ͣ�
�����ñ���temp��¼�ڵ�����д��ʶ��������Ϣ�Ĳ��ֵĵ�ַ������
��Ĳ���ʽ�����temp������ݽ��и�ֵ������������Ͳ��ֵ���
д��
*/
void Parser::process12()
{
	symbolStack.push(GetNonTerminal(NonTerminalType::BASE_TYPE));
	tempDecKindPtr = &((ASTDecNode*)currentP)->decKind;
}

/*
���������ͱ�ʶ�������������������������ͱ�ʶ�������ģ��ڵ�ǰ
�ڵ�洢�˱�ʶ�������֣��ڵ��ϱ�ʶ���ĸ�����1��
*/
void Parser::process14()
{
	((ASTDecNode*)currentP)->decKind = ASTDecKind::ID_K;
	//*tempDecKindPtr = ASTDecKind::ID_K;
	storeTokenSem(TokenType::IDENTIFIER);
}
/*
���������������ͣ���temp�����ַ�����ݸ�ֵ����������Ϣ������
���ڵ㡣
*/
void Parser::process15()
{
	*tempDecKindPtr = ASTDecKind::INTEGER_K;
	symbolStack.push(GetTerminal(TokenType::INTEGER));
}

/*
�������������ַ��ͣ���temp�����ַ�����ݸ�ֵ�����ַ�����Ϣ��
�������ڵ㡣
*/
void Parser::process16()
{
	*tempDecKindPtr = ASTDecKind::CHAR_K;
	symbolStack.push(GetTerminal(TokenType::CHAR));
}

/*
������������Ϣ��ֵΪ��������ArrayK,����temp��¼��д�����
�����Ͳ��ֵĵ�ַ���Ժ��temp��ַ�����ݸ�ֵ��������˶������
������Ϣ����д��
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
�Ҳ��ռ��������ջ������������ֵд���������������ڵ���½硣
*/
void Parser::process20()
{
	symbolStack.push(GetTerminal(TokenType::INT));
	((ASTDecNode*)currentP)->decAttr.arrayAttr.low = std::stoi(currentT.sem);
}

/*
�Ҳ��ռ��������ջ������������ֵд���������������ڵ���Ͻ硣
*/
void Parser::process21()
{
	symbolStack.push(GetTerminal(TokenType::INT));
	((ASTDecNode*)currentP)->decAttr.arrayAttr.up = std::stoi(currentT.sem);
}


/*
���������Ͳ��ָ�ֵΪ��¼����RecordK,�ñ���saveP���浱ǰָ��
��¼���������ڵ��ָ�룬�Ա㴦�����¼�ĸ������Ժ��ܹ��ص���
¼���ͽڵ㴦��û����ɵ���Ϣ����ѹ��ָ���¼�ĵ�һ�����ָ��
���﷨��ջ���ں����ָ�븳ֵ��
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
���ɼ�¼���͵�һ���򣬽ڵ�Ϊ�������͵Ľڵ㣬������κ���Ϣ��
�������ڻ����ͣ���temp��¼��д������Ϣ�ĳ�Ա��ַ���Դ��Ժ���
д���������ͻ����ַ����͡����﷨��ջ����ָ��ָ������ڵ㡣��
�ǵ�һ��������record���͵��ӽڵ�ָ��ǰ�ڵ㣻��������һ��
��¼���������ֵܽڵ㡣���ѹ��ָ���¼������һ�����ָ�룬
�Դ�������
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
* ���ɼ�¼���͵�һ���򣬽ڵ�Ϊ�����ڵ㣬�������������ͣ������
�κ���Ϣ�����﷨��ջ����ָ��ָ������ڵ㣬���ǵ�һ��������record
���͵��ӽ��ָ��ǰ��㣬��������һ����¼���������ֵܽ�㣬
���ѹ��ָ���¼������һ�����ָ�룬�Դ�������
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
* û�м�¼���͵���һ�����ˣ�����ջ����������һ������ֵܽڵ㣬
��ʾ��¼����ȫ�������ꣻ������saveP�ָ���ǰ��¼���ͽڵ��ָ
�롣
*/
void Parser::process25()
{
	ASTStack.pop();
	currentP = (ASTNodeBase*)saveP;
}
/*
�Ҳ������ջ��������ǰ��ʶ�������֣�����ڵ��У���ʶ��������1��
*/
void Parser::process27()
{
	pushSymbolStack({
		GetNonTerminal(NonTerminalType::ID_MORE)
					});
	storeTokenSem(TokenType::IDENTIFIER);
}

/*
�����������������ʽ�Ҳ�ѹ�����ջ���﷨�����֣����ɱ�������
��־�ڵ㣬���﷨��ջ���õ�ָ��ĵ�ַ����ָ��ָ��������ڵ㣬
ʹ�ô˽ڵ���Ϊ���ڵ�Ķ��ӽڵ�������ͱ�ʶ�ڵ���ֵܽڵ��
�֡���ǰ���������ڵ���ֵܽڵ�Ӧ��ָ���������ڵ���������
�ڵ㣬���ӽڵ���Ӧָ�����������ڵ㣬�ʽ���ǰ�ڵ���ֵܽڵ�
�͵�һ�����ӽڵ�ѹ���﷨��ջ���Դ��Ժ���
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
�����ı����������﷨�������֣�����һ���������ͽڵ㣬��
�ڴ�����κ�������Ϣ�����﷨��ջ���õ�ָ��ĵ�ַ����ָ��ָ��
���������ͽڵ㣬���ǵ�һ�������ڵ㣬����VarK���͵��ӽڵ�ָ��
��ǰ�ڵ㣬������Ϊ��һ�������������ֵܽڵ���֡������˽ڵ�
���ֵܽڵ�ѹ���﷨��ջ���Ա㴦����һ������������
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
	tempDecKindPtr = (ASTDecKind*)currentP;
	linkStackTop(currentP);
	ASTStack.push(&currentP->sibling);
	//printTree(root, 0);
}
/*
����������������ɸ���ʶ��������ǰ��ʶ�������֣�����ڵ��У�
��ʶ��������1��
*/
void Parser::process36()
{
	pushSymbolStack({
		GetNonTerminal(NonTerminalType::VAR_ID_MORE)
					});
	storeTokenSem(TokenType::IDENTIFIER);
}
/*
���ɺ���������־�ڵ�
*/
void Parser::process40()
{
	symbolStack.push(GetNonTerminal(NonTerminalType::PROC_DEC));
	currentP = GetASTLabelNode(ASTNodeKind::PROC_K);
	linkStackTop(currentP);
	//printTree(root, 0);
	ASTStack.push(&currentP->child[0]);
}
/*
���������������ʽ�Ҳ�ѹ�����ջ���﷨�����֣����ɹ���ͷ��
��ProcK,���﷨��ջ���õ�ָ��ĵ�ַ����ָ��ָ��������ڵ㣬
ʹ�ô˽ڵ���Ϊ���ڵ�Ķ��ӽڵ�������ͱ�ʶ�ڵ㣬���߱�����ʶ
�ڵ���ֵܽڵ���֡���ǰ���������ڵ���ֵܽڵ�ָ����һ������
�����ڵ��������нڵ㣬���������ӽڵ�ָ������岿�֣��ڶ���
�ӽڵ�ָ������е��������֣���û�����������ָ��Ϊ�գ���һ��
�ӽڵ�ָ�������ββ��֡��ʽ���ǰ�ڵ���ֵܽڵ���������ӽ�
��ѹ���﷨��ջ���Դ��Ժ���
*/
void Parser::process41()
{
	pushSymbolStack({
		GetNonTerminal(NonTerminalType::PROC_DEC_MORE),
		GetNonTerminal(NonTerminalType::PROC_BODY),
		GetNonTerminal(NonTerminalType::PROC_DEC_PART),
		GetTerminal(TokenType::SEMICOLON),
		//GetNonTerminal(NonTerminalType::BASE_TYPE),
		//GetTerminal(TokenType::COLON),
		GetTerminal(TokenType::BRACKET_CLOSE),
		GetNonTerminal(NonTerminalType::PARAM_LIST),
		GetTerminal(TokenType::BRACKET_OPEN),
		GetNonTerminal(NonTerminalType::PROC_NAME),
		GetTerminal(TokenType::PROCEDURE)
					});
	currentP = (ASTNodeBase*)GetASTDecNode(ASTDecKind::PROC_DEC_K);
	linkStackTop(currentP);
	ASTStack.push(&currentP->sibling);
	ASTStack.push(&currentP->child[2]);
	ASTStack.push(&currentP->child[1]);
	ASTStack.push(&currentP->child[0]);
	saveP = (ASTDecNode*)currentP;
}
/*
* ����û�б�Ĳ�����˵���������ββ��ִ�����ϣ��������һ����
���������ֵܽڵ㣻������saveFuncP�ָ���ǰ�ڵ�Ϊ���������ڵ㣬
*/
void Parser::process48()
{
	ASTStack.pop();
	currentP = (ASTNodeBase*)saveP;

}
/*
�����β��������ò���ʽ˵������Ϊֵ�Ρ��﷨�������֣�����һ
���������ͽڵ㣬�ڽڵ�Ĳ������Ͳ���д���βΣ����﷨��ջ����
��ָ��ĵ�ַ����ָ��ָ����������ͽڵ㣬���ǵ�һ�������ڵ㣬
����ProcK���͵ĵ�һ���ӽ��ָ��ǰ��㣻������Ϊ��һ����
���������ֵܽ����֣������˽ڵ���ֵܽڵ�ѹ���﷨��ջ���Ա�
������һ���β�������

*/
void Parser::process50()
{
	pushSymbolStack({
		GetNonTerminal(NonTerminalType::FORM_LIST),
		GetNonTerminal(NonTerminalType::TYPE_DEF)
					});
	currentP = (ASTNodeBase*)GetASTDecNode(ASTDecKind::ID_K);
	((ASTDecNode*)currentP)->decAttr.procAttr.paramType = ASTParamType::VAL_PARAM_TYPE;
	linkStackTop(currentP);
	ASTStack.push(&currentP->sibling);
}
/*
�����β��������������ʽ˵������Ϊ��Ρ��﷨�������֣�����
һ���������ͽڵ㣬�ڽڵ�Ĳ������Ͳ���д�ϱ�Σ����﷨��ջ��
�õ�ָ��ĵ�ַ����ָ��ָ����������ͽڵ㣬���ǵ�һ�������ڵ㣬
����FuncK���͵ĵ�һ���ӽ��ָ��ǰ��㣬������Ϊ��һ����
���������ֵܽ����֡������˽ڵ���ֵܽڵ�ѹ���﷨��ջ���Ա�
������һ���β�������
*/
void Parser::process51()
{
	pushSymbolStack({
		GetNonTerminal(NonTerminalType::FORM_LIST),
		GetNonTerminal(NonTerminalType::TYPE_DEF),
		GetTerminal(TokenType::VAR)
					});
	currentP = (ASTNodeBase*)GetASTDecNode(ASTDecKind::ID_K);
	((ASTDecNode*)currentP)->decAttr.procAttr.paramType = ASTParamType::VAR_PARAM_TYPE;
	linkStackTop(currentP);
	ASTStack.push(&currentP->sibling);
}
/*
�Ҳ�������ջ��������ʶ��������д�뵱ǰ�﷨���ڵ��У��ڵ��б�
ʶ������idnum��1��
*/
void Parser::process52()
{
	pushSymbolStack({
		GetNonTerminal(NonTerminalType::FID_MORE),
					});
	storeTokenSem(TokenType::IDENTIFIER);
}
/*
����������У�����ʽ�Ҳ�ѹ�����ջ���﷨�����֣��ȵ��﷨��ջ��
ɾ��ջ���Ķ���ָ�룬�Ա�֤һ����child[2]ָ��������нڵ㣻
����������б�־�ڵ㣬�˽ڵ��µ����нڵ㶼�����ڵ㡣���﷨
��ջ����ָ��ָ��˱�־�ڵ㣬ʹ�ýڵ���Ϊ���ڵ����ͷ�ڵ��
���һ�����ӽڵ���֡�����ǰ�ڵ�ĵ�һ�����ӽڵ�ѹ���﷨��ջ��
�Դ��Ժ���

*/
void Parser::process57()
{
	pushSymbolStack({
			GetTerminal(TokenType::END),
			GetNonTerminal(NonTerminalType::STM_LIST),
			GetTerminal(TokenType::BEGIN)
					});
	ASTStack.pop();
	currentP = GetASTLabelNode(ASTNodeKind::STM_L_K);
	linkStackTop(currentP);
	ASTStack.push(&currentP->child[0]);

}
/*
��һ��������ͽڵ㣬��������ֵ��Ϊ�������IfK,���﷨��ջ��
����ָ��ĵ�ַ������Ӧָ��ָ����������ڵ㣬��ѹ��if����
�ֵܽڵ�ָ���ַ�Դ�����һ����䡣
*/
void Parser::process61()
{
	symbolStack.push(GetNonTerminal(NonTerminalType::CONDITIONAL_STM));
	currentP = (ASTNodeBase*)GetASTStmtNode(ASTStmtKind::IF_K);
	linkStackTop(currentP);
	ASTStack.push(&currentP->sibling);
}
/*
����һ��������ͽڵ㣬��������ֵ��Ϊѭ�����WhileK,���﷨��
ջ�е���ָ��ĵ�ַ������Ӧָ��ָ���ѭ�����ڵ㣬��ѹ��while
�����ֵܽڵ�ָ���ַ�Դ�����һ����䡣

*/
void Parser::process62()
{
	symbolStack.push(GetNonTerminal(NonTerminalType::LOOP_STM));
	currentP = (ASTNodeBase*)GetASTStmtNode(ASTStmtKind::WHILE_K);
	linkStackTop(currentP);
	ASTStack.push(&currentP->sibling);
}
/*
����һ��������ͽڵ㣬��������ֵ��Ϊ�������ReadK,���﷨��
ջ�е���ָ��ĵ�ַ������Ӧָ��ָ����������ڵ㣬��ѹ��read
�����ֵܽڵ�ָ���ַ�Դ�����һ����䡣
*/
void Parser::process63()
{
	symbolStack.push(GetNonTerminal(NonTerminalType::INPUT_STM));
	currentP = (ASTNodeBase*)GetASTStmtNode(ASTStmtKind::READ_K);
	linkStackTop(currentP);
	ASTStack.push(&currentP->sibling);
}
/*
����һ��������ͽڵ㣬��������ֵ��Ϊ������WriteK,���﷨��
ջ�е���ָ��ĵ�ַ������Ӧָ��ָ���������ڵ㣬��ѹ��write
�����ֵܽڵ�ָ���ַ�Դ�����һ����䡣
*/
void Parser::process64()
{
	symbolStack.push(GetNonTerminal(NonTerminalType::OUTPUT_STM));
	currentP = (ASTNodeBase*)GetASTStmtNode(ASTStmtKind::WRITE_K);
	linkStackTop(currentP);
	ASTStack.push(&currentP->sibling);
}
/*
����һ��������ͽڵ㣬��������ֵ��Ϊ�������ReturnK,���﷨��
ջ�е���ָ��ĵ�ַ������Ӧָ��ָ��˷������ڵ㣬��ѹ��return
�����ֵܽڵ�ָ���ַ�Դ�����һ����䡣
*/
void Parser::process65()
{
	symbolStack.push(GetNonTerminal(NonTerminalType::RETURN_STM));
	currentP = (ASTNodeBase*)GetASTStmtNode(ASTStmtKind::RETURN_K);
	linkStackTop(currentP);
	ASTStack.push(&currentP->sibling);
}
/*
����һ��������ͽڵ㣬���ڸ�ֵ���͹��̵�����䶼�Ա�ʶ����
ʼ�������ڲ���ȷ�������ľ������ͣ�����һ���������ʽ�ڵ㣬
��¼��ֵ�󲿣���ʹ���Ϊ�����ڵ�ĵ�һ�����ӽڵ㣻���﷨��
ջ�е���ָ��ĵ�ַ������Ӧָ��ָ��˷������ڵ㣬��ѹ�����
����ֵܽڵ�ָ���ַ�Դ�����һ����䡣
*/
void Parser::process66()
{
	pushSymbolStack({
			GetNonTerminal(NonTerminalType::ASS_CALL),
					});
	currentP = (ASTNodeBase*)GetASTStmtNode();
	currentP->child[0] = (ASTNodeBase*)GetASTExpNode(ASTEXPKind::ID_K);
	((ASTExpNode*)currentP->child[0])->expAttr.varType = ASTVarType::ID_V;
	auto tmp = currentP;
	currentP = currentP->child[0];
	storeTokenSem(TokenType::IDENTIFIER);
	currentP = tmp;
	linkStackTop(currentP);
	ASTStack.push(&currentP->sibling);
}
/*
����ʽ�Ҳ����ռ��������ջ�����ľ�������ֵȷ��Ϊ��ֵ��䣬
��AssignKд�뵱ǰ����ڵ��С�
*/
void Parser::process67()
{
	pushSymbolStack({
			GetNonTerminal(NonTerminalType::ASSIGNMENT_REST),
					});
	((ASTStmtNode*)currentP)->stmtKind = ASTStmtKind::ASSIGN_K;

}
/*
��ʽ�Ҳ����ռ��������ջ�������ľ�������ȷ��Ϊ��־��������
���ľ�������ֵȷ��Ϊ���̵�����䣬��CallKд�뵱ǰ����ڵ�
�С�
*/
void Parser::process68()
{
	pushSymbolStack({
			GetNonTerminal(NonTerminalType::CALL_STM_REST)
					});
	((ASTStmtNode*)currentP)->stmtKind = ASTStmtKind::CALL_K;
	((ASTExpNode*)currentP->child[0])->expAttr.varType = ASTVarType::ID_V;
}
/*

��ֵ���ڵ�ĸ�ֵ�Ҳ��ӽڵ�ָ��ѹջ���ı䵱ǰ�ڵ�ָ��Ϊ��ֵ
���ӽڵ㡣ѹ������ջ�ױ�־��
*/
void Parser::process69()
{
	pushSymbolStack({
			GetNonTerminal(NonTerminalType::EXP),
			GetTerminal(TokenType::ASSIGN),
			GetNonTerminal(NonTerminalType::VARI_MORE)
					});
	ASTStack.push(&currentP->child[1]);
	currentP = currentP->child[0];
	initOpStack();
	getExpResult = true;
}
/*

if������������֣��������ʽ���֣�then���ֺ�else���֣��ֱ���
�������ӽڵ�ָ����Ӧ�Ĳ��֡��ʽ��������ӽڵ㰴���������﷨��
ջ���Դ��Ժ�ֵ��
*/
void Parser::process70()
{
	pushSymbolStack({
			GetTerminal(TokenType::FI),
			GetNonTerminal(NonTerminalType::STM_LIST),
			GetTerminal(TokenType::ELSE),
			GetNonTerminal(NonTerminalType::STM_LIST),
			GetTerminal(TokenType::THEN),
			GetNonTerminal(NonTerminalType::REL_EXP),
			GetTerminal(TokenType::IF)
					});
	ASTStack.push(&currentP->child[2]);
	ASTStack.push(&currentP->child[1]);
	ASTStack.push(&currentP->child[0]);
}

/*
while����������֣��������ʽ��ѭ���塣�ֱ����������ӽڵ�ָ��
ָ�򡣽��������ӽڵ�ָ���ַ���մ�����������ѹ���﷨��ջ��
*/
void Parser::process71()
{
	pushSymbolStack({
			GetTerminal(TokenType::END_WHILE),
			GetNonTerminal(NonTerminalType::STM_LIST),
			GetTerminal(TokenType::DO),
			GetNonTerminal(NonTerminalType::REL_EXP),
			GetTerminal(TokenType::WHILE)
					});
	ASTStack.push(&currentP->child[1]);
	ASTStack.push(&currentP->child[0]);
}
/*
������ڵ�ĵ�һ�����ӽڵ�Ӧָ����ʽ�����ĸ��ڵ㡣���Ƚ�
������ĵ�һ�����ӽڵ�ָ���ַѹ�﷨��ջ�����⣬Ҫ������
ʽ����Ӧ���ȳ�ʼ�����ʽ������Ҫ�Ĳ�����ջ��ѹ��һ�������
�����������������ȼ���͡�������ջ�Ͳ�����ջ�д�ŵĶ���ָ��
���ڵ�ָ�룬����ɱ��ʽ�����ڵ�����ӡ�����������һ����"END"
��Ϊ���ݵĲ������ڵ㣬�ٽ���ָ��ѹ�������ջ��ʵ�ֳ�ʼ�����ܡ�

*/
void Parser::process74()
{
	pushSymbolStack({
			GetTerminal(TokenType::BRACKET_CLOSE),
			GetNonTerminal(NonTerminalType::EXP),
			GetTerminal(TokenType::BRACKET_OPEN),
			GetTerminal(TokenType::WRITE)
					});
	ASTStack.push(&currentP->child[0]);
	initOpStack();
	getExpResult = true;
}
/*
������������䣬����ѹ�뺯���������ĵ�һ(����)�����ӽڵ㡣
*/
void Parser::process76()
{
	pushSymbolStack({
			GetTerminal(TokenType::BRACKET_CLOSE),
			GetNonTerminal(NonTerminalType::ACT_PARAM_LIST),
			GetTerminal(TokenType::BRACKET_OPEN)
					});
	ASTStack.push(&currentP->child[1]);
}
/*
������������ʵ���Ǳ��ʽ����Ҫ������ʽ�����ȳ�ʼ������
��ջ������һ����������ȼ���͵Ĳ�������
*/
void Parser::process78()
{
	pushSymbolStack({
			GetNonTerminal(NonTerminalType::ACT_PARAM_MORE),
			GetNonTerminal(NonTerminalType::EXP)
					});
	initOpStack();
	getExpResult = true;
}
/*
���б��ʵ�Σ�����ʽ�Ҳ������ջ��ѹ��ǰʵ�νڵ���ֵܽ�
�����﷨��ջ��
*/
void Parser::process80()
{
	pushSymbolStack({
			GetNonTerminal(NonTerminalType::ACT_PARAM_LIST),
			GetTerminal(TokenType::COMMA)
					});
	ASTStack.push(&currentP->sibling);
}
/*
�����������ʽ����if��while����н��õ��������ʽ���������
���ʽ�Ĳ�����ջ���г�ʼ����ѹ��һ�����ȼ���͵Ĳ��������ͽ�
���ָ�롣���⣬������getExpResult��ֵΪ�٣�һ����ʽ������
��process84ʱ�����ʽ����������������ʽ�Ĳ�����ջ�Ͳ�����
ջ���õ����ʽ���ֵ����ṹ������������У������ڹ�ϵ���ʽ��
��ϵ������󲿵ı��ʽ����������������ʽ��δ�����꣬����ջ��
�������Ҳ��ı��ʽʱ���Ž�����

*/
void Parser::process81()
{
	pushSymbolStack({
			GetNonTerminal(NonTerminalType::OTHER_REL_E),
			GetNonTerminal(NonTerminalType::EXP)
					});
	initOpStack();
	getExpResult = false;
}
/*
����һ�����������ͱ��ʽ�ڵ㣬��¼�����ϵ����������ݡ��Ƚ�
ջ������������������������ȼ�����ջ�������������ȼ����ڻ���
���ڵ�ǰ��������Ӳ�����ջ����һ����������ָ��t,�Ӳ�����ջ��
�������������ڵ�ָ�룬�ֱ���Ϊt���������ӽڵ㣬�ٽ��������
��t�Ľڵ�ָ��ѹ�������ջ����Ϊ��һ�����������������������
ǰ������ָ��ѹ�������ջ������ֱ�ӽ���ǰָ��ѹ�������ջ��
���⣬ѡ�����������ʽ��˵����Ϊ��ϵ�����������ı��ʽExp,
�Ѿ������꣬Ҫ�����ҷ����Ĵ�����Ϊ�ҷ�����Exp����������
��ϵ���ʽҲ�������������ں���process84�н������õ����ʽ��
����ʶgetExpResultΪ�档
*/
void Parser::process82()
{
	pushSymbolStack({
			GetNonTerminal(NonTerminalType::EXP),
			GetNonTerminal(NonTerminalType::CMP_OP)
					});

	currentP = (ASTNodeBase*)GetASTExpOpNode(TokenType2OpType(currentT.type));
	while (GetOpPriority(operatorStack.top()->expAttr.op)
		   >= GetOpPriority(((ASTExpNode*)currentP)->expAttr.op)
		   )
	{
		auto t = operatorStack.top();
		operatorStack.pop();
		auto rNum = operandStack.top();
		operandStack.pop();
		auto lNum = operandStack.top();
		operandStack.pop();
		t->nodeBase.child[0] = (ASTNodeBase*)lNum;
		t->nodeBase.child[1] = (ASTNodeBase*)rNum;
		operandStack.push(t);
	}
	operatorStack.push((ASTExpNode*)currentP);
	getExpResult = true;
}
/*
����write,return�������������)��Ϊ�������ʽ�Ľ��������ұ��
ʽ�������ֿ�����(���ʽ),�������������ţ���Ҫ���������
�����ǰ��������Ϊ������),����expflag��Ϊ0,˵��ǰ����
��֮��Ե�������(,��ʱ���������Ǳ��ʽ��һ���֣�ѭ��������
��һ��������ջ���ݣ���������������ջ���ݣ��Բ������ڵ������
���ӽڵ㸳ֵ�������ڵ�ָ�룬��������Ϊ��������������������
������ָ��ѹ�������ջ����Ϊ��һ����������������������ѭ����
ֱ��������ջ��Ϊ(,����(,����expflag��1,˵����������ʽ
�е�һ�����ţ�������
���򣬵�ǰ�������ʱ�ʶ��exp�Ĵ���������жϣ����
getExpResultΪ�����getExpResult2Ϊ�棬�����ǰ���ʽ�����
����ѭ����������һ��������ջ���ݣ���������������ջ���ݣ�����
�����������ҷ��������ѽ��ָ��ѹ�������ջ��ֱ������������ջ
�ױ�ʶEND,����ջ�ױ�ʶ�����ʽ����������ϣ����ڵ��ڲ�����
ջ�У��������������﷨��ջ�е���Ӧָ����ʽ�������ڵ��ָ��
��ַ����ֵָ��ָ����ʽ���ڵ㡣
���У�getExpResult2ר���������Ա�������������Ա���ʽ��
�����ʱ����Ҫ��������������ڵ������������Ա���ʽ�ĸ����
��Ϊ�����Ա�����ڵ�Ķ��ӽڵ㡣

*/
void Parser::process84()
{
	//printTree(root, 0);
	if (!(currentT.type == TokenType::BRACKET_CLOSE && expFlag != 0))
	{
		//���ʽ����
		if (getExpResult || getExpResult2)
		{
			while (operatorStack.top()->expAttr.op != ASTOpType::STACK_END)
			{
				auto t = operatorStack.top();
				operatorStack.pop();
				auto rNum = operandStack.top();
				operandStack.pop();
				auto lNum = operandStack.top();
				operandStack.pop();
				t->nodeBase.child[0] = (ASTNodeBase*)lNum;
				t->nodeBase.child[1] = (ASTNodeBase*)rNum;
				operandStack.push(t);
			}
			delete operatorStack.top();
			operatorStack.pop();
			currentP = (ASTNodeBase*)operandStack.top();
			operandStack.pop();
			linkStackTop(currentP);
			
			//if (getExpResult && !getExpResult2) getExpResult = false;
			//else getExpResult2 = false;
			getExpResult2 = false;
			//printTree(root, 0);
		}
	}
	else
	{
		//�����ű��ʽ
		while (operatorStack.top()->expAttr.op != ASTOpType::BRACKET_OPEN)
		{
			auto t = operatorStack.top();
			operatorStack.pop();
			auto rNum = operandStack.top();
			operandStack.pop();
			auto lNum = operandStack.top();
			operandStack.pop();
			t->nodeBase.child[0] = (ASTNodeBase*)lNum;
			t->nodeBase.child[1] = (ASTNodeBase*)rNum;
			operandStack.push(t);
		}
		delete operatorStack.top();
		operatorStack.pop();
		expFlag--;

	}
}
/*
�����ӷ������������һ�����ʽ�ڵ㣬���������ǲ��������ͣ���
¼����ӷ�����������ݡ�����ǰ������ջ�����ݣ���ջ����������
���ȼ����ڻ���ڵ�ǰ��������Ӳ�����ջ����һ����������ָ��t,
�Ӳ�����ջ���������������ڵ�ָ�룬�ֱ���Ϊt���������ӽڵ㣬
�ٽ����������t�Ľڵ�ָ��ѹ�������ջ����Ϊ��һ������������
�����������ǰ������ָ��ѹ�������ջ������ֱ�ӽ���ǰָ��ѹ
�������ջ��

*/
void Parser::process85()
{
	pushSymbolStack({
			GetNonTerminal(NonTerminalType::EXP),
			GetNonTerminal(NonTerminalType::ADD_OP)
					});
	currentP = (ASTNodeBase*)GetASTExpOpNode(TokenType2OpType(currentT.type));
	while (GetOpPriority(operatorStack.top()->expAttr.op)
		   >= GetOpPriority(((ASTExpNode*)currentP)->expAttr.op)
		   )
	{
		auto t = operatorStack.top();
		operatorStack.pop();
		auto rNum = operandStack.top();
		operandStack.pop();
		auto lNum = operandStack.top();
		operandStack.pop();
		t->nodeBase.child[0] = (ASTNodeBase*)lNum;
		t->nodeBase.child[1] = (ASTNodeBase*)rNum;
		operandStack.push(t);
	}
	operatorStack.push((ASTExpNode*)currentP);
	//getExpResult = true;
}
/*
�����˷������������һ�����ʽ�ڵ㣬���������ǲ��������ͣ���
¼����˷�����������ݡ�����ǰ������ջ�����ݣ�����*, /˵��ջ
�������������ȼ����ڵ�ǰ��������Ӳ�����ջ����һ����������ָ
��t, �Ӳ�����ջ���������������ڵ�ָ�룬�ֱ���Ϊt���������ӽ�
�㣬�ٽ����������t�Ľڵ�ָ��ѹ�������ջ����Ϊ��һ��������
���������������ǰ������ָ��ѹ�������ջ����֤���ʽ�ļ�����
�����ϡ�����ֱ�ӽ���ǰָ��ѹ�������ջ��
*/
void Parser::process88()
{
	pushSymbolStack({
			GetNonTerminal(NonTerminalType::MULTI_OP),
			GetNonTerminal(NonTerminalType::TERM)
					});
	currentP = (ASTNodeBase*)GetASTExpOpNode(TokenType2OpType(currentT.type));
	while (GetOpPriority(operatorStack.top()->expAttr.op)
		   >= GetOpPriority(((ASTExpNode*)currentP)->expAttr.op)
		   )
	{
		auto t = operatorStack.top();
		operatorStack.pop();
		auto rNum = operandStack.top();
		operandStack.pop();
		auto lNum = operandStack.top();
		operandStack.pop();
		t->nodeBase.child[0] = (ASTNodeBase*)lNum;
		t->nodeBase.child[1] = (ASTNodeBase*)rNum;
		operandStack.push(t);
	}
	operatorStack.push((ASTExpNode*)currentP);
}
/*
���ʽ������Ϊ�����ŵı��ʽ����ǰ����Ϊ�����ţ�������ҲҪ��
������ջ�����ԣ�����һ�����������ʽ�ڵ㣬���ݸ�ֵΪ������(,
ָ��ѹ�������ջ�У�����expflag��1,������expflag��ô���
(δ��ԡ�
*/
void Parser::process89()
{
	pushSymbolStack({
			GetTerminal(TokenType::BRACKET_CLOSE),
			GetNonTerminal(NonTerminalType::EXP),
			GetTerminal(TokenType::BRACKET_OPEN)
					});
	currentP = (ASTNodeBase*)GetASTExpOpNode(ASTOpType::BRACKET_OPEN);
	operatorStack.push((ASTExpNode*)currentP);
	expFlag++;
}
/*
����һ�����������ʽ�ڵ㣬��������Ϊ�������ʽ����������ֵд
��ڵ��У���������������ڵ�ָ��ѹ�������ջ��
*/
void Parser::process90()
{
	pushSymbolStack({
			GetTerminal(TokenType::INT),
					});
	currentP = (ASTNodeBase*)GetASTExpConstNode();
	((ASTExpNode*)currentP)->expAttr.val = std::stoi(currentT.sem);
	operandStack.push((ASTExpNode*)currentP);
}
/*
����һ�������ı��ʽ�ڵ㣬����־�����ּ���ڵ��У������˽ڵ�
ѹ�������ջ��
*/
void Parser::process92()
{
	pushSymbolStack({
			GetNonTerminal(NonTerminalType::VARI_MORE)
					});
	currentP = (ASTNodeBase*)GetASTExpVarNode();
	storeTokenSem(TokenType::IDENTIFIER);
	operandStack.push((ASTExpNode*)currentP);
}
/*
�������ʽ�ľ��������Ǳ�־�������������������Ϣ���뵱ǰ����
���ʽ�ڵ㡣
*/
void Parser::process93()
{
	((ASTExpNode*)currentP)->expAttr.varType = ASTVarType::ID_V;
}
/*
��ʽ�Ҳ������ջ�������ľ�����������Ϊ�����Ա������ѹ��ָ
�������Ա���ʽ�Ķ��ӽڵ�ָ�룬ѹ�����������ȼ�������ջ��
��־��
*/
void Parser::process94()
{
	pushSymbolStack({
			GetTerminal(TokenType::SQUARE_BRACKET_CLOSE),
			GetNonTerminal(NonTerminalType::EXP),
			GetTerminal(TokenType::SQUARE_BRACKET_OPEN)
					});
	((ASTExpNode*)currentP)->expAttr.varType = ASTVarType::ARRAY_MEMB_V;
	ASTStack.push(&currentP->child[0]);
	//initOpStack();
	operatorStack.push(GetASTExpOpNode(ASTOpType::STACK_END));
	getExpResult2 = true;
}
/*
����ʽ�Ҳ������ջ����ǰ�����ڵ�ľ�����������Ϊ���Ա���ͱ�
����ѹ�����Ա�����ڵ�Ķ��ӽڵ㣬�Դ������Ա���ʽ��
*/
void Parser::process95()
{
	pushSymbolStack({
			GetNonTerminal(NonTerminalType::FIELD_VAR),
			GetTerminal(TokenType::DOT)
					});
	((ASTExpNode*)currentP)->expAttr.varType = ASTVarType::FIELD_MEMB_V;
	ASTStack.push(&currentP->child[0]);
}
/*
����һ�������ı��ʽ�ڵ㣻����־��������ڵ��У����﷨��ջ��
�õ�������Ķ��ӽڵ�ָ�룬ָ������Ա�ڵ㡣
*/
void Parser::process96()
{
	pushSymbolStack({
			GetNonTerminal(NonTerminalType::FIELD_VAR_MORE)
					});
	currentP = (ASTNodeBase*)GetASTExpVarNode();
	storeTokenSem(TokenType::IDENTIFIER);
	linkStackTop(currentP);
}
/*
˵���������ʽ�ľ��������Ǳ�־�������������������Ϣ���뵱ǰ
�������ʽ�ڵ㡣
*/
void Parser::process97()
{
	((ASTExpNode*)currentP)->expAttr.varType = ASTVarType::ID_V;
}




































































