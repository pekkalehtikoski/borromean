# epython/epylib/build/cmake/CmakeLists.txt - Cmake build for epy library.
cmake_minimum_required(VERSION 2.8.11)

# Set project name (= project root folder name).
set(E_PROJECT "epyo")
project(${E_PROJECT})

# include build information common to all projects.
include(../../../../build/cmake/e-build-defs.txt)
include(e-build-epyo-defs.txt)

# Set path to where to keep libraries.
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY $ENV{E_LIB})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY $ENV{E_LIB})

# Set paths to source files.
set(E_SOURCE_PATH "$ENV{E_ROOT}/epython/${E_PROJECT}/code")

# Set include paths.
include_directories($ENV{E_INCLUDE})

# Add library main header.
set(MAIN_HEADER "$ENV{E_ROOT}/epython/${E_PROJECT}/${E_PROJECT}.h")

# Add header files, the file(GLOB_RECURSE...) allows for wildcards and recurses subdirs.
file(GLOB_RECURSE HEADERS "${E_SOURCE_PATH}/*.h")

# Add source files.
file(GLOB_RECURSE SOURCES "${E_SOURCE_PATH}/*.cpp")

# Generate the static library
add_library(${E_PROJECT}s${E_POSTFIX} STATIC ${MAIN_HEADER} ${HEADERS} ${SOURCES})

# Generate the static library
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY $ENV{E_BIN})
add_library(${E_PROJECT}${E_POSTFIX} SHARED ${MAIN_HEADER} ${HEADERS} ${SOURCES})

# Build executable. Set library folder and libraries to link with
# link_directories($ENV{E_LIB})
# add_executable(${E_PROJECT}${E_POSTFIX} ${HEADERS} ${SOURCES})
# target_link_libraries(${E_PROJECT}${E_POSTFIX} $ENV{E_COMMON_CONSOLE_APP_LIBS})
