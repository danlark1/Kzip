#include "Node.h"

Node::Node(char d, int s, Node_type t) { //make a leaf
  type = t;
  data = d;
  frequensy = s;
  left = NULL;
  right = NULL;
}

Node::Node(Node* l, Node* r, Node_type t) { //make a bind
  type = t;
  data = 0;
  frequensy = l->getFrequency() + r->getFrequency();
  left = l;
  right = r;
}

Node::~Node {
  delete left;
  delete right;
}

int Node::getFrequency() { //frequency of a node
  return frequensy;
}

//fill node with data
void fill(std::map<char, std::pair<int, int>>& enc, int bits, int nbits) {
  if (type == LEAF) {
    enc[data] = std::pair<int, int>(bits, nbits);
  } else if (type == BIND) {
    ++nbits;
    bits *= 2;
    left->fill(enc, bits, nbits);
    ++bits;
    right->fill(enc, bits, nbits);
    bits /= 2;
    --nbits;
  }
}
