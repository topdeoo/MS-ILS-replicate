cmake_minimum_required(VERSION 3.24)
project(capmds)

set(CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-O3 -Wall -Werror")
set(CMAKE_BUILD_TYPE Debug)

include_directories("include") 

add_library(capmds_shared SHARED src/graph.cpp src/capmds.cpp)

add_executable(capmds src/main.cpp)

target_link_libraries(capmds capmds_shared)
