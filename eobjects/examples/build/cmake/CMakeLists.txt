cmake_minimum_required(VERSION 2.8.9)
project(eobjects-examples)
 
# Build individual projects
add_subdirectory(../../container/build/cmake "${CMAKE_CURRENT_BINARY_DIR}/container_example")
