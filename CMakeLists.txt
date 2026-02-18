cmake_minimum_required(VERSION 3.16)
project(NetworkFramework)

set(CMAKE_CXX_STANDARD 23)

include_directories(include)

add_library(Network
    src/TCPSocket.cpp
    src/Server.cpp
    src/Client.cpp
)

add_executable(server examples/simple_server.cpp)
target_link_libraries(server Network)

add_executable(client examples/simple_client.cpp)
target_link_libraries(client Network)
