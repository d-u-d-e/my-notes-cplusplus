#pragma once
#include <utility>

template <unsigned Height, typename T>
class TupleElt
{
  T value;

  public:
  TupleElt() = default;
  template <typename U>
  TupleElt(U&& other)
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

/* With this solution, we can produce a Tuple that applies the EBCO while maintaining initialization
order and support for multiple elements of the same type. We can actually improve over this version:
TupleElt can be extended slightly to inherit from the element type when it is safe to do so, allowing to save even more!
See tupleelt2.hpp
*/