cmake_minimum_required(VERSION 3.2)
project(tester)



set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -std=c++1z")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Wshadow -flto")

include_directories("tester/" "src/" "codecs/")

set(SOURCES
    main.cpp
    src/Dict.cpp
    src/Huffman.cpp
    src/Node.cpp
    src/Trie.cpp
    src/Suffix_tree.cpp
    tester/tester.cpp)

set(HEADERS
    codec/codec.h
    src/Huffman.h
    src/Node.h
    src/Trie.h
    src/Suffix_tree.h
    tester/tester.h
    src/Util.h)

add_executable(kzip ${SOURCES})
