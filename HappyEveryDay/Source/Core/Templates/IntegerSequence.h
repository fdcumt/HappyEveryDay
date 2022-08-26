#pragma once

template<typename T, T... Indices>
struct TIntegerSequence
{

};

template<typename T, T N>
using TMakeIntegerSequence = __make_integer_seq<TIntegerSequence, T, N>;
