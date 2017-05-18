cmake_minimum_required(VERSION 3.6)
project(dt2017)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -v -stdlib=libc++")

include_directories(./include)
file(GLOB sources ./src/*.cpp)

add_executable(dt2017 ${sources})
