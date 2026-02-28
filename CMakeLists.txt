cmake_minimum_required(VERSION 3.16)
project(tiny_renderer)

set(CMAKE_CXX_STANDARD 17)

# Define Library and Optimization
add_library(tools src/tools.cpp)
target_include_directories(tools PUBLIC src)
# -O3 for speed, -march=native to use your specific CPU's power
target_compile_options(tools PRIVATE -O3 -march=native)

# Define Executable and Optimization
add_executable(main src/main.cpp)
target_link_libraries(main PRIVATE tools)
target_compile_options(main PRIVATE -O3 -march=native)
