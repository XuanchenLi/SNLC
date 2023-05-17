#include "type_error.h"

const char* TypeError::what()
{
	std::cout << "At line: " + std::to_string(lineNum)
		+ " colunm: " + std::to_string(colNum)
		+ " expect type " + TokenTypeName(expect)
		+ " but got " + TokenTypeName(got)
		<< std::endl;
}