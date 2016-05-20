cmake_minimum_required(VERSION 3.2)
project(tester)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast -std=c++14")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -ggdb -flto")

include_directories("tester/" "src/" "codecs/")

set(SOURCES
    main.cpp
    src/Huffman_better.cpp
    src/Node_better.cpp
    src/Trie.cpp
    src/Suffix_tree.cpp
    tester/tester.cpp)

set(HEADERS
    codec/codec.h
    src/Huffman_better.h
    src/Node_better.h
    src/Trie.h
    src/Suffix_tree.h
    tester/tester.h)

add_executable(tester ${SOURCES})
