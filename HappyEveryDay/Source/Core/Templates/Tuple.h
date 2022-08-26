#pragma once
#include "HAL/Platform.h"
#include "IntegerSequence.h"


template<uint32 Index, typename T>
struct TTupleElement
{
	using Type = T;

	explicit TTupleElement(T &&InValue)
		: Value(Forward<T>(InValue))
	{

	}

	explicit TTupleElement(T& InValue)
		: Value(Forward<T>(InValue))
	{
		
	}

	explicit TTupleElement()
		: Value()
	{

	}

	T Value;
};


template<uint32 IterIndex, uint32 WantedIndex, typename... Types>
struct TNthTupleElementImpl;

template<uint32 IterIndex, uint32 WantedIndex, typename ElementType, typename... Types>
struct TNthTupleElementImpl<IterIndex, WantedIndex, ElementType, Types...> : public TNthTupleElementImpl<IterIndex+1, WantedIndex, Types...>
{

};

template<uint32 WantedIndex, typename ElementType, typename... Types>
struct TNthTupleElementImpl<WantedIndex, WantedIndex, ElementType, Types...> 
{
	using Type = ElementType;
	using UnwrappedThisType = TNthTupleElementImpl<WantedIndex, WantedIndex, ElementType, Types...>;

	template<typename TupleType>
	static Type& Get(TupleType &InTuple)
	{
		return (static_cast<TTupleElement<WantedIndex, ElementType>&>(InTuple)).Value;
	}

	template<typename TupleType>
	static const Type& Get(const TupleType& InTuple)
	{
		return Get(const_cast<TupleType&>(InTuple));
	}
};

template<uint32 WantedIndex, typename... Types>
struct TNthTupleElement : public TNthTupleElementImpl<0, WantedIndex, Types...>
{

};



template<typename Indices, typename... Types>
struct TTupleStorage;

template<uint32... Indices, typename... Types>
struct TTupleStorage<TIntegerSequence<uint32, Indices...>, Types...> : public TTupleElement<Indices, Types>...
{
	template<typename ...ArgTypes>
	TTupleStorage(ArgTypes&& ...Args)
		: TTupleElement<Indices, Types>(Forward<ArgTypes>(Args))...
	{

	}

	template<uint32 Index>
	const typename TNthTupleElement<Index, Types...>::Type& Get() const
	{
		return TNthTupleElement<Index, Types...>::Get(*this);
	}

	template<uint32 Index>
	typename TNthTupleElement<Index, Types...>::Type& Get()
	{
		return TNthTupleElement<Index, Types...>::Get(*this);
	}

	template<typename FuncType, typename...ArgTypes>
	auto ApplyAfter(FuncType&& Func, ArgTypes&&...Args)
	{
		return Func(Forward<ArgTypes>(Args)..., this->Get<Indices>()...);
	}

	template<typename FuncType, typename...ArgTypes>
	auto ApplyAfter(FuncType&& Func, ArgTypes&&...Args) const
	{
		return Func(Forward<ArgTypes>(Args)..., this->Get<Indices>()...);
	}

	template<typename FuncType, typename...ArgTypes>
	auto ApplyBefore(FuncType&& Func, ArgTypes&&...Args)
	{
		return Func(this->Get<Indices>()..., Forward<ArgTyps>(Args)...);
	}

	template<typename FuncType, typename...ArgTypes>
	auto ApplyBefore(FuncType&& Func, ArgTypes&&...Args) const
	{
		return Func(this->Get<Indices>()..., Forward<ArgTypes>(Args)...);
	}
};

template<typename... Types>
struct TTuple : public TTupleStorage<TMakeIntegerSequence<uint32, sizeof...(Types)>, Types...>
{
public:
	using Super = TTupleStorage<TMakeIntegerSequence<uint32, sizeof...(Types)>, Types...>;

public:
	template<typename ...ArgTypes>
	TTuple(ArgTypes&& ...Args)
		: Super(Forward<ArgTypes>(Args)...)
	{

	}

	TTuple() = default;
	TTuple(TTuple&&) = default;
	TTuple(const TTuple&) = default;
	TTuple& operator=(TTuple&&) = default;
	TTuple& operator=(const TTuple&) = default;

};
