#include "Huffman.h"
#include "tester.h"
#include <iostream>

int main () {
  Tester tester;
  Codecs::HuffmanCodec huffman;
  tester.set_codec(huffman);
  tester.readfile("../Downloads/data1");
  std::cout << "gtugthugthu";
  tester.learn_codec();
  tester.test_encode();
  tester.test_decode();
  tester.check_correctness();
  return 0;
}
