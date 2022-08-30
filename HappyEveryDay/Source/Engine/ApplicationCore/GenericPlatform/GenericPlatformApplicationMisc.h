#pragma once
#include "GenericPlatform/GenericApplication.h"


struct FGenericPlatformApplicationMisc
{
	static class FGenericApplication* CreateApplication();
	static void DestroyApplication();
};

