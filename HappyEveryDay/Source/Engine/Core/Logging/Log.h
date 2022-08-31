#pragma once
#include "HAL/Platform.h"
#include "Containers/String.h"


enum class ELogVerbosity : uint8
{
	Verbose,
	Log,
	Display,
	Warning,
	Break,
	Error,
	Fatal
};

extern const char* LogVerbosityToString(const ELogVerbosity eLogVerbosity);

struct FLogCategory
{
public:
	explicit FLogCategory(const char* InCategoryName)
		:CategoryName(InCategoryName)
	{

	}

public:
	FString CategoryName;
};

class FLog
{
public:
	FLog() = default;

public:
	static void Logf(const FLogCategory&CategoryName, ELogVerbosity eVerbosity, const char* FormatStr, ...);
	static void DoBreak();
	static void DoAbort();

protected:
	static bool bOutputToConsole;

};
