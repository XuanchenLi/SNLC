#pragma once
#include <iostream>
#include <string>


class FileReader
{
public:
	FileReader(const char* filename, const int bufsize = 1024);
	~FileReader();
	bool getNextChar(char& ch);
private:
	FileReader(const FileReader&) = delete;
	FileReader& operator=(const FileReader&) = delete;

	FILE* fp;
	char* buf;
	int curIdx;
	int bufEnd;
	bool fileIsEnd;
	const int bufSize;
};