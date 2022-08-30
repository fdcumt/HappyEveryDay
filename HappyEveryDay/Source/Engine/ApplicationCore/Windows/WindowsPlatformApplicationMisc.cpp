#include "WindowsPlatformApplicationMisc.h"
#include "WindowsApplication.h"

class FGenericApplication* FWindowsPlatformApplicationMisc::CreateApplication()
{
	return FWindowsApplication::CreateApplication();
}

void FWindowsPlatformApplicationMisc::DestroyApplication()
{
	FWindowsApplication::DestroyApplication();
}

