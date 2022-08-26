#pragma once

class FPlaceNewTest
{
public:
	~FPlaceNewTest()
	{
		Unbind();
	}

	void Unbind()
	{
		if (Data)
		{
			Data = nullptr;
			delete Data;
			DataSize = 0;
		}
	}

protected:
	int DataSize = 0;
	void* Data = nullptr;

private:
	void* Allocate(int Size)
	{
		DataSize = Size;
		Data = new unsigned char[Size];

		return Data;
	}

	void ProcessDelegate(void* pData)
	{
		if (pData==Data)
		{
			Unbind();
		}
	}

	friend inline void* operator new (size_t Size, FPlaceNewTest& Base);
	friend inline void operator delete (void* pData, FPlaceNewTest& Base);
};

inline void* operator new (size_t Size, FPlaceNewTest& Base)
{
	return Base.Allocate(static_cast<int>(Size));
}

inline void operator delete (void* pData, FPlaceNewTest& Base)
{
	Base.ProcessDelegate(pData);
}