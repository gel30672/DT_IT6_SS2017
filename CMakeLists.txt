cmake_minimum_required(VERSION 3.7)
project(dt2017)

set(CMAKE_CXX_STANDARD 14)

file(GLOB_RECURSE header ./include/*.h)
file(GLOB_RECURSE sources ./src/*.cpp)

add_executable(dt2017 ${sources} ${header})