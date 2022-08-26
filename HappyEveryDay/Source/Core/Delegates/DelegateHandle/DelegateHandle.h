#pragma once
#include "Templates/UtilityTemplate.h"
#include "Templates/Tuple.h"
#include "Delegates/DelegateInstance/LambdaDelegateInstance.h"
#include "Delegates/DelegateInstance/StaticDelegateInstance.h"
#include "Delegates/DelegateInstance/RawMethodDelegateInstance.h"


/**
 * ArgTypes:part of function parameters and active passed in when function callback
 * ParamTypes: need to storage param list and auto passed in when function callback
 */
template<typename RetValType, typename... ArgTypes>
class TDelegateHandleBase : FDelegateHandleBase
{
public:
	typedef RetValType FuncType(ArgTypes...);
	using DelegateInstanceType = TDelegateInstanceBase<RetValType, ArgTypes...>;


public:
	template<typename FunctorType, typename... ParamTypes>
	static TDelegateHandleBase<RetValType, ArgTypes...> CreateLambda(FunctorType&& InFunctor, ParamTypes... Params)
	{
		TDelegateHandleBase<RetValType, ArgTypes...> DelegateBase;
		TLambdaDelegateInstanceBase<FuncType, typename TRemoveReference<FunctorType>::Type, ParamTypes...>::Create(DelegateBase, Forward<FunctorType>(InFunctor), Params...);
		return DelegateBase;
	}

	template<typename... ParamTypes>
	static TDelegateHandleBase<RetValType, ArgTypes...> CreateStatic(typename TStaticDelegateInstance<FuncType, ParamTypes...>::FuncPtrType InFunctionPtr, ParamTypes... Params)
	{
		TDelegateHandleBase<RetValType, ArgTypes...> DelegateBase;
		TStaticDelegateInstance<FuncType, ParamTypes...>::Create(DelegateBase, InFunctionPtr, Params...);
		return DelegateBase;
	}

	template<typename ClassType, typename... ParamTypes>
	static TDelegateHandleBase<RetValType, ArgTypes...> CreateRaw(ClassType *InClassObj, typename TMemFunPtrType<true, ClassType, RetValType (ArgTypes..., ParamTypes...)>::Type InFunctionPtr, ParamTypes... Params)
	{
		TDelegateHandleBase<RetValType, ArgTypes...> DelegateBase;
		TRawMethodDelegateInstanceBase<true, FuncType, ClassType, ParamTypes...>::Create(DelegateBase, InClassObj, InFunctionPtr, Params...);
		return DelegateBase;
	}

	template<typename ClassType, typename... ParamTypes>
	static TDelegateHandleBase<RetValType, ArgTypes...> CreateRaw(ClassType* InClassObj, typename TMemFunPtrType<false, ClassType, RetValType(ArgTypes..., ParamTypes...)>::Type InFunctionPtr, ParamTypes... Params)
	{
		TDelegateHandleBase<RetValType, ArgTypes...> DelegateBase;
		TRawMethodDelegateInstanceBase<false, FuncType, ClassType, ParamTypes...>::Create(DelegateBase, InClassObj, InFunctionPtr, Params...);
		return DelegateBase;
	}

public:
	RetValType Execute(ArgTypes...Args) const
	{
		DelegateInstanceType* pDelegateInstance = static_cast<DelegateInstanceType*>(GetInstance());
		return pDelegateInstance->Execute(Args...);
	}

};