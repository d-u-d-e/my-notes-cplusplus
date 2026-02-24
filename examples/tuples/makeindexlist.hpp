#pragma once
#include "../typelists.hpp"

template <unsigned N, typename Result = ValueList<unsigned>>
struct MakeIndexListT : MakeIndexListT<N - 1, PushFront<CTValue<unsigned, N - 1>, Result>>
{ };

// basis case
template <typename Result>
struct MakeIndexListT<0, Result>
{
  using Type = Result;
};

template <unsigned N>
using MakeIndexList = typename MakeIndexListT<N>::Type;