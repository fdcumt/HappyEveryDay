#pragma once
#include "GenericPlatform/GenericApplication.h"
#include "AndroidWindow.h"


class FAndroidApplication : public FGenericApplication
{
public:
	static FGenericApplication* CreateApplication();
	static void DestroyApplication();


public:
	virtual FGenericWindow* MakeWindow();
	virtual void DestroyWindow();

	virtual bool PreInit();
	virtual bool Init();

protected:
	static FAndroidApplication* AndroidApplication;


protected:
	FAndroidWindow* Window = nullptr;
};

