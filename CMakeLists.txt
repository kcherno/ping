cmake_minimum_required(VERSION 3.23)

project(ping LANGUAGES CXX)

add_executable(${PROJECT_NAME})

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/net)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

target_sources(${PROJECT_NAME}
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/ping/ping.cpp
    
    PRIVATE
        FILE_SET  HEADERS
        BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include)
