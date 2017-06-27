cmake_minimum_required(VERSION 3.6)
project(dt2017)

include_directories(./*)
file(GLOB_RECURSE SOURCE_FILES main.cpp "*.c")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS}")
#set(CMAKE_CXX_COMPILER /usr/bin/arm-linux-gnueabihf-g++)
add_executable(dt2017 ${SOURCE_FILES})
target_link_libraries(dt2017 routedriver -I /usr/local/include -L /usr/local/lib -lwiringPi -lpthread -lpython2.7 -lxl -Wl,-rpath,/usr/local/lib64)


