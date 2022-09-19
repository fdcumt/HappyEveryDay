#pragma once
#include "Logging/LogMarcos.h"
#include <winerror.h>
#include <stdexcept>
#include "Containers/String.h"
#include <intsafe.h>


inline FString HrToString(HRESULT hr)
{
	char s_str[64] = {0};

	sprintf_s(s_str, "HRESULT of 0x%08X", static_cast<UINT>(hr));
	return FString(s_str);
}
 
class FHrExeception : public std::runtime_error
{
public:
	FHrExeception(HRESULT hr) 
		: std::runtime_error(HrToString(hr))
		, m_hr(hr)
	{
	}

	HRESULT Error() const { return m_hr; }

private:
	HRESULT m_hr;
};


inline void ThrowIfFailed(HRESULT hr)
{
	if (FAILED(hr))
	{
		throw FHrExeception(hr);
	}
}
