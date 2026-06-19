cmake_minimum_required(VERSION 3.23)

project(ping LANGUAGES CXX)

option(enable_ping_debug_mode "enable debug mode" OFF)

add_executable(${PROJECT_NAME})

if (enable_ping_debug_mode)

    set(enable_net_debug_mode ON)

endif()

add_subdirectory(${PROJECT_SOURCE_DIR}/net)
add_subdirectory(${PROJECT_SOURCE_DIR}/options)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

target_link_libraries(${PROJECT_NAME} PRIVATE net PRIVATE options)

target_compile_options(${PROJECT_NAME}
    PRIVATE "$<$<CXX_COMPILER_ID:GNU>:--debug;-Wall;-Werror;-Wextra>")

target_sources(${PROJECT_NAME}
    PRIVATE ${PROJECT_SOURCE_DIR}/src/ping/detail/icmp_executor.cpp

    PRIVATE ${PROJECT_SOURCE_DIR}/src/ping/application.cpp
    PRIVATE ${PROJECT_SOURCE_DIR}/src/ping/ping.cpp
    
    PRIVATE
        FILE_SET  HEADERS
        BASE_DIRS ${PROJECT_SOURCE_DIR}/include
)
