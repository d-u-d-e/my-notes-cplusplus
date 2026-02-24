#include "../typelists.hpp"
#include "tuple.hpp"

template <>
struct IsEmpty<Tuple<>>
{
  static constexpr bool value = true;
};

// extract front element:
template <typename Head, typename... Tail>
class FrontT<Tuple<Head, Tail...>>
{
  public:
  using Type = Head;
};

// remove front element:
template <typename Head, typename... Tail>
class PopFrontT<Tuple<Head, Tail...>>
{
  public:
  using Type = Tuple<Tail...>;
};

// add element to the front:
template <typename... Types, typename Element>
class PushFrontT<Element, Tuple<Types...>>
{
  public:
  using Type = Tuple<Element, Types...>;
};