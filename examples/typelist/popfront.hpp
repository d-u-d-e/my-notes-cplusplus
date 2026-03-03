#pragma once
#include "typelist.hpp"
#include <type_traits>

// PopFront
template <typename Head, typename... Tail>
struct PopFrontT;

template <typename Head, typename... Tail>
struct PopFrontT<TypeList<Head, Tail...>>
{
  using Type = TypeList<Tail...>;
};

template <typename List>
using PopFront = typename PopFrontT<List>::Type;

static_assert(std::is_same_v<PopFront<TypeList<int, char, long>>, TypeList<char, long>>);
static_assert(std::is_same_v<PopFront<TypeList<int>>, TypeList<>>);