# eosal/build/cmake/CmakeLists.txt - Cmake build for eobjects library.
cmake_minimum_required(VERSION 2.8.9)

# Set project name (= project root folder name).
set(E_PROJECT "eobjects")
project(${E_PROJECT})

# Set path to where to keep libraries.
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY $ENV{E_LIB})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY $ENV{E_LIB})

# Set path to source files.
set(E_SOURCE_PATH "$ENV{E_ROOT}/${E_PROJECT}/code")

# Set include paths.
include_directories($ENV{E_INCLUDE})

# Add library main header.
set(MAIN_HEADER "$ENV{E_ROOT}/${E_PROJECT}/${E_PROJECT}.h")

# Add header files, the file(GLOB_RECURSE...) allows for wildcards and recurses subdirs.
file(GLOB_RECURSE HEADERS "${E_SOURCE_PATH}/*.h")

# Add source files.
file(GLOB_RECURSE SOURCES "${E_SOURCE_PATH}/*.cpp")

# Generate the static library
add_library(eobjects STATIC ${MAIN_HEADER} ${HEADERS} ${SOURCES})
