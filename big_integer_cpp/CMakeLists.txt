cmake_minimum_required(VERSION 3.15)
project(big_integer_cpp)

set(CMAKE_CXX_STANDARD 17)

add_executable(big_integer_cpp main.cpp big_integer.h)
target_link_libraries(big_integer_cpp -lgmp -lgmpxx -lpthread)