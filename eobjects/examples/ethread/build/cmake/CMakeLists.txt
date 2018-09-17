# eobjects/examples/ethread/build/cmake/CmakeLists.txt - Cmake build for eobject thread example.
cmake_minimum_required(VERSION 2.8.11)

# Set project name (= project root folder name).
set(E_PROJECT "ethread")
project(${E_PROJECT})

# include build information common to all projects.
include(../../../../../eosal/build/cmake/eosal-defs.txt)
include(../../../../build/cmake/eobjects-defs.txt)

# Set path to where to keep libraries.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $ENV{E_BIN})

# Set path to source files.
set(E_SOURCE_PATH "$ENV{E_ROOT}/eobjects/examples/${E_PROJECT}/code")

# Set include paths.
# include_directories($ENV{E_INCLUDE})

# Add header files, the file(GLOB_RECURSE...) allows for wildcards and recurses subdirs.
file(GLOB_RECURSE HEADERS "${E_SOURCE_PATH}/*.h")

# Add source files.
file(GLOB_RECURSE SOURCES "${E_SOURCE_PATH}/*.cpp")

# Build executable. Set library folder and libraries to link with
link_directories($ENV{E_LIB})
add_executable(${E_PROJECT}${E_POSTFIX} ${HEADERS} ${SOURCES})
target_link_libraries(${E_PROJECT}${E_POSTFIX} $ENV{E_COMMON_CONSOLE_APP_LIBS})
