#pragma once
#include "front.hpp"
#include "popfront.hpp"

// Nth element
template <typename List, unsigned int N>
struct NthElementT
{
  using Type = typename NthElementT<PopFront<List>, N - 1>::Type;
};

template <typename List>
struct NthElementT<List, 0U>
{
  using Type = Front<List>;
};

template <typename List, std::size_t N>
using NthElement = typename NthElementT<List, N>::Type;

static_assert(std::is_same_v<NthElement<TypeList<int>, 0>, int>);
static_assert(std::is_same_v<NthElement<TypeList<int, short, double, char>, 2>, double>);