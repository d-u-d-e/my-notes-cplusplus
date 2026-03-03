#pragma once
#include "front.hpp"
#include "isempty.hpp"
#include "pushback.hpp"
#include "typelist.hpp"

// Reverse
template <typename List, bool is_empty = IsEmpty<List>::value>
struct ReverseT
{
  using Tail = PopFront<List>;
  using Head = Front<List>;
  using Type = PushBack<Head, typename ReverseT<Tail, IsEmpty<Tail>::value>::Type>;
};

template <typename List>
struct ReverseT<List, true>
{
  using Type = List;
};

template <typename List>
using Reverse = typename ReverseT<List>::Type;

static_assert(std::is_same_v<Reverse<TypeList<float, int, char>>, TypeList<char, int, float>>);