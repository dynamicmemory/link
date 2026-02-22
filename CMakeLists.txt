cmake_minimum_required(VERSION 3.16)
project(NetworkFramework)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(Network
    src/network.cpp
    src/server.cpp
    src/client.cpp
)

target_include_directories(Network PUBLIC include)

add_executable(server
    src/server.cpp
)

add_executable(client
    src/client.cpp
)

target_link_libraries(server PRIVATE Network)
target_link_libraries(client PRIVATE Network)

