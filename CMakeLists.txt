cmake_minimum_required(VERSION 3.16)
project(NetworkFramework)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(OpenSSL REQUIRED)
find_package(pybind11 CONFIG REQUIRED)

# Lib
add_library(Network SHARED
    src/tcpsocket.cpp
    src/tcptransport.cpp
    src/tlstransport.cpp

    src/selectmultiplexer.cpp

    src/defaultprotocol.cpp

    src/network.cpp
    src/server.cpp
    src/client.cpp

    c_api/link_c_api.cpp
)

target_include_directories(Network PUBLIC include c_api)
target_link_libraries(Network PUBLIC OpenSSL::SSL OpenSSL::Crypto)

# Executables 
add_executable(server_main examples/cpp/server_main.cpp)
add_executable(client_main examples/cpp/client_main.cpp)

pybind11_add_module(link bindings/py_network.cpp)

target_link_libraries(server_main PRIVATE Network)
target_link_libraries(client_main PRIVATE Network)
target_link_libraries(link PRIVATE Network)


# Testing

# Gather all test sources
file(GLOB_RECURSE TEST_SOURCES tests/*.cpp)

# Create test executable
add_executable(network_tests ${TEST_SOURCES})
target_link_libraries(network_tests PRIVATE Network)

# Make tests run automatically after build
add_custom_target(run_tests ALL
    DEPENDS network_tests
    COMMAND network_tests
    COMMENT "Running Catch2 tests..."
)

# Fail build if tests fail
set_property(TARGET run_tests PROPERTY FOLDER "Tests")
