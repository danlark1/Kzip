// SELF-MADE LIBRARIES
#include "Trie.h"

// NEEDED LIBRARIES
#include <vector>
#include <string>

#include <iostream>

Trie::Trie() {
  nodes.push_back(Uzel(0));
}

void Trie::make_transfer(int32_t i, unsigned char c, bool is_end) {
  nodes.push_back(Uzel(is_end));
  nodes[i].next[c] = nodes.size() - 1;
}

void Trie::insert(const std::string& s) {
  int32_t i = 0;
  size_t j = 0;
  while (j < s.size() && is_next(i, static_cast<unsigned char>(s[j]))) {
    i = next(i, static_cast<unsigned char>(s[j]));
    ++j;
  }
  if (j == s.size()) {
    nodes[i].is_terminal = true;
    return;
  }

  while (j < s.size()) {
    make_transfer(i, static_cast<unsigned char>(s[j]), false);
    i = nodes.size() - 1;
    ++j;
  }
  nodes[i].is_terminal = true;
  return;
}

void Trie::insert(const std::string& s, const std::vector<int32_t>& code) {
  int32_t i = 0;
  size_t j = 0;
  while (j < s.size() && is_next(i, static_cast<unsigned char>(s[j]))) {
    i = next(i, static_cast<unsigned char>(s[j]));
    ++j;
  }
  if (j == s.size()) {
    nodes[i].is_terminal = true;
    nodes[i].code = code;
    return;
  }

  while (j < s.size()) {
    make_transfer(i, static_cast<unsigned char>(s[j]), false);
    i = nodes.size() - 1;
    ++j;
  }
  nodes[i].is_terminal = true;
  nodes[i].code = code;
  return;
}

bool Trie::is_next(int32_t uz, unsigned char c) const {
  return (nodes[uz].next[c] == -1 ? 0 : 1);
}

int32_t Trie::next(int32_t uz, unsigned char c) const {
  return nodes[uz].next[c];
}

Trie::~Trie() {
  // nodes.clear();
  nodes.shrink_to_fit();
}
