#pragma once
#include "HAL/Platform.h"


class IDelegateInstance
{
public:
	virtual ~IDelegateInstance() = default;
};


/**
 * ArgTypes:part of function parameters and active passed in when function callback
 */
template<typename RetValType, typename... ArgTypes>
class TDelegateInstanceBase : public IDelegateInstance
{
public:
	virtual RetValType Execute(ArgTypes...) const = 0;

};
