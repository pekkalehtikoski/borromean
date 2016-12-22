cmake_minimum_required(VERSION 2.8.9)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ../../../lib/win32)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ../../../lib/win32)
project(containerexample)
 
# Set include paths
include_directories(../../../../..)

# Add source files, the file(GLOB...) allows for wildcards
file(GLOB_RECURSE SOURCES "../code/*.cpp")

# Add header files
file(GLOB_RECURSE HEADERS "../code*.h")
 
# target_link_libraries (helloDemo LINK_PUBLIC Hello)

add_executable(containerexample ../../code/eobjects_container_example.cpp)
target_link_libraries(containerexample ../../../../../lib/win32/Debug/eobjects ../../../../../lib/win32/Debug/eosal)

# Generate the static library

# add_library(eobjects STATIC ${HEADERS} ${SOURCES})
