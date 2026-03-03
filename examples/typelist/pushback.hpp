#pragma once
#include "front.hpp"
#include "isempty.hpp"
#include "popfront.hpp"
#include "pushfront.hpp"
#include "typelist.hpp"

// PushBack
template <typename E, typename List, bool is_empty = IsEmpty<List>::value>
struct PushBackT
{
  using Head = Front<List>;
  using Tail = PopFront<List>;
  using Type = PushFront<Head, typename PushBackT<E, Tail, IsEmpty<Tail>::value>::Type>;
};

template <typename E, typename List>
struct PushBackT<E, List, true>
{
  using Type = PushFront<E, List>;
};

template <typename E, typename List>
using PushBack = typename PushBackT<E, List, IsEmpty<List>::value>::Type;

static_assert(std::is_same_v<PushBack<int, TypeList<>>, TypeList<int>>);
static_assert(std::is_same_v<PushBack<int, TypeList<float>>, TypeList<float, int>>);