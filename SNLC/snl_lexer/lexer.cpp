#include "lexer.h"


TokenList Lexer::genTokens(FileReader& fScanner)
{
	TokenList res;

	int scanLine = 1;
	int scanCol = 0;

	int tokenLine = 1;
	int tokenCol = 1;

	char ch = ' ';
	std::string sem = "";

	LexState state = LexState::START;

	bool notEnd = fScanner.getNextChar(ch);
	scanCol++;

	while (notEnd)
	{

		if (ch == '\n' && (state == LexState::START || state == LexState::IN_COMMENT))
		{
			scanLine++;
			scanCol = 0;
		}
		
		switch (state)
		{
			case LexState::START:
			{

				if (isspace(ch))
				{
					// 重回起点
					notEnd = fScanner.getNextChar(ch);
					scanCol++;
					continue;
				}
				sem += ch;
				//更新当前有效单词位置
				tokenLine = scanLine;
				tokenCol = scanCol;
				//进入不同状态
				if (isalpha(ch))
				{
					state = LexState::IN_ID;
				}
				else if (isdigit(ch))
				{
					state = LexState::IN_NUM;
				}
				else if (ch == ':')
				{
					state = LexState::IN_ASSIGN;
				}
				else if (ch == '{')
				{
					state = LexState::IN_COMMENT;
				}
				else if (ch == '.')
				{
					state = LexState::IN_DOT;
				}
				else
				{
					//单分界符，回到起点
					TokenType tp = getID(sem);
					if (tp != TokenType::NOT_ID)
					{
						res.appendToken(
							Token(
								tokenLine, tokenCol, sem, tp
							)
						);
						sem = "";
					}
					else
					{
						//不是保留字，必定非法
						std::string err = "Line: " + std::to_string(tokenLine) + 
							" Column: " + std::to_string(tokenCol) +
							"ERROR: Expect a single delimiter word but get: " + sem + "\n";
						throw std::exception(err.c_str());
					}
				}
				//无论进入下一个状态还是重回起点，都读取下一个字符
				notEnd = fScanner.getNextChar(ch);
				scanCol++;
				break;
			}
			case LexState::IN_ID:
			{
				if (isalnum(ch))
				{
					//循环该状态，取下一个字符
					sem += ch;
					notEnd = fScanner.getNextChar(ch);
					if (!notEnd)
					{
						TokenType tp = getID(sem);
						res.appendToken(
							Token(
								tokenLine, tokenCol, sem,
								tp == TokenType::NOT_ID ? TokenType::IDENTIFIER : tp
							)
						);
					}
					scanCol++;
					continue;
				}
				else
				{
					TokenType tp = getID(sem);
					res.appendToken(
						Token(
							tokenLine, tokenCol, sem,
							tp == TokenType::NOT_ID ? TokenType::IDENTIFIER : tp
						)
					);
					// 回到起点，无需取下一个字符
					sem = "";
					state = LexState::START;
				}
				break;
			}
			case LexState::IN_NUM:
			{
				if (isdigit(ch))
				{
					//循环该状态，取下一个字符
					sem += ch;
					notEnd = fScanner.getNextChar(ch);
					if (!notEnd)
					{
						res.appendToken(
							Token(
								tokenLine, tokenCol, sem, TokenType::INT
							)
						);
					}
					scanCol++;
					continue;
				}
				else
				{
					res.appendToken(
						Token(
							tokenLine, tokenCol, sem, TokenType::INT
						)
					);
					// 回到起点，无需取下一个字符
					sem = "";
					state = LexState::START;
				}
				break;
			}
			case LexState::IN_COMMENT:
			{	
				if (ch != '}')
				{
					// 循环该状态，取下一个字符
					notEnd = fScanner.getNextChar(ch);
					if (!notEnd)
					{
						std::cout << "ERROR: Unclosed comment region.\n";
					}
					scanCol++;
					continue;
				}
				// 回到起点，取下一个字符
				sem = "";
				notEnd = fScanner.getNextChar(ch);
				scanCol++;
				state = LexState::START;
				break;
			}
			case LexState::IN_ASSIGN:
			{
				if (ch == '=')
				{
					sem += ch;
					res.appendToken(
						Token(
							tokenLine, tokenCol, sem, TokenType::ASSIGN
						)
					);
					// 回到起点，取下一个字符
					sem = "";
					notEnd = fScanner.getNextChar(ch);
					scanCol++;
					state = LexState::START;
				}
				else
				{
					std::string err = "Line: " + std::to_string(tokenLine) +
						" Column: " + std::to_string(tokenCol) + 
						"ERROR: Invalid character: " + ch + std::string(" after : expect =\n");
					throw std::logic_error(err.c_str());
				}
				break;
			}
			case LexState::IN_DOT:
			{
				if (ch == '.')
				{
					sem += ch;
					res.appendToken(
						Token(
							tokenLine, tokenCol, sem, TokenType::DOT_DOT
						)
					);
					// 回到起点，取下一个字符
					sem = "";
					notEnd = fScanner.getNextChar(ch);
					scanCol++;
					state = LexState::START;
				}
				else
				{
					res.appendToken(
						Token(
							tokenLine, tokenCol, sem, TokenType::DOT
						)
					);
					// 回到起点，无需取下一个字符
					sem = "";
					state = LexState::START;
				}
				break;
			}
		}
	}
	
	if (sem == ".")
	{
		res.appendToken(
			Token(
				tokenLine, tokenCol, sem, TokenType::DOT
			)
		);
	}
	else if (sem == ":")
	{
		res.appendToken(
			Token(
				tokenLine, tokenCol, sem, TokenType::COLON
			)
		);
	}
	
	res.appendToken(
		Token(tokenLine, tokenCol, "", TokenType::EOF_)
	);
	return res;
}