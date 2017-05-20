cmake_minimum_required(VERSION 3.6)
project(dt2017)

set(CMAKE_CXX_STANDARD 14)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ -v")

include_directories(./include)
file(GLOB sources main.cpp ./src/*.cpp)

add_executable(dt2017 ${sources})
