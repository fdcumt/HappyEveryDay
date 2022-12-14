#pragma once
#include <intrin.h>

#define CheckSlow(expr) \
{ \
	if(UNLIKELY(!(expr))) \
	{ \
		PLATFORM_BREAK(); \
	} \
}

#define Checkf(expr, format, ...) CHECK_F_IMPL(expr, format, ##__VA_ARGS__)

#define UNLIKELY(x)			(x)

#define PLATFORM_BREAK() (__nop(), __debugbreak())

#define CHECK_F_IMPL(expr, format, ...) \
		{ \
			if(UNLIKELY(!(expr))) \
			{ \
				printf(format, ##__VA_ARGS__);\
				PLATFORM_BREAK(); \
			} \
		}
