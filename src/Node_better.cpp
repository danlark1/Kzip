// SELF-MADE
#include "Node_better.h"

// NEEDED LIBRARIES
#include <vector>
#include <string>

Node::Node() : left(nullptr), right(nullptr), frequency(0), data("") {}

Node::Node(const std::string& d, const int64_t s, const Node_type t) { // make a leaf
  type = t;
  data = std::move(d);
  frequency = s;
  left = nullptr;
  right = nullptr;
  for (size_t i = 0; i < (1 << CHAR_SIZE); ++i) {
    to_go.push_back({"", nullptr});
  }
}

Node::Node(Node* l, Node* r, Node_type t) { // make a bind
  type = t;
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
  if (this->left) {
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
