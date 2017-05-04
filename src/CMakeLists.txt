cmake_minimum_required(VERSION 3.7)
project(ReadData)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES main.cpp ReadDataFromSerial.cpp ReadDataFromSerial.h LocDet.h LocDet.cpp)
add_executable(ReadData ${SOURCE_FILES})