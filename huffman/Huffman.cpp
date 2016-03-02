#include "Huffman.h"
#include "Node.h"
#include <iostream>
#include <fstream>
#include <list>
#include <algorithm>
#include <unordered_map>

namespace Codecs {

  bool comp(Node* l, Node* r) {
    return l->getFrequency() > r->getFrequency();
  }

  void HuffmanCodec::encode(string& encoded, const string_view& raw) const {
    unsigned char buf = 0;
    size_t count = 0;
    for (size_t i = 0; i < raw.size(); ++i) {
      unsigned char c = raw[i];
      auto it = table.find(c);
      size_t size_of_path = it->second.size();
      size_t j = 0;
      while (j < size_of_path) {
        while (j < size_of_path && count < CHAR_SIZE) {
          buf = buf | ((it->second)[j] << (CHAR_SIZE - count - 1));
          ++j;
          ++count;
        }
        if (CHAR_SIZE == count) {
          count = 0;
          encoded.push_back(buf);
          buf = 0;
        }
      }
    }
    if (count != 0) {
      encoded.push_back(buf);
    }
    encoded.push_back(static_cast<unsigned char>(count));
  }


  void HuffmanCodec::decode(string& raw, const string_view& encoded) const {
    int64_t count = 0;
    Node* cur = root_for_decode;
    unsigned char byte = encoded[0];
    int64_t encoded_size = static_cast<int64_t>(encoded.size()) - 2;

    //till almost the last element
    for (int64_t i = 0; i < encoded_size; ++i) {
      for (size_t j = 0; j < CHAR_SIZE; ++j) {
        if (byte & (1 << (CHAR_SIZE - j - 1))) {
          cur = cur->right;
        } else {
          cur = cur->left;
        }
        if (cur->left == cur->right) { //is equal to cur->left == nullptr == cur->right
          raw.push_back(cur->getData());
          cur = root_for_decode;
        }
      }
      byte = encoded[i + 1];
    }

    //last element
    while (true) {
      if (byte & (1 << (CHAR_SIZE - count - 1))) {
        cur = cur->right;
      } else {
        cur = cur->left;
      }
      if (cur->left == cur->right) {
        raw.push_back(cur->getData());
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
    for (auto it = chars.begin(); it != chars.end(); ++it) {
      output << static_cast<uint16_t>(it->first) << " " << it->second << std::endl;
    }
    output.close();
    return string();
  }

  void HuffmanCodec::Build_table(Node* root_for_table, std::vector<bool>& code) {
    if (root_for_table->left != nullptr) {
      code.push_back(false);
      Build_table(root_for_table->left, code);
    }

    if (root_for_table->right != nullptr) {
      code.push_back(true);
      Build_table(root_for_table->right, code);
    }

    if (root_for_table->left == nullptr && root_for_table->left == nullptr) {
      table.insert({root_for_table->getData(), code});
    } 

    code.pop_back();
  }

  void HuffmanCodec::load(const string_view& config) {
    std::ifstream input(config.to_string());
    for (size_t i = 0; i < (1 << CHAR_SIZE); ++i) {
      uint32_t sym;
      uint64_t cnt;
      input >> sym >> cnt;
      chars.insert({static_cast<unsigned char>(sym), cnt});
    }

    std::vector<Node*> table_cur;
    for (auto it = chars.begin(); it != chars.end(); ++it) {
      Node *p = new Node(it->first, it->second);
      table_cur.push_back(p);
    }

    while (table_cur.size() != 1) {
      std::stable_sort(table_cur.begin(), table_cur.end(), comp);
      Node *left_son = table_cur.back();
      table_cur.pop_back();
      Node *right_son = table_cur.back();
      table_cur.pop_back();
      Node *parent = new Node(left_son, right_son);
      table_cur.push_back(parent);
    }
    root_for_decode = table_cur.front();
    Node* root_for_table = root_for_decode;


    std::vector<bool> code;
    Build_table(root_for_table, code);
    code.clear();
  }

  size_t HuffmanCodec::sample_size(size_t records_total) const {
    return std::min(static_cast<size_t>(10000), records_total);
  }

  void HuffmanCodec::learn(const StringViewVector& sample) {
    size_t total_count = 0;

    for (int32_t c = 0; c < (1 << CHAR_SIZE); ++c) {
      chars.insert({static_cast<unsigned char>(c), 0});
    }

    for (auto& cur_string : sample) {
      total_count += cur_string.size();
      for (auto& one_char : cur_string) {
        auto iterator = chars.find(one_char);
        if (iterator != chars.end()) {
          ++(iterator->second);
        }
      }
    }
    std::vector<Node*> table_cur;
    for (auto it = chars.begin(); it != chars.end(); ++it) {
      Node *p = new Node(it->first, it->second);
      table_cur.push_back(p);
    }
    auto it = table_cur.begin();
    while (table_cur.size() != 1) {
      std::stable_sort(it, table_cur.end(), comp);
      Node *left_son = table_cur.back();
      table_cur.pop_back();
      Node *right_son = table_cur.back();
      table_cur.pop_back();
      Node *parent = new Node(left_son, right_son);
      table_cur.push_back(parent);
    }
    root_for_encode = table_cur.front();
    Node* root_for_table = root_for_encode;

    std::vector<bool> code;
    Build_table(root_for_table, code);
    code.clear();
  }

  void HuffmanCodec::reset() {
    table.clear();
    chars.clear();
  }
}  // namespace Codecs
