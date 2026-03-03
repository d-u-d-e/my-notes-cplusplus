#pragma once

#include <utility>

template <typename... Types>
class Tuple;

/* TupleElt template is used to wrap each element type so that Tuple can inherit from it */

template <typename T>
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

template <typename Head, typename... Tail>
class Tuple<Head, Tail...> : private TupleElt<Head>, private Tuple<Tail...>
{
  public:
  Head& get_head()
  {
    // potentially ambiguous
    return static_cast<TupleElt<Head>*>(this)->get();
  }
  Head const& get_head() const
  {
    // potentially ambiguous
    return static_cast<TupleElt<Head> const*>(this)->get();
  }
  Tuple<Tail...>& get_tail()
  {
    return *this;
  }
  Tuple<Tail...> const& get_tail() const
  {
    return *this;
  }
};

// basis case:
template <>
class Tuple<>
{
  // no storage required
};

/*
    While this approach has solved the initialization-ordering problem, it has introduced a new (worse)
    problem: We can no longer extract elements from a tuple that has two elements of the same type,
    such as Tuple<int, int>, because the derived-to-base conversion from a tuple to TupleElt of that
    type (e.g., TupleElt<int>) will be ambiguous. To break the ambiguity, we need to ensure that each TupleElt base class 
    is unique within a given Tuple. One approach is to encode the "height" of this value within its tuple, that is, the length of the
    tail tuple. See tuplestorage3.hpp
*/