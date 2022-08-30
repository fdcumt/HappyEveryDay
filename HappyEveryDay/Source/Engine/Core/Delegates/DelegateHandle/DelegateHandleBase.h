#pragma once
#include "HAL/Platform.h"
#include "Delegates/DelegateInstance/DelegateInstanceBase.h"


class FDelegateHandleBase
{
public:
	FDelegateHandleBase() = default;

	FDelegateHandleBase(const FDelegateHandleBase& Other)
		: FDelegateHandleBase(const_cast<FDelegateHandleBase&>(Other))
	{

	}

	FDelegateHandleBase(FDelegateHandleBase& Other)
		: Data(Other.Data)
		, DataSize(Other.DataSize)
	{
		Other.Reset();
	}

	FDelegateHandleBase(FDelegateHandleBase&& Other) noexcept
	{
		Data = Other.Data;
		DataSize = Other.DataSize;
		Other.Reset();
	}

	FDelegateHandleBase& operator=(FDelegateHandleBase& Other)
	{
		Unbind(); // unbind self
		Data = Other.Data;
		DataSize = Other.DataSize;
		Other.Reset();

		return *this;
	}

	FDelegateHandleBase& operator=(const FDelegateHandleBase& Other)
	{
		*this = const_cast<FDelegateHandleBase&>(Other);
		return *this;
	}

	FDelegateHandleBase& operator=(FDelegateHandleBase&& Other) noexcept
	{
		*this = Other;
		return *this;
	}

	~FDelegateHandleBase()
	{
		Unbind();
	}


public:
	void Unbind()
	{
		if (IsBind())
		{
			IDelegateInstance* pDelegateInstance = (IDelegateInstance*)Data;
			pDelegateInstance->~IDelegateInstance();
			DataSize = 0;
			delete[]Data;
			Data = nullptr;
		}
	}

	IDelegateInstance* GetInstance() const
	{
		return (IDelegateInstance*)Data;
	}

	bool IsBind() const
	{
		return Data != nullptr;
	}

protected:
	uint8* Data = nullptr;
	uint32 DataSize = 0;


private:
	friend void* operator new(size_t Size, FDelegateHandleBase& DelegateHandleBase);
	friend void operator delete(void* pData, FDelegateHandleBase& DelegateHandleBase);

	void* Allocate(uint32 Size)
	{
		Data = new uint8[Size];
		DataSize = Size;
		return Data;
	}

	void ProcessDelete(void* pData)
	{
		if (pData == Data)
		{
			Unbind();
		}
	}

	void Reset()
	{
		DataSize = 0;
		Data = nullptr;
	}
};

inline void* operator new(size_t Size, FDelegateHandleBase& DelegateHandleBase)
{
	return DelegateHandleBase.Allocate(static_cast<uint32>(Size));
}

inline void operator delete(void* pData, FDelegateHandleBase& DelegateHandleBase)
{
	DelegateHandleBase.ProcessDelete(pData);
}
