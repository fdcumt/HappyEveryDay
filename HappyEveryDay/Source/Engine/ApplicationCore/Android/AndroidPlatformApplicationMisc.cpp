#include "AndroidPlatformApplicationMisc.h"
#include "AndroidApplication.h"

class FGenericApplication* FAndroidPlatformApplicationMisc::CreateApplication()
{
	return FAndroidApplication::CreateApplication();
}

void FAndroidPlatformApplicationMisc::DestroyApplication()
{
	FAndroidApplication::DestroyApplication();
}

