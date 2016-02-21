#pragma once

#include "codec.h"
#include "Node.h"
#include <unordered_map>
#include <list>
#include <climits>

namespace Codecs {

  const size_t CHAR_SIZE = (sizeof(char) * CHAR_BIT);

  class HuffmanCodec : public CodecIFace {
  public:  
    void encode(string& encoded, const string_view& raw) const override;
    void decode(string& raw, const string_view& encoded) const override;
    string save() const override;
    void load(const string_view&) override;
    size_t sample_size(size_t records_total) const override;
    void learn(const StringViewVector&) override;
    void reset() override;
    void decode_current_machine(string& raw, const string_view& encoded);
  private:
    Node* root_for_encode;
    Node* root_for_decode;
    std::unordered_map<unsigned char, uint32_t> chars;
    std::unordered_map<unsigned char, std::vector<bool> > table;
    void Build_table(Node* root, std::vector<bool>& code);
  };

} 