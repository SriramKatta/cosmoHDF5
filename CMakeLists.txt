cmake_minimum_required(VERSION 3.28.3 FATAL_ERROR)
project(HDF5cpp)

set(CMAKE_BUILD_TYPE Release)

set(HDF_FILE_DIR ${CMAKE_SOURCE_DIR}/HDF5_FILES)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

file(MAKE_DIRECTORY ${HDF_FILE_DIR})

SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(MPI REQUIRED COMPONENTS CXX)


include(cmake/CPM.cmake)
include(cmake/fmt.cmake)
include(cmake/mpicpp.cmake)
include(cmake/HDF5.cmake)
include(cmake/argparse.cmake)

include_directories(${CMAKE_SOURCE_DIR}/header)

add_subdirectory(proj)
