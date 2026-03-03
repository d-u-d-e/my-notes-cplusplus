#pragma once
#include <utility>

using std::declval;

template <typename T, typename U>
class CommonTypeT
{
  public:
  /* C++ already has a notion of a reasonable result type.
  In the ternary expression b ? x : y, the type of the expression is the common type between
  the types of x and y. */
  using Type = decltype(true ? declval<T>() : declval<U>());
};
template <typename T, typename U>
using CommonType = typename CommonTypeT<T, U>::Type;