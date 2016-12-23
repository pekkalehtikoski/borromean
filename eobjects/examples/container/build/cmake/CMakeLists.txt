# eobjects/examples/container/build/cmake/CmakeLists.txt - Cmake build for eobject container example.
cmake_minimum_required(VERSION 2.8.9)

# Set project name (= project root folder name).
set(E_PROJECT "container")
# project(${E_PROJECT})
project(container)

# Set path to where to keep libraries.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $ENV{E_BIN})

# Set path to source files.
set(E_SOURCE_PATH "$ENV{E_ROOT}/${E_PROJECT}/code")

# Set include paths.
include_directories($ENV{E_INCLUDE})

# Add header files, the file(GLOB_RECURSE...) allows for wildcards and recurses subdirs.
file(GLOB_RECURSE HEADERS "${E_SOURCE_PATH}/*.h")

# Add source files.
file(GLOB_RECURSE SOURCES "${E_SOURCE_PATH}/*.cpp")
 
# Build executable
add_executable(container ${SOURCES})

# Set link libraries to use.
target_link_libraries(container $ENV{E_LIB}/Debug/eobjects $ENV{E_LIB}/Debug/eosal)
