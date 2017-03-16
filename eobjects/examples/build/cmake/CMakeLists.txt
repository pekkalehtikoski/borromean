# eobjects/examples/build/cmake/CmakeLists.txt - Cmake build for all eobjects examples.
cmake_minimum_required(VERSION 2.8.11)
project(eobjects-examples)

# include build information common to all projects.
include(../../../../eosal/build/cmake/eosal-defs.txt)
 
# Build individual projects.
add_subdirectory($ENV{E_ROOT}/eosal/build/cmake "${CMAKE_CURRENT_BINARY_DIR}/eosal")
add_subdirectory($ENV{E_ROOT}/eobjects/build/cmake "${CMAKE_CURRENT_BINARY_DIR}/eobjects")
add_subdirectory($ENV{E_ROOT}/eobjects/examples/econtainer/build/cmake "${CMAKE_CURRENT_BINARY_DIR}/econtainer_example")
add_subdirectory($ENV{E_ROOT}/eobjects/examples/evariables/build/cmake "${CMAKE_CURRENT_BINARY_DIR}/evariables_example")
add_subdirectory($ENV{E_ROOT}/eobjects/examples/enames/build/cmake "${CMAKE_CURRENT_BINARY_DIR}/enames_example")
add_subdirectory($ENV{E_ROOT}/eobjects/examples/ethread/build/cmake "${CMAKE_CURRENT_BINARY_DIR}/ethread_example")
add_subdirectory($ENV{E_ROOT}/eobjects/examples/eproperty/build/cmake "${CMAKE_CURRENT_BINARY_DIR}/eproperty_example")
add_subdirectory($ENV{E_ROOT}/eobjects/examples/econnection/build/cmake "${CMAKE_CURRENT_BINARY_DIR}/econnection_example")
add_subdirectory($ENV{E_ROOT}/eobjects/examples/eendpoint/build/cmake "${CMAKE_CURRENT_BINARY_DIR}/eendpoint_example")
