// Copyright 2017-2018, Danila Kutenin
#pragma once

#include "../codecs/codec.h"
#include "Node.h"
#include "Suffix_tree.h"
#include "Trie.h"

#include <climits>
#include <cmath>
#include <string>
#include <vector>

namespace NCodecs {
    // ceil and log are not constexpr
    const size_t LOG_CHAR_SIZE = std::ceil(std::log2(CHAR_SIZE));

    class HuffmanCodec : public ICodecFace {
    public:
        void Encode(std::string&, const std::string_view) const override;
        void Decode(std::string&, const std::string_view) const override;
        void Save(const std::string_view) const override;
        void Load(const std::string_view) override;
        size_t SampleSize(size_t) const override;
        void Learn(std::vector<std::string_view>&, const size_t) override;
        void Reset() override;

    private:
        size_t concat_size;
        Trie trie;
        Node* root_for_decode;
        std::vector<std::pair<std::string, int64_t>> ans;
        void Shrinking(std::vector<std::pair<std::string, int64_t>>&, const size_t);
        void BuildTable(Node*, std::vector<int8_t>&);
        void BuildJumps(Node*);
        void BuildTree();
        void FinalStats(const std::unordered_map<std::string, std::pair<int64_t, int64_t>>&);
        void RunOnSample(const std::string&, const Trie&, std::unordered_map<std::string, std::pair<int64_t, int64_t>>&,
                         const unsigned char);
        void FindStats(const std::string&, Trie&, std::unordered_map<std::string, std::pair<int64_t, int64_t>>&,
                       const unsigned char, const size_t);
        std::string MakeOneString(std::vector<std::string_view>&, const unsigned char);
        unsigned char MinCharSample(const std::vector<std::string_view>&);
    };
} // namespace NCodecs
