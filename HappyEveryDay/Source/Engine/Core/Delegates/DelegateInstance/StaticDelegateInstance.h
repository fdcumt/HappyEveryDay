#pragma once
#include "DelegateInstanceBase.h"
#include "Delegates/DelegateHandle/DelegateHandleBase.h"


/**
 * ArgTypes:part of function parameters and active passed in when function callback
 * ParamTypes: need to storage param list and auto passed in when function callback
 */
template<typename FuncType, typename... ParamTypes>
class TStaticDelegateInstance;


template<typename RetValType, typename... ArgTypes, typename... ParamTypes>
class TStaticDelegateInstance<RetValType(ArgTypes...), ParamTypes...> : public TDelegateInstanceBase<RetValType, ArgTypes...>
{
public:
	typedef RetValType(*FuncPtrType)(ArgTypes..., ParamTypes...);
	using SuperType = TDelegateInstanceBase<RetValType, ArgTypes...>;
	using UnwrappedThisType = TStaticDelegateInstance<RetValType(ArgTypes...), ParamTypes...>;


public:
	TStaticDelegateInstance(FuncPtrType InFuncPtr, ParamTypes... Params)
		: StaticFuncPtr(InFuncPtr)
		, Payload(Params...)
	{

	}

	virtual RetValType Execute(ArgTypes... Args) const override
	{
		return Payload.ApplyAfter(StaticFuncPtr, Args...);
	}

	static void Create(FDelegateHandleBase& Base, FuncPtrType InFunctor, ParamTypes... Params)
	{
		new (Base) UnwrappedThisType(InFunctor, Params...);
	}

protected:
	FuncPtrType StaticFuncPtr;
	TTuple<ParamTypes...> Payload; // 参数负载, 额外附带的参数
};
