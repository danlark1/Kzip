// Copyright 2017, Danila Kutenin
#pragma once

#include "../codecs/codec.h"
#include "Node.h"
#include "Suffix_tree.h"
#include "Trie.h"

#include <climits>
#include <cmath>
#include <string>
#include <vector>

namespace Codecs {
  constexpr size_t CHAR_SIZE = (sizeof(char) * CHAR_BIT);
  // ceil and log are not constexpr
  const size_t LOG_CHAR_SIZE = std::ceil(std::log2(CHAR_SIZE));
  constexpr size_t MAX_CONCAT_SIZE = 1e6;

  class HuffmanCodec : public CodecIFace {
  public:
    void Encode(string& encoded, const string_view raw) const override;
    void Decode(string& raw, const string_view encoded) const override;
    void Save(const string_view) const override;
    void Load(const string_view) override;
    size_t SampleSize(size_t records_total) const override;
    void Learn(StringViewVector&, const size_t dict_size) override;
    void Reset() override;
  private:
    size_t concat_size;
    Trie trie;
    Node* root_for_decode;
    void Shrinking(std::vector<std::pair<std::string, int64_t> >& ans_copy, const size_t concat_size);
    std::vector<std::pair<std::string, int64_t> > ans;
    void BuildTable(Node* root, std::vector<int8_t>& code);
    void BuildJumps(Node* a);
  };
}
