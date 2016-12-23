cmake_minimum_required(VERSION 2.8.9)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ../../../lib/win32)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ../../../lib/win32)
project(eobjects)
 
# Set include paths
include_directories(../../..)

# Add source files, the file(GLOB...) allows for wildcards
file(GLOB_RECURSE SOURCES "../../code/*.cpp")

# Add header files
file(GLOB_RECURSE HEADERS "../../code/*.h")

# Add library main header.
set(MAIN_HEADER "../../eobjects.h")

 
# Generate the static library
add_library(eobjects STATIC ${MAIN_HEADER} ${HEADERS} ${SOURCES})
