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
add_executable(getiplist socket/getiplist.cpp)

add_executable(echocli4 socket/echocli4.cpp)
add_executable(echosrv4 socket/echosrv4.cpp)

add_executable(echosrv5 socket/echosrv5.cpp)
add_executable(echocli5 socket/echocli5.cpp)

add_executable(echocli6 socket/echocli6.cpp)
add_executable(echosrv6 socket/echosrv6.cpp)

add_executable(echocli7 socket/echocli7.cpp)
add_executable(echosrv7 socket/echosrv7.cpp)