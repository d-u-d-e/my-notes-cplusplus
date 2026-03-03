#pragma once

#include "variantchoice.hpp"
#include "variantstorage.hpp"
#include <cassert>
#include <exception>

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

  template <typename T>
  T&& get() &&;

  using VariantChoice<Types, Types...>::VariantChoice...;
  Variant()
  {
    this->set_discriminator(0);
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
};

class EmptyVariant : public std::exception
{ };

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
T&& Variant<Types...>::get() &&
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