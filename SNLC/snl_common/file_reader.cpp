#include "file_reader.h"


FileReader::FileReader(const char* filename, const int bufsize) :bufSize(bufsize)
{
	fp = fopen(filename, "r");
	if (!fp)
	{
		std::cout << "ERROR: Fail to open file: " + std::string(filename) << std::endl;
		return;
	}
	if (bufSize <= 0)
	{
		std::cout << "ERROR: Bad buf size: " + std::to_string(bufSize) << std::endl;
		return;
	}
	buf = new char[bufSize];
	curIdx = 0;
	bufEnd = 0;
	fileIsEnd = false;
}


FileReader::~FileReader()
{
	if (buf) delete[] buf;
	if (fp) fclose(fp);
}

bool FileReader::getNextChar(char& ch)
{
	if (!fp || !buf)
	{
		std::cout << "ERROR: Incompleted File Reader." << std::endl;
		return false;
	}
	
	if (curIdx == bufEnd)
	{
		if (fileIsEnd) return false;
		bufEnd = fread(buf, sizeof(char), bufSize, fp);
		curIdx = 0;
		if (feof(fp))
		{
			fileIsEnd = true;
			if (!bufEnd) return false;
		}
	}
	if (curIdx < bufEnd)
	{
		ch = buf[curIdx++];
	}

	return true;
}