#pragma once
#include "typelist.hpp"
#include <type_traits>

// Front
template <typename Head, typename... Tail>
struct FrontT;

template <typename Head, typename... Tail>
struct FrontT<TypeList<Head, Tail...>>
{
  using Type = Head;
};

template <typename List>
using Front = typename FrontT<List>::Type;

static_assert(std::is_same_v<Front<TypeList<int, char, long>>, int>);