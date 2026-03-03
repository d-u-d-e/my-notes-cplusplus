#pragma once
#include "front.hpp"
#include "isempty.hpp"
#include "pushfront.hpp"
#include "typelist.hpp"

// Transforming

template <template <typename T> class F, typename List, bool is_empty = IsEmpty<List>::value>
struct TransformT
{
  using Head = Front<List>;
  using Tail = PopFront<List>;
  using Type =
    PushFront<typename F<Head>::Type, typename TransformT<F, Tail, IsEmpty<Tail>::value>::Type>;
};

template <template <typename T> class F, typename List>
struct TransformT<F, List, true>
{
  using Type = List;
};

// Specialization, more optimized!
template <template <typename T> class F, typename... Elements>
struct TransformT<F, TypeList<Elements...>, false>
{
  using Type = TypeList<typename F<Elements>::Type...>;
};

template <template <typename T> class F, typename List>
using Transform = typename TransformT<F, List>::Type;