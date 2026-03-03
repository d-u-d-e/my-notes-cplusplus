#pragma once
#include "../typelist/genericlargesttype.hpp"
#include "../typelist/typelist.hpp"
#include <new>

template <typename... Types>
class VariantStorage
{
  using LargestT = LargestType<TypeList<Types...>>;
  alignas(Types...) unsigned char buffer[sizeof(LargestT)];
  unsigned char discriminator = 0;

  public:
  unsigned char get_discriminator() const
  {
    return discriminator;
  }

  void set_discriminator(unsigned char d)
  {
    discriminator = d;
  }

  void* get_raw_buff()
  {
    return buffer;
  }

  const void* get_raw_buff() const
  {
    return buffer;
  }

  template <typename T>
  T* get_buff_as()
  {
    return std::launder(reinterpret_cast<T*>(buffer));
  }

  template <typename T>
  T const* get_buff_as() const
  {
    return std::launder(reinterpret_cast<T const*>(buffer));
  }
};