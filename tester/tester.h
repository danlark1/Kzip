#pragma once

#include <string>
#include <vector>
#include <experimental/string_view>
#include "codec.h"

using StringViewVector = std::vector< std::experimental::string_view >;


class Tester {
public:
  void readfile(const std::string&);
  void learn_codec();
  void set_codec(Codecs::CodecIFace);
  void check_correctness();
  void test_encode();
  void test_decode();
protected:
  Codecs::CodecIFace* codec;
  StringViewVector data;
  StringViewVector decoded_data;
  StringViewVector encoded_data;
};
