function(add_srun_target EXE_NAME)
    set(one_value_args NP)
    cmake_parse_arguments(ARG "" "${one_value_args}" "" ${ARGN})

    if (NOT ARG_NP)
        set(ARG_NP 1)
    endif()

    add_executable(${EXE_NAME} main.cpp)
    target_link_libraries(${EXE_NAME} PRIVATE HDF5::HDF5)

    add_custom_target(run_${EXE_NAME}
        COMMAND ${CMAKE_COMMAND} -E echo '>>> Starting ${EXE_NAME} with srun...'
        COMMAND mpirun -n ${ARG_NP} $<TARGET_FILE:${EXE_NAME}>
        COMMAND ${CMAKE_COMMAND} -E echo '>>> Finished running ${EXE_NAME}'
        DEPENDS ${EXE_NAME}
        WORKING_DIRECTORY ${HDF_FILE_DIR}
        COMMENT "Running ${EXE_NAME} with srun"
    )
endfunction()
