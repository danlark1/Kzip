// Copyright 2017, Danila Kutenin
#include "Trie.h"

#include <iostream>
#include <string>
#include <vector>

Trie::Trie() {
  nodes.push_back(Uzel(0));
}

void Trie::MakeTransfer(int32_t i, unsigned char c, bool is_end) {
  nodes.push_back(Uzel(is_end));
  nodes[i].next[c] = nodes.size() - 1;
}

void Trie::Insert(const std::string& s) {
  int32_t i = 0;
  size_t j = 0;
  while (j < s.size() && IsNext(i, static_cast<unsigned char>(s[j]))) {
    i = Next(i, static_cast<unsigned char>(s[j]));
    ++j;
  }
  if (j == s.size()) {
    nodes[i].is_terminal = true;
    return;
  }

  while (j < s.size()) {
    MakeTransfer(i, static_cast<unsigned char>(s[j]), false);
    i = nodes.size() - 1;
    ++j;
  }
  nodes[i].is_terminal = true;
  return;
}

void Trie::Insert(const std::string& s, const std::vector<int8_t>& code) {
  int32_t i = 0;
  size_t j = 0;
  while (j < s.size() && IsNext(i, static_cast<unsigned char>(s[j]))) {
    i = Next(i, static_cast<unsigned char>(s[j]));
    ++j;
  }
  
  if (j == s.size()) {
    nodes[i].is_terminal = true;
    nodes[i].code = code;
    return;
  }

  while (j < s.size()) {
    MakeTransfer(i, static_cast<unsigned char>(s[j]), false);
    i = nodes.size() - 1;
    ++j;
  }
  nodes[i].is_terminal = true;
  nodes[i].code = code;
}

bool Trie::IsNext(int32_t uz, unsigned char c) const {
  return (nodes[uz].next[c] == -1 ? 0 : 1);
}

int32_t Trie::Next(int32_t uz, unsigned char c) const {
  return nodes[uz].next[c];
}
