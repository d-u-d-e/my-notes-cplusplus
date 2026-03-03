#pragma once
#include "findindexof.hpp"
#include <utility>

template <typename... Types>
class Variant;

template <typename T, typename... Types>
class VariantChoice
{
  using Derived = Variant<Types...>;
  Derived& get_derived()
  {
    return static_cast<Derived&>(*this);
  }

  const Derived& get_derived() const
  {
    return static_cast<Derived const&>(*this);
  }

  protected:
  /*
    The discriminator value 0 is reserved for cases where the variant contains no value, which is an
    odd state that can only be observed when an exception is thrown during assignment.
  */
  constexpr static unsigned discriminator = FindIndexOfT<TypeList<Types...>, T>::value + 1;

  public:
  VariantChoice() { }
  VariantChoice(T const& value);
  VariantChoice(T&& value);
  Derived& operator=(T const& value);
  Derived& operator=(T&& value);
  bool destroy();
};

// constructor
template <typename T, typename... Types>
VariantChoice<T, Types...>::VariantChoice(T const& value)
{
  // place value in buffer and set type discriminator:
  new(get_derived().get_raw_buff()) T(value);
  get_derived().set_discriminator(discriminator);
}

template <typename T, typename... Types>
VariantChoice<T, Types...>::VariantChoice(T&& value)
{
  // place moved value in buffer and set type discriminator:
  new(get_derived().get_raw_buff()) T(std::move(value));
  get_derived().set_discriminator(discriminator);
}

// destroy
template <typename T, typename... Types>
bool VariantChoice<T, Types...>::destroy()
{
  if(get_derived().get_discriminator() == discriminator)
  {
    // if type matches, call placement delete:
    get_derived().template get_buff_as<T>()->~T();
    return true;
  }
  return false;
}

// assignment
/*
    If the destruction of the existing value completes but the initialization of the new value throws an
    exception, what is the state of the variant? In our implementation, Variant::destroy() resets
    the discriminator value to 0. In nonexceptional cases, the discriminator will be set appropriately
    after initialization completes. When an exception occurs during initialization of the new value, the
    discriminator remains 0 to indicate that the variant does not store a value.
*/

template <typename T, typename... Types>
auto VariantChoice<T, Types...>::operator=(T const& value) -> Derived&
{
  if(get_derived().get_discriminator() == discriminator)
  {
    // assign new value of same type:
    *get_derived().template get_buff_as<T>() = value;
  }
  else
  {
    // assign new value of different type:
    get_derived().destroy(); // try destroy() for all types
    new(get_derived().get_raw_buff()) T(value); // place new value
    get_derived().set_discriminator(discriminator);
  }
  return get_derived();
}

template <typename T, typename... Types>
auto VariantChoice<T, Types...>::operator=(T&& value) -> Derived&
{
  if(get_derived().get_discriminator() == discriminator)
  {
    // assign new value of same type:
    *get_derived().template get_buff_as<T>() = std::move(value);
  }
  else
  {
    // assign new value of different type:
    get_derived().destroy(); // try destroy() for all types
    new(get_derived().get_raw_buff()) T(std::move(value)); // place new value
    get_derived().set_discriminator(discriminator);
  }
  return get_derived();
}