#pragma once


#ifndef PLATFORM_WINDOWS
	#define PLATFORM_WINDOWS 0
#endif // PLATFORM_WINDOWS

#ifndef PLATFORM_ANDROID
	#define PLATFORM_ANDROID 0
#endif // PLATFORM_ANDROID

#ifndef PLATFORM_IOS
	#define PLATFORM_IOS 0
#endif // PLATFORM_IOS

#ifndef PLATFORM_MAC
	#define PLATFORM_MAC 0
#endif // PLATFORM_MAC


using uint8 = unsigned char;
using int8  = signed char;

using uint16 = unsigned short;
using int16 = short;

using uint32 = unsigned int;
using int32 = signed int;

using uint64 = unsigned long long;
using int64 = signed long long;


