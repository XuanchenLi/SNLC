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

ASTNodeBase* Parser::parse(TokenList& tokenList)
{
	if (!tokenList.hasNext())
	{
		return root;
	}
	Token token = tokenList.getCurToken();
	currentP->lineNum = token.lineNum;
	while (!symbolStack.empty())
	{
		if (symbolStack.top().symbolType == SymbolType::TERMINAL)
		{
			if (token.type == symbolStack.top().symbolName.tokenType)
			{
				symbolStack.pop();
				if (!tokenList.hasNext())
				{
					throw std::exception(
						"ERROR: Unexcept end of tokens."
					);
				}
				tokenList.moveNext();
				token = tokenList.getCurToken();
				currentP->lineNum = token.lineNum;
			}
			else
			{
				throw std::exception(
					("ERROR: Except to get: " + std::string(TokenTypeName(symbolStack.top().symbolName.tokenType))
					 + " but got: " + TokenTypeName(token.type) + " at line " + std::to_string(token.lineNum)
					 + " column " + std::to_string(token.colNum)).c_str()
				);
			}
		}
		else
		{
			NonTerminalType line = symbolStack.top().symbolName.nonTerminalType;
			TokenType col = token.type;
			std::function<void()> preFun = predictTable[PredictTableKey(line, col)];
			symbolStack.pop();
			preFun();
		}

	}
	if (token.type != TokenType::EOF_) {
		throw std::exception(("ERROR: Unexcept end of file at line " + std::to_string(token.lineNum)).c_str());
	}
	return root;
}

void Parser::initPredictTable()
{
	predictTable.clear();
	insertPredictTable(NonTerminalType::PROGRAM,
					   { TokenType::PROGRAM },
					   std::function<void()>([this]
											 {
												 pushSymbolStack({ GetNonTerminal(NonTerminalType::PROGRAM_BODY),
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
			{}
				)
	);
	insertPredictTable(
		NonTerminalType::TYPE_DEC,
		{ TokenType::TYPE},
		std::function<void()>(
			[this]
			{
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
			}
				)
	);
	insertPredictTable(
		NonTerminalType::ID_MORE,
		{ TokenType::COMMA },
		std::function<void()>(
			[this]
			{
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
			}
				)
	);
	insertPredictTable(
		NonTerminalType::VAR_DEC,
		{ TokenType::VAR },
		std::function<void()>(
			[this]
			{
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
				this->process36();
			}
				)
	);
	insertPredictTable(
		NonTerminalType::VAR_ID_MORE,
		{ TokenType::SEMICOLON },
		std::function<void()>(
			[]
			{
			}
				)
	);
	insertPredictTable(
		NonTerminalType::VAR_ID_MORE,
		{ TokenType::COMMA},
		std::function<void()>(
			[this]
			{
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
			}
				)
	);
	insertPredictTable(
		NonTerminalType::PROC_DECPART,
		{ TokenType::PROCEDURE },
		std::function<void()>(
			[this]
			{
				pushSymbolStack({
					GetNonTerminal(NonTerminalType::PROC_DEC)
								});
			}
				)
	);
	insertPredictTable(
		NonTerminalType::PROC_DEC,
		{ TokenType::PROCEDURE },
		std::function<void()>(
			[this]
			{
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
			}
				)
	);
	insertPredictTable(
		NonTerminalType::PROC_DEC_MORE,
		{ TokenType::PROCEDURE },
		std::function<void()>(
			[this]
			{
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
				pushSymbolStack({
					GetNonTerminal(NonTerminalType::PARAM_LIST)
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
				pushSymbolStack({
					GetNonTerminal(NonTerminalType::PARAM_MORE),
					GetNonTerminal(NonTerminalType::PARAM)
								});
			}
				)
	);
	insertPredictTable(
		NonTerminalType::PARAM_MORE,
		{ TokenType::BRACKET_OPEN },
		std::function<void()>(
			[this]
			{
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
			}
				)
	);
	insertPredictTable(
		NonTerminalType::FID_MORE,
		{ TokenType::COMMA },
		std::function<void()>(
			[this]
			{
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
				process65();
			}
				)
	);
	insertPredictTable(
		NonTerminalType::STM,
		{ TokenType::IDENTIFIER },
		std::function<void()>(
			[this]
			{
				process66();
			}
				)
	);
	insertPredictTable(
		NonTerminalType::ASS_CALL,
		{ TokenType::ASSIGN },
		std::function<void()>(
			[this]
			{
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
			}
				)
	);
	insertPredictTable(
		NonTerminalType::ACT_PARAM_MORE,
		{ TokenType::COMMA },
		std::function<void()>(
			[this]
			{
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

			}
				)
	);
	insertPredictTable(
		NonTerminalType::OTHER_FACTOR,
		{ TokenType::MULTIPLY, TokenType::DIVIDE },
		std::function<void()>(
			[this]
			{
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
				process96();
			}
				)
	);
	insertPredictTable(
		NonTerminalType::FIELD_VAR_MORE,
		{ TokenType::IDENTIFIER },
		std::function<void()>(
			[this]
			{
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
声明的类型部分赋值为记录类型RecordK,用变量saveP保存当前指向
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
		GetNonTerminal(NonTerminalType::ID_MORE)
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
		GetNonTerminal(NonTerminalType::VAR_ID_MORE)
					});
	storeTokenSem(TokenType::IDENTIFIER);
}
/*
生成函数声明标志节点
*/
void Parser::process40()
{
	symbolStack.push(GetNonTerminal(NonTerminalType::PROC_DEC));
	currentP = GetASTLabelNode(ASTNodeKind::PROC_DEC_K);
	linkStackTop(currentP);
	ASTStack.push(&currentP->child[0]);
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
		//GetNonTerminal(NonTerminalType::BASE_TYPE),
		//GetTerminal(TokenType::COLON),
		GetTerminal(TokenType::BRACKET_CLOSE),
		GetNonTerminal(NonTerminalType::PARAM_LIST),
		GetTerminal(TokenType::BRACKET_OPEN),
		GetNonTerminal(NonTerminalType::PROC_NAME),
		GetTerminal(TokenType::PROCEDURE)
					});
	currentP = (ASTNodeBase*)GetASTDecNode(ASTDecKind::PROC_K);
	linkStackTop(currentP);
	ASTStack.push(&currentP->sibling);
	ASTStack.push(&currentP->child[2]);
	ASTStack.push(&currentP->child[1]);
	ASTStack.push(&currentP->child[0]);
	saveP = (ASTDecNode*)currentP;
}
/*
* 后面没有别的参数，说明函数的形参部分处理完毕，弹出最后一个形
参声明的兄弟节点；并利用saveFuncP恢复当前节点为函数声明节点，
*/
void Parser::process48()
{
	ASTStack.pop();
	currentP = (ASTNodeBase*)saveP;

}
/*
进入形参声明。该产生式说明参数为值参。语法树处理部分，生成一
个声明类型节点，在节点的参数类型部分写上形参，弹语法树栈，得
到指针的地址，令指针指向此声明类型节点，若是第一个声明节点，
则是ProcK类型的第一个子结点指向当前结点；否则，作为上一个形
参声明的兄弟结点出现，并将此节点的兄弟节点压入语法树栈，以便
处理下一个形参声明。

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
进入形参声明。这个产生式说明参数为变参。语法树处理部分，生成
一个声明类型节点，在节点的参数类型部分写上变参，弹语法树栈，
得到指针的地址，令指针指向此声明类型节点，若是第一个声明节点，
则是FuncK类型的第一个子结点指向当前结点，否则，作为上一个形
参声明的兄弟结点出现。并将此节点的兄弟节点压入语法树栈，以便
处理下一个形参声明。
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
右部进符号栈，并将标识符的名字写入当前语法树节点中，节点中标
识符个数idnum加1。
*/
void Parser::process52()
{
	pushSymbolStack({
		GetNonTerminal(NonTerminalType::FID_MORE),
					});
	storeTokenSem(TokenType::IDENTIFIER);
}
/*
处理语句序列，产生式右部压入符号栈；语法树部分，先弹语法树栈，
删除栈项保存的多余指针，以保证一定是child[2]指向语句序列节点；
生成语句序列标志节点，此节点下的所有节点都是语句节点。弹语法
树栈，令指针指向此标志节点，使得节点作为根节点或函数头节点的
最后一个儿子节点出现。将当前节点的第一个儿子节点压入语法树栈，
以待以后处理。

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
建一个语句类型节点，具体类型值设为条件语句IfK,从语法树栈中
弹出指针的地址，令相应指针指向此条件语句节点，并压入if语句的
兄弟节点指针地址以处理下一条语句。
*/
void Parser::process61()
{
	symbolStack.push(GetNonTerminal(NonTerminalType::CONDITIONAL_STM));
	currentP = (ASTNodeBase*)GetASTStmtNode(ASTStmtKind::IF_K);
	linkStackTop(currentP);
	ASTStack.push(&currentP->sibling);
}
/*
创建一个语句类型节点，具体类型值设为循环语句WhileK,从语法树
栈中弹出指针的地址，令相应指针指向此循环语句节点，并压入while
语句的兄弟节点指针地址以处理下一条语句。

*/
void Parser::process62()
{
	symbolStack.push(GetNonTerminal(NonTerminalType::LOOP_STM));
	currentP = (ASTNodeBase*)GetASTStmtNode(ASTStmtKind::WHILE_K);
	linkStackTop(currentP);
	ASTStack.push(&currentP->sibling);
}
/*
创建一个语句类型节点，具体类型值设为输入语句ReadK,从语法树
栈中弹出指针的地址，令相应指针指向此输入语句节点，并压入read
语句的兄弟节点指针地址以处理下一条语句。
*/
void Parser::process63()
{
	symbolStack.push(GetNonTerminal(NonTerminalType::INPUT_STM));
	currentP = (ASTNodeBase*)GetASTStmtNode(ASTStmtKind::READ_K);
	linkStackTop(currentP);
	ASTStack.push(&currentP->sibling);
}
/*
创建一个语句类型节点，具体类型值设为输出语句WriteK,从语法树
栈中弹出指针的地址，令相应指针指向此输出语句节点，并压入write
语句的兄弟节点指针地址以处理下一条语句。
*/
void Parser::process64()
{
	symbolStack.push(GetNonTerminal(NonTerminalType::OUTPUT_STM));
	currentP = (ASTNodeBase*)GetASTStmtNode(ASTStmtKind::WRITE_K);
	linkStackTop(currentP);
	ASTStack.push(&currentP->sibling);
}
/*
创建一个语句类型节点，具体类型值设为返回语句ReturnK,从语法树
栈中弹出指针的地址，令相应指针指向此返回语句节点，并压入return
语句的兄弟节点指针地址以处理下一条语句。
*/
void Parser::process65()
{
	symbolStack.push(GetNonTerminal(NonTerminalType::RETURN_STM));
	currentP = (ASTNodeBase*)GetASTStmtNode(ASTStmtKind::RETURN_K);
	linkStackTop(currentP);
	ASTStack.push(&currentP->sibling);
}
/*
创建一个语句类型节点，由于赋值语句和过程调用语句都以标识符开
始，故现在不能确定此语句的具体类型；创建一个变量表达式节点，
记录赋值左部，并使其成为此语句节点的第一个儿子节点；从语法树
栈中弹出指针的地址，令相应指针指向此返回语句节点，并压入此语
句的兄弟节点指针地址以处理下一条语句。
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
产生式右部非终极符入符号栈；语句的具体类型值确定为赋值语句，
将AssignK写入当前处理节点中。
*/
void Parser::process67()
{
	pushSymbolStack({
			GetNonTerminal(NonTerminalType::ASSIGNMENT_REST),
					});
	((ASTStmtNode*)currentP)->stmtKind = ASTStmtKind::ASSIGN_K;

}
/*
生式右部非终极符入符号栈；变量的具体类型确定为标志符变量；
语句的具体类型值确定为过程调用语句，将CallK写入当前处理节点
中。
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

赋值语句节点的赋值右部子节点指针压栈，改变当前节点指针为赋值
左部子节点。压入特殊栈底标志。
*/
void Parser::process69()
{
	ASTStack.push(&currentP->child[1]);
	currentP = currentP->child[0];
	initOpStack();
}
/*

if语句有三个部分，条件表达式部分，then部分和else部分，分别用
三个儿子节点指向相应的部分。故将三个儿子节点按照逆序入语法树
栈，以待以后赋值。
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
while语句有两部分，条件表达式和循环体。分别用两个儿子节点指针
指向。将两个儿子节点指针地址按照处理次序的逆序压入语法树栈中
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
输出语句节点的第一个儿子节点应指向表达式子树的根节点。故先将
输出语句的第一个儿子节点指针地址压语法树栈。另外，要进入表达
式处理，应该先初始化表达式处理需要的操作符栈，压入一个特殊的
操作符，令它的优先级最低。操作符栈和操作数栈中存放的都是指向
树节点指针，以完成表达式子树节点的联接。这里先生成一个以"END"
作为内容的操作符节点，再将其指针压入操作符栈，实现初始化功能。

*/
void Parser::process74()
{
	pushSymbolStack({
			GetTerminal(TokenType::WRITE),
			GetTerminal(TokenType::BRACKET_OPEN),
			GetNonTerminal(NonTerminalType::EXP),
			GetTerminal(TokenType::BRACKET_CLOSE)
					});
	ASTStack.push(&currentP->child[0]);
	initOpStack();
}
/*
处理函数调用语句，首先压入函数调用语句的第一(二？)个儿子节点。
*/
void Parser::process76()
{
	ASTStack.push(&currentP->child[1]);
}
/*
函数调用语句的实参是表达式。将要进入表达式处理，先初始化操作
符栈，放入一个特殊的优先级最低的操作符。
*/
void Parser::process78()
{
	pushSymbolStack({
			GetNonTerminal(NonTerminalType::ACT_PARAM_MORE),
			GetNonTerminal(NonTerminalType::EXP)
					});
	initOpStack();
}
/*
还有别的实参，产生式右部入符号栈；压当前实参节点的兄弟节
点入语法树栈。
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
处理条件表达式，在if和while语句中将用到条件表达式，在这里对
表达式的操作符栈进行初始化，压入一个优先级最低的操作符类型节
点的指针。另外，将变量getExpResult赋值为假，一般表达式，在遇
到process840时，表达式处理结束，弹出表达式的操作数栈和操作符
栈，得到表达式部分的树结构，并连入语句中；而对于关系表达式，
关系运算符左部的表达式处理结束，整个表达式并未处理完，不弹栈，
处理完右部的表达式时，才结束。

*/
void Parser::process81()
{
	pushSymbolStack({
			GetNonTerminal(NonTerminalType::OTHER_REL_E),
			GetNonTerminal(NonTerminalType::EXP)
					});
	initOpStack();
	getExpResault = false;
}
/*
建立一个操作符类型表达式节点，记录这个关系运算符的内容。比较
栈顶操作符和这个操作符的优先级，若栈顶操作符的优先级高于或者
等于当前运算符，从操作符栈弹出一个操作符的指针t,从操作数栈弹
出两个操作数节点指针，分别作为t的两个儿子节点，再将这个操作
符t的节点指针压入操作数栈，作为下一个操作符的运算分量，将当
前操作符指针压入操作符栈。否则，直接将当前指针压入操作符栈。
另外，选择了这个产生式，说明作为关系运算符左分量的表达式Exp,
已经处理完，要进入右分量的处理，作为右分量的Exp结束后，整个
关系表达式也结束，故设置在函数process840中结束，得到表达式结
果标识getExpResult为真。
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
	getExpResault = true;
}
/*
由于write,return等语句以右括号)作为参数表达式的结束，而且表达
式的因子又可以是(表达式),所以遇到右括号，需要分情况处理：
如果当前所读单词为右括号),而且expflag不为0,说明前面有
与之配对的左括号(,这时的右括号是表达式的一部分，循环做：弹
出一个操作符栈内容，弹出两个操作数栈内容，对操作符节点的两个
儿子节点赋值操作数节点指针，将他们作为左右运算分量，并把这个
操作符指针压入操作数栈，作为下一个操作符的运算分量，如此循环，
直到操作符栈顶为(,弹出(,并将expflag减1,说明处理完表达式
中的一对括号，结束。
否则，当前所读单词标识了exp的处理结束，判断：如果
getExpResult为真或者getExpResult2为真，则代表当前表达式处理结
束，循环做：弹出一个操作符栈内容，弹出两个操作数栈内容，连接
操作符的左右分量，并把结果指针压入操作数栈。直到遇到操作符栈
底标识END,弹出栈底标识，表达式子树生成完毕，根节点在操作数
栈中，弹出来，并从语法树栈中弹出应指向表达式子树根节点的指针
地址，赋值指针指向表达式根节点。
其中，getExpResult2专用于数组成员变量，当数组成员表达式处
理结束时，需要将其与数组变量节点相连，数组成员表达式的根结点
作为数组成员变量节点的儿子节点。

*/
void Parser::process84()
{
	if (currentT.type == TokenType::BRACKET_CLOSE && expFlag == 0)
	{
		//表达式结束
		if (getExpResault || getExpResault2)
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
			getExpResault2 = false;
		}
	}
	else
	{
		//子括号表达式
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
遇到加法运算符，建立一个表达式节点，具体类型是操作符类型，记
录这个加法运算符的内容。看当前操作符栈顶内容，若栈顶操作符的
优先级高于或等于当前运算符，从操作符栈弹出一个操作符的指针t,
从操作数栈弹出两个操作数节点指针，分别作为t的两个儿子节点，
再将这个操作符t的节点指针压入操作数栈，作为下一个操作符的运
算分量，将当前操作符指针压入操作符栈。否则，直接将当前指针压
入操作符栈。

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
}
/*
遇到乘法运算符，建立一个表达式节点，具体类型是操作符类型，记
录这个乘法运算符的内容。看当前操作符栈顶内容，若是*, /说明栈
顶操作符的优先级等于当前运算符，从操作符栈弹出一个操作符的指
针t, 从操作数栈弹出两个操作数节点指针，分别作为t的两个儿子节
点，再将这个操作符t的节点指针压入操作数栈，作为下一个操作符
的运算分量，将当前操作符指针压入操作符栈。保证表达式的计算属
于左结合。否则，直接将当前指针压入操作符栈。
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
表达式的因子为带括号的表达式，当前单词为左括号，左括号也要进
操作符栈。所以，建立一个操作符表达式节点，内容赋值为左括号(,
指针压入操作符栈中，并将expflag加1,代表还有expflag这么多的
(未配对。
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
创建一个操作数表达式节点，具体类型为常数表达式，将整数的值写
入节点中，将这个常操作数节点指针压入操作数栈。
*/
void Parser::process90()
{
	pushSymbolStack({
			GetTerminal(TokenType::INTEGER),
					});
	currentP = (ASTNodeBase*)GetASTExpConstNode();
	((ASTExpNode*)currentP)->expAttr.val = std::stoi(currentT.sem);
	operandStack.push((ASTExpNode*)currentP);
}
/*
创建一个变量的表达式节点，将标志符名字记入节点中；并将此节点
压入操作数栈。
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
变量表达式的具体类型是标志符变量；将变量类别信息填入当前变量
表达式节点。
*/
void Parser::process93()
{
	((ASTExpNode*)currentP)->expAttr.varType = ASTVarType::ID_V;
}
/*
生式右部入符号栈，变量的具体类型设置为数组成员变量并压入指
向数组成员表达式的儿子节点指针，压入具有最低优先级的特殊栈底
标志。
*/
void Parser::process94()
{
	pushSymbolStack({
			GetTerminal(TokenType::SQUARE_BRACKET_OPEN),
			GetNonTerminal(NonTerminalType::EXP),
			GetTerminal(TokenType::SQUARE_BRACKET_CLOSE)
					});
	((ASTExpNode*)currentP)->expAttr.varType = ASTVarType::ARRAY_MEMB_V;
	ASTStack.push(&currentP->child[0]);
	initOpStack();
	getExpResault2 = true;
}
/*
产生式右部入符号栈；当前变量节点的具体类型设置为域成员类型变
量；压入域成员变量节点的儿子节点，以处理域成员表达式。
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
创建一个变量的表达式节点；将标志符名记入节点中，弹语法树栈，
得到域变量的儿子节点指针，指向此域成员节点。
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
说明变量表达式的具体类型是标志符变量；将变量类别信息填入当前
变量表达式节点。
*/
void Parser::process97()
{
	((ASTExpNode*)currentP)->expAttr.varType = ASTVarType::ID_V;
}




































































