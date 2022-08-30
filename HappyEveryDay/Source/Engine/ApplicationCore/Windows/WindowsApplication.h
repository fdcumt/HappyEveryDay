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


protected:
	static FWindowsApplication *WindowApplication;

protected:
	FWindowsWindow *Window = nullptr;
};

