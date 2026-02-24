#pragma once
#include "tupleelt1.hpp"
#include <utility>

/* With this solution, we can produce a Tuple that applies the EBCO while maintaining initialization
order and support for multiple elements of the same type: */

template <typename... Types>
class Tuple3;

// recursive case:
template <typename Head, typename... Tail>
class Tuple3<Head, Tail...> : private TupleElt<sizeof...(Tail), Head>, private Tuple3<Tail...>
{
  using HeadElt = TupleElt<sizeof...(Tail), Head>;

  public:
  Tuple3() { }

  Tuple3(Head const& head, Tuple3<Tail...> const& tail)
    : HeadElt(head)
    , Tuple3<Tail...>(tail)
  { }

  template <typename VHead,
            typename... VTail,
            typename = std::enable_if_t<sizeof...(VTail) == sizeof...(Tail)>>
  Tuple3(VHead&& vhead, VTail&&... vtail)
    : HeadElt(std::forward<VHead>(vhead))
    , Tuple3<Tail...>(std::forward<VTail>(vtail)...)
  { }

  template <typename VHead,
            typename... VTail,
            typename = std::enable_if_t<sizeof...(VTail) == sizeof...(Tail)>>
  Tuple3(Tuple3<VHead, VTail...> const& other)
    : HeadElt(other.get_head())
    , Tuple3<Tail...>(other.get_tail())
  { }

  Head& get_head()
  {
    return static_cast<HeadElt*>(this)->get();
  }
  Head const& get_head() const
  {
    return static_cast<HeadElt const*>(this)->get();
  }
  Tuple3<Tail...>& get_tail()
  {
    return *this;
  }
  Tuple3<Tail...> const& get_tail() const
  {
    return *this;
  }
};

template <>
class Tuple3<>
{
  // no storage required
};