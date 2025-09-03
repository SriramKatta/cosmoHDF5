set(HDF5_PREFER_PARALLEL true)
find_package(HDF5 REQUIRED COMPONENTS CXX)

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