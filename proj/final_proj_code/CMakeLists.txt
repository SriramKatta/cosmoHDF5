set(EXE_NAME final)

add_executable(${EXE_NAME} main.cpp)
target_link_libraries(${EXE_NAME} PRIVATE fmt::fmt)
target_link_libraries(${EXE_NAME} PRIVATE mpicpp)
target_link_libraries(${EXE_NAME} PRIVATE argparse::argparse)
target_link_libraries(${EXE_NAME} PRIVATE HDF5::HDF5)


add_custom_target(run_${EXE_NAME}
    COMMAND ${CMAKE_COMMAND} -E echo '>>> Starting ${EXE_NAME} with srun...'
    COMMAND bash -c "srun -vp dc-cpu-devel --time=0:15:0  -A punch_astro -N \${NODES} -n \${TASKS} \"$<TARGET_FILE:${EXE_NAME}>\" \${INPUT_DIR}"
    COMMAND ${CMAKE_COMMAND} -E echo '>>> Finished running ${EXE_NAME}'
    DEPENDS ${EXE_NAME}
    WORKING_DIRECTORY ${HDF_FILE_DIR}
    COMMENT 'Running ${EXE_NAME} with srun'
)



