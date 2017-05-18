cmake_minimum_required(VERSION 3.6)
project(dt2017)

include_directories(../include)
file(GLOB SOURCE_FILES "*.cpp")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

add_subdirectory(libmdrv)
