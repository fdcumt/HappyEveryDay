#pragma once

class FGenericWindow
{
public:
	FGenericWindow();

public:
	virtual void Test() = 0;

	virtual bool Init() { return true; }
	virtual bool Tick() { return true; }
};

