cmake_minimum_required(VERSION 3.2)
project(tester)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Ofast")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

include_directories("tester/" "huffman/" "codecs/")

set(SOURCES
    main.cpp
    huffman/Huffman.cpp
    tester/tester.cpp)

set(HEADERS
    codec/codec.h
    huffman/Huffman.h
    tester/tester.h)

add_executable(tester ${SOURCES})