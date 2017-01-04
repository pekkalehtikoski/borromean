# eobjects/examples/build/cmake/CmakeLists.txt - Cmake build for all eobjects examples.
cmake_minimum_required(VERSION 2.8.9)
project(eobjects-examples)
 
# Build individual projects.
add_subdirectory($ENV{E_ROOT}/eobjects/examples/econtainer/build/cmake "${CMAKE_CURRENT_BINARY_DIR}/econtainer_example")
add_subdirectory($ENV{E_ROOT}/eobjects/examples/evariables/build/cmake "${CMAKE_CURRENT_BINARY_DIR}/evariables_example")
add_subdirectory($ENV{E_ROOT}/eobjects/examples/enames/build/cmake "${CMAKE_CURRENT_BINARY_DIR}/enames_example")
add_subdirectory($ENV{E_ROOT}/eobjects/examples/ethread/build/cmake "${CMAKE_CURRENT_BINARY_DIR}/ethread_example")
add_subdirectory($ENV{E_ROOT}/eobjects/examples/eproperty/build/cmake "${CMAKE_CURRENT_BINARY_DIR}/eproperty_example")
