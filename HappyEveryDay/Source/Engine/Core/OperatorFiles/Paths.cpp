#include "Paths.h"
#include <direct.h>

FStdString FPaths::GetContentDir()
{
	FStdString Path = GetBaseDir();
	FStdString ContentDir = Path + "/../../../Content/";
	return ContentDir;
}

const char* FPaths::GetBaseDir()
{
	static bool bGet = false;
	static char path[MAX_PATH_LENGTH] = {0};
	if (bGet)
	{
		return path;
	}
	else
	{
		_getcwd(path, MAX_PATH_LENGTH);
		bGet = true;
		return path;
	}
}

