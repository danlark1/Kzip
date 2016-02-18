#pragma once

#include <string>
#include <vector>
#include <ctime>
#include <experimental/string_view>
#include "codec.h"

using StringViewVector = std::vector< std::experimental::string_view >;


class Tester {
public:
  void readfile(const std::string&);
  void writefile(const std::string&);
  void learn_codec();
  void set_codec(Codecs::CodecIFace&);
  void check_correctness();
  void save_config();
  void saved_memory();
  void test_encode();
  void test_decode();
protected:
  Codecs::CodecIFace* codec;
  std::vector<std::string> data;
  std::vector<std::string> decoded_data;
  std::vector<std::string> encoded_data;
};
