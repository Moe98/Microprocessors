cmake_minimum_required(VERSION 3.3)
project(Mil3 C)

set(CMAKE_C_STANDARD 11)
find_package(MPI)
include_directories(SYSTEM ${MPI_INCLUDE_PATH})

add_executable(Mil3 main.c)
target_link_libraries(Mil3 ${MPI_C_LIBRARIES})