#include "WindowsApplication.h"
#include "WindowsWindow.h"
#include "GenericPlatform/GenericWindow.h"


FWindowsApplication* FWindowsApplication::WindowApplication = nullptr;



FGenericApplication* FWindowsApplication::CreateApplication()
{
	if (WindowApplication==nullptr)
	{
		WindowApplication = new FWindowsApplication();
	}

	return WindowApplication;
}

void FWindowsApplication::DestroyApplication()
{
	if (WindowApplication)
	{
		delete WindowApplication;
		WindowApplication = nullptr;
	}
}

FGenericWindow* FWindowsApplication::MakeWindow()
{
	if (Window==nullptr)
	{
		Window = new FWindowsWindow();
	}

	return Window;
}

void FWindowsApplication::DestroyWindow()
{
	if (Window)
	{
		delete Window;
		Window = nullptr;
	}
}

bool FWindowsApplication::PreInit()
{
	return true;
}

bool FWindowsApplication::Init()
{
	FGenericWindow*pWindow = MakeWindow();
	if (pWindow)
	{
		pWindow->Init();
	}

	return pWindow!=nullptr;
}

bool FWindowsApplication::Tick()
{
	if (Window)
	{
		return Window->Tick();
	}

	return false;
}

