#include "Huffman.h"
#include <iostream>
#include <unordered_map>

namespace Codecs {

  void HuffmanCodec::encode(string& encoded, const string_view& raw) const {
    encoded = raw.to_string();
  }
  void HuffmanCodec::decode(string& raw, const string_view& encoded) const {
    raw = encoded.to_string();
  }

  string HuffmanCodec::save() const {
    return string();
  }

  void HuffmanCodec::load(const string& config) {

  }

  size_t HuffmanCodec::sample_size(size_t records_total) const {
    return records_total;
  }

  void HuffmanCodec::learn(const StringViewVector& sample) {
    std::unordered_map<char, int> chars;
    size_t total_count = 0;
    for (auto& cur_string : sample) {
      total_count += cur_string.size();
      for (auto& one_char : cur_string) {
        auto iterator = chars.find(one_char);
        if (iterator != chars.end()) {
          ++(iterator->second);
        } else {
          chars.insert(std::make_pair(one_char, 1));
        }
      }
    }
    for (auto it = chars.begin(); it != chars.end(); ++it) {
      std::cout << it->first << " " << it->second << std::endl;
    }

  }

  void HuffmanCodec::reset() {

  }
}  // namespace Codecs