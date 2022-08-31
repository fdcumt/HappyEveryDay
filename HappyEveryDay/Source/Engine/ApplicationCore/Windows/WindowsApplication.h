#pragma once
#include "GenericPlatform/GenericApplication.h"
#include "WindowsWindow.h"


class FWindowsApplication : public FGenericApplication
{
public:
	static FGenericApplication* CreateApplication();
	static void DestroyApplication();


public:
	virtual FGenericWindow* MakeWindow();
	virtual void DestroyWindow();

	virtual bool PreInit();
	virtual bool Init();

	virtual bool Tick();


protected:
	static FWindowsApplication *WindowApplication;

protected:
	FWindowsWindow *Window = nullptr;
};

