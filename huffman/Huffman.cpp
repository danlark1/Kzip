#include "Huffman.h"

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
    return records_total / 12;
  }

  void HuffmanCodec::learn(const StringViewVector& sample) {

  }

  void HuffmanCodec::reset() {

  }
}  // namespace Codecs