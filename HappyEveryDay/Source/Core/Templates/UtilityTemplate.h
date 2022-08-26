#pragma once
#include "HAL/Platform.h"

template<typename... Types>
struct TTypeList
{

};


/**
 * TIfThenElse
 */
template<bool C, typename A, typename B> 
struct TIfThenElse;

template<typename A, typename B> 
struct TIfThenElse<true, A, B>  
{ 
	using Type = A;
};

template<typename A, typename B> 
struct TIfThenElse<false, A, B> 
{ 
	using Type = B; 
};




/**
 * TUnwrapType
 */
template<typename T>
struct TUnwrapType
{
	using Type = T;
};




/**
 * TRemoveConstType
 */
template<typename T>
struct TRemoveConstType
{
	using Type = T;
};

template<typename T>
struct TRemoveConstType<const T>
{
	using Type = T;
};


/**
 * TRemoveReference
 */
template<typename T>
struct TRemoveReference
{
	using Type = T;
};

template<typename T>
struct TRemoveReference<T &>
{
	using Type = T;
};

template<typename T>
struct TRemoveReference<T&&>
{
	using Type = T;
};




/**
 * TAreSameType
 */
template<typename A, typename B>
struct TAreSameType
{
	enum { Value = false };
};

template<typename A>
struct TAreSameType<A, A>
{
	enum { Value = true };
};








/**
 * TIsLValueReference
 */
template<typename T>
struct TIsLValueReference
{
	enum { Value = false };
};

template<typename T>
struct TIsLValueReference<T&>
{
	enum { Value = true };
};


/**
 * TIsRValueReference
 */
template<typename T>
struct TIsRValueReference
{
	enum { Value = false };
};

template<typename T>
struct TIsRValueReference<T&&>
{
	enum { Value = true };
};







/**
 * Forward
 */
template<typename T>
T&& Forward(typename TRemoveReference<T>::Type& Obj)
{
	return (T&&)Obj;
}

template<typename T>
T&& Forward(typename TRemoveReference<T>::Type &&Obj)
{
	return (T&&)Obj;
}



/**
 * Move
 */
template<typename T>
typename TRemoveReference<T>::Type&& MoveTemp(T&& Obj)
{
	using CastType = typename TRemoveReference<T>::Type;

	static_assert(TIsLValueReference<T>::Value, "MoveTemp called on an rvalue");
	static_assert(!TAreSameType<CastType&, const CastType&>::Value, "MoveTemp called on a const object");

	return (CastType&&)Obj;
}





/**
 * TNthTypeFromParameterPack
 */
template<uint32 N, typename... Types>
struct TNthTypeFromParameterPack;

template<uint32 N, typename ElementType, typename... Types>
struct TNthTypeFromParameterPack<N, ElementType, Types...> : TNthTypeFromParameterPack<N-1, Types...>
{

};

template<typename ElementType, typename... Types>
struct TNthTypeFromParameterPack<0, ElementType, Types...>
{
	using Type = ElementType;
};




/**
 * TMemFunPtrType
 */
template<bool bConst, typename ClassType, typename FuncType>
struct TMemFunPtrType;

template<typename ClassType, typename RetType, typename... ArgTypes>
struct TMemFunPtrType<true, ClassType, RetType(ArgTypes...)>
{
	typedef RetType (ClassType::*Type)(ArgTypes...) const;
};

template<typename ClassType, typename RetType, typename... ArgTypes>
struct TMemFunPtrType<false, ClassType, RetType(ArgTypes...)>
{
	typedef RetType (ClassType::*Type)(ArgTypes...);
};





/**
 * TMemberFunctionCaller
 */
template<typename ClassType, typename MemFunPtrType>
struct TMemberFunctionCaller
{
	explicit TMemberFunctionCaller(ClassType* InObj, MemFunPtrType InMethodPtr)
		: pObj(InObj)
		, FuncMethodPtr(InMethodPtr)
	{

	}

	template<typename... ArgTypes>
	auto operator() (ArgTypes&&... Args)
	{
		return (pObj->*FuncMethodPtr)(Forward<ArgTypes>(Args)...);
	}

public:
	ClassType* pObj;
	MemFunPtrType FuncMethodPtr;
};
