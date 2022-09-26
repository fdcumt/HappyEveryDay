#pragma once
#include "Containers/String.h"

#define MAX_PATH_LENGTH 4096

class FPaths
{
public:
	static FStdString GetContentDir();

	// get all base dir, 当前exe运行路径, 其它的目录是以此为基础计算出来的
	static const char* GetBaseDir();
};

