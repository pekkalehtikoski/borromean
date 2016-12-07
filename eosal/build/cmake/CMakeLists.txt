cmake_minimum_required(VERSION 2.8.9)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ../../../lib/win32)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ../../../lib/win32)

project(eosal)
 
# Set include paths.
include_directories(../../..)

# Add operating system specific files.
if(WIN32)
  file(GLOB OS_SOURCES "../../*/*/windows/*.c")
  file(GLOB OS_HEADERS "../../*/*/windows/*.h")
else()
  file(GLOB OS_SOURCES "../../*/*/linux/*.c")
  file(GLOB OS_HEADERS "../../*/*/linux/*.h")
endif()

# Add files common to all operating systems.
file(GLOB SOURCES "../../*/*/common/*.c")
file(GLOB HEADERS "../../*/*/common/*.h")

# Add main headers.
file(GLOB MAIN_HEADERS "../../*.h")
 
# Generate the static library
add_library(eosal STATIC ${MAIN_HEADERS} ${HEADERS} ${OS_HEADERS} ${SOURCES} ${OS_SOURCES})
