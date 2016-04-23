#pragma once

#include <vector>
#include <string>

struct Uzel {
  bool is_terminal;
  int32_t* next;
  std::vector<int32_t> code;
  Uzel (bool is_end) {
    is_terminal = is_end;
    next = new int32_t[256];
    for (size_t i = 0; i < 256; ++i) {
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
  void reset();
  void insert(std::string& s);
  void insert(std::string& s, std::vector<int32_t>& code);
  bool is_next(int32_t uz, unsigned char c) const;
  int32_t next(int32_t uz, unsigned char c) const;
};
