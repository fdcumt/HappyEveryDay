#include <iostream>
#include <string>
#include <iostream>
#include <Windows.h>

#include "MathTest/MathTest.h"
#include "Test/UtilityTest.h"
#include "Templates/UtilityTemplate.h"
#include "Delegates/Delegate.h"
#include "HAL/PlatformApplicationMisc.h"
#include "D3D12/D3D12Manager.h"
#include "Misc/CoreGlobals.h"


int main()
{
	FGenericApplication *GenericApplication = FPlatformApplicationMisc::CreateApplication();
	
	GenericApplication->PreInit();
	GenericApplication->Init();

	bool bInitOk = false;
	FD3D12Manager D3D12Manager;

	if (FGenericWindow* Window = GenericApplication->MakeWindow())
	{
		bInitOk = D3D12Manager.Initialize(Window->GetWnd());
	}

	if (bInitOk)
	{
		while (!GIsRequestingExit)
		{
			GenericApplication->ProcessMsgInput();
			D3D12Manager.Render();
		}
	}

	// 清理操作
	D3D12Manager.Cleanup();

	return 0;
}
