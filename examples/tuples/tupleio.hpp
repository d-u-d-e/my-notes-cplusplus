#pragma once
#include "tuple.hpp"
#include <iostream>

inline void print_tuple(std::ostream& strm, Tuple<> const&, bool isFirst = true)
{
  strm << (isFirst ? '(' : ')');
}

template <typename Head, typename... Tail>
void print_tuple(std::ostream& strm, Tuple<Head, Tail...> const& t, bool isFirst = true)
{
  strm << (isFirst ? "(" : ", ");
  strm << t.get_head();
  print_tuple(strm, t.get_tail(), false);
}

template <typename... Types>
std::ostream& operator<<(std::ostream& strm, Tuple<Types...> const& t)
{
  print_tuple(strm, t);
  return strm;
}