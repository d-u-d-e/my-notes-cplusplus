#include "variant.hpp"
#include <iostream>
#include <ostream>
#include <string>

class CopiedNonCopyable : public std::exception
{ };

class NonCopyable
{
  public:
  NonCopyable() { }
  NonCopyable(NonCopyable const&)
  {
    throw CopiedNonCopyable();
  }
  NonCopyable(NonCopyable&&) = default;
  NonCopyable& operator=(NonCopyable const&)
  {
    throw CopiedNonCopyable();
  }

  NonCopyable& operator=(NonCopyable&&) = default;
};

int main()
{
  Variant<int, std::string> v1;
  std::cout << std::boolalpha << "v1.empty() is: " << v1.empty() << std::endl;

  v1 = "string example";
  std::cout << "v1.is<std::string>() is: " << v1.is<std::string>() << std::endl;
  std::cout << "v1.get<std::string>() is: \"" << v1.get<std::string>() << "\"" << std::endl;

  v1 = 5;
  std::cout << "v1.is<std::string>() is: " << v1.is<std::string>() << std::endl;
  std::cout << "v1.get<int>() is: " << v1.get<int>() << std::endl;

  Variant<int, NonCopyable> v2(17);
  try
  {
    NonCopyable nc;
    v2 = nc;
  }
  catch(CopiedNonCopyable)
  {
    std::cout << "Copy assignment of NonCopyable failed." << '\n';
    if(!v2.is<int>() && !v2.is<NonCopyable>())
    {
      std::cout << "Variant has no value." << '\n';
    }
  }

  auto visitor = [](const auto v) {
    std::cout << "content of v1 inside visitor: " << v << std::endl;
  };
  v1.visit<void>(visitor);
  v1 = 18;
  v1.visit<void>(visitor);
  v1 = "string example";
  v1.visit<void>(visitor);

  // we don't have to specify the return type at all, since this is computed as the common type
  // for the result of the visitor on each type
  v1.visit(visitor);

  Variant<int, short, double, float> v3(1.5);
  auto result = v3.visit([](auto const& value) { return value + 1; });
  std::cout << "v3.visit() is: " << result << ", v3.visit() type is: " << typeid(result).name() << std::endl;

  return 0;
}