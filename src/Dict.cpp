// Copyright 2017, Danila Kutenin

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

namespace Codecs {

  void HuffmanCodec::Learn(StringViewVector& sample, const size_t dict_size) {
    std::unordered_map<std::string, std::pair<int64_t, int64_t> > to_check;
    trie = Trie();
    Trie trie_ch = Trie();
    for (int32_t c = 0; c < (1 << CHAR_SIZE); ++c) {
      std::string s(1, c);
      ans.push_back({s, 0});
      trie_ch.Insert(s, {});
      to_check[s] = {0, 0};
    }
    std::vector<int64_t> least_char;
    least_char.resize(1 << CHAR_SIZE);
    for (size_t i = 0; i < sample.size(); ++i) {
      for (size_t j = 0; j < sample[i].size(); ++j) {
        ++least_char[static_cast<unsigned char>(sample[i][j])];
      }
    }
    unsigned char min_char = 0;
    int64_t min_freq = least_char[0];
    for (int32_t c = 0; c < (1 << CHAR_SIZE); ++c) {
      if (least_char[c] < min_freq) {
        min_freq = least_char[c];
        min_char = static_cast<unsigned char>(c);
      }
    }

    // ayee, boy
    std::random_device rd;
    std::mt19937 gen(rd());

    std::shuffle(sample.begin(), sample.end(), gen);
    {
      std::string concat;
      concat.reserve(MAX_CONCAT_SIZE);
      int64_t cnt_letters = 0;
      for (size_t i = 0; i < sample.size(); ++i) {
        concat += sample[i].data() + std::string(1, min_char);
        cnt_letters += sample[i].size() + 1;
        if (cnt_letters >= static_cast<int64_t>(MAX_CONCAT_SIZE)) {
          concat.resize(MAX_CONCAT_SIZE);
          break;
        }
      }
      concat += std::string(1, min_char);
      concat_size = concat.size();

      {
        SuffTree tree(concat);
        std::vector<std::pair<std::string, int64_t> > ans1 = tree.FindSubstrings(dict_size, min_char);
        for (const auto& t : ans1) {
          if (t.first.size() > 1) {
            trie_ch.Insert(t.first);
            to_check[t.first] = {0, t.second};
          }
        }
        ans1.clear();
      }

      int64_t uz = 0;
      size_t size_uz = 0;
      string cur;
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
      concat.clear();
      concat.shrink_to_fit();
    }

    ans.resize(1 << CHAR_SIZE);
    {
      std::vector<std::pair<int64_t, std::string> > ans2;
      for (const auto& str : to_check) {
        // if string occurs at least once than add to dictionary
        if ((str.second.first > 0 && str.first.size() >= 2)) {
          ans2.push_back({str.second.first, str.first});
        } else if (str.first.size() == 1) {        
          ans[static_cast<unsigned char>(str.first[0])].second =
          std::max(str.second.first, ans[static_cast<unsigned char>(str.first[0])].second);
        }
      }
      sort(ans2.rbegin(), ans2.rend());
      for (const auto& c: ans2) {
        ans.push_back({c.second, c.first});
      }
    }
    Shrinking(ans, concat_size);
    BuildTree();
  }

  void HuffmanCodec::Shrinking(std::vector<std::pair<std::string, int64_t> >& ans_copied, const size_t concat_size) {
    std::map<KeyStr, ValueStr> CheckingStrMap;
    for (auto& str : ans_copied) {
      CheckingStrMap[{str.first.size(), str.first}] = {str.second, true};
    }
    for (auto& str : CheckingStrMap) {
      const auto& [FirstStrSize, FirstStrStr] = str.first;
      auto& [SecondStrOccur, SecondStrTaken] = str.second;
      (void) SecondStrTaken;
      if (FirstStrSize > 1) {
        for (size_t i = 1; i < FirstStrSize; ++i) {
          auto lhs = FirstStrStr.substr(0, i);
          auto rhs = FirstStrStr.substr(i);
          KeyStr lhsKS = {lhs.size(), lhs};
          KeyStr rhsKS = {rhs.size(), rhs};
          if (CheckingStrMap.count(lhsKS) && CheckingStrMap.count(rhsKS) 
            && CheckingStrMap[lhsKS].taken && CheckingStrMap[rhsKS].taken) {
            if (CheckingStrMap[lhsKS].occur * CheckingStrMap[rhsKS].occur 
              > SecondStrOccur * static_cast<int64_t>(concat_size)) {
              SecondStrTaken = false;
              break;
            }
          }
        }
      }
    }
    ans_copied.resize(1 << CHAR_SIZE);
    std::vector<std::pair<int64_t, std::string> > ans_copy;
    for (const auto& str : CheckingStrMap) {
      auto FirstStr = str.first;
      auto SecondStr = str.second;
      if (SecondStr.taken && FirstStr.size >= 2) {
        ans_copy.push_back({SecondStr.occur, FirstStr.str});
      }
    }
    sort(ans_copy.rbegin(), ans_copy.rend());
    for (const auto& str : ans_copy) {
      ans_copied.push_back({str.second, str.first});
    }
  }
} // namespace Codecs
