#pragma once
#include "insertionsort.hpp"

template <typename T, T Value> // a compile time value
struct CTValue
{
  static constexpr T value = Value;
};

template <typename T, T... Values>
struct ValueList
{ };

template <typename T, T... Values>
struct IsEmpty<ValueList<T, Values...>>
{
  static constexpr bool value = sizeof...(Values) == 0;
};

template <typename T, T Head, T... Tail>
struct FrontT<ValueList<T, Head, Tail...>>
{
  using Type = CTValue<T, Head>;
  static constexpr T value = Head;
};

template <typename T, T Head, T... Tail>
struct PopFrontT<ValueList<T, Head, Tail...>>
{
  using Type = ValueList<T, Tail...>;
};

template <typename T, T New, T... Values>
struct PushFrontT<CTValue<T, New>, ValueList<T, Values...>>
{
  using Type = ValueList<T, New, Values...>;
};

template <typename T, typename U>
struct GreaterThanT;

template <typename T, T First, T Second>
struct GreaterThanT<CTValue<T, First>, CTValue<T, Second>>
{
  static constexpr bool value = First > Second;
};

using Integers = ValueList<int, 6, 2, 4, 9, 5, 2, 1, 7>;
using SortedIntegers = InsertionSort<Integers, GreaterThanT>;
static_assert(std::is_same_v<SortedIntegers, ValueList<int, 9, 7, 6, 5, 4, 2, 2, 1>>);