#pragma once
#include <Windows.h>
#include "HAL/Platform.h"
#include "GenericPlatform/GenericWindow.h"
#include "Containers/Map.h"
#include "Logging/LogMarcos.h"

DECLARE_LOG_CATEGORY_EXTERN(LogWindow);

class FWindowsWindow : public FGenericWindow
{
public:
	FWindowsWindow() = default;

public:
	virtual void Test();

public:
	virtual bool Init();
	virtual bool Tick() override;
	void ProcessInputMsg();

	virtual HWND GetWnd() const override { return hWnd; }

public:
	bool RegisterClass();

public:
	static LRESULT CALLBACK AppWndProc(HWND hWnd, uint32 msg, WPARAM wParam, LPARAM lParam);

	/** Processes a single Windows message. */
	LRESULT ProcessMessageInner(HWND hWnd, uint32 msg, WPARAM wParam, LPARAM lParam);

	static HWND GetFirstWindowHandle();
	static FWindowsWindow* GetWindow(HWND InHWnd);
	static void AddWindow(HWND InHWnd, FWindowsWindow*InWindow);


protected:
	HWND hWnd = nullptr;
	HINSTANCE hInstance = nullptr;

protected:
	static TMap<HWND, FWindowsWindow*> WindowMap;
};

