#pragma once

// SELF-MADE
#include "../codecs/codec.h"
#include "Node_better.h"
#include "Trie.h"
#include "Suffix_tree.h"

// NEEDED FOR THIS FILE
#include <math.h>
#include <climits>
#include <string>
#include <vector>

namespace Codecs {

  const size_t CHAR_SIZE = (sizeof(char) * CHAR_BIT);
  const size_t LOG_CHAR_SIZE = ceil(log(CHAR_SIZE));

  class HuffmanCodec : public CodecIFace {
  public:
    void encode(string& encoded, const string_view& raw) const override;
    void decode(string& raw, const string_view& encoded) const override;
    string save() const override;
    void load(const string_view&) override;
    size_t sample_size(size_t records_total) const override;
    void learn(StringViewVector&, const size_t dict_size) override;
    void reset() override;
    ~HuffmanCodec();
    HuffmanCodec();
  private:
    Trie trie;
    Node* root_for_decode;
    std::vector<std::pair<std::string, int64_t> > ans;
    void Build_table(Node* root, std::vector<int32_t>& code);
    void build_jumps(Node* a);
  };

}
