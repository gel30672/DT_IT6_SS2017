cmake_minimum_required(VERSION 3.6)
project(Motordriver)


include_directories(./*)
file(GLOB_RECURSE SOURCE_FILES "*.c")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} ")
add_executable(Motordriver ${SOURCE_FILES})
target_link_libraries(Motordriver -I /usr/local/include -L /usr/local/lib -lwiringPi) # -rpath,/usr/local/lib)


