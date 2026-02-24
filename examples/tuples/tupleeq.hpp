#pragma once
#include "tuple.hpp"

inline bool operator==(Tuple<> const&, Tuple<> const&)
{
  // empty tuples are always equivalent
  return true;
}

template <typename Head1,
          typename... Tail1,
          typename Head2,
          typename... Tail2,
          typename = std::enable_if_t<sizeof...(Tail1) == sizeof...(Tail2)>>
bool operator==(Tuple<Head1, Tail1...> const& lhs, Tuple<Head2, Tail2...> const& rhs)
{
  return lhs.get_head() == rhs.get_head() && lhs.get_tail() == rhs.get_tail();
}
