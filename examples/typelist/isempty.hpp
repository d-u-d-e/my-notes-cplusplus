#pragma once
#include "popfront.hpp"
#include "typelist.hpp"

// Empty
template <typename List>
struct IsEmpty
{
  constexpr static bool value = false;
};

template <>
struct IsEmpty<TypeList<>>
{
  constexpr static bool value = true;
};

static_assert(IsEmpty<PopFront<TypeList<int>>>::value);
static_assert(!IsEmpty<TypeList<int>>::value);