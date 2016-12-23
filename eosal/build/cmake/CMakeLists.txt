cmake_minimum_required(VERSION 2.8.9)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ../../../lib/win32)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ../../../lib/win32)

project(eosal)
 
include(../../../build/cmake/<file|module> [OPTIONAL] [RESULT_VARIABLE <VAR>]
                      [NO_POLICY_SCOPE])
# Set include paths.
include_directories(../../..)

# Add operating system specific files.
if(WIN32)
  file(GLOB BB_OS_SOURCES "../../backbone/*/windows/*.c")
  file(GLOB BB_OS_HEADERS "../../backbone/*/windows/*.h")
  file(GLOB EX_OS_SOURCES "../../extensions/*/windows/*.c")
  file(GLOB EX_OS_HEADERS "../../extensions/*/windows/*.h")
else()
  file(GLOB BB_OS_SOURCES "../../*/*/linux/*.c")
  file(GLOB BB_OS_HEADERS "../../*/*/linux/*.h")
endif()

# Add files common to all operating systems.
file(GLOB BB_CM_SOURCES "../../backbone/*/common/*.c")
file(GLOB BB_CM_HEADERS "../../backbone/*/common/*.h")
file(GLOB EX_CM_SOURCES "../../extensions/*/common/*.c")
file(GLOB EX_CM_HEADERS "../../extensions/*/common/*.h")

# Add main headers.
set(MAIN_HEADERS "../../osal.h ../../osalx.h ")
 
# Generate the static library
add_library(eosal STATIC ${MAIN_HEADERS} ${BB_CM_HEADERS} ${BB_OS_HEADERS} ${EX_CM_HEADERS} ${EX_OS_HEADERS} ${BB_CM_SOURCES} ${BB_OS_SOURCES} ${EX_CM_SOURCES} ${EX_OS_SOURCES})
