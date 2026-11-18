cmake_minimum_required(VERSION 4.0)
project(kari_final_project)

set(CMAKE_CXX_STANDARD 14)

include_directories(final_project)

add_executable(kari_final_project
        final_project/CReedSolomon.cpp
        final_project/CReedSolomon.h
        final_project/DES.cpp
        final_project/DES.h
        final_project/main.cpp)

target_compile_options(kari_final_project PRIVATE -Wno-c++11-narrowing)