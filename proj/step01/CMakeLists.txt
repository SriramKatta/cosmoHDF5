set(EXE_NAME mpirankdistribute)

add_executable(${EXE_NAME} main.cpp)
target_link_libraries(${EXE_NAME} PRIVATE fmt::fmt)
target_link_libraries(${EXE_NAME} PRIVATE mpicpp)
