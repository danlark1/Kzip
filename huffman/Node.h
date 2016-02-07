#ifndef NODE_H
#define NODE_H

#include <map>
#include <vector>
#include <string>
#include <experimental/string_view>

enum Node_type {LEAF, BIND};

class Node {
private:
  Node_type type;
  int frequency;
  char data;
  Node* left;
  Node* right;
public:
  Node(char d, int s, Node_type = LEAF); // Construct a list
  Node(Node* l, Node* r, Node_type = BIND); // Construct a bind node
  ~Node();
  int getFrequency();
  void fill(std::map<char, std::pair<int, int>>& enc, int bits, int nbits);
};

#endif