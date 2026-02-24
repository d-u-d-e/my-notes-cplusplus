#include <type_traits>

template <typename T>
struct IdentityT
{
  using Type = T;
};

template <typename... Args>
struct TypeList
{ };

// Front
template <typename Head, typename... Tail>
struct FrontT;

template <typename Head, typename... Tail>
struct FrontT<TypeList<Head, Tail...>>
{
  using Type = Head;
};

template <typename List>
using Front = typename FrontT<List>::Type;

static_assert(std::is_same_v<Front<TypeList<int, char, long>>, int>);

// PopFront
template <typename Head, typename... Tail>
struct PopFrontT;

template <typename Head, typename... Tail>
struct PopFrontT<TypeList<Head, Tail...>>
{
  using Type = TypeList<Tail...>;
};

template <typename List>
using PopFront = typename PopFrontT<List>::Type;

static_assert(std::is_same_v<PopFront<TypeList<int, char, long>>, TypeList<char, long>>);
static_assert(std::is_same_v<PopFront<TypeList<int>>, TypeList<>>);

// Empty
template <typename List>
struct IsEmpty
{
  constexpr static bool value = false;
};

template <>
struct IsEmpty<TypeList<>>
{
  constexpr static bool value = true;
};

static_assert(IsEmpty<PopFront<TypeList<int>>>::value);
static_assert(!IsEmpty<TypeList<int>>::value);

// PushFront
template <typename E, typename... Elements>
struct PushFrontT;

template <typename E, typename... Elements>
struct PushFrontT<E, TypeList<Elements...>>
{
  using Type = TypeList<E, Elements...>;
};

template <typename E, typename List>
using PushFront = typename PushFrontT<E, List>::Type;

static_assert(std::is_same_v<PushFront<short, TypeList<int>>, TypeList<short, int>>);

// Nth element
template <typename List, unsigned int N>
struct NthElementT
{
  using Type = typename NthElementT<PopFront<List>, N - 1>::Type;
};

template <typename List>
struct NthElementT<List, 0U>
{
  using Type = Front<List>;
};

template <typename List, std::size_t N>
using NthElement = typename NthElementT<List, N>::Type;

static_assert(std::is_same_v<NthElement<TypeList<int>, 0>, int>);
static_assert(std::is_same_v<NthElement<TypeList<int, short, double, char>, 2>, double>);

// PushBack
template <typename E, typename List, bool is_empty = IsEmpty<List>::value>
struct PushBackT
{
  using Head = Front<List>;
  using Tail = PopFront<List>;
  using Type = PushFront<Head, typename PushBackT<E, Tail, IsEmpty<Tail>::value>::Type>;
};

template <typename E, typename List>
struct PushBackT<E, List, true>
{
  using Type = PushFront<E, List>;
};

template <typename E, typename List>
using PushBack = typename PushBackT<E, List, IsEmpty<List>::value>::Type;

static_assert(std::is_same_v<PushBack<int, TypeList<>>, TypeList<int>>);
static_assert(std::is_same_v<PushBack<int, TypeList<float>>, TypeList<float, int>>);

// Reverse
template <typename List, bool is_empty = IsEmpty<List>::value>
struct ReverseT
{
  using Tail = PopFront<List>;
  using Head = Front<List>;
  using Type = PushBack<Head, typename ReverseT<Tail, IsEmpty<Tail>::value>::Type>;
};

template <typename List>
struct ReverseT<List, true>
{
  using Type = List;
};

template <typename List>
using Reverse = typename ReverseT<List>::Type;

static_assert(std::is_same_v<Reverse<TypeList<float, int, char>>, TypeList<char, int, float>>);

// Transforming

template <template <typename T> class F, typename List, bool is_empty = IsEmpty<List>::value>
struct TransformT
{
  using Head = Front<List>;
  using Tail = PopFront<List>;
  using Type =
    PushFront<typename F<Head>::Type, typename TransformT<F, Tail, IsEmpty<Tail>::value>::Type>;
};

template <template <typename T> class F, typename List>
struct TransformT<F, List, true>
{
  using Type = List;
};

// Specialization, more optimized!
template <template <typename T> class F, typename... Elements>
struct TransformT<F, TypeList<Elements...>, false>
{
  using Type = TypeList<typename F<Elements>::Type...>;
};

template <template <typename T> class F, typename List>
using Transform = typename TransformT<F, List>::Type;

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
          template <typename T, typename U>
          class Compare,
          bool = IsEmpty<List>::value>
class InsertionSortT;

template <typename List, template <typename T, typename U> class Compare>
using InsertionSort = typename InsertionSortT<List, Compare>::Type;

// Insert an element into a sorted typelist
template <typename List,
          typename Element,
          template <typename T, typename U>
          class Compare,
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

// nontype typelists

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