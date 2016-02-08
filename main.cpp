#include "Huffman.h"
#include "tester.h"
#include <iostream>
#include <fstream>

int main () {
  Tester tester;
  Codecs::HuffmanCodec huffman;
  tester.set_codec(huffman);
  tester.readfile("../../Downloads/data3");
  tester.learn_codec();
  tester.test_encode();
  tester.test_decode();
  tester.check_correctness();
  return 0;
}
