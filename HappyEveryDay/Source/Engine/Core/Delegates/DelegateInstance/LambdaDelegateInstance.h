#pragma once
#include "DelegateInstanceBase.h"
#include "Delegates/DelegateHandle/DelegateHandleBase.h"


/**
 * ArgTypes:part of function parameters and active passed in when function callback
 * ParamTypes: need to storage param list and auto passed in when function callback
 */
template<typename FuncType, typename FunctorType, typename... ParamTypes>
class TLambdaDelegateInstanceBase;


template<typename RetValType, typename... ArgTypes, typename FunctorType, typename... ParamTypes>
class TLambdaDelegateInstanceBase<RetValType(ArgTypes...), FunctorType, ParamTypes...> : public TDelegateInstanceBase<RetValType, ArgTypes...>
{
public:
	using SuperType = TDelegateInstanceBase<RetValType, ArgTypes...>;
	using UnwrappedThisType = TLambdaDelegateInstanceBase<RetValType(ArgTypes...), FunctorType, ParamTypes...>;


public:
	TLambdaDelegateInstanceBase(const FunctorType& InFunctor, ParamTypes... Params)
		: Functor(InFunctor)
		, Payload(Params...)
	{

	}

	TLambdaDelegateInstanceBase(FunctorType&& InFunctor, ParamTypes... Params)
		: Functor(MoveTemp(InFunctor))
		, Payload(Params...)
	{

	}

	virtual RetValType Execute(ArgTypes... Args) const override
	{
		return Payload.ApplyAfter(Functor, Args...);
	}

	static void Create(FDelegateHandleBase& Base, FunctorType&& InFunctor, ParamTypes... Params)
	{
		new (Base) UnwrappedThisType(MoveTemp(InFunctor), Params...);
	}

	static void Create(FDelegateHandleBase& Base, FunctorType& InFunctor, ParamTypes... Params)
	{
		new (Base) UnwrappedThisType(InFunctor, Params...);
	}

protected:
	FunctorType Functor;
	TTuple<ParamTypes...> Payload; // 参数负载, 额外附带的参数
};

