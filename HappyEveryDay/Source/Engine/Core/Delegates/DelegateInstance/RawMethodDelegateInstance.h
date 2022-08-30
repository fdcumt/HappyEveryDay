#pragma once
#include "Templates/UtilityTemplate.h"
#include "DelegateInstanceBase.h"
#include "Delegates/DelegateHandle/DelegateHandleBase.h"

 
/**
 * ArgTypes:part of function parameters and active passed in when function callback
 * ParamTypes: need to storage param list and auto passed in when function callback
 */
template<bool bConst, typename FuncType, typename ClassType, typename... ParamTypes>
class TRawMethodDelegateInstanceBase;


template<bool bConst, typename RetValType, typename... ArgTypes, typename ClassType, typename... ParamTypes>
class TRawMethodDelegateInstanceBase<bConst, RetValType(ArgTypes...), ClassType, ParamTypes...> : public TDelegateInstanceBase<RetValType, ArgTypes...>
{
public:
	typedef RetValType FuncType(ArgTypes...);
	using FMemMethodType = typename TMemFunPtrType<bConst, ClassType, RetValType (ArgTypes..., ParamTypes...)>::Type ;
	using SuperType = TDelegateInstanceBase<RetValType, ArgTypes...>;
	using UnwrappedThisType = TRawMethodDelegateInstanceBase<bConst, RetValType(ArgTypes...), ClassType, ParamTypes...>;


public:
	TRawMethodDelegateInstanceBase(ClassType* InObj, const FMemMethodType& InMethodPtr, ParamTypes... Params)
		: pObj(InObj)
		, MethodPtr(InMethodPtr)
		, Payload(Params...)
	{

	}

	virtual RetValType Execute(ArgTypes... Args) const override
	{
		return Payload.ApplyAfter(TMemberFunctionCaller<ClassType, FMemMethodType>(pObj, MethodPtr), Args...);
	}


	static void Create(FDelegateHandleBase& Base, ClassType* InObj, FMemMethodType InMethodPtr, ParamTypes... Params)
	{
		new (Base) UnwrappedThisType(InObj, InMethodPtr, Params...);
	}

protected:
	ClassType* pObj;
	FMemMethodType MethodPtr;
	TTuple<ParamTypes...> Payload; // 参数负载, 额外附带的参数
};
