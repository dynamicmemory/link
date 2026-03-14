cmake_minimum_required(VERSION 3.16)
project(NetworkFramework)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(OpenSSL REQUIRED)

add_library(Network SHARED
    src/tcpsocket.cpp
    src/tcptransport.cpp
    src/tlstransport.cpp

    src/selectmultiplexer.cpp

    src/defaultprotocol.cpp

    src/network.cpp
    src/server.cpp
    src/client.cpp

    c_api/network_c_api.cpp
)

target_include_directories(Network PUBLIC include c_api)
target_link_libraries(Network PUBLIC OpenSSL::SSL OpenSSL::Crypto)

add_executable(server_main src/server_main.cpp)
add_executable(client_main src/client_main.cpp)

target_link_libraries(server_main PRIVATE Network)
target_link_libraries(client_main PRIVATE Network)

