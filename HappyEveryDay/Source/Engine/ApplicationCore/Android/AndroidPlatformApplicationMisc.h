#pragma once
#include "GenericPlatform/GenericPlatformApplicationMisc.h"


struct FAndroidPlatformApplicationMisc : public FGenericPlatformApplicationMisc
{
	static FGenericApplication* CreateApplication();
	static void DestroyApplication();
};

typedef FAndroidPlatformApplicationMisc FPlatformApplicationMisc;
