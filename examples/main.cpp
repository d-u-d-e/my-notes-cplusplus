#include "tuples/algos.hpp"
#include "tuples/tuple.hpp"
#include "tuples/tuple_typelist.hpp"
#include "tuples/tupleeq.hpp"
#include "tuples/tupleio.hpp"
#include <iostream>
#include <string>

int main()
{

  Tuple<int, double, std::string> t1(17, 3.14, "Hello, World!");
  auto v = get<1>(t1);
  std::cout << std::boolalpha << "t1<1> is: " << v << std::endl;

  /* The following fails if we don't disable the variadic constructor

    template <typename VHead, typename... VTail>
    Tuple(VHead&& vhead, VTail&&... vtail)

    because this is a better match than the most constrained constructor 
    accepting a const ref to a Tuple
  */

  Tuple<long int, long double, std::string> t2(t1);

  std::cout << "t1 is: " << t1 << std::endl;
  std::cout << "t2 is: " << t2 << std::endl;

  std::cout << "t1==t2 is: " << (t1 == t2) << std::endl;

  using T3 = PopFront<PushBack<bool, decltype(t1)>>;
  T3 t3(get<1>(t1), get<2>(t1), true);
  std::cout << "t3 is: " << t3 << std::endl;

  auto e0 = push_front(t3, 11);
  std::cout << "push_front(t3, 11) is: " << e0 << std::endl;

  auto e1 = push_back(t3, 11);
  std::cout << "push_back(t3, 11) is: " << e1 << std::endl;

  auto e2 = pop_front(t3);
  std::cout << "pop_front(t3) is: " << e2 << std::endl;

  auto e3 = reverse(t3);
  std::cout << "reverse(t3) is: " << e3 << std::endl;

  auto e4 = pop_back(t3);
  std::cout << "pop_back(t3) is: " << e4 << std::endl;

  return 0;
}