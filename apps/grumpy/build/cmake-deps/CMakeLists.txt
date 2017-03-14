# grumpy/build/cmake-deps/CmakeLists.txt - cmake build for grumpy + dependencies.
cmake_minimum_required(VERSION 2.8.11)
project(grumpy-deps)

# include build information common to all projects.
include(../../../../build/cmake/e-build-defs.txt)
 
# Build individual projects.
add_subdirectory($ENV{E_ROOT}/eosal/build/cmake "${CMAKE_CURRENT_BINARY_DIR}/eosal")
add_subdirectory($ENV{E_ROOT}/eobjects/build/cmake "${CMAKE_CURRENT_BINARY_DIR}/eobjects")
add_subdirectory($ENV{E_ROOT}/apps/grumpy/build/cmake "${CMAKE_CURRENT_BINARY_DIR}/grumpy")
