#include "Node_another.h"
#include <cstdio>

Node::Node(std::string d, int64_t s, Node_type t) { //make a leaf
  type = t;
  data = d;
  frequency = s;
  left = nullptr;
  right = nullptr;
  for (size_t i = 0; i < (1 << CHAR_SIZE); ++i) {
    to_go.push_back({"", nullptr});
  }
}

Node::Node(Node* l, Node* r, Node_type t) { //make a bind
  type = t;
  data = "";
  data.shrink_to_fit();
  frequency = l->getFrequency() + r->getFrequency();
  left = l;
  right = r;  
  for (size_t i = 0; i < (1 << CHAR_SIZE); ++i) {
    to_go.push_back({"", nullptr});
  }
}

Node::~Node() {
  this->to_go.clear();
  delete this->left;
  this->left = nullptr;
  delete this->left;
  delete this->right;
  this->right = nullptr;
  delete this->right;
}

int64_t Node::getFrequency() { //frequency of a node
  return frequency;
}

std::string Node::getData() {
  return data;
}
