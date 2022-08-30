#pragma once
#include "GenericPlatform/GenericPlatformApplicationMisc.h"


struct FWindowsPlatformApplicationMisc : public FGenericPlatformApplicationMisc
{
	static class FGenericApplication* CreateApplication();
	static void DestroyApplication();
};

typedef FWindowsPlatformApplicationMisc FPlatformApplicationMisc;
