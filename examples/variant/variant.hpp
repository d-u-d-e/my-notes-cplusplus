#pragma once

#include "emptyvariant.hpp"
#include "variantchoice.hpp"
#include "variantstorage.hpp"
#include "variantvisitimpl.hpp"
#include <cassert>

template <typename... Types>
class Variant : private VariantStorage<Types...>, private VariantChoice<Types, Types...>...
{
  template <typename T, typename... OtherTypes>
  friend class VariantChoice;

  public:
  template <typename T>
  bool is() const;

  template <typename T>
  T& get() &;

  template <typename T>
  T const& get() const&;

  using VariantChoice<Types, Types...>::VariantChoice...;
  Variant()
  {
    /*
    making this the default initialization behavior would promote the
    exceptional state of an empty variant to a common one 
    We follow the semantics of C++17’s std::variant<> and default-construct a value of the first type in
    the list of types */

    //this->set_discriminator(0);
    *this = Front<TypeList<Types...>>();
  }

  Variant(Variant const& source);
  Variant(Variant&& source);

  using VariantChoice<Types, Types...>::operator=...;
  Variant& operator=(Variant const& source);
  Variant& operator=(Variant&& source);

  bool empty() const;
  ~Variant()
  {
    destroy();
  }
  void destroy();

  // visitors
  template <typename R = ComputedResultType, typename Visitor>
  VisitResult<R, Visitor, Types&...> visit(Visitor&& vis) &;
  template <typename R = ComputedResultType, typename Visitor>
  VisitResult<R, Visitor, Types const&...> visit(Visitor&& vis) const&;
  template <typename R = ComputedResultType, typename Visitor>
  VisitResult<R, Visitor, Types&&...> visit(Visitor&& vis) &&;

  // templated constructors
  template <typename... SourceTypes>
  Variant(Variant<SourceTypes...> const& source);
  template <typename... SourceTypes>
  Variant(Variant<SourceTypes...>&& source);
};

template <typename... Types>
template <typename T>
T& Variant<Types...>::get() &
{
  if(empty())
  {
    throw EmptyVariant();
  }

  assert(is<T>());
  return *(this->template get_buff_as<T>());
}

template <typename... Types>
template <typename T>
const T& Variant<Types...>::get() const&
{
  if(empty())
  {
    throw EmptyVariant();
  }

  assert(is<T>());
  return *(this->template get_buff_as<T>());
}

template <typename... Types>
template <typename T>
bool Variant<Types...>::is() const
{
  /*
    If the type we’re looking for (T) is not found in the list, the VariantChoice base class will fail
    to instantiate because FindIndexOfT will not contain a value member, causing an (intentional)
    compilation failure in is<T>(). This prevents user errors where the user is asking for a type that
    cannot possibly be stored in the variant.
    */
  return this->get_discriminator() == VariantChoice<T, Types...>::discriminator;
}

template <typename... Types>
void Variant<Types...>::destroy()
{
  // call destroy() on each VariantChoice base class; at most one will succeed:
  (VariantChoice<Types, Types...>::destroy(), ...);

  // indicate that the variant does not store a value
  this->set_discriminator(0);
}

template <typename... Types>
bool Variant<Types...>::empty() const
{
  return this->get_discriminator() == 0;
}

template <typename... Types>
template <typename R, typename Visitor>
VisitResult<R, Visitor, Types&...> Variant<Types...>::visit(Visitor&& vis) &
{
  using Result = VisitResult<R, Visitor, Types&...>;
  return variant_visit_impl<Result>(*this, std::forward<Visitor>(vis), TypeList<Types...>{});
}

template <typename... Types>
template <typename R, typename Visitor>
VisitResult<R, Visitor, Types const&...> Variant<Types...>::visit(Visitor&& vis) const&
{
  using Result = VisitResult<R, Visitor, Types const&...>;
  return variant_visit_impl<Result>(*this, std::forward<Visitor>(vis), TypeList<Types...>{});
}

template <typename... Types>
template <typename R, typename Visitor>
VisitResult<R, Visitor, Types&&...> Variant<Types...>::visit(Visitor&& vis) &&
{
  using Result = VisitResult<R, Visitor, Types&&...>;
  return variant_visit_impl<Result>(
    std::move(*this), std::forward<Visitor>(vis), TypeList<Types...>{});
}

template <typename... Types>
Variant<Types...>::Variant(Variant const& source)
{
  /*
  To copy a source variant, we need to determine
  which type it is currently storing, copy-construct that value into the buffer, and set that discrimi-
  nator. Fortunately, visit() handles decoding the active value of the source variant, and the copy-
  assignment operator inherited from VariantChoice will copy-construct a value into the buffer, 
  leading to a compact implementation.
  */

  if(!source.empty())
  {
    source.visit([&](auto const& value) { *this = value; });
  }
}

template <typename... Types>
Variant<Types...>::Variant(Variant&& source)
{
  if(!source.empty())
  {
    std::move(source).visit([&](auto&& value) { *this = std::move(value); });
  }
}

template <typename... Types>
template <typename... SourceTypes>
Variant<Types...>::Variant(Variant<SourceTypes...> const& source)
{
  if(!source.empty())
  {
    source.visit([&](auto const& value) { *this = value; });
  }
}

template <typename... Types>
template <typename... SourceTypes>
Variant<Types...>::Variant(Variant<SourceTypes...>&& source)
{
  if(!source.empty())
  {
    std::move(source).visit([&](auto&& value) { *this = std::move(value); });
  }
}

template <typename... Types>
Variant<Types...>& Variant<Types...>::operator=(Variant const& source)
{
  if(!source.empty())
  {
    source.visit([&](auto const& value) { *this = value; });
  }
  else
  {
    destroy();
  }
  return *this;
}

template <typename... Types>
Variant<Types...>& Variant<Types...>::operator=(Variant&& source)
{
  if(!source.empty())
  {
    std::move(source).visit([&](auto&& value) { *this = std::move(value); });
  }
  else
  {
    destroy();
  }
  return *this;
}