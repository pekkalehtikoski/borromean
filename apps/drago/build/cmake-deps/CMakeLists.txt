# drago/build/cmake-deps/CmakeLists.txt - cmake build for drago and dependencies.
cmake_minimum_required(VERSION 2.8.11)
project(drago-deps)

# include build information common to all projects.
include(../../../../eosal/build/cmake/eosal-defs.txt)

# Build individual projects.
add_subdirectory($ENV{E_ROOT}/eosal/build/cmake "${CMAKE_CURRENT_BINARY_DIR}/eosal")
add_subdirectory($ENV{E_ROOT}/eobjects/build/cmake "${CMAKE_CURRENT_BINARY_DIR}/eobjects")
add_subdirectory($ENV{E_ROOT}/egui/build/cmake "${CMAKE_CURRENT_BINARY_DIR}/egui")
add_subdirectory($ENV{E_ROOT}/apps/drago/build/cmake "${CMAKE_CURRENT_BINARY_DIR}/drago")
