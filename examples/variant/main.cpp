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

  auto printer_visitor = [](const auto v) {
    std::cout << "content of variant inside visitor: " << v << std::endl;
  };
  v1.visit<void>(printer_visitor);
  v1 = 18;
  v1.visit<void>(printer_visitor);
  v1 = "string example";
  v1.visit<void>(printer_visitor);

  // we don't have to specify the return type at all, since this is computed as the common type
  // for the result of the visitor on each type
  v1.visit(printer_visitor);

  Variant<int, short, double, float> v3(1.5);
  auto result = v3.visit([](auto const& value) { return value + 1; });
  std::cout << "v3.visit() is: " << result << ", v3.visit() type is: " << typeid(result).name()
            << std::endl;

  Variant<int, double> v4;
  if(v4.is<int>())
  {
    std::cout << "Default-constructed v4 stores the int " << v4.get<int>() << '\n';
  }

  Variant<short, float, char const*> v5((short)123);
  Variant<int, std::string, double> v6(v5);
  std::cout << "v6 contains the integer " << v6.get<int>() << '\n';

  v5 = 3.14f;
  Variant<double, int, std::string> v7(std::move(v5));
  std::cout << "v7 contains the double " << v7.get<double>() << '\n';

  v5 = "hello";
  Variant<double, int, std::string> v8(std::move(v5));
  std::cout << "v8 contains the string " << v8.get<std::string>() << '\n';

  Variant<int, NonCopyable> v9;
  std::cout << "v9 contains int: " << v9.is<int>() << '\n';
  v9 = v2;
  std::cout << "v9 empty: " << v9.empty() << '\n';

  return 0;
}