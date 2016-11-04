cmake_minimum_required(VERSION 3.6)
project(Matrix_Multiplication)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    matrix-seq.c
    matrix.c)

add_executable(Matrix_Multiplication ${SOURCE_FILES})