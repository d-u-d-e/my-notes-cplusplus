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
  std::cout << "t1<1> is: " << v << std::endl;

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

  return 0;
}