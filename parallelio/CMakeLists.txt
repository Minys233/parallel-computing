set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_COMPILER mpic++)
set(CMAKE_C_COMPILER mpicc)

find_package(MPI REQUIRED)
include_directories(${MPI_INNCLUDE_PATH})
add_executable(parallelio parallelio.cpp)

target_link_libraries(parallelio ${MPI_LIBRARIES})