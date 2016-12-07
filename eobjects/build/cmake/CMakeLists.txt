cmake_minimum_required(VERSION 2.8.9)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ../../../lib/win32)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ../../../lib/win32)
project(eobjects)
 
# Set include paths
include_directories(../../..)

# Add source files, the file(GLOB...) allows for wildcards
file(GLOB_RECURSE SOURCES "../../*.cpp")

# Add header files
file(GLOB_RECURSE HEADERS "../../*.h")
 
# Generate the static library
add_library(eobjects STATIC ${HEADERS} ${SOURCES})
