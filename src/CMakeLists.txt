cmake_minimum_required(VERSION 3.7)
project(src)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES library.cpp library.h MapVector.h MapPoint.h MapRouting.h Map.h)
add_library(src SHARED ${SOURCE_FILES})