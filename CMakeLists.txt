cmake_minimum_required(VERSION 3.22)
project(NetworkProgramming)

set(CMAKE_CXX_STANDARD 14)

add_executable(echosrv echosrv.cpp)
add_executable(echocli echocli.cpp)