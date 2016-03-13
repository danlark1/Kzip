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
  tester.write_encoded_file("data1coded");
  tester.save_info();
  tester.save_config();
  tester.read_decoded_file("data1coded");
  tester.test_decode();
  tester.check_correctness();
  tester.write_decoded_file("data1");
  tester.saved_memory();
  huffman.reset();
  return 0;
}
