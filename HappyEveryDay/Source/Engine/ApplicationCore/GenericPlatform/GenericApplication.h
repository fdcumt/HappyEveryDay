#pragma once
#include "GenericWindow.h"
#include "GenericPlatform/GenericWindow.h"


class FGenericApplication
{
public:
	FGenericApplication() = default;
	virtual ~FGenericApplication() {}


public:
	virtual FGenericWindow* MakeWindow() = 0;
	virtual void DestroyWindow() = 0;

	virtual bool PreInit();
	virtual bool Init();

	virtual bool Tick() { return true; }

	virtual void ProcessMsgInput() { }

protected:
	
};

