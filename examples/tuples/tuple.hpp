#pragma once
#include <type_traits>
#include <utility>

template <typename... Types>
class Tuple;

template <typename Head, typename... Tail>
class Tuple<Head, Tail...>
{
  private:
  Head head;
  Tuple<Tail...> tail;

  public:
  Tuple() { }

  Tuple(Head const& head, Tuple<Tail...> const& tail)
    : head(head)
    , tail(tail)
  { }

  template <typename VHead,
            typename... VTail,
            typename = std::enable_if_t<sizeof...(VTail) == sizeof...(Tail)>>
  Tuple(VHead&& vhead, VTail&&... vtail)
    : head(std::forward<VHead>(vhead))
    , tail(std::forward<VTail>(vtail)...)
  { }

  template <typename VHead,
            typename... VTail,
            typename = std::enable_if_t<sizeof...(VTail) == sizeof...(Tail)>>
  Tuple(Tuple<VHead, VTail...> const& other)
    : head(other.get_head())
    , tail(other.get_tail())
  { }

  Head& get_head()
  {
    return head;
  }
  Head const& get_head() const
  {
    return head;
  }
  Tuple<Tail...>& get_tail()
  {
    return tail;
  }
  Tuple<Tail...> const& get_tail() const
  {
    return tail;
  }
};

// basis case
template <>
class Tuple<>
{ };

template <unsigned N>
struct TupleGet
{
  template <typename Head, typename... Tail>
  static auto apply(Tuple<Head, Tail...> const& t)
  {
    return TupleGet<N - 1>::apply(t.get_tail());
  }
};

// basis case
template <>
struct TupleGet<0>
{
  template <typename Head, typename... Tail>
  static Head const& apply(Tuple<Head, Tail...> const& t)
  {
    return t.get_head();
  }
};

/* Note that the function template get is simply a thin wrapper over a call to a static member function
of TupleGet. This technique is effectively a workaround for the lack of partial specialization of
function templates (discussed in Section 17.3 on page 356), which we use to specialize on the value
of N.*/

template <unsigned N, typename... Types>
auto get(Tuple<Types...> const& t)
{
  return TupleGet<N>::apply(t);
}

template <typename... Types>
auto make_tuple(Types&&... elems)
{
  return Tuple<std::decay_t<Types>...>(std::forward<Types>(elems)...);
}
