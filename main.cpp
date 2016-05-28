#include "Huffman_better.h"
#include "tester.h"
#include <algorithm>
#include <string>
#include <cstdio>
#include <iostream>

int main (int argc, char* argv[]) {
  Tester tester;
  Codecs::HuffmanCodec huffman;
  tester.set_codec(&huffman);
  //path to file
  // std::cout << argv[1] << std::endl;
  if (argc == 1) {
    std::cout << "No arguments :(, type --help for help\n";
    return 0;
  }
  if (argc == 2 && static_cast<std::string>(argv[1]) == "--help") {
    std::cout << "KUTzip is here\n";
    std::cout << "To run program use ./tester [DICT SIZE] [PARSING ARGUMENT] [PATH TO FILE]\n";
    std::cout << "OPTIONS:\n";
    std::cout << "DICT SIZE: [integer]. Dictionary size (12400 default) \n";
    std::cout << "PARSING ARGUMENT: [\"true\", \"false\"]. Parsing with LE int(true) or \\n(false), (false default)\n";
    std::cout << "Use both or 0 arguments for correct running\n";
    return 0;
  }
  if (argc >= 3 && static_cast<std::string>(argv[argc - 2]) == "true") {
    tester.readfile_uint(argv[argc - 1]);
    tester.learn_codec(std::atoi(argv[argc - 3]));
  } else {
    tester.readfile(argv[argc - 1]);
    tester.learn_codec(0);
  }
  tester.test_encode_decode();
  huffman.reset();

  // tester.test_encode();
  // // tester.write_encoded_file("data1coded");
  // // tester.save_info();
  // tester.save_config();
  // // tester.read_decoded_file("data1coded");
  // huffman.reset();
  // tester.test_decode();
  // // huffman.reset();
  
  // tester.check_correctness();
  // // tester.write_decoded_file("data1");
  // tester.saved_memory();
  // huffman.reset();
  tester.reset();
  return 0;
}
