#include "Node_better.h"
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
  frequency = l->getFrequency() + r->getFrequency();
  left = l;
  right = r;  
  for (size_t i = 0; i < (1 << CHAR_SIZE); ++i) {
    to_go.push_back({"", nullptr});
  }
}



Node::~Node() {
  delete left;
  delete right;
}

int64_t Node::getFrequency() const { //frequency of a node
  return frequency;
}

std::string Node::getData() const {
  return data;
}
