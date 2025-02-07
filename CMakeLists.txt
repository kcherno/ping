cmake_minimum_required(VERSION 3.27.0)

project(ping LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD          20)

find_package(Boost REQUIRED COMPONENTS program_options)

add_executable(ping
    ${CMAKE_SOURCE_DIR}/src/configuration/options.cpp
    ${CMAKE_SOURCE_DIR}/proto/src/net/icmp_echo.cpp
    ${CMAKE_SOURCE_DIR}/proto/src/net/icmp.cpp
    ${CMAKE_SOURCE_DIR}/src/core/executor.cpp
    ${CMAKE_SOURCE_DIR}/src/ping.cpp)

target_include_directories(ping
    PRIVATE ${CMAKE_SOURCE_DIR}/proto/include
    PRIVATE ${CMAKE_SOURCE_DIR}/include)

target_link_libraries(ping PRIVATE Boost::program_options)

set_target_properties(ping
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
