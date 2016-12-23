# eobjects/examples/build/cmake/CmakeLists.txt - Cmake build for all eobjects examples.
cmake_minimum_required(VERSION 2.8.9)
project(eobjects-examples)
 
# Build individual projects.
add_subdirectory($ENV{E_ROOT}/eobjects/examples/container/build/cmake "${CMAKE_CURRENT_BINARY_DIR}/container_example")
