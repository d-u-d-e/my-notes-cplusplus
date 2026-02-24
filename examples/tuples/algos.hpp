#pragma once
#include "../typelists.hpp"
#include "makeindexlist.hpp"
#include "tuple.hpp"

template <typename... Types, typename V>
auto push_front(Tuple<Types...> const& tuple, V const& value)
{
  return PushFront<V, Tuple<Types...>>(value, tuple);
}

// basis case
template <typename V>
auto push_back(Tuple<> const&, V const& value)
{
  return Tuple<V>(value);
}

// recursive case
template <typename Head, typename... Tail, typename V>
auto push_back(Tuple<Head, Tail...> const& tuple, V const& value)
{
  return Tuple<Head, Tail..., V>(tuple.get_head(), push_back(tuple.get_tail(), value));
}

template <typename... Types>
auto pop_front(Tuple<Types...> const& tuple)
{
  return tuple.get_tail();
}

/* This is a very inefficient implementation of reverse! 
   We can do better using index lists */

/*

// basis case
inline Tuple<> reverse(Tuple<> const& t)
{
  return t;
}

// recursive case
template <typename Head, typename... Tail>
Reverse<Tuple<Head, Tail...>> reverse(Tuple<Head, Tail...> const& t)
{
  return push_back(reverse(t.get_tail()), t.get_head());
}*/

template <typename... Elements, unsigned... Indices>
auto select(Tuple<Elements...> const& t, ValueList<unsigned, Indices...>)
{
  return make_Tuple(get<Indices>(t)...);
}

template <typename... Elements>
auto reverse(Tuple<Elements...> const& t)
{
  return select(t, Reverse<MakeIndexList<sizeof...(Elements)>>{});
}

template <typename... Types>
auto pop_back(Tuple<Types...> const& tuple)
{
  return reverse(pop_front(reverse(tuple)));
}

// apply
template <typename F, typename... Elements, unsigned... Indices>
auto apply_impl(F f, Tuple<Elements...> const& t, ValueList<unsigned, Indices...>)
  -> decltype(f(get<Indices>(t)...))
{
  return f(get<Indices>(t)...);
}

template <typename F, typename... Elements, unsigned N = sizeof...(Elements)>
auto apply(F f, Tuple<Elements...> const& t) -> decltype(apply_impl(f, t, MakeIndexList<N>()))
{
  return apply_impl(f, t, MakeIndexList<N>{});
}