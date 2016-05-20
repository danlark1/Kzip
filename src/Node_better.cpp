// SELF-MADE
#include "Node_better.h"

// NEEDED LIBRARIES
#include <vector>
#include <string>

Node::Node() {
  left = nullptr;
  right = nullptr;
  frequency = 0;
  data = "";
}

Node::Node(std::string d, int64_t s, Node_type t) { // make a leaf
  type = t;
  data = d;
  frequency = s;
  left = nullptr;
  right = nullptr;
  for (size_t i = 0; i < (1 << CHAR_SIZE); ++i) {
    to_go.push_back({"", nullptr});
  }
}

Node::Node(Node* l, Node* r, Node_type t) { // make a bind
  type = t;
  data = "";
  data.clear();
  data.shrink_to_fit();
  frequency = l->getFrequency() + r->getFrequency();
  left = l;
  right = r;  
  for (size_t i = 0; i < (1 << CHAR_SIZE); ++i) {
    to_go.push_back({"", nullptr});
  }
}

Node::~Node() {
  to_go.clear();
  to_go.shrink_to_fit();
  if (this->left != nullptr) {
    delete left;
    delete right;
  }
}

int64_t Node::getFrequency() const { // frequency of a node
  return frequency;
}

std::string Node::getData() const {
  return data;
}
