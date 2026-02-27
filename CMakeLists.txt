cmake_minimum_required(VERSION 3.16)
project(tiny_renderer)

set(CMAKE_CXX_STANDARD 17)

add_library(tools src/tools.cpp)
target_include_directories(tools PUBLIC src)
add_executable(main src/main.cpp)
target_link_libraries(main PRIVATE tools)
