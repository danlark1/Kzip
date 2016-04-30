cmake_minimum_required(VERSION 3.2)
project(tester)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Ofast")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -ggdb -flto")

include_directories("tester/" "huffman_better/" "codecs/")

set(SOURCES
    main.cpp
    huffman_better/Huffman_better.cpp
    huffman_better/Node_better.cpp
    huffman_better/Trie.cpp
    huffman_better/Suffix_tree.cpp
    tester/tester.cpp)

set(HEADERS
    codec/codec.h
    huffman_better/Huffman_better.h
    huffman_better/Node_better.h
    huffman_better/Trie.h
    huffman_better/Suffix_tree.h
    tester/tester.h)

add_executable(tester ${SOURCES})
