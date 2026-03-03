#include "variant.hpp"
#include <iostream>
#include <ostream>
#include <string>

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

  return 0;
}