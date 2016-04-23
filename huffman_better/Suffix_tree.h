#pragma once
#include <unordered_map>
#include <cstdio>
#include <algorithm>
#include <vector>
#include <string>
#include <cmath>
#include <fstream>


// positions are characterized by the vertex and how many
// levels we have to jump up
struct Position {
  int32_t vertex, len_up;
  Position(int32_t v, int32_t up) {
    this->vertex = v;
    this->len_up = up;
  }
};

// Node_s of suffix tree
struct Node_s {
  int32_t parent, left, right, link;  //  [left, right)
  std::unordered_map<unsigned char, int32_t> next;
  Node_s(int32_t l, int32_t r, int32_t p, int32_t suflink = -1) {
    this->parent = p;
    this->left = l;
    this->right = r; 
    this->link = suflink;
  }
};


class suff_tree {

public:
  suff_tree(const std::string& str);
  ~suff_tree();
  std::vector<std::pair<std::string, int64_t> > find_substr();

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

  int32_t get_length(const int32_t v) const;
  Position go(Position pos, int32_t l, int32_t r);
  int32_t split(const Position pos);
  int32_t get_link(const int32_t v);
  Position extend_ukkonen(Position ptr, const int32_t i);
  void dfs(int32_t v);

};
