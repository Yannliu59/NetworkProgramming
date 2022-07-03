cmake_minimum_required(VERSION 3.22)
project(NetworkProgramming)

set(CMAKE_CXX_STANDARD 14)

add_executable(echosrv socket/echosrv.cpp)
add_executable(echosrv2 socket/echosrv2.cpp)
add_executable(echocli_1 socket/echocli.cpp)
add_executable(echocli_2 socket/echocli.cpp)
add_executable(p2pcli socket/p2pcli.cpp)
add_executable(p2psrv socket/p2psrv.cpp)
add_executable(echosev3 socket/echosrv3.cpp)