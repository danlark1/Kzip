// Copyright 2017, Danila Kutenin
#pragma once

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

// positions are characterized by the vertex and how many
// levels we have to jump up
struct Position {
  int32_t vertex, len_up;
  Position(int32_t v, int32_t up)
      : vertex(v)
      , len_up(up) {
  }
};

// Node_s of suffix tree
struct Node_s {
  int32_t parent, left, right, link;  //  [left, right)
  std::unordered_map<unsigned char, int32_t> next;
  Node_s(int32_t l, int32_t r, int32_t p, int32_t suflink = -1)
      : parent(p)
      , left(l)
      , right(r)
      , link(suflink) {
  }
};

class SuffTree {

  public:
  SuffTree(const std::string& str);
  std::vector<std::pair<std::string, int64_t>> FindSubstrings(const size_t dict_size, const unsigned char min_char);

  private:
  // states of a suffix tree
  std::vector<Node_s> st;
  std::string s;  // string

  // number of lists and sum of a string to this vertex
  std::vector<int32_t> num_of_lists;
  std::vector<int32_t> sum_str;

  // cur --- how many vertices are now into the suffix tree
  // n --- size of a suffix tree
  int32_t cur;
  size_t n;

  inline int32_t GetLength(const int32_t v) const;
  Position Go(Position pos, int32_t l, int32_t r);
  int32_t Split(const Position pos);
  int32_t GetLink(const int32_t v);
  Position ExtendUkkonen(Position ptr, const int32_t i);
  void Dfs(int32_t v);
};
