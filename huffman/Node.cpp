#include "Node.h"

Node::Node(unsigned char d, int s, Node_type t) { //make a leaf
  type = t;
  data = d;
  frequency = s;
  left = nullptr;
  right = nullptr;
}

Node::Node(Node* l, Node* r, Node_type t) { //make a bind
  type = t;
  data = 0;
  frequency = l->getFrequency() + r->getFrequency();
  left = l;
  right = r;
}

Node::~Node() {
  delete left;
  delete right;
}

int Node::getFrequency() { //frequency of a node
  return frequency;
}

char Node::getData() {
  return data;
}
