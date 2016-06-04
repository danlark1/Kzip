#pragma once

// SELF-MADE LIBRARIES
#include "Node_better.h"

// NEEDED LIBRARIES
#include <vector>
#include <string>

struct Uzel {
  bool is_terminal;
  std::vector<int32_t> next;
  std::vector<int8_t> code;
  Uzel(bool is_end) {
    is_terminal = is_end;
    this->next.resize(1 << CHAR_SIZE);
    for (size_t i = 0; i < (1 << CHAR_SIZE); ++i) {
      next[i] = -1;
    }
  }
};

class Trie {
private:
  void make_transfer(int32_t i, unsigned char c, bool is_end);
public:
  std::vector<Uzel> nodes;
  Trie();
  ~Trie();
  void insert(const std::string& s);
  void insert(const std::string& s, const std::vector<int8_t>& code);
  bool is_next(int32_t uz, unsigned char c) const;
  int32_t next(int32_t uz, unsigned char c) const;
};
