#pragma once

#include <string>
#include <vector>
#include <experimental/string_view>
#include "codec.h"

using StringViewVector = std::vector< std::experimental::string_view >;


class Tester {
public:
  void readfile(const std::string&);
  void readfile_uint(const std::string&);
  void read_decoded_file(const std::string&);
  void write_encoded_file(const std::string&);
  void write_decoded_file(const std::string&);
  void learn_codec(const size_t dict_size);
  void set_codec(Codecs::CodecIFace*);
  void check_correctness();
  void save_config();
  void save_info();
  void saved_memory();
  void test_encode();
  void test_decode();
  void reset();
  void load();
  void test_encode_decode();
protected:
  Codecs::CodecIFace* codec;
  std::vector<std::string> data;
  std::vector<std::string> decoded_data;
  std::vector<std::string> encoded_data;
};
