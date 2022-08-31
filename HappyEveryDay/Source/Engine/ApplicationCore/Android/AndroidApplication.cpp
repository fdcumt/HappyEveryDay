#include "AndroidApplication.h"
#include "AndroidWindow.h"


FAndroidApplication* FAndroidApplication::AndroidApplication = nullptr;


FGenericApplication* FAndroidApplication::CreateApplication()
{
	if (AndroidApplication==nullptr)
	{
		AndroidApplication = new FAndroidApplication();
	}

	return AndroidApplication;
}

void FAndroidApplication::DestroyApplication()
{
	if (AndroidApplication)
	{
		delete AndroidApplication;
		AndroidApplication = nullptr;
	}
}

FGenericWindow* FAndroidApplication::MakeWindow()
{
	if (Window==nullptr)
	{
		Window = new FAndroidWindow();
	}

	return Window;
}

void FAndroidApplication::DestroyWindow()
{
	if (Window)
	{
		delete Window;
		Window = nullptr;
	}
}

bool FAndroidApplication::PreInit()
{
	return true;
}

bool FAndroidApplication::Init()
{
	return true;
}

