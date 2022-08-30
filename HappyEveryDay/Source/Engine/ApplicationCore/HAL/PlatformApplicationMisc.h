#pragma once
#include "HAL/Platform.h"

#if PLATFORM_WINDOWS
	#include "Windows/WindowsPlatformApplicationMisc.h"
#elif PLATFORM_ANDROID
	#include "Android/AndroidPlatformApplicationMisc.h"
#endif
