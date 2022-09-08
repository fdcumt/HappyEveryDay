#pragma once
#include <windows.h>

class FGenericWindow
{
public:
	FGenericWindow();

public:
	virtual void Test() = 0;

	virtual bool Init() { return true; }
	virtual bool Tick() { return true; }

	virtual HWND GetWnd() const { return nullptr; }

	virtual void ProcessInputMsg() { }

};

