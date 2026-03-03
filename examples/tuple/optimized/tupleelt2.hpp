#pragma once
#include <type_traits>
#include <utility>

template <unsigned Height, typename T, bool = std::is_class<T>::value && !std::is_final<T>::value>
class TupleElt2;

// As in tupleelt1.hpp
template <unsigned Height, typename T>
class TupleElt2<Height, T, false>
{
  T value;

  public:
  TupleElt2() = default;
  template <typename U>
  TupleElt2(U&& other)
    : value(std::forward<U>(other))
  { }
  T& get()
  {
    return value;
  }
  T const& get() const
  {
    return value;
  }
};

template <unsigned Height, typename T>
class TupleElt2<Height, T, true> : private T
{
  public:
  TupleElt2() = default;
  template <typename U>
  TupleElt2(U&& other)
    : T(std::forward<U>(other))
  { }
  T& get()
  {
    return *this;
  }
  T const& get() const
  {
    return *this;
  }
};