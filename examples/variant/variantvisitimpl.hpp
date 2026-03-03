#pragma once
#include "../typelist/accumulate.hpp"
#include "../typelist/typelist.hpp"
#include "commontype.hpp"
#include "emptyvariant.hpp"

/*
    When calling a visitor that may produce different types for each of the variant’s element types, how
    can we combine those types into a single result type for visit()?
    The result of visit when the return type is specified is simply R.
    If no R is specified, it's default value is the incomplete sentinel type ComputedResultType.
    There are some obvious cases - if the visitor returns the same type for each element type, that should be the result type of visit().
*/
template <typename R, typename Visitor, typename... ElementTypes>
class VisitResultT
{
  public:
  using Type = R;
};

// the result type produced when calling a visitor with a value of type T:
template <typename Visitor, typename T>
using VisitElementResult = decltype(declval<Visitor>()(declval<T>()));

class ComputedResultType;
// the common result type for a visitor called with each of the given element types:
template <typename Visitor, typename... ElementTypes>
class VisitResultT<ComputedResultType, Visitor, ElementTypes...>
{
  using ResultTypes = TypeList<VisitElementResult<Visitor, ElementTypes>...>;

  public:
  using Type = Accumulate<PopFront<ResultTypes>, CommonTypeT, Front<ResultTypes>>;
};
/*
    The VisitResult computation occurs in two stages. First, VisitElementResult computes the
    result type produced when calling the visitor with a value of type T. This metafunction is applied to
    each of the given element types to determine all of the result types that the visitor could produce,
    capturing the result in the typelist ResultTypes.

    Next, the computation uses the Accumulate algorithm to apply 
    the common-type computation to the typelist of result types. Its initial value (the third argument
    to Accumulate) is the first result type.
*/

template <typename R, typename Visitor, typename... ElementTypes>
using VisitResult = typename VisitResultT<R, Visitor, ElementTypes...>::Type;

/*
    // This is the simpler alternative using std::common_type_t
    template<typename Visitor, typename... ElementTypes>
    class VisitResultT<ComputedResultType, Visitor, ElementTypes...>
    {
    public:
        using Type = std::common_type_t<VisitElementResult<Visitor, ElementTypes>...>;
    };
*/

template <typename R, typename V, typename Visitor, typename Head, typename... Tail>
R variant_visit_impl(V&& variant, Visitor&& vis, TypeList<Head, Tail...>)
{
  if(variant.template is<Head>())
  {
    return static_cast<R>(
      std::forward<Visitor>(vis)(std::forward<V>(variant).template get<Head>()));
  }
  else if constexpr(sizeof...(Tail) > 0)
  {
    return variant_visit_impl<R>(
      std::forward<V>(variant), std::forward<Visitor>(vis), TypeList<Tail...>());
  }
  else
  {
    throw EmptyVariant();
  }
}