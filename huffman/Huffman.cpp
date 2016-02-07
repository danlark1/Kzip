#include "Huffman.h"

namespace Codecs {

  void HuffmanCodec::encode(string_view& encoded, const string_view& raw) const { 
    encoded = raw;
  }

  void HuffmanCodec::decode(string_view& raw, const string_view& encoded) const { 
    raw = encoded;
  }

  void HuffmanCodec::learn(const StringViewVector& all_samples) { 

  }

}  // namespace Codecs