#pragma once

#include <map>
#include <vector>
#include <string>
#include <experimental/string_view>

enum Node_type {LEAF, BIND};

class Node {
public:
  Node_type type;
  int frequency;
  char data;
  Node* left;
  Node* right;
  Node(char d, int s, Node_type = LEAF); // Construct a list
  Node(Node* l, Node* r, Node_type = BIND); // Construct a bind node
  ~Node();
  int getFrequency();
  char getData();
};
