// Copyright 2017-2018, Danila Kutenin

#include "Huffman.h"
#include "Node.h"
#include "Suffix_tree.h"
#include "Trie.h"
#include "Util.h"

#include <algorithm>
#include <cinttypes>
#include <fstream>
#include <iostream>
#include <map>
#include <queue>
#include <random>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

namespace NCodecs {

    void HuffmanCodec::Learn(std::vector<std::string_view>& sample, const size_t dict_size) {
        std::unordered_map<std::string, std::pair<int64_t, int64_t>> to_check;
        ans.clear();
        trie.nodes.clear();
        trie = Trie();
        Trie trie_ch = Trie();
        for (uint32_t c = 0; c < CHAR_SIZE_POWER; ++c) {
            std::string s(1, c);
            ans.push_back({s, 0});
            trie_ch.Insert(s, {});
            to_check[s] = {0, 0};
        }
        unsigned char min_char = MinCharSample(sample);
        std::string concat = MakeOneString(sample, min_char);
        FindStats(concat, trie_ch, to_check, min_char, dict_size);
        RunOnSample(concat, trie_ch, to_check, min_char);
        FinalStats(to_check);
        Shrinking(ans, concat_size);
        BuildTree();
    }

    unsigned char HuffmanCodec::MinCharSample(const std::vector<std::string_view>& sample) {
        std::array<int64_t, CHAR_SIZE_POWER> least_char{};
        for (size_t i = 0; i < sample.size(); ++i) {
            for (size_t j = 0; j < sample[i].size(); ++j) {
                ++least_char[static_cast<unsigned char>(sample[i][j])];
            }
        }
        return static_cast<unsigned char>(
            std::distance(least_char.begin(), std::min_element(least_char.begin(), least_char.end())));
    }

    std::string HuffmanCodec::MakeOneString(std::vector<std::string_view>& sample, const unsigned char min_char) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::shuffle(sample.begin(), sample.end(), gen);
        std::string concat;
        concat.reserve(MAX_CONCAT_SIZE);
        int64_t cnt_letters = 0;
        for (size_t i = 0; i < sample.size(); ++i) {
            concat += std::string(sample[i].data(), sample[i].size()) + std::string(1, min_char);
            cnt_letters += sample[i].size() + 1;
            if (cnt_letters >= static_cast<int64_t>(MAX_CONCAT_SIZE)) {
                concat.resize(MAX_CONCAT_SIZE);
                break;
            }
        }
        concat += std::string(1, min_char);
        concat_size = concat.size();
        return concat;
    }

    void HuffmanCodec::FindStats(const std::string& concat, Trie& trie_ch,
                                 std::unordered_map<std::string, std::pair<int64_t, int64_t>>& to_check,
                                 const unsigned char min_char, const size_t dict_size) {
        SuffTree tree(concat);
        auto SuffStatisctics = tree.FindSubstrings(dict_size, min_char);
        for (const auto& element : SuffStatisctics) {
            if (element.first.size() > 1) {
                trie_ch.Insert(element.first);
                to_check[element.first] = {0, element.second};
            }
        }
        SuffStatisctics.clear();
    }

    void HuffmanCodec::RunOnSample(const std::string& concat, const Trie& trie_ch,
                                   std::unordered_map<std::string, std::pair<int64_t, int64_t>>& to_check,
                                   const unsigned char min_char) {
        int64_t uz = 0;
        size_t size_uz = 0;
        std::string cur;
        for (size_t i = 0; i < concat.size(); ++i) {
            if (static_cast<unsigned char>(concat[i]) != min_char) {
                if (trie_ch.IsNext(uz, concat[i])) {
                    uz = trie_ch.Next(uz, concat[i]);
                    if (trie_ch.nodes[uz].is_terminal) {
                        size_uz = cur.size() + 1;
                    }
                    cur.push_back(concat[i]);
                } else {
                    while (cur.size() > size_uz) {
                        cur.pop_back();
                        --i;
                    }
                    ++to_check[cur].first;
                    cur.clear();
                    cur.push_back(concat[i]);
                    uz = trie_ch.Next(0, concat[i]);
                    size_uz = 1;
                }
            } else {
                while (cur.size() > size_uz) {
                    cur.pop_back();
                    --i;
                }
                if (cur.size() != 0) {
                    ++to_check[cur].first;
                }
                uz = 0;
                size_uz = 0;
                cur.clear();
            }
        }
    }

    void HuffmanCodec::FinalStats(const std::unordered_map<std::string, std::pair<int64_t, int64_t>>& to_check) {
        for (const auto& str : to_check) {
            // if std::string occurs at least once than add to dictionary
            if (str.second.first > 0 && str.first.size() >= 2) {
                ans.push_back({str.first, str.second.first});
            } else {
                ans[static_cast<unsigned char>(str.first[0])].second =
                    std::max(str.second.first, ans[static_cast<unsigned char>(str.first[0])].second);
            }
        }
        sort(ans.rbegin(), ans.rend() - (CHAR_SIZE_POWER),
             [](auto& left, auto& right) { return left.second < right.second; });
    }

    void HuffmanCodec::Shrinking(std::vector<std::pair<std::string, int64_t>>& ans_copied, const size_t concat_size_local) {
        std::map<KeyStr, ValueStr> CheckingStrMap;
        for (auto& str : ans_copied) {
            CheckingStrMap[{str.first.size(), str.first}] = {str.second, true};
        }
        for (auto& str : CheckingStrMap) {
            const auto& [FirstStrSize, FirstStrStr] = str.first;
            auto& [SecondStrOccur, SecondStrTaken] = str.second;
            UNUSED(SecondStrTaken);
            if (FirstStrSize > 1) {
                for (size_t i = 1; i < FirstStrSize; ++i) {
                    auto lhs = FirstStrStr.substr(0, i);
                    auto rhs = FirstStrStr.substr(i);
                    KeyStr lhsKS = {lhs.size(), lhs};
                    KeyStr rhsKS = {rhs.size(), rhs};
                    if (CheckingStrMap.count(lhsKS) && CheckingStrMap.count(rhsKS) && CheckingStrMap[lhsKS].taken &&
                        CheckingStrMap[rhsKS].taken) {
                        if (CheckingStrMap[lhsKS].occur * CheckingStrMap[rhsKS].occur >
                            SecondStrOccur * static_cast<int64_t>(concat_size_local)) {
                            SecondStrTaken = false;
                            break;
                        }
                    }
                }
            }
        }
        ans_copied.resize(CHAR_SIZE_POWER);
        for (const auto& [FirstStr, SecondStr] : CheckingStrMap) {
            if (SecondStr.taken && FirstStr.size >= 2) {
                ans_copied.push_back({FirstStr.str, SecondStr.occur});
            }
        }
        sort(ans_copied.rbegin(), ans_copied.rend() - (CHAR_SIZE_POWER),
             [](auto& left, auto& right) { return left.second < right.second; });
    }

} // namespace NCodecs
