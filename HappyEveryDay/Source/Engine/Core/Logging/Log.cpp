#include "Log.h"
#include <stdarg.h>
#include <windows.h>
#include <stdio.h>
//#include <charconv>
#include <intrin.h>
#include <stdlib.h>

#define PLATFORM_BREAK() (__nop(), __debugbreak())


bool FLog::bOutputToConsole = true;


void FLog::Logf(const FLogCategory&CategoryName, ELogVerbosity eVerbosity, const char* FormatStr, ...)
{
	const char* pLogVerbosityName = LogVerbosityToString(eVerbosity);
	int32 AllocFormatLen = static_cast<int32>(::strlen(FormatStr)+CategoryName.CategoryName.length()+::strlen(pLogVerbosityName)+10);
	char *RealFormatStr =  new char[AllocFormatLen];
	int32 RealFormatStrUseSize = snprintf(RealFormatStr, AllocFormatLen, "%s %s: %s\n", CategoryName.CategoryName.data(), pLogVerbosityName, FormatStr);
	if (RealFormatStrUseSize<AllocFormatLen)
	{
		RealFormatStr[RealFormatStrUseSize+1] = '\0';
	}
	else
	{
		DoBreak();
	}


	char* buf = nullptr;
	int32 WrittenLen = 0;
	int32 BuffLen = AllocFormatLen * 2;

    while(true)
    {
		if (buf!=nullptr)
		{
			delete []buf;
		}

		buf = new char[BuffLen];
		va_list va;
		va_start(va, FormatStr);
		WrittenLen = vsnprintf(buf, BuffLen, RealFormatStr, va);
		va_end(va);

		if (WrittenLen<BuffLen)
		{
			break;
		}
		else
		{
			BuffLen *= 2;
		}
    } 

	if (buf)
	{
		::OutputDebugString(buf);
		
		if (bOutputToConsole)
		{
			printf("%s", buf);
		}

		delete []buf;
	}
	else
	{
		DoBreak();
	}

	switch(eVerbosity)
	{
	case ELogVerbosity::Break:
	{
		DoBreak();
		break;
	}
	case ELogVerbosity::Fatal:
	{
		DoBreak();
		DoAbort();
		break;
	}
	}
}

void FLog::DoBreak()
{
	PLATFORM_BREAK();
}

void FLog::DoAbort()
{
	abort();
}


const char* LogVerbosityToString(const ELogVerbosity eLogVerbosity)
{
	switch (eLogVerbosity)
	{
	case ELogVerbosity::Verbose:
		return "Verbose";
		break;
	case ELogVerbosity::Log:
		return "Log";
		break;
	case ELogVerbosity::Display:
		return "Display";
		break;
	case ELogVerbosity::Warning:
		return "Warning";
		break;
	case ELogVerbosity::Error:
		return "Error";
		break;
	case ELogVerbosity::Break:
		return "Break";
		break;
	case ELogVerbosity::Fatal:
		return "Fatal";
		break;
	default:
		break;
	}

	return "UnknownVerbosity";
}
