cmake_minimum_required(VERSION 3.26)

project(
  book_examples
  VERSION 1.0.0
  LANGUAGES C CXX ASM
)

set(CMAKE_CXX_STANDARD 20)

add_executable(
  ${PROJECT_NAME}_tuple
  tuple/main.cpp
)

add_executable(
  ${PROJECT_NAME}_variant
  variant/main.cpp
)