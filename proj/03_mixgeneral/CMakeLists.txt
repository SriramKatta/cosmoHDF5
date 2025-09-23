# === Base settings ===
set(SRC main.cpp)
set(LIBS fmt::fmt mpicpp argparse::argparse HDF5::HDF5)

# --- Serial read + Serial write ---
add_executable(mix_sread_swrite ${SRC})
target_link_libraries(mix_sread_swrite PRIVATE ${LIBS})

# --- Serial read + Parallel write ---
add_executable(mix_sread_pwrite ${SRC})
target_compile_definitions(mix_sread_pwrite PRIVATE WRITE_PARALLEL)
target_link_libraries(mix_sread_pwrite PRIVATE ${LIBS})

# --- Parallel read + Serial write ---
add_executable(mix_pread_swrite ${SRC})
target_compile_definitions(mix_pread_swrite PRIVATE READ_PARALLEL)
target_link_libraries(mix_pread_swrite PRIVATE ${LIBS})

# --- Parallel read + Parallel write ---
add_executable(mix_pread_pwrite ${SRC})
target_compile_definitions(mix_pread_pwrite PRIVATE READ_PARALLEL WRITE_PARALLEL)
target_link_libraries(mix_pread_pwrite PRIVATE ${LIBS})
