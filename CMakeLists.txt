cmake_minimum_required(VERSION 3.7)
project(algorithms_cpp)

set(CMAKE_CXX_STANDARD 14)

file(GLOB_RECURSE header ./include/*.h)
file(GLOB_RECURSE sources ./src/*.cpp)

add_executable(algorithms_cpp ${sources} ${header})
