#include "Huffman_better.h"
#include "tester.h"
#include <iostream>
#include <fstream>

int main () {
  Tester tester;
  Codecs::HuffmanCodec huffman;
  tester.set_codec(huffman);

  //path to file
  tester.readfile("../../DATAX/data5");
  tester.learn_codec();
  tester.test_encode();
  //tester.write_encoded_file("data1coded");
  // tester.save_info();
  tester.save_config();
  //tester.read_decoded_file("data1coded");
  tester.test_decode();
  huffman.reset();
  tester.check_correctness();
  // tester.write_decoded_file("data1");
  tester.saved_memory();
  tester.reset();
  return 0;
}
