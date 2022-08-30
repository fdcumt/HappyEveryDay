#include <iostream>
#include <string>
#include "MathTest/MathTest.h"
#include "Test/UtilityTest.h"
#include "Templates/UtilityTemplate.h"
#include "Delegates/Delegate.h"

#include <iostream>
#include <Windows.h>
#include "HAL/PlatformApplicationMisc.h"

using namespace std;


LRESULT CALLBACK __WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) 
{
	switch (msg) 
	{
	case WM_CLOSE:
		MessageBox(NULL, "WM_CLOSE", "", NULL);
		break;
	default:
		break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

int main()
{
	FGenericApplication *GenericApplication = FPlatformApplicationMisc::CreateApplication();
	
	FGenericWindow* GenericWindow = GenericApplication->MakeWindow();
	GenericWindow->Test();

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
	wc.lpfnWndProc = __WndProc;							// 窗口处理函数的指针
	wc.lpszMenuName = NULL;								// 指向菜单的指针
	wc.lpszClassName = "LYSM_class";					// 指向类名称的指针

	// 为窗口注册一个窗口类
	if (!RegisterClassEx(&wc)) 
	{
		cout << "RegisterClassEx error : " << GetLastError() << endl;
	}

	// 创建窗口
	HWND hWnd = CreateWindowEx(
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
		cout << "CreateWindowEx error : " << GetLastError() << endl;
		return 0;
	}
	
	UpdateWindow(hWnd);
	ShowWindow(hWnd, SW_SHOW);

	// 消息循环（没有会导致窗口卡死）
	MSG msg = { 0 };
	while (msg.message != WM_QUIT) 
	{
		// 从消息队列中删除一条消息
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			DispatchMessage(&msg);
		}
	}

	cout << "finished." << endl;
	return 0;
}
