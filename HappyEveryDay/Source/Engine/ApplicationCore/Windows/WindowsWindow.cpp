#include "WindowsWindow.h"
#include <stdio.h>
#include "Logging/Log.h"
#include "Misc/AssertionMacros.h"
#include "Misc/CoreGlobals.h"

DEFINE_LOG_CATEGORY(LogWindow);

TMap<HWND, FWindowsWindow*> FWindowsWindow::WindowMap;


void FWindowsWindow::Test()
{
	printf("FWindowsWindow::Test");
}

bool FWindowsWindow::Init()
{
	bool bRegisterClassSucceed = RegisterClass();
	if (bRegisterClassSucceed)
	{
		::UpdateWindow(hWnd);
		::ShowWindow(hWnd, SW_SHOW);
	}

	return bRegisterClassSucceed;
}

bool FWindowsWindow::Tick()
{
	MSG msg = { 0 };

	// 从消息队列中删除一条消息
	if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
	{
		DispatchMessage(&msg);
	}

	return msg.message != WM_QUIT;
}

void FWindowsWindow::ProcessInputMsg()
{
	MSG msg = { 0 };

	// 处理这一帧接收到的所有windows消息
	while (::PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
	{
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}
}

bool FWindowsWindow::RegisterClass()
{
	// 窗口属性初始化
	HINSTANCE hIns = GetModuleHandle(0);
	WNDCLASSEX wc;
	wc.cbSize = sizeof(wc);								// 定义结构大小
	wc.style = CS_HREDRAW | CS_VREDRAW;					// 如果改变了客户区域的宽度或高度，则重新绘制整个窗口 
	wc.cbClsExtra = 0;									// 窗口结构的附加字节数
	wc.cbWndExtra = 0;									// 窗口实例的附加字节数
	wc.hInstance = hIns;								// 本模块的实例句柄
	wc.hIcon = NULL;									// 图标的句柄
	wc.hIconSm = NULL;									// 和窗口类关联的小图标的句柄
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;			// 背景画刷的句柄
	wc.hCursor = NULL;									// 光标的句柄
	wc.lpfnWndProc = AppWndProc;							// 窗口处理函数的指针
	wc.lpszMenuName = NULL;								// 指向菜单的指针
	wc.lpszClassName = "LYSM_class";					// 指向类名称的指针

	// 为窗口注册一个窗口类
	if (!RegisterClassEx(&wc))
	{
		ErrorLog(LogWindow, "FWindowsWindow::RegisterClass RegisterClassEx error [%lu]", GetLastError());
		return false;
	}

	// 创建窗口
	hWnd = CreateWindowEx(
		WS_EX_TOPMOST,				// 窗口扩展样式：顶级窗口
		"LYSM_class",				// 窗口类名
		"LYSM_title",				// 窗口标题
		WS_OVERLAPPEDWINDOW,		// 窗口样式：重叠窗口
		0,							// 窗口初始x坐标
		0,							// 窗口初始y坐标
		800,						// 窗口宽度
		600,						// 窗口高度
		0,							// 父窗口句柄
		0,							// 菜单句柄 
		hIns,						// 与窗口关联的模块实例的句柄
		0							// 用来传递给窗口WM_CREATE消息
	);

	if (hWnd == 0)
	{
		ErrorLog(LogWindow, "FWindowsWindow::RegisterClass CreateWindowEx error [%lu]", GetLastError());
		return false;
	}
	AddWindow(hWnd, this);

	return true;
}

LRESULT CALLBACK FWindowsWindow::AppWndProc(HWND hWnd, uint32 msg, WPARAM wParam, LPARAM lParam)
{
	FWindowsWindow* Window = GetWindow(hWnd);
	if (Window)
	{
		return Window->ProcessMessageInner(hWnd, msg, wParam, lParam);
	}
	else
	{
		WarningLog(LogWindow, "FWindowsWindow::AppWndProc not find window, use default");
		return ::DefWindowProc(hWnd, msg, wParam, lParam);
	}
}

LRESULT FWindowsWindow::ProcessMessageInner(HWND InhWnd, uint32 msg, WPARAM wParam, LPARAM lParam)
{
	Checkf(InhWnd==hWnd, "FWindowsWindow::ProcessMessage InhWnd Error");

	switch (msg)
	{
	case WM_QUIT:
		GIsRequestingExit = true;
		return 0;
	case WM_DESTROY:
		GIsRequestingExit = true;
		return 0;
	}

	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

HWND FWindowsWindow::GetFirstWindowHandle()
{
	for (const auto& WindowMapItem : WindowMap)
	{
		return WindowMapItem.first;
	}

	return nullptr;
}

FWindowsWindow* FWindowsWindow::GetWindow(HWND InHWnd)
{
	auto it = WindowMap.find(InHWnd);
	if (it!= WindowMap.end())
	{
		return it->second;
	}
	else
	{
		return nullptr;
	}
}

void FWindowsWindow::AddWindow(HWND InHWnd, FWindowsWindow* InWindow)
{
	WindowMap.insert(std::pair<HWND, FWindowsWindow*>(InHWnd, InWindow));
}

