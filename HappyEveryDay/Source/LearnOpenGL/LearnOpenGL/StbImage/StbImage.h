#pragma once
#include "HAL/Platform.h"

class FSTBImage
{
public:
	static uint8* StbiLoad(const char* filename, int* x, int* y, int* channels_in_file, int desired_channels);
	static void StbiImageFree(void *pData);
};
