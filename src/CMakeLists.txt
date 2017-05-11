cmake_minimum_required(VERSION 3.7)
project(locsrv)

include_directories(../include)
file(GLOB SOURCE_FILES "*.cpp")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
add_executable(locsrv ${SOURCE_FILES})

if (APPLE)
    target_link_libraries(locsrv -I /usr/local/include -L /usr/lib -lpython2.7) #Mac
elseif (UNIX AND NOT APPLE)
    target_link_libraries(locsrv -I /usr/local/include -L /usr/local/lib64 -lpython2.7 -lxl -Wl,-rpath,/usr/local/lib64) #Linux
endif()
