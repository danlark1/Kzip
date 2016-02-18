#include "Huffman.h"
#include "tester.h"
#include <iostream>
#include <fstream>

int main () {
  Tester tester;
  Codecs::HuffmanCodec huffman;
  tester.set_codec(huffman);
  tester.readfile("../../Downloads/data1");
  tester.learn_codec();
  tester.test_encode();
  tester.test_decode();
  tester.check_correctness();
  tester.saved_memory();
  tester.writefile("data1coded");
  tester.readfile("data1coded");
  tester.save_config();
  huffman.reset();
  return 0;
}
