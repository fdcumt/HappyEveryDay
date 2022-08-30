#pragma once
#include "GenericWindow.h"
#include "GenericPlatform/GenericWindow.h"


class FGenericApplication
{
public:
	virtual FGenericWindow* MakeWindow() = 0;
	virtual void DestroyWindow() = 0;

};

