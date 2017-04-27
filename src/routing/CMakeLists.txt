cmake_minimum_required(VERSION 3.6)
project(routing)

set(CMAKE_CXX_STANDARD 14)

#add_executable(routing ${SOURCE_FILES})

include_directories(./*)
file(GLOB_RECURSE SOURCE_FILES "*.cpp")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} ")
set(SOURCE_FILES main.cpp Node.h Node.cpp Route.h Route.cpp PriorityQueue.h PriorityQueue.cpp Map.h Map.cpp Element.h Element.cpp)
add_executable(routing ${SOURCE_FILES})
target_link_libraries(routing -I /usr/local/include -L) # -rpath,/usr/local/lib)
