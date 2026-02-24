#pragma once
#include "tupleelt2.hpp"
#include <utility>

/* Same as Tuple3, but uses TupleElt2 */

template <typename... Types>
class Tuple4;

// recursive case:
template <typename Head, typename... Tail>
class Tuple4<Head, Tail...> : private TupleElt2<sizeof...(Tail), Head>, private Tuple4<Tail...>
{
  using HeadElt = TupleElt2<sizeof...(Tail), Head>;

  public:
  Tuple4() { }

  Tuple4(Head const& head, Tuple4<Tail...> const& tail)
    : HeadElt(head)
    , Tuple4<Tail...>(tail)
  { }

  template <typename VHead,
            typename... VTail,
            typename = std::enable_if_t<sizeof...(VTail) == sizeof...(Tail)>>
  Tuple4(VHead&& vhead, VTail&&... vtail)
    : HeadElt(std::forward<VHead>(vhead))
    , Tuple4<Tail...>(std::forward<VTail>(vtail)...)
  { }

  template <typename VHead,
            typename... VTail,
            typename = std::enable_if_t<sizeof...(VTail) == sizeof...(Tail)>>
  Tuple4(Tuple4<VHead, VTail...> const& other)
    : HeadElt(other.get_head())
    , Tuple4<Tail...>(other.get_tail())
  { }

  Head& get_head()
  {
    return static_cast<HeadElt*>(this)->get();
  }
  Head const& get_head() const
  {
    return static_cast<HeadElt const*>(this)->get();
  }
  Tuple4<Tail...>& get_tail()
  {
    return *this;
  }
  Tuple4<Tail...> const& get_tail() const
  {
    return *this;
  }

  template <unsigned I, typename... Elements>
  friend auto get(Tuple4<Elements...>& t);
};

template <>
class Tuple4<>
{
  // no storage required
};