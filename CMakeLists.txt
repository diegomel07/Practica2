cmake_minimum_required(VERSION 3.5)

project(pipe-example)

set(CMAKE_CXX_STANDARD 11)

# Compile the helper functions into a static library
add_library(helpers STATIC helpers.cpp )
target_include_directories(helpers PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Compile the menu program
add_executable(server p2-server.cpp)
target_link_libraries(server PRIVATE helpers)
target_include_directories(server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Compile the reader program
add_executable(client p2-client.cpp)
target_link_libraries(client PRIVATE helpers)
target_include_directories(client PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
