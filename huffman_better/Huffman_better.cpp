#include "Huffman_better.h"
#include "Node_better.h"
#include "Suffix_tree.h"
#include "Trie.h"
#include <fstream>
#include <vector>
#include <string>
#include <clocale>
#include <iostream>
#include <algorithm>
#include <utility>

namespace Codecs {


  bool comp(Node* l, Node* r) {
    return l->getFrequency() > r->getFrequency();
  }

  void HuffmanCodec::encode(string& encoded, const string_view& raw) const {
    encoded.reserve(2 * raw.size());
    unsigned char buf = 0;
    int8_t count = CHAR_SIZE;
    size_t j = 0;
    size_t i = 0;
    size_t uz = 0;
    size_t len_uz = 0;
    size_t last_uz = 0;

    string cur;
    cur.reserve(10);
    while (i < raw.size()) {
      j = 0;
      uz = 0;
      cur.clear();
      cur.push_back(raw[i]);

      uz = trie->next(uz, static_cast<unsigned char>(raw[i]));
      last_uz = uz;
      len_uz = 1;
      while (i + 1 < raw.size() && trie->is_next(uz, raw[i + 1])) {
        uz = trie->next(uz, raw[i + 1]);
        ++i;
        if (trie->nodes[uz].is_terminal) {
          len_uz = cur.size() + 1;
          last_uz = uz;
        }
        cur.push_back(raw[i]);
      }
      while (__builtin_expect(cur.size() > len_uz, false)) {
        cur.pop_back();
        --i;
      }

      // fuck this shit
      size_t size_of_path = trie->nodes[last_uz].code.size();
      while (j < size_of_path) {
        while (j < size_of_path && count) {
          buf <<= 1;
          buf += trie->nodes[last_uz].code[j];
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
    if (count != CHAR_SIZE) {
      encoded.push_back(buf << count);
    }
    encoded.push_back(static_cast<uint8_t>(CHAR_SIZE - count));
  }


  void HuffmanCodec::decode(string& raw, const string_view& encoded) const {
    if (__builtin_expect(encoded.size() == 1, false)) {
      return;
    }
    raw.reserve(2 * encoded.size());
    int8_t count = 0;
    Node* cur = root_for_decode;
    unsigned char byte = encoded[0];
    int64_t encoded_size = static_cast<int64_t>(encoded.size()) - 2;

    // till almost the last element
    for (int64_t i = 0; i < encoded_size; ++i) {
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
      if (count == CHAR_SIZE || encoded[encoded_size + 1] == count) {
        count = 0;
        byte = encoded[encoded_size];
        break;
      }
    }
  }

  string HuffmanCodec::save() const {
    std::ofstream output("config", std::ios_base::binary);
    for (const auto& c : ans) {
      output << c.first << " " << c.second << std::endl;
    }
    output.close();
    return string();
  }

  void HuffmanCodec::Build_table(Node* root_for_table,
    std::vector<int32_t>& code) {
    if (root_for_table->left != nullptr) {
      code.push_back(false);
      Build_table(root_for_table->left, code);
    }

    if (root_for_table->right != nullptr) {
      code.push_back(true);
      Build_table(root_for_table->right, code);
    }

    if (root_for_table->left == root_for_table->right) {
      string s = root_for_table->getData();
      trie->insert(s, code);
    }
    code.pop_back();
  }

  void HuffmanCodec::load(const string_view& config) {
    trie->reset();
    ans.clear();
    ans.shrink_to_fit();

    for (int32_t c = 0; c < (1 << CHAR_SIZE); ++c) {
      std::string s(1, c);
      ans.push_back({s, 0});
    }

    std::ifstream input(config.to_string(), std::ios_base::binary);
    std::string now;
    while (input.good()) {
      getline(input, now);
      int64_t count = 0;
      int64_t i = now.size() - 1;
      while (now[i] <= '9' && now[i] >= '0') {
        --i;
      }
      for (size_t j = i + 1; j < now.size(); ++j) {
        count = 10 * count + (now[j] - '0');
      }
      if (i == 1) {
        ans[static_cast<unsigned char>(now[0])].second = count;
      } else {
        ans.push_back({now.substr(0, i), count});
      }
    }
    input.close();


    std::vector<Node*> table_cur;
    for (const auto& c : ans) {
      Node* p = new Node(c.first, c.second);
      table_cur.push_back(p);
    }

    auto it = table_cur.begin();
    while (table_cur.size() != 1) {
      std::stable_sort(it, table_cur.end(), comp);
      Node* left_son = table_cur.back();
      table_cur.pop_back();
      Node* right_son = table_cur.back();
      table_cur.pop_back();
      Node* parent = new Node(left_son, right_son);
      table_cur.push_back(parent);
    }
    root_for_decode = table_cur.front();
    Node* root_for_table = root_for_encode;

    root_for_table = root_for_decode;
    build_jumps(root_for_table);
  }

  size_t HuffmanCodec::sample_size(size_t records_total) const {
    return std::min(static_cast<size_t>(10000), records_total);
  }

  void HuffmanCodec::learn(const StringViewVector& sample) {
    trie = new Trie();

    for (int32_t c = 0; c < (1 << CHAR_SIZE); ++c) {
      std::string s(1, c);
      ans.push_back({s, 0});
      trie->insert(s);
    }

    std::string concat;
    concat.reserve(1e6);
    int64_t cnt_letters = 0;
    for (size_t i = 0; i < sample.size(); ++i) {
      concat += sample[i].to_string() + '\0';
      cnt_letters += sample[i].size() + 1;
      if (cnt_letters >= static_cast<int64_t>(1e6)) {
      // constant TODO(danlark1)
        break;
      }
    }
    concat += "\0";
    tree = new suff_tree(concat);
    std::vector<std::pair<std::string, int64_t> > ans1 = tree->find_substr();
    for (const auto& t : ans1) {
      if (t.first.size() == 1) {
        ans[static_cast<unsigned char>(t.first[0])].second = t.second;
      } else {
        ans.push_back({t.first, t.second});
      }
    }
    delete tree;
    concat.clear();
    concat.shrink_to_fit();
    ans1.clear();
    ans1.shrink_to_fit();


    std::vector<Node*> table_cur;
    for (const auto& c : ans) {
      Node* p = new Node(c.first, c.second);
      table_cur.push_back(p);
    }

    auto it = table_cur.begin();
    while (table_cur.size() != 1) {
      std::stable_sort(it, table_cur.end(), comp);
      Node* left_son = table_cur.back();
      table_cur.pop_back();
      Node* right_son = table_cur.back();
      table_cur.pop_back();
      Node* parent = new Node(left_son, right_son);
      table_cur.push_back(parent);
    }
    root_for_encode = table_cur.front();
    Node* root_for_table = root_for_encode;

    std::vector<int32_t> code;
    Build_table(root_for_table, code);
    code.clear();
    code.shrink_to_fit();
  }

  void HuffmanCodec::build_jumps(Node* node) {
    Node* cur;
    for (size_t byte = 0; byte < (1 << CHAR_SIZE); ++byte) {
      cur = node;
      if (!cur->right || !cur->left) {
        // cur = root_for_decode;
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
    if (node->left != nullptr) {
      build_jumps(node->left);
      build_jumps(node->right);
    }
  }

  void HuffmanCodec::reset() {
    trie->reset();
    ans.clear();

    delete root_for_decode;
    delete root_for_encode;
  }
}  // namespace Codecs
