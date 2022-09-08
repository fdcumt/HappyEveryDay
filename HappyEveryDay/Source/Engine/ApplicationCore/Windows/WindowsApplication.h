#pragma once
#include "GenericPlatform/GenericApplication.h"
#include "WindowsWindow.h"


class FWindowsApplication : public FGenericApplication
{
public:
	static FGenericApplication* CreateApplication();
	static void DestroyApplication();


public:
	virtual FGenericWindow* MakeWindow() override;
	virtual void DestroyWindow() override;

	virtual bool PreInit() override;
	virtual bool Init() override;

	virtual bool Tick() override;

	virtual void ProcessMsgInput() override;


protected:
	static FWindowsApplication *WindowApplication;

protected:
	FWindowsWindow *Window = nullptr;
};

