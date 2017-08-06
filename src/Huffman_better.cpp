/*
  Okay, lets start compressing;
  Load -- need file, named "config" for decoding, pls be sure that this file
  exists in the same path. DO NOT CHANGE this file either you'll fail decoding.
  encode and decode --- works as well
  Save -- saves the file, called "config"
  everything else should be intuitively understandable
*/

// SELF-MADE LIBRARIES
#include "Huffman_better.h"
#include "Node_better.h"
#include "Suffix_tree.h"
#include "Trie.h"


// NEEDED LIBRARIES
#include <inttypes.h>
#include <algorithm>
#include <fstream>
#include <map>
#include <queue>
#include <cassert>
#include <string>
#include <utility>
#include <vector>

// DEBUG LIBRARY (REMOVE TODO)
#include <iostream>

namespace Codecs {

  class comp {
   public:
    bool operator()(std::pair<Node*, int64_t>& l, std::pair<Node*, int64_t>& r) {
      if (l.first->getFrequency() == r.first->getFrequency()) {
        return r.second > l.second;
      }
      return l.first->getFrequency() > r.first->getFrequency();
    }
  };

  void HuffmanCodec::encode(string& encoded, const string_view& raw) const {
    if (__builtin_expect(raw.size() == 0, false)) {
      return;
    }
    encoded.reserve(2 * raw.size());
    unsigned char buf = 0;
    int8_t count = CHAR_SIZE - LOG_CHAR_SIZE;
    size_t j = 0;
    size_t i = 0;
    size_t uz = 0;
    size_t len_uz = 0;
    size_t last_uz = 0;

    size_t cur_size = 0;

    while (i < raw.size()) {
      j = 0;
      uz = 0;
      cur_size = 1;

      uz = trie.next(uz, static_cast<unsigned char>(raw[i]));
      last_uz = uz;
      len_uz = 1;

      while (i + 1 < raw.size() && trie.is_next(uz, raw[i + 1])) {
        uz = trie.next(uz, raw[i + 1]);
        ++i;
        if (trie.nodes[uz].is_terminal) {
          len_uz = cur_size + 1;
          last_uz = uz;
        }
        ++cur_size;
      }
      while (cur_size > len_uz) {
        --cur_size;
        --i;
      }
      size_t size_of_path = trie.nodes[last_uz].code.size();

      while (j < size_of_path) {
        while (j < size_of_path && count) {
          buf <<= 1;
          buf += trie.nodes[last_uz].code[j];
          ++j;
          --count;
        }
        if (!count) {
          count = CHAR_SIZE;
          encoded.push_back(buf);
          buf = 0;
        }
      }
      ++i;
    }
    if (__builtin_expect(count != CHAR_SIZE, true)) {
      encoded.push_back(buf << count);
    }
    unsigned char c = encoded[0];
    c |= (static_cast<unsigned char>(CHAR_SIZE - count)) <<
     (CHAR_SIZE - LOG_CHAR_SIZE);
    encoded[0] = c;
  }


  void HuffmanCodec::decode(string& raw, const string_view& encoded) const {
    if (encoded.size() == 0) {
      return;
    }
    raw.reserve(2 * encoded.size());
    int8_t count = LOG_CHAR_SIZE;
    Node* cur = root_for_decode;
    unsigned char byte = encoded[0];
    int64_t encoded_size = static_cast<int64_t>(encoded.size()) - 1;

    while (encoded_size >= 1) {
      if (byte & (1 << (CHAR_SIZE - count - 1))) {
        cur = cur->right;
      } else {
        cur = cur->left;
      }
      if (!cur->left) {
        raw += cur->getData();
        cur = root_for_decode;
      }
      ++count;
      if (count == CHAR_SIZE || (encoded_size == 0 && 
        (static_cast<unsigned char>(encoded[0]) >> (CHAR_SIZE - LOG_CHAR_SIZE)) 
        == count)) {
        count = 0;
        if (encoded_size >= 1) {
          byte = encoded[1];
        }
        break;
      }
    }

    // till almost the last element
    for (int64_t i = 1; i < encoded_size; ++i) {
      raw += cur->to_go[byte].first;
      cur = cur->to_go[byte].second;
      byte = encoded[i + 1];
    }

    // last element
    while (true) {
      if (byte & (1 << (CHAR_SIZE - count - 1))) {
        cur = cur->right;
      } else {
        cur = cur->left;
      }
      if (!cur->left) {
        raw += cur->getData();
        cur = root_for_decode;
      }
      ++count;
      if (count == CHAR_SIZE ||
        (static_cast<unsigned char>(encoded[0]) >> (CHAR_SIZE - LOG_CHAR_SIZE)) 
          == count) {
        break;
      }
    }
  }


  string HuffmanCodec::save() const {
    std::ofstream output("config", std::ios_base::binary);
    for (const auto& str : ans) {
      output << str.first.size() << " " << str.first << " " << str.second << std::endl;
    }
    output.close();
    return string();
  }

  void HuffmanCodec::Build_table(Node* root_for_table,
    std::vector<int8_t>& code) {
    if (root_for_table->left) {
      code.push_back(false);
      Build_table(root_for_table->left, code);
    }

    if (root_for_table->right) {
      code.push_back(true);
      Build_table(root_for_table->right, code);
    }
    if (root_for_table->left == root_for_table->right) {
      string s = root_for_table->getData();
      trie.insert(s, code);
    }
    code.pop_back();
  }

  void HuffmanCodec::load(const string_view& config) {
    ans.clear();
    ans.shrink_to_fit();

    for (int32_t c = 0; c < (1 << CHAR_SIZE); ++c) {
      std::string s(1, c);
      ans.push_back({s, 0});
    }

    std::ifstream input(config.to_string(), std::ios_base::binary);
    while (input.good()) {
      size_t len_string;
      string current;
      input >> len_string;
      input.get();
      if (!input.good()) {
        break;
      }
      for (size_t i = 0; i < len_string; ++i) {
        current += input.get();
      }
      size_t frequency;
      input >> frequency;
      if (len_string == 1) {
        ans[static_cast<unsigned char>(current[0])].second = frequency;
      } else {
        ans.push_back({current, frequency});
      }
    }
    input.close();


    std::priority_queue<std::pair<Node*, int64_t>,
     std::vector<std::pair<Node*, int64_t> >, comp> table_cur;
    int64_t i = 0;
    for (const auto& c : ans) {
      Node* p = new Node(c.first, c.second);
      table_cur.push({p, i});
      ++i;
    }

    Node* parent;
    while (table_cur.size() != 1) {
      Node* left_son = table_cur.top().first;
      table_cur.pop();
      Node* right_son = table_cur.top().first;
      table_cur.pop();
      parent = new Node(left_son, right_son);
      table_cur.push({parent, i});
      ++i;
    }
    root_for_decode = table_cur.top().first;
    build_jumps(root_for_decode);
    std::vector<int8_t> code;
    Build_table(root_for_decode, code);
    code.clear();
    code.shrink_to_fit();
  }

  size_t HuffmanCodec::sample_size(size_t records_total) const {
    return std::min(static_cast<size_t>(MAX_CONCAT_SIZE), records_total);
  }

  void HuffmanCodec::shrinking(std::vector<std::pair<std::string, int64_t> >& ans_copied, const size_t concat_size) {
    std::map<std::pair<int64_t, std::string>, std::pair<int64_t, bool> > IS_STRING_OK;
    for (auto& str : ans_copied) {
      IS_STRING_OK[{str.first.size(), str.first}] = {str.second, true};
    }
    for (auto& str : IS_STRING_OK) {
      if (str.first.second.size() > 1) {
        for (size_t i = 1; i < str.first.second.size(); ++i) {
          string s_1;
          string s_2;
          for (size_t j = 0; j < i; ++j) {
            s_1.push_back(str.first.second[j]);
          }
          for (size_t j = i; j < str.first.second.size(); ++j) {
            s_2.push_back(str.first.second[j]);
          }
          if (IS_STRING_OK.count({s_1.size(), s_1}) && IS_STRING_OK.count({s_2.size(), s_2}) 
            && IS_STRING_OK[{s_1.size(), s_1}].second && IS_STRING_OK[{s_2.size(), s_2}].second) {
            if (IS_STRING_OK[{s_1.size(), s_1}].first * IS_STRING_OK[{s_2.size(), s_2}].first 
              > str.second.first * static_cast<int64_t>(concat_size)) {
              str.second.second = false;
            }
          }
        }
      }
    }
    ans_copied.resize(1 << CHAR_SIZE);
    std::vector<std::pair<int64_t, std::string> > ans_copy;
    for (auto& str : IS_STRING_OK) {
      if (str.second.second && str.first.first >= 2) {
        ans_copy.push_back({str.second.first, str.first.second});
      }
    }
    sort(ans_copy.rbegin(), ans_copy.rend());
    for (auto& str : ans_copy) {
      ans_copied.push_back({str.second, str.first});
    }
  }

  void HuffmanCodec::learn(StringViewVector& sample, const size_t dict_size) {
    std::unordered_map<std::string, std::pair<int64_t, int64_t> > to_check;
    trie = Trie();
    Trie trie_ch = Trie();
    for (int32_t c = 0; c < (1 << CHAR_SIZE); ++c) {
      std::string s(1, c);
      ans.push_back({s, 0});
      trie_ch.insert(s, {});
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

    srand(time(NULL));
    std::random_shuffle(sample.begin(), sample.end());
    {
      std::string concat;
      concat.reserve(MAX_CONCAT_SIZE);
      int64_t cnt_letters = 0;
      for (size_t i = 0; i < sample.size(); ++i) {
        concat += sample[i].to_string() + std::string(1, min_char);
        cnt_letters += sample[i].size() + 1;
        if (cnt_letters >= static_cast<int64_t>(MAX_CONCAT_SIZE)) {
          concat.resize(MAX_CONCAT_SIZE);
          break;
        }
      }
      concat += std::string(1, min_char);
      concat_size = concat.size();

      {
        suff_tree tree(concat);
        std::vector<std::pair<std::string, int64_t> > ans1 = tree.find_substr(dict_size, min_char);
        for (const auto& t : ans1) {
          if (t.first.size() > 1) {
            trie_ch.insert(t.first);
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
          if (trie_ch.is_next(uz, concat[i])) {
            uz = trie_ch.next(uz, concat[i]);
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
            uz = trie_ch.next(0, concat[i]);
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
        }
        ans[static_cast<unsigned char>(str.first[0])].second =
        std::max(str.second.first, ans[static_cast<unsigned char>(str.first[0])].second);
      }
      sort(ans2.rbegin(), ans2.rend());
      for (const auto& c: ans2) {
        ans.push_back({c.second, c.first});
      }
    }
    shrinking(ans, concat_size);


    // need to use pair because of the non-deterministic Heap;
    std::priority_queue<std::pair<Node*, int64_t>,
     std::vector<std::pair<Node*, int64_t> >, comp> table_cur;
    int64_t i = 0;
    for (const auto& c : ans) {
      Node* p = new Node(c.first, c.second);
      table_cur.push({p, i});
      ++i;
    }

    while (table_cur.size() != 1) {
      Node* left_son = table_cur.top().first;
      table_cur.pop();
      Node* right_son = table_cur.top().first;
      table_cur.pop();
      Node* parent = new Node(left_son, right_son);
      table_cur.push({parent, i});
      ++i;
    }
    root_for_decode = table_cur.top().first;
    std::vector<int8_t> code;
    Build_table(root_for_decode, code);
    code.clear();
    code.shrink_to_fit();
  }

  void HuffmanCodec::build_jumps(Node* node) {
    Node* cur;
    for (size_t byte = 0; byte < (1 << CHAR_SIZE); ++byte) {
      cur = node;
      if (!cur->right || !cur->left) {
        continue;
      }
      for (size_t j = 0; j < CHAR_SIZE; ++j) {
        if (byte & (1 << (CHAR_SIZE - j - 1))) {
          cur = cur->right;
        } else {
          cur = cur->left;
        }
        if (!cur->left) {  // is equal to cur->left == nullptr == cur->right
          node->to_go[byte].first += cur->getData();
          cur = root_for_decode;
        }
      }
      node->to_go[byte].second = cur;
    }
    if (node->left) {
      build_jumps(node->left);
      build_jumps(node->right);
    }
  }

  void HuffmanCodec::reset() {
    ans.clear();
    ans.shrink_to_fit();
    if (root_for_decode) {
      delete root_for_decode;
    }
  }
}  // namespace Codecs
