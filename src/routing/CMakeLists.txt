cmake_minimum_required(VERSION 3.6)
project(routing)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES main.cpp Node.h Node.cpp Route.h Route.cpp PriorityQueue.h PriorityQueue.cpp Map.h Map.cpp Element.h Element.cpp)
add_executable(routing ${SOURCE_FILES})