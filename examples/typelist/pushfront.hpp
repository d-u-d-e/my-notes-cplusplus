#pragma once
#include "typelist.hpp"
#include <type_traits>

// PushFront
template <typename E, typename... Elements>
struct PushFrontT;

template <typename E, typename... Elements>
struct PushFrontT<E, TypeList<Elements...>>
{
  using Type = TypeList<E, Elements...>;
};

template <typename E, typename List>
using PushFront = typename PushFrontT<E, List>::Type;

static_assert(std::is_same_v<PushFront<short, TypeList<int>>, TypeList<short, int>>);