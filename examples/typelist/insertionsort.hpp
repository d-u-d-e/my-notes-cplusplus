#pragma once
#include "transform.hpp"
#include "typelist.hpp"

template <typename T>
struct AddConstT
{
  using Type = T const;
};

template <typename T>
using AddConst = typename AddConstT<T>::Type;

static_assert(
  std::is_same_v<Transform<AddConstT, TypeList<int, char>>, TypeList<const int, const char>>);

static_assert(std::is_same_v<Transform<AddConstT, TypeList<>>, TypeList<>>);

// Insertion Sort
template <typename List,
          template <typename T, typename U> class Compare,
          bool = IsEmpty<List>::value>
class InsertionSortT;

template <typename List, template <typename T, typename U> class Compare>
using InsertionSort = typename InsertionSortT<List, Compare>::Type;

// Insert an element into a sorted typelist
template <typename List,
          typename Element,
          template <typename T, typename U> class Compare,
          bool = IsEmpty<List>::value>
class InsertSortedT;

template <typename List, template <typename T, typename U> class Compare>
class InsertionSortT<List, Compare, false>
  : public InsertSortedT<InsertionSort<PopFront<List>, Compare>, Front<List>, Compare>
{ };

// empty list is already sorted
template <typename List, template <typename T, typename U> class Compare>
class InsertionSortT<List, Compare, true>
{
  public:
  using Type = List;
};

// the core is InsertSortedT
template <typename List, typename Element, template <typename T, typename U> class Compare>
class InsertSortedT<List, Element, Compare, true> : public PushFrontT<Element, List>
{ };

template <typename List, typename Element, template <typename T, typename U> class Compare>
class InsertSortedT<List, Element, Compare, false>
{
  using NewTail = std::conditional_t<Compare<Element, Front<List>>::value,
                                     IdentityT<List>,
                                     InsertSortedT<PopFront<List>, Element, Compare>>::Type;

  using NewHead = std::conditional_t<Compare<Element, Front<List>>::value, Element, Front<List>>;

  public:
  using Type = PushFront<NewHead, NewTail>;
};

template <typename T, typename U>
struct SmallerThanT
{
  static constexpr bool value = sizeof(T) < sizeof(U);
};

static_assert(std::is_same_v<TypeList<char, int, double>,
                             InsertionSort<TypeList<double, int, char>, SmallerThanT>>);