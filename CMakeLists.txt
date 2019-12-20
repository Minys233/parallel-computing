cmake_minimum_required(VERSION 3.10)
project(homework)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# OpenMP: Go to CMakeLists.txt in this dir, change a compiler that supports '-fopenmp'
add_subdirectory("${PROJECT_SOURCE_DIR}/cg")
add_subdirectory("${PROJECT_SOURCE_DIR}/mandelbrot")

# OpenMPI: make sure you installed OpenMPI, mpic & mpic++ is ready to use
add_subdirectory("${PROJECT_SOURCE_DIR}/parallelio")
add_subdirectory("${PROJECT_SOURCE_DIR}/random_reduction")
