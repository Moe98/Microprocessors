cmake_minimum_required(VERSION 3.10)
project(Microprocessors C)

set(CMAKE_C_STANDARD 11)
find_package(MPI)
include_directories(SYSTEM ${MPI_INCLUDE_PATH})

add_executable(Microprocessors main.c)
target_link_libraries(Microprocessors ${MPI_C_LIBRARIES})