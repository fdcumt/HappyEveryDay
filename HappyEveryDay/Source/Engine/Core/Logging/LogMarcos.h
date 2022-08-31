#pragma once


#define DECLARE_LOG_CATEGORY_EXTERN(CategoryName) extern struct FLogCategory LogCategory_##CategoryName

#define DEFINE_LOG_CATEGORY(CategoryName) FLogCategory LogCategory_##CategoryName(#CategoryName)


#define	LOGImpl(CategoryName, VerboseLevel, Format, ...) FLog::Logf(LogCategory_##CategoryName, VerboseLevel, Format, ##__VA_ARGS__)

#define VerboseLog(CategoryName, Format, ...) LOGImpl(CategoryName, ELogVerbosity::Verbose, Format, ##__VA_ARGS__)
#define DebugLog(CategoryName, Format, ...) LOGImpl(CategoryName, ELogVerbosity::Log, Format, ##__VA_ARGS__)
#define DisplayLog(CategoryName, Format, ...) LOGImpl(CategoryName, ELogVerbosity::Display, Format, ##__VA_ARGS__)
#define WarningLog(CategoryName, Format, ...) LOGImpl(CategoryName, ELogVerbosity::Warning, Format, ##__VA_ARGS__)
#define BreakLog(CategoryName, Format, ...) LOGImpl(CategoryName, ELogVerbosity::Break, Format, ##__VA_ARGS__)
#define ErrorLog(CategoryName, Format, ...) LOGImpl(CategoryName, ELogVerbosity::Error, Format, ##__VA_ARGS__)
#define FatalLog(CategoryName, Format, ...) LOGImpl(CategoryName, ELogVerbosity::Fatal, Format, ##__VA_ARGS__)



