#include "Huffman_better.h"
#include "Node_better.h"
#include <fstream>
#include <iostream>
#include <algorithm>

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
    while (i < raw.size()) {
      j = 0;
      string cur(1, raw[i]);
      string cur1(1, raw[i]);
      while (i + 1 < raw.size() && table.count(cur + raw[i + 1]) > 0) {
        ++i;
        cur += raw[i];
      }
      size_t size_of_path = table.at(cur).size();

      while (j < size_of_path) {
        while (j < size_of_path && count) {
          buf <<= 1;
          buf += table.at(cur)[j];
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

    //till almost the last element
    for (int64_t i = 0; i < encoded_size; ++i) {
      raw += cur->to_go[byte].first;
      cur = cur->to_go[byte].second;
      byte = encoded[i + 1];
    }

    //last element
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
    std::ofstream output("config");
    for (int32_t c = 0; c < (1 << CHAR_SIZE); ++c) {
      std::string str(1, c);
      output << static_cast<uint16_t>(c) << " " << chars.at(str) << std::endl;
    }
    output.close();
    return string();
  }

  void HuffmanCodec::Build_table(Node* root_for_table, std::vector<int32_t>& code) {
    if (root_for_table->left != nullptr) {
      code.push_back(false);
      Build_table(root_for_table->left, code);
    }

    if (root_for_table->right != nullptr) {
      code.push_back(true);
      Build_table(root_for_table->right, code);
    }

    if (root_for_table->left == root_for_table->right) {
      table[(root_for_table->getData())] = code;
    }

    code.pop_back();
  }

  void HuffmanCodec::load(const string_view& config) {
    //TODO
    std::ifstream input("../huffman/output.txt");
    std::string now;
    while (input.good()) {
      getline(input, now);
      int64_t count = 0;  
      int64_t i = now.size() - 1;
      while (now[i] <= '9' && now[i] >= '0') {
        --i;
      }
      for (int64_t j = i + 1; j < now.size(); ++j) {
        count = 10 * count + (now[j] - '0');
      }
      chars[now.substr(0, i)] = count;
    }
    input.close();


    

    std::vector<Node*> table_cur;
    for (const auto& c : chars) {
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

    std::vector<int32_t> code;
    Build_table(root_for_table, code);
    code.clear();
    code.shrink_to_fit();

    root_for_table = root_for_decode;
    build_jumps(root_for_table);
  }

  size_t HuffmanCodec::sample_size(size_t records_total) const {
    return std::min(static_cast<size_t>(100000), records_total);
  }

  void HuffmanCodec::learn(const StringViewVector& sample) {
    for (int32_t c = 0; c < (1 << CHAR_SIZE); ++c) {
      std::string s(1, c);
      chars[s] = 0;
      table[s] = {};
    }


    //TODO
    std::ifstream input("../huffman/output.txt");
    std::string now;
    while (input.good()) {
      getline(input, now);
      int64_t count = 0;

      int64_t i = now.size() - 1;
      while (now[i] <= '9' && now[i] >= '0') {
        --i;
      }
      for (int64_t j = i + 1; j < now.size(); ++j) {
        count = 10 * count + (now[j] - '0');
      }
      chars[now.substr(0, i)] = count;
    }
    input.close();
    

    std::vector<Node*> table_cur;
    for (const auto& c : chars) {
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
        if (!cur->left) { //is equal to cur->left == nullptr == cur->right
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
    delete root_for_decode;
    delete root_for_encode;
  }
}  // namespace Codecs