# eosal/build/cmake/CmakeLists.txt - Cmake build for eosal library.
cmake_minimum_required(VERSION 2.8.9)

# Set project name (= project root folder name).
set(E_PROJECT "eosal")
project(${E_PROJECT})

# include build information common to all projects.
include(../../../build/cmake/e-build-defs.txt)

# Set path to where to keep libraries.
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY $ENV{E_LIB})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY $ENV{E_LIB})

# Set path to source files.
set(BACKBONE "$ENV{E_ROOT}/${E_PROJECT}/backbone")
set(EXTENSIONS "$ENV{E_ROOT}/${E_PROJECT}/extensions")

# Set include paths.
include_directories($ENV{E_INCLUDE})

# Add main headers.
set(MAIN_HEADERS "$ENV{E_ROOT}/${E_PROJECT}/${E_PROJECT}.h $ENV{E_ROOT}/${E_PROJECT}/${E_PROJECT}x.h")

# Add source files common to all operating systems.
file(GLOB BB_CM_SOURCES "${BACKBONE}/*/common/*.c")
file(GLOB BB_CM_HEADERS "${BACKBONE}/*/common/*.h")
file(GLOB EX_CM_SOURCES "${EXTENSIONS}/*/common/*.c")
file(GLOB EX_CM_HEADERS "${EXTENSIONS}/*/common/*.h")

# Add operating system specific files.
file(GLOB BB_OS_SOURCES "${BACKBONE}/*/windows/*.c")
file(GLOB BB_OS_HEADERS "${BACKBONE}/*/windows/*.h")
file(GLOB EX_OS_SOURCES "${EXTENSIONS}/*/windows/*.c")
file(GLOB EX_OS_HEADERS "${EXTENSIONS}/*/windows/*.h")

# Generate the static library
add_library(eosal STATIC ${MAIN_HEADERS} ${BB_CM_HEADERS} ${BB_OS_HEADERS} ${EX_CM_HEADERS} ${EX_OS_HEADERS} ${BB_CM_SOURCES} ${BB_OS_SOURCES} ${EX_CM_SOURCES} ${EX_OS_SOURCES})

# message(${BACKBONE})
