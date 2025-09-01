cmake_minimum_required(VERSION 3.28.3 FATAL_ERROR)
project(HDF5cpp)


set(HDF_FILE_DIR ${CMAKE_SOURCE_DIR}/HDF5_FILES)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

file(MAKE_DIRECTORY ${HDF_FILE_DIR})

SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)


set(HDF5_PREFER_PARALLEL true)
find_package(HDF5 REQUIRED COMPONENTS CXX)
find_package(MPI REQUIRED COMPONENTS CXX)

if (HDF5_FOUND)
    message(STATUS "HDF5 found")
else()
    message(FATAL_ERROR "HDF5 not found")
endif()

if (HDF5_IS_PARALLEL)
    message(STATUS "HDF5 is built with parallel support")
else()
    message(FATAL_ERROR "HDF5 is built without parallel support")
endif()

include(cmake/CPM.cmake)
include(cmake/fmt.cmake)
include(cmake/mpicpp.cmake)
include(cmake/argparse.cmake)

add_subdirectory(proj)
