cmake_minimum_required(VERSION 3.19)
project(untitled)

set(CMAKE_CXX_STANDARD 14)
include_directories(./src ./include)

add_executable(untitled ./src/Edge.cpp ./src/Cloud.cpp main.cpp test.cpp)