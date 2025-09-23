set(SRC main.cpp) 
set(PREF bm)
set(LIBS fmt::fmt mpicpp argparse::argparse HDF5::HDF5)


add_executable(${PREF}_sread_swrite ${SRC})
target_link_libraries(${PREF}_sread_swrite ${LIBS})


add_executable(${PREF}_sread_pwrite ${SRC})
target_link_libraries(${PREF}_sread_pwrite ${LIBS})
target_compile_definitions(${PREF}_sread_pwrite PUBLIC WRITE_PARALLEL)


add_executable(${PREF}_pread_swrite ${SRC})
target_link_libraries(${PREF}_pread_swrite ${LIBS})
target_compile_definitions(${PREF}_pread_swrite PUBLIC READ_PARALLEL)

add_executable(${PREF}_pread_pwrite ${SRC})
target_link_libraries(${PREF}_pread_pwrite ${LIBS})
target_compile_definitions(${PREF}_pread_pwrite PUBLIC READ_PARALLEL WRITE_PARALLEL)


