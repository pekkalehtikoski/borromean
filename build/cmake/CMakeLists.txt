cmake_minimum_required(VERSION 2.8.9)
project(borromean)
 
# Build individual projects
# add_subdirectory(../../eosal/build/cmake "${CMAKE_CURRENT_BINARY_DIR}/eosal_build")
# add_subdirectory(../../eobjects/build/cmake "${CMAKE_CURRENT_BINARY_DIR}/eobjects_build")
add_subdirectory(../../eosal/examples/build/cmake "${CMAKE_CURRENT_BINARY_DIR}/eosal_examples")
add_subdirectory(../../eobjects/examples/build/cmake "${CMAKE_CURRENT_BINARY_DIR}/eobjects_examples")
