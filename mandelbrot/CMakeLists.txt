set(CMAKE_CXX_STANDARD 11)
### modify here! your compiler should be have -fopenmp!
set(CMAKE_CXX_COMPILER g++)
set(CMAKE_C_COMPILER gcc)
###
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
include_directories( ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} )
add_executable(mandelbrot mandelbrot.cpp)
add_executable(mandelbrot-multi mandelbrot-multi.cpp )

target_link_libraries(mandelbrot ${OPENGL_LIBRARY} ${GLUT_LIBRARY} )
target_link_libraries(mandelbrot-multi ${OPENGL_LIBRARY} ${GLUT_LIBRARY} )

