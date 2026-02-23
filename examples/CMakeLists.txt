cmake_minimum_required(VERSION 3.26)

project(
  Examples
  VERSION 1.0.0
  LANGUAGES C CXX ASM
)

set(CMAKE_CXX_STANDARD 20)

# Add an executable target
add_executable(
  ${PROJECT_NAME}
  main.cpp
)