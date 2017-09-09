// Copyright 2017, Danila Kutenin
#pragma once

#include "Node.h"

#include <string>
#include <vector>

struct Uzel {
  bool is_terminal;
  std::vector<int32_t> next;
  std::vector<int8_t> code;
  Uzel(bool is_end)
      : is_terminal(is_end) {
    this->next.resize(1 << CHAR_SIZE);
    std::fill(next.begin(), next.end(), -1);
  }
};

class Trie {
  private:
  void MakeTransfer(int32_t i, unsigned char c, bool is_end);

  public:
  std::vector<Uzel> nodes;
  Trie();
  void Insert(const std::string&);
  void Insert(const std::string&, const std::vector<int8_t>&);
  bool IsNext(int32_t uz, unsigned char c) const;
  int32_t Next(int32_t uz, unsigned char c) const;
};
