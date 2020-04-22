cmake_minimum_required(VERSION 3.16)
project(huff_codec)

set(CMAKE_CXX_STANDARD 17)

add_executable(huff_codec src/main.cpp src/DataFile.cpp src/DataFile.h src/Tree.cpp src/Tree.h)
