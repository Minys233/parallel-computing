set(CMAKE_CXX_STANDARD 11)

### modify here! your compiler should be have -fopenmp!
set(CMAKE_CXX_COMPILER g++)
set(CMAKE_C_COMPILER gcc)
###

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
add_executable(cg cg.cpp)
add_executable(cg-multi cg-multi.cpp)
