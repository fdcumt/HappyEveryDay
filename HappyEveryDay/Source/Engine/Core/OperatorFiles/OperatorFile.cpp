#include "OperatorFile.h"
#include "Logging/LogMarcos.h"

DEFINE_LOG_CATEGORY(OperatorFile);

char* FOperatorFile::ReadFile(const char* FileName, int32& ContentLen)
{
	FILE *pFile = fopen(FileName, "rb");
	if (pFile == nullptr)
	{
		ErrorLog(OperatorFile, "FOperatorFile::ReadFile[%s] Failed", FileName);
		return nullptr;
	}

	fseek(pFile, 0, SEEK_END); // 定位到文件末尾
	ContentLen = ftell(pFile); // 计算当前文件位置到文件首的偏移字节数

	char *FileContent = new char[ContentLen+1];

	rewind(pFile); // 重新定位到文件首
	FileContent[ContentLen] = 0;

	ContentLen = fread(FileContent, 1, ContentLen, pFile); // 按字节数读取文件
	FileContent[ContentLen] = 0;
	fclose(pFile);
	return FileContent;
}

