#include <type_traits>

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
template <typename E, typename List>
struct PushBackT
{
  using Head = Front<List>;
  using Tail = PopFront<List>;
  using Type = PushFront<Head, typename PushBackT<E, Tail>::Type>;
};

template <typename E>
struct PushBackT<E, TypeList<>>
{
  using Type = TypeList<E>;
};

template <typename E, typename List>
using PushBack = typename PushBackT<E, List>::Type;

static_assert(std::is_same_v<PushBack<int, TypeList<>>, TypeList<int>>);
static_assert(std::is_same_v<PushBack<int, TypeList<float>>, TypeList<float, int>>);

// Reverse
template <typename List>
struct ReverseT
{
  using Tail = PopFront<List>;
  using Head = Front<List>;
  using Type = PushBack<Head, typename ReverseT<Tail>::Type>;
};

template <>
struct ReverseT<TypeList<>>
{
  using Type = TypeList<>;
};

template <typename List>
using Reverse = typename ReverseT<List>::Type;

static_assert(std::is_same_v<Reverse<TypeList<float, int, char>>, TypeList<char, int, float>>);

// Transforming