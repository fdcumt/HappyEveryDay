#pragma once
#include "HAL/Platform.h"

class FOperatorFile
{
public:
	static char* ReadFile(const char*FileName, int32 &ContentLen);
};


