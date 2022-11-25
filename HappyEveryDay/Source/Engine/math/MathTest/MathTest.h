#pragma once

class FMath
{
	template <class T>
	static T Clamp(const T InValue, const T InMin, const T InMax)
	{
		return InValue>InMax ? InMax : (InValue<InMin ? InMin:InValue);
	}
};
