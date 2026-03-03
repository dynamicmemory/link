cmake_minimum_required(VERSION 3.16)
project(NetworkFramework)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(Network
    src/tcpsocket.cpp
    src/tcptransport.cpp

    src/defaultprotocol.cpp

    src/network.cpp
    src/server.cpp
    src/client.cpp
)

target_include_directories(Network PUBLIC include)

add_executable(server_main
    src/server_main.cpp
)

add_executable(client_main
    src/client_main.cpp
)

target_link_libraries(server_main PRIVATE Network)
target_link_libraries(client_main PRIVATE Network)

